#include "blackbird/context.hpp"
#include <filesystem>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
#define CHECK(x) do { if (!(x)) throw std::runtime_error("resume line " + std::to_string(__LINE__)); } while(false)
template<class T> T id(unsigned char n) { IdentityBytes b{}; b[0]=std::byte{n}; return unwrap(T::from_bytes(b)); }
int main() {
  char pattern[]="/tmp/bb-resume-XXXXXX"; const auto path=mkdtemp(pattern); if (!path) return 2;
  const JournalHeader h{id<EnvironmentId>(1),id<AuditStreamId>(2),3,{1024*1024,4*1024*1024},std::nullopt};
  const JournalCapacity cap{32*1024*1024,16};
  try {
    auto dir=unwrap(NativeJournalDirectory::open(path)); JournalResume resume{{h.journal,0,journal_header_size},0,0};
    const std::vector<std::byte> payload(512*1024,std::byte{'x'});
    const std::array draft{JournalDraft{FrameKind::source,payload}};
    {
      auto journal=unwrap(FramedJournal::create(dir,"audit",h,cap));
      for (unsigned i=0;i<8;++i) unwrap(journal->append(draft));
      resume.cursor=journal->cursor(); resume.indexed_records=journal->physical_records().size();
      auto anchor=unwrap(journal->read_original_range(resume.cursor.end_offset-56,56)); resume.commit_checksum=crc32c(anchor.bytes);
      for (unsigned i=0;i<3;++i) unwrap(journal->append(draft));
    }
    {
unsigned selected = 0;
auto journal=unwrap(FramedJournal::open(dir,"audit",h,cap,SyncStrength::full,false,std::nullopt,
    [&](FramedJournal &locked) -> Result<std::optional<JournalResume>> {
      ++selected; CHECK(locked.header()==h);
      CHECK(locked.staged_records().empty()); // before the archive scan
      auto rival=unwrap(dir.open_existing("audit",FileAccess::read_write));
      CHECK(!rival->lock_writer().has_value()); // selection owns writer custody
      return Result<std::optional<JournalResume>>::success(resume);
    }));
CHECK(selected==1);
      CHECK(journal->staged_records().size()==3); CHECK(journal->usage().indexed_records==11);
      unwrap(journal->confirm_recovery()); unwrap(journal->resume_after_reconciliation());
      for (unsigned i=0;i<5;++i) unwrap(journal->append(draft));
      CHECK(journal->usage().remaining_records()==0); CHECK(!journal->append(draft).has_value());
      CHECK(!journal->publish_scan_checkpoint().has_value()); CHECK(!journal->restage().has_value());
      CHECK(journal->state()==JournalWriterState::blocked);
    }
    {
      auto full=unwrap(FramedJournal::open(dir,"audit",h,cap)); CHECK(full->staged_records().size()==16);
    }
    auto bad=resume; ++bad.commit_checksum; CHECK(!FramedJournal::open(dir,"audit",h,cap,SyncStrength::full,false,bad).has_value());
    std::filesystem::resize_file(std::string(path)+"/audit",resume.cursor.end_offset-1);
    CHECK(!FramedJournal::open(dir,"audit",h,cap,SyncStrength::full,false,resume).has_value());
    std::filesystem::remove_all(path); std::cout<<"physical saved boundary/suffix/capacity/anchor/truncation passed (semantic bypass inactive)\n"; return 0;
  } catch(const Error &e) { std::cerr<<error_name(e.code)<<'\n'; } catch(const std::exception &e) { std::cerr<<e.what()<<'\n'; }
  std::filesystem::remove_all(path); return 1;
}
