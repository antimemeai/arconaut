#include "blackbird/retained_state.hpp"
#include <array>
#include <chrono>
#include <csignal>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
using namespace blackbird;
namespace {
#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (false)
template<class T> T require(Result<T> r) { CHECK(r.has_value()); return std::move(r).value(); }
void require(Result<void> r) { CHECK(r.has_value()); }
template<class T> T id(unsigned char n) { IdentityBytes b{}; b[15]=std::byte{n}; return require(T::from_bytes(b)); }
std::uint64_t counter(const IdentityBytes& b) {
  std::uint64_t n=0;
  for (unsigned i=0;i<8;++i) n |= std::uint64_t(std::to_integer<unsigned char>(b[i+8]))<<(i*8);
  return n;
}
struct Temp {
  std::string path;
  Temp() { char p[]="/tmp/blackbird-issuer-XXXXXX"; CHECK(mkdtemp(p)); path=p; }
  ~Temp() { std::error_code e; std::filesystem::remove_all(path,e); }
};
struct Counts { std::uint64_t syncs=0; };
class File final: public JournalFile {
  std::unique_ptr<JournalFile> file_; Counts& counts_;
public:
  File(std::unique_ptr<JournalFile> f, Counts& c):file_(std::move(f)),counts_(c){}
  Result<void> lock_writer() override { return file_->lock_writer(); }
  Result<std::uint64_t> extent() override { return file_->extent(); }
  Result<std::size_t> read_at(std::uint64_t o,MutableByteView b) override { return file_->read_at(o,b); }
  Result<std::size_t> write_at(std::uint64_t o,ByteView b) override { return file_->write_at(o,b); }
  Result<void> synchronize(SyncStrength s) override { ++counts_.syncs; return file_->synchronize(s); }
};
class Directory final: public JournalDirectory {
  NativeJournalDirectory dir_; Counts& counts_;
  Result<std::unique_ptr<JournalFile>> wrap(Result<std::unique_ptr<JournalFile>> r) {
    if (!r.has_value()) return r;
    return Result<std::unique_ptr<JournalFile>>::success(std::make_unique<File>(std::move(r).value(),counts_));
  }
public:
  Directory(const std::string& p, Counts& c):dir_(require(NativeJournalDirectory::open(p))),counts_(c){}
  Result<std::unique_ptr<JournalFile>> create_exclusive(std::string_view n) override { return wrap(dir_.create_exclusive(n)); }
  Result<std::unique_ptr<JournalFile>> open_existing(std::string_view n,FileAccess a) override { return wrap(dir_.open_existing(n,a)); }
  Result<void> synchronize_directory(SyncStrength s) override { return dir_.synchronize_directory(s); }
};
JournalHeader header(std::uint64_t ns=3) { return {id<EnvironmentId>(1),id<AuditStreamId>(2),ns,{1024,4096},std::nullopt}; }
constexpr JournalCapacity capacity{16<<20,100000};
class Custody final: public CustodyVerifier {
  Result<void> verify(std::span<const AttemptState> a) override { CHECK(a.empty()); return Result<void>::success(); }
};
void resume(RetainedState& s) { require(s.confirm_recovery()); Custody c; require(s.reconcile(c)); }
void workload(std::uint64_t width) {
  Temp t; Counts c;
  auto state=require(RetainedState::create(std::make_unique<Directory>(t.path,c),"journal",header(),capacity));
  auto syncs=c.syncs;
  auto begin=std::chrono::steady_clock::now();
  for(std::uint64_t i=1;i<=14610;++i) {
    auto b=require(state->issue<ParticipantId>()).bytes();
    CHECK(counter(b)==i && b[0]==std::byte{3});
  }
  auto elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count();
  const auto expected=(14610+width-1)/width;
  CHECK(state->committed_facts().size()==expected);
  CHECK(c.syncs-syncs==expected);
  CHECK(state->issuer_counter()==expected*width);
  for(std::size_t i=0;i<expected;++i)
    CHECK(std::get<IssuerReservationEvent>(state->committed_facts()[i].event.body).counter==(i+1)*width);
  std::printf("{\"issued\":14610,\"width\":%llu,\"reservations\":%llu,\"sync_delta\":%llu,\"seconds\":%.9f}\n",
    static_cast<unsigned long long>(width),static_cast<unsigned long long>(expected),static_cast<unsigned long long>(c.syncs-syncs),elapsed);
  // Reopen burns the remainder, replay agrees with durable highwater.
  const auto high=state->issuer_counter(); state.reset();
  state=require(RetainedState::open(std::make_unique<Directory>(t.path,c),"journal",header(),capacity));
  CHECK(state->issuer_counter()==high); resume(*state);
  CHECK(counter(require(state->issue<ParticipantId>()).bytes())==high+1);
}
void crash_and_namespace() {
  Temp t;
  const auto child=fork(); CHECK(child>=0);
  if(child==0) {
    Counts c;
    auto s=require(RetainedState::create(std::make_unique<Directory>(t.path,c),"journal",header(),capacity));
    CHECK(counter(require(s->issue<ParticipantId>()).bytes())==1);
    ::kill(getpid(),SIGKILL); ::_exit(99);
  }
  int status=0; CHECK(waitpid(child,&status,0)==child); CHECK(WIFSIGNALED(status)&&WTERMSIG(status)==SIGKILL);
  Counts c;
  auto s=require(RetainedState::open(std::make_unique<Directory>(t.path,c),"journal",header(),capacity));
  CHECK(s->issuer_counter()==1024); resume(*s);
  CHECK(counter(require(s->issue<ParticipantId>()).bytes())==1025);
  // Distinct namespaces can start counters anew but cannot collide as full IDs.
  Temp other;
  auto n=require(RetainedState::create(std::make_unique<Directory>(other.path,c),"journal",header(4),capacity));
  auto b=require(n->issue<ParticipantId>()).bytes(); CHECK(counter(b)==1 && b[0]==std::byte{4});
}
}
int main(int argc,char** argv) {
  try {
    const auto width=argc==2?std::stoull(argv[1]):1024;
    CHECK(width==1||width==1024); workload(width);
    if(width==1024) crash_and_namespace();
  } catch(const std::exception& e) { std::fprintf(stderr,"FAIL issuer: %s\n",e.what()); return 1; }
}
