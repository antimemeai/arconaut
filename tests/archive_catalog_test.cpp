#include "blackbird/archive_catalog.hpp"
#include "blackbird/context.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
#define CHECK(x) do { if (!(x)) throw std::runtime_error("archive line " + std::to_string(__LINE__)); } while(false)
template<class T> T id(unsigned char n) { IdentityBytes b{}; b[0]=std::byte{n}; return unwrap(T::from_bytes(b)); }
int main() {
  char pattern[]="/tmp/bb-archive-XXXXXX"; const auto path=mkdtemp(pattern); if (!path) return 2;
  const JournalHeader h{id<EnvironmentId>(1),id<AuditStreamId>(2),3,{1024*1024,4*1024*1024},std::nullopt};
  const JournalCursor cursor{h.journal,2001,1000000};
  try {
    auto dir=unwrap(NativeJournalDirectory::open(path)); std::vector<ArchiveEntry> entries;
    for (std::uint64_t i=0;i<1600;++i) {
      RecoveryKey key{}; key[0]=std::byte{6}; key[38]=static_cast<std::byte>(i>>8); key[39]=static_cast<std::byte>(i&255);
      entries.push_back({key,{h.environment,h.journal,FrameKind::semantic,i+1,1,144+i*48,16,17}});
    }
    unwrap(ArchiveCatalog::publish(dir,"catalog",h,cursor,entries));
    CHECK(std::filesystem::file_size(std::string(path)+"/catalog")==160+1600*96);
    auto catalog=unwrap(ArchiveCatalog::open(dir,"catalog",h,cursor,2000)); CHECK(catalog->count()==1600);
    for (std::size_t i=0;i<entries.size();++i) CHECK(unwrap(catalog->find(entries[i].key))==entries[i].record);
    RecoveryKey absent{}; absent[0]=std::byte{7}; CHECK(!unwrap(catalog->find(absent)));
    auto wrong=cursor; ++wrong.sequence; CHECK(!ArchiveCatalog::open(dir,"catalog",h,wrong,2000).has_value());
    CHECK(!ArchiveCatalog::open(dir,"catalog",h,cursor,1599).has_value());
    auto invalid=entries; invalid[1].key=invalid[0].key; CHECK(!ArchiveCatalog::publish(dir,"catalog",h,cursor,invalid).has_value());
    // Replacement retains old reader ownership and never mutates its generation.
    auto newer=cursor; ++newer.sequence;
    unwrap(ArchiveCatalog::publish(dir,"catalog",h,newer,entries));
    CHECK(unwrap(catalog->find(entries[1599].key))==entries[1599].record);
    auto current=unwrap(ArchiveCatalog::open(dir,"catalog",h,newer,2000));
    { std::fstream f(std::string(path)+"/catalog",std::ios::binary|std::ios::in|std::ios::out); f.seekp(160+800*96+5); const char c='!'; f.write(&c,1); }
    CHECK(!current->find(entries[800].key).has_value());
    std::filesystem::resize_file(std::string(path)+"/catalog",160+20);
    CHECK(!ArchiveCatalog::open(dir,"catalog",h,newer,2000).has_value());
    CHECK(!current->find(entries[1599].key).has_value());
    std::filesystem::remove_all(path); std::cout<<"archive 1600-key bulk bytes=153760 exact queries/generations/corrupt/short reads passed\n"; return 0;
  } catch(const Error &e) { std::cerr<<error_name(e.code)<<'\n'; } catch(const std::exception &e) { std::cerr<<e.what()<<'\n'; }
  std::filesystem::remove_all(path); return 1;
}
