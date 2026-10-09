#include "blackbird/recovery_index.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>
using namespace blackbird;
#define CHECK(x)                                                                       \
  do {                                                                                 \
    if (!(x))                                                                          \
      throw std::runtime_error("index line " + std::to_string(__LINE__));              \
  } while (false)
template <class T> T take(Result<T> r) {
  CHECK(r.has_value());
  return std::move(r).value();
}
void take(Result<void> r) { CHECK(r.has_value()); }
struct Bytes {
  std::vector<std::byte> bytes;
  std::size_t chunk = SIZE_MAX;
  bool fail = false, zero = false, oversize = false;
  std::size_t reads = 0;
  std::size_t writes_before_failure = SIZE_MAX;
};
class File final : public JournalFile {
public:
  explicit File(std::shared_ptr<Bytes> b) : b_(std::move(b)) {}
  Result<void> lock_writer() override { return Result<void>::success(); }
  Result<std::uint64_t> extent() override {
    return Result<std::uint64_t>::success(b_->bytes.size());
  }
  Result<void> synchronize(SyncStrength) override {
    return b_->fail ? Result<void>::failure({ErrorCode::io}) : Result<void>::success();
  }
  Result<std::size_t> read_at(std::uint64_t offset, MutableByteView out) override {
    ++b_->reads;
    if (b_->fail)
      return Result<std::size_t>::failure({ErrorCode::io});
    if (b_->oversize)
      return Result<std::size_t>::success(out.size() + 1);
    if (b_->zero || offset >= b_->bytes.size())
      return Result<std::size_t>::success(0);
    const auto n = std::min(
        {out.size(), b_->chunk, b_->bytes.size() - static_cast<std::size_t>(offset)});
    std::copy_n(b_->bytes.begin() + static_cast<std::ptrdiff_t>(offset), n,
                out.begin());
    return Result<std::size_t>::success(n);
  }
  Result<std::size_t> write_at(std::uint64_t offset, ByteView in) override {
    if (b_->writes_before_failure == 0)
      return Result<std::size_t>::failure({ErrorCode::io});
    if (b_->writes_before_failure != SIZE_MAX)
      --b_->writes_before_failure;
    if (b_->fail)
      return Result<std::size_t>::failure({ErrorCode::io});
    if (b_->oversize)
      return Result<std::size_t>::success(in.size() + 1);
    if (b_->zero)
      return Result<std::size_t>::success(0);
    const auto n = std::min(in.size(), b_->chunk);
    b_->bytes.resize(std::max(b_->bytes.size(), static_cast<std::size_t>(offset) + n));
    std::copy_n(in.begin(), n, b_->bytes.begin() + static_cast<std::ptrdiff_t>(offset));
    return Result<std::size_t>::success(n);
  }

private:
  std::shared_ptr<Bytes> b_;
};
RecoveryKey key(std::uint64_t n) {
  RecoveryKey k{};
  k[0] = std::byte{1};
  for (std::size_t i = 0; i < 8; ++i) {
    k[8 - i] = static_cast<std::byte>(n & 255);
    n >>= 8;
  }
  return k;
}
int main() {
  try {
    auto b = std::make_shared<Bytes>();
    auto index =
        take(RecoveryIndex::open(std::make_unique<File>(b), 128 * 1024 * 1024));
    RecoveryPageRef root{}, old{};
    // A permutation creates leaf and root splits, not just right-edge appends.
    for (std::uint64_t i = 0; i < 1600; ++i)
      root = take(index->put(root, key((i * 997) % 1600), ((i * 997) % 1600) * 3));
    old = root;
    for (std::uint64_t i = 0; i < 1600; ++i)
      CHECK(take(index->find(root, key(i))) == i * 3);
    CHECK(!take(index->find(root, key(1600))));
    root = take(index->put(root, key(400), 77));
    CHECK(take(index->find(old, key(400))) == 1200 &&
          take(index->find(root, key(400))) == 77);
    std::uint64_t expected = 398;
    take(index->range(root, key(398), 7, [&](RecoveryIndexEntry e) {
      CHECK(e.key == key(expected));
      CHECK(e.value == (expected == 400 ? 77 : expected * 3));
      ++expected;
      return Result<bool>::success(true);
    }));
    CHECK(expected == 405);
    b->reads = 0;
    take(index->range(root, key(0), 1600,
                      [](RecoveryIndexEntry) { return Result<bool>::success(false); }));
    CHECK(b->reads <= 3);
    take(index->synchronize());
    const auto bytes = index->bytes();
    index.reset();
    index = take(RecoveryIndex::open(std::make_unique<File>(b), 128 * 1024 * 1024));
    CHECK(index->bytes() == bytes && take(index->find(root, key(400))) == 77);
    b->chunk = 7;
    CHECK(take(index->find(root, key(400))) == 77);
    b->chunk = SIZE_MAX;
    for (unsigned mode = 0; mode < 3; ++mode) {
      b->fail = mode == 0;
      b->zero = mode == 1;
      b->oversize = mode == 2;
      CHECK(!index->find(root, key(400)).has_value());
      CHECK(!index->put(root, key(1600), 1).has_value());
      b->fail = b->zero = b->oversize = false;
      CHECK(take(index->find(old, key(400))) == 1200);
    }
    // Fail after a copied child is written but before its new parent is complete.
    b->writes_before_failure = 1;
    CHECK(!index->put(root, key(1600), 4800).has_value());
    b->writes_before_failure = SIZE_MAX;
    CHECK(!take(index->find(root, key(1600))) &&
          take(index->find(root, key(400))) == 77);
    auto wrong = root;
    ++wrong.generation;
    CHECK(!index->find(wrong, key(1)).has_value());
    wrong = root;
    ++wrong.offset;
    CHECK(!index->find(wrong, key(1)).has_value());
    wrong = root;
    wrong.checksum ^= 1;
    CHECK(!index->find(wrong, key(1)).has_value());
    b->bytes[static_cast<std::size_t>(root.offset) + 40] ^= std::byte{1};
    CHECK(!index->find(root, key(1)).has_value());
    CHECK(take(index->find(old, key(400))) == 1200);
    b->bytes.resize(static_cast<std::size_t>(old.offset) + 100);
    CHECK(!index->find(old, key(1)).has_value());
    auto limited = std::make_shared<Bytes>();
    auto small = take(
        RecoveryIndex::open(std::make_unique<File>(limited), RecoveryIndex::page_size));
    const auto only = take(small->put({}, key(1), 9));
    CHECK(!small->put(only, key(2), 10).has_value());
    CHECK(take(small->find(only, key(1))) == 9);
    std::cout << "paged index splits/exact/ranges/rollback/short I/O/faults passed; "
                 "derived bytes="
              << bytes << '\n';
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
