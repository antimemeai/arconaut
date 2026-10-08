#pragma once
#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

namespace blackbird {
// Append-oriented persistent radix sequence. Copies pin one root; writes copy
// only the changed path and one <=32-element leaf. No contiguous-span promise.
// Mutable references are internal borrows ending at the next copy or mutation;
// pinned readers use const access to their independently owned root.
template <class T> class SharedSequence {
  static constexpr std::size_t width = 32;
  struct Node {
    std::array<std::shared_ptr<Node>, width> children{};
    std::vector<T> values;
  };
  std::shared_ptr<Node> root_;
  std::size_t size_ = 0, levels_ = 0;
  static void own(std::shared_ptr<Node> &node) {
    if (!node)
      node = std::make_shared<Node>();
    else if (node.use_count() != 1)
      node = std::make_shared<Node>(*node);
  }
  std::size_t capacity() const {
    std::size_t result = width;
    for (std::size_t i = 0; i < levels_; ++i) {
      if (result > max_size() / width)
        return max_size();
      result *= width;
    }
    return result;
  }
  std::vector<T> &leaf(std::size_t index) {
    own(root_);
    auto *node = root_.get();
    for (auto level = levels_; level; --level) {
      const auto digit = (index >> (5 * level)) & (width - 1);
      own(node->children[digit]);
      node = node->children[digit].get();
    }
    return node->values;
  }
  const std::vector<T> &leaf(std::size_t index) const {
    auto *node = root_.get();
    for (auto level = levels_; level; --level)
      node = node->children[(index >> (5 * level)) & (width - 1)].get();
    return node->values;
  }

public:
  using value_type = T;
  using size_type = std::size_t;
  std::size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return !size_; }
  static constexpr std::size_t max_size() {
    return std::numeric_limits<std::ptrdiff_t>::max();
  }
  void reserve(std::size_t) {} // Leaves reserve as they become live.
  T &operator[](std::size_t index) { return leaf(index)[index & (width - 1)]; }
  const T &operator[](std::size_t index) const {
    return leaf(index)[index & (width - 1)];
  }
  T &back() { return (*this)[size_ - 1]; }
  const T &back() const { return (*this)[size_ - 1]; }
  void push_back(T value) {
    if (size_ == max_size())
      throw std::length_error("shared sequence capacity");
    if (size_ == capacity()) {
      auto next = std::make_shared<Node>();
      next->children[0] = root_;
      root_ = std::move(next);
      ++levels_;
    }
    auto &values = leaf(size_);
    if (values.empty())
      values.reserve(width);
    values.push_back(std::move(value));
    ++size_;
  }
  void pop_back() {
    leaf(size_ - 1).pop_back();
    --size_;
    if (!size_)
      clear();
  }
  void clear() noexcept {
    root_.reset();
    size_ = levels_ = 0;
  }
  void swap(SharedSequence &other) noexcept {
    root_.swap(other.root_);
    std::swap(size_, other.size_);
    std::swap(levels_, other.levels_);
  }
  template <bool Constant> class Iterator {
    using Owner = std::conditional_t<Constant, const SharedSequence, SharedSequence>;
    Owner *owner_ = nullptr;
    std::ptrdiff_t index_ = 0;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = std::conditional_t<Constant, const T &, T &>;
    using pointer = std::conditional_t<Constant, const T *, T *>;
    Iterator() = default;
    Iterator(Owner *owner, std::ptrdiff_t index) : owner_(owner), index_(index) {}
    reference operator*() const { return (*owner_)[static_cast<std::size_t>(index_)]; }
    pointer operator->() const { return &**this; }
    reference operator[](difference_type n) const { return *(*this + n); }
    Iterator &operator++() {
      ++index_;
      return *this;
    }
    Iterator operator++(int) {
      auto old = *this;
      ++*this;
      return old;
    }
    Iterator &operator--() {
      --index_;
      return *this;
    }
    Iterator operator--(int) {
      auto old = *this;
      --*this;
      return old;
    }
    Iterator &operator+=(difference_type n) {
      index_ += n;
      return *this;
    }
    Iterator &operator-=(difference_type n) {
      index_ -= n;
      return *this;
    }
    friend Iterator operator+(Iterator it, difference_type n) { return it += n; }
    friend Iterator operator+(difference_type n, Iterator it) { return it += n; }
    friend Iterator operator-(Iterator it, difference_type n) { return it -= n; }
    friend difference_type operator-(Iterator a, Iterator b) {
      return a.index_ - b.index_;
    }
    bool operator==(const Iterator &) const = default;
    auto operator<=>(const Iterator &other) const { return index_ <=> other.index_; }
  };
  auto begin() { return Iterator<false>{this, 0}; }
  auto end() { return Iterator<false>{this, static_cast<std::ptrdiff_t>(size_)}; }
  auto begin() const { return Iterator<true>{this, 0}; }
  auto end() const { return Iterator<true>{this, static_cast<std::ptrdiff_t>(size_)}; }
  auto rbegin() { return std::reverse_iterator{end()}; }
  auto rend() { return std::reverse_iterator{begin()}; }
  auto rbegin() const { return std::reverse_iterator{end()}; }
  auto rend() const { return std::reverse_iterator{begin()}; }
};
} // namespace blackbird
