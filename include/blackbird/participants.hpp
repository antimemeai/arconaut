#pragma once
#include "blackbird/colleague.hpp"
#include <memory>
#include <vector>
namespace blackbird {
using ParticipantTransport = std::function<Value(
    const Value &, const ColleagueCapture &, const std::function<bool()> &)>;
// One owner calls controls/drain. Workers only touch their guarded state and
// bounded capture queue; admission is retained by the owner before launch/send.
class Participants {
public:
  using Retain = std::function<void(std::string_view, std::string_view, const Value &)>;
  using Save = std::function<void(const Value &)>;
  using Publish = std::function<void(const Value &)>;
  Participants(const Value &saved, Retain retain, Save save, Publish publish = {});
  ~Participants();
  Participants(const Participants &) = delete;
  Participants &operator=(const Participants &) = delete;
  Value configure(const Value &arguments);
  Value start(std::string id, const Value &arguments,
              const ParticipantTransport &transport);
  Value read(const Value &arguments = Value::object({}));
  Value send(const Value &arguments);
  Value cancel(const Value &arguments);
  Value await(const Value &arguments, const std::function<bool()> &cancelled = {});
  Value join(const Value &arguments, const std::function<bool()> &cancelled = {});
  Value archive(const Value &arguments);
  void drain();
  bool active() const;
  void shutdown();

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
} // namespace blackbird
