#pragma once
#include "blackbird/colleague.hpp"
#include <memory>
#include <vector>
namespace blackbird {
using ParticipantTransport = std::function<Json(const Json &, const ColleagueCapture &,
                                                const std::function<bool()> &)>;
// One owner calls controls/drain. Workers only touch their guarded state and
// bounded capture queue; admission is retained by the owner before launch/send.
class Participants {
public:
  using Retain = std::function<void(std::string_view, std::string_view, const Json &)>;
  using Save = std::function<void(const Json &)>;
  using Publish = std::function<void(const Json &)>;
  Participants(const Json &saved, Retain retain, Save save, Publish publish = {});
  ~Participants();
  Participants(const Participants &) = delete;
  Participants &operator=(const Participants &) = delete;
  Json configure(const Json &arguments);
  Json start(std::string id, const Json &arguments,
             const ParticipantTransport &transport);
  Json read(const Json &arguments = Json::object({}));
  Json send(const Json &arguments);
  Json cancel(const Json &arguments);
  Json await(const Json &arguments, const std::function<bool()> &cancelled = {});
  Json join(const Json &arguments, const std::function<bool()> &cancelled = {});
  Json archive(const Json &arguments);
  void drain();
  bool active() const;
  void shutdown();

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
} // namespace blackbird
