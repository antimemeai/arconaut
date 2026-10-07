#include "blackbird/context.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace blackbird;
namespace {
void check(bool value, int line) { if (!value) throw std::runtime_error("checkpoint line " + std::to_string(line)); }
#define CHECK(x) check((x), __LINE__)
template<class T> T id(unsigned char n) { IdentityBytes b{}; b[0]=std::byte{n}; return unwrap(T::from_bytes(b)); }
const JournalHeader header{id<EnvironmentId>(1),id<AuditStreamId>(2),3,{1024*1024,4*1024*1024},std::nullopt};
const JournalCapacity capacity{32*1024*1024,4096};
struct Workspace {
  std::string path;
  Workspace() { char pattern[]="/tmp/bb-checkpoint-XXXXXX"; auto p=mkdtemp(pattern); CHECK(p); path=p; }
  ~Workspace() { std::error_code ignored; std::filesystem::remove_all(path,ignored); }
};
std::unique_ptr<JournalDirectory> directory(const std::string &path) {
  return std::make_unique<NativeJournalDirectory>(unwrap(NativeJournalDirectory::open(path)));
}
std::vector<std::byte> bytes(const std::string &path) {
  std::ifstream file(path,std::ios::binary); CHECK(file.good());
  file.seekg(0,std::ios::end); const auto length=file.tellg(); CHECK(length>=0);
  std::vector<std::byte> result(static_cast<std::size_t>(length)); file.seekg(0);
  file.read(reinterpret_cast<char *>(result.data()),static_cast<std::streamsize>(result.size())); CHECK(file.good()); return result;
}
void write(const std::string &path, ByteView input) {
  std::ofstream file(path,std::ios::binary|std::ios::trunc);
  file.write(reinterpret_cast<const char *>(input.data()),static_cast<std::streamsize>(input.size())); CHECK(file.good());
}
void checksum(std::vector<std::byte> &input) {
  auto crc=crc32c(ByteView{input}.first(input.size()-4));
  for (std::size_t i=0;i<4;++i) input[input.size()-4+i]=static_cast<std::byte>((crc>>(8*i))&255U);
}
struct Snapshot { std::vector<RetainedFact> facts; Json view, originals; std::uint64_t highwater; AttemptState attempt; };
Snapshot snapshot(const std::string &path, bool indexed, bool expected_used) {
  auto root=unwrap(RetainedState::open(directory(path),"audit",header,capacity,indexed));
  CHECK(root->used_scan_checkpoint()==expected_used);
  CHECK(root->state()==JournalWriterState::recovery_pending);
  CHECK(!root->publish_scan_checkpoint().has_value());
  unwrap(root->confirm_recovery());
  AuditLog log{*root}; ContextStore context{log};
  auto attempt=unwrap(root->attempt(id<OperationAttemptId>(15)));
  CHECK(attempt.opened && attempt.reconciliation_required && !attempt.observation);
  struct NoEffect final : EffectBoundary { unsigned calls=0; Result<void> dispatch(const EffectIntent &) override { ++calls; return Result<void>::success(); } } effect;
  CHECK(!root->dispatch(id<OperationAttemptId>(15),effect).has_value()); CHECK(effect.calls==0);
  return {{root->committed_facts().begin(),root->committed_facts().end()},context.view(),context.originals(),root->issuer_counter(),attempt};
}
void equal(const Snapshot &a,const Snapshot &b) {
  CHECK(a.facts.size()==b.facts.size());
  for (std::size_t i=0;i<a.facts.size();++i) {
    CHECK(a.facts[i].record==b.facts[i].record);
    CHECK(a.facts[i].event==b.facts[i].event);
    CHECK(a.facts[i].evidence==b.facts[i].evidence);
  }
  CHECK(a.view==b.view && a.originals==b.originals && a.highwater==b.highwater);
  CHECK(a.attempt.admission==b.attempt.admission && a.attempt.evidence==b.attempt.evidence);
}
void reduction_and_corruption() {
  Workspace workspace; auto root=unwrap(RetainedState::create(directory(workspace.path),"audit",header,capacity));
  {
    AuditLog log{*root}; ContextStore context{log};
    context.append({Json::object({{"role",Json{"user"}},{"content",Json{"original"}}})},"fixture");
    const auto view=context.view(); const auto entry=string_field(field(view,"entries").array()[0],"id");
    const auto edit=Json::object({{"base",field(view,"base")},{"entries",Json{Json::Array{}}}});
    CHECK(std::get<bool>(field(context.edit(edit),"accepted").value()));
    CHECK(!std::get<bool>(field(context.edit(edit),"accepted").value()));
    context.restore(entry); // original repair across checkpoint
    unwrap(root->publish_scan_checkpoint());
    const auto d=id<DecisionId>(10); const auto v=id<InvocationId>(11);
    unwrap(root->submit({{},DecisionEvent{d,id<ParticipantId>(12),id<ConversationId>(13),id<WorkflowId>(14),
      id<DefinitionGenerationId>(20),id<ContextRevisionId>(21),{v},{std::byte{7}}}}));
    unwrap(root->submit({{},InvocationEvent{v,d,id<DefinitionGenerationId>(20),{std::byte{7}}}}));
    unwrap(root->submit({{},AttemptAdmissionEvent{id<OperationAttemptId>(15),v,d,{std::byte{7}}}}));
    unwrap(root->submit({{},AttemptOpenEvent{id<OperationAttemptId>(15)}}));
    unwrap(root->publish_scan_checkpoint());
    // Durable highwater and no-op/rejected context records in recovery tail.
    unwrap(root->submit({{},IssuerReservationEvent{root->issuer_counter()+1000}}));
    CHECK(!std::get<bool>(field(context.edit(edit),"accepted").value()));
    context.append({},"no-op");
  }
  const auto complete_end=root->cursor().end_offset; root.reset();
  const auto full=snapshot(workspace.path,false,false);
  equal(full,snapshot(workspace.path,true,true));
  const auto a=bytes(workspace.path+"/audit.scan.0"); const auto b=bytes(workspace.path+"/audit.scan.1");
  auto damaged=b; damaged[0]^=std::byte{1}; write(workspace.path+"/audit.scan.1",damaged);
  equal(full,snapshot(workspace.path,true,true)); // prior root + larger tail
  damaged=a; damaged.resize(17); write(workspace.path+"/audit.scan.0",damaged);
  equal(full,snapshot(workspace.path,true,false)); // both unusable -> full scan
  write(workspace.path+"/audit.scan.0",a); write(workspace.path+"/audit.scan.1",b);
  damaged=b; damaged[16]^=std::byte{1}; checksum(damaged); write(workspace.path+"/audit.scan.1",damaged);
  equal(full,snapshot(workspace.path,true,true)); // wrong identity, even with valid CRC
  damaged=b; for (std::size_t i=0;i<8;++i) damaged[160+24+i]=std::byte{255}; checksum(damaged);
  write(workspace.path+"/audit.scan.1",damaged); equal(full,snapshot(workspace.path,true,true)); // bad location bounds
  write(workspace.path+"/audit.scan.1",b);
  std::filesystem::remove(workspace.path+"/audit.scan.0"); std::filesystem::remove(workspace.path+"/audit.scan.1");
  equal(full,snapshot(workspace.path,true,false)); // missing indexes
  write(workspace.path+"/audit.scan.0",a); write(workspace.path+"/audit.scan.1",b);
  // Torn suffix is observed, not re-executed. Preserve highwater and custody fence.
  { std::ofstream file(workspace.path+"/audit",std::ios::binary|std::ios::app); file.put('x'); }
  for (bool indexed : {false,true}) {
    auto reopened=unwrap(RetainedState::open(directory(workspace.path),"audit",header,capacity,indexed));
    CHECK(reopened->recovery_report().problem.has_value());
    CHECK(reopened->issuer_counter()==full.highwater);
    CHECK(unwrap(reopened->attempt(id<OperationAttemptId>(15))).reconciliation_required);
    unwrap(reopened->confirm_recovery()); CHECK(reopened->state()!=JournalWriterState::live); CHECK(!reopened->issue<ApplicationRecordId>().has_value());
  }
  std::filesystem::resize_file(workspace.path+"/audit",complete_end);
  // A truncated journal cannot use a root beyond its available boundary.
  std::filesystem::resize_file(workspace.path+"/audit",112);
  auto reopened=RetainedState::open(directory(workspace.path),"audit",header,capacity,true);
  CHECK(!reopened.has_value()); CHECK(reopened.error().code==ErrorCode::incomplete);
}

enum class Fault { none,journal_sync,create,write,root_sync,replace_before,replace_after,directory_sync };
struct FaultControl { Fault fault=Fault::none; unsigned hits=0; bool hit(Fault f) { if (fault!=f) return false; ++hits; return true; } };
class FaultFile final : public JournalFile {
  std::unique_ptr<JournalFile> file_; FaultControl &control_; bool root_;
public:
  FaultFile(std::unique_ptr<JournalFile> file,FaultControl &control,bool root) : file_(std::move(file)),control_(control),root_(root) {}
  Result<std::size_t> read_at(std::uint64_t offset,MutableByteView output) override { return file_->read_at(offset,output); }
  Result<std::size_t> write_at(std::uint64_t offset,ByteView input) override {
    if (root_ && control_.hit(Fault::write)) {
      auto partial=file_->write_at(offset,input.first(input.size()/2));
      if (!partial.has_value()) return partial;
      return Result<std::size_t>::failure({ErrorCode::io});
    }
    return file_->write_at(offset,input);
  }
  Result<std::uint64_t> extent() override { return file_->extent(); }
  Result<void> synchronize(SyncStrength strength) override {
    if (control_.hit(root_ ? Fault::root_sync : Fault::journal_sync)) return Result<void>::failure({ErrorCode::io});
    return file_->synchronize(strength);
  }
  Result<void> lock_writer() override { return file_->lock_writer(); }
};
class FaultDirectory final : public JournalDirectory {
  std::unique_ptr<JournalDirectory> directory_; FaultControl &control_;
  Result<std::unique_ptr<JournalFile>> wrap(Result<std::unique_ptr<JournalFile>> file,bool root) {
    if (!file.has_value()) return file;
    return Result<std::unique_ptr<JournalFile>>::success(std::make_unique<FaultFile>(std::move(file).value(),control_,root));
  }
public:
  FaultDirectory(std::unique_ptr<JournalDirectory> dir,FaultControl &control) : directory_(std::move(dir)),control_(control) {}
  Result<std::unique_ptr<JournalFile>> create_exclusive(std::string_view name) override {
    if (name.find(".scan.tmp.")!=std::string_view::npos && control_.hit(Fault::create))
      return Result<std::unique_ptr<JournalFile>>::failure({ErrorCode::io});
    return wrap(directory_->create_exclusive(name),name!="audit");
  }
  Result<std::unique_ptr<JournalFile>> open_existing(std::string_view name,FileAccess access) override {
    return wrap(directory_->open_existing(name,access),name!="audit");
  }
  Result<void> synchronize_directory(SyncStrength strength) override {
    if (control_.hit(Fault::directory_sync)) return Result<void>::failure({ErrorCode::io});
    return directory_->synchronize_directory(strength);
  }
  Result<void> replace_file(std::string_view from,std::string_view to) override {
    if (control_.hit(Fault::replace_before)) return Result<void>::failure({ErrorCode::io});
    auto result=directory_->replace_file(from,to); if (!result.has_value()) return result;
    if (control_.hit(Fault::replace_after)) return Result<void>::failure({ErrorCode::external_unknown});
    return result;
  }
};
void publication_failures() {
  for (auto fault : {Fault::journal_sync,Fault::create,Fault::write,Fault::root_sync,Fault::replace_before,Fault::replace_after,Fault::directory_sync}) {
    Workspace workspace; FaultControl control;
    auto root=unwrap(RetainedState::create(std::make_unique<FaultDirectory>(directory(workspace.path),control),"audit",header,capacity));
    unwrap(root->submit({{},IssuerReservationEvent{10}})); unwrap(root->publish_scan_checkpoint());
    const auto prior=bytes(workspace.path+"/audit.scan.0");
    unwrap(root->submit({{},IssuerReservationEvent{20}})); control.fault=fault;
    CHECK(!root->publish_scan_checkpoint().has_value()); CHECK(control.hits==1); // no blind retries
    CHECK(bytes(workspace.path+"/audit.scan.0")==prior);
    control.fault=Fault::none; unwrap(root->publish_scan_checkpoint()); // distinct explicit attempt, never automatic retry
    root.reset();
    auto reopened=unwrap(RetainedState::open(directory(workspace.path),"audit",header,capacity,true));
    CHECK(reopened->used_scan_checkpoint()); CHECK(reopened->issuer_counter()==20);
    CHECK(reopened->recovery_report().clean()); unwrap(reopened->confirm_recovery());
    CHECK(reopened->committed_facts().size()==2);
  }
}
} // namespace
int main() {
  try { reduction_and_corruption(); publication_failures(); std::cout<<"checkpoint reduction, references and publication failures passed\n"; return 0; }
  catch (const Error &e) { std::cerr<<"error "<<error_name(e.code)<<'\n'; }
  catch (const std::exception &e) { std::cerr<<e.what()<<'\n'; }
  return 1;
}
