#include "../src/stream_capture.hpp"
#include <iostream>
#include <stdexcept>
using blackbird::StreamCapture;
void check(bool ok) {
  if (!ok)
    throw std::runtime_error("capture check failed");
}
int main() {
  std::string output;
  std::size_t writes = 0;
  StreamCapture capture{[&](std::string_view block) {
    check(block.size() <= StreamCapture::block_bytes);
    output += block;
    ++writes;
  }};
  // Exactly the same delivered bytes independent of transport fragmentation.
  const std::string expected(100000, 'x');
  for (char byte : expected) {
    capture.append(std::string_view{&byte, 1});
    check(capture.pending_bytes() < StreamCapture::block_bytes);
  }
  check(writes == 1);
  capture.flush();
  check(writes == 2 && output == expected);
  capture.flush();
  check(writes == 2);
  capture.append(std::string(3 * StreamCapture::block_bytes + 7, 'y'));
  capture.flush();
  check(writes == 6);
  // Cancellation/error boundary flushes the partial block explicitly.
  capture.append("partial");
  try {
    throw std::runtime_error("cancel");
  } catch (...) {
    capture.flush();
  }
  check(output.ends_with("partial"));
  std::size_t attempts = 0;
  StreamCapture failing{[&](std::string_view) {
    ++attempts;
    throw std::runtime_error("uncertain write");
  }};
  bool failed = false;
  try {
    failing.append(std::string(StreamCapture::block_bytes, 'z'));
  } catch (...) {
    failed = true;
  }
  failing.flush(); // Must not repeat the write.
  check(failed && attempts == 1);
  try {
    failing.append("more");
    check(false);
  } catch (const std::logic_error &) {
  }
  check(attempts == 1);
  // Destruction does not perform I/O; crash-like loss is permitted for pending
  // diagnostics.
  {
    StreamCapture abandoned{[&](std::string_view) { check(false); }};
    abandoned.append("discardable diagnostic tail");
  }
  std::cout << "100000 one-byte callbacks -> 2 bounded capture writes; sink failure "
               "not replayed\n";
}
