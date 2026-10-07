#include "blackbird/context.hpp"
#include "blackbird/local_timing.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
using namespace blackbird;
template <class T> T id(unsigned char n) {
  IdentityBytes b{};
  b[0] = std::byte{n};
  return unwrap(T::from_bytes(b));
}
int main(int argc, char **argv) {
  if (argc != 5)
    return 2;
  const auto history = std::stoul(argv[1]);
  const auto samples = std::stoul(argv[2]);
  const bool enabled = std::string_view(argv[3]) == "on";
  const std::string prefix = argv[4];
  char temp[] = "/tmp/p1-history-XXXXXX";
  if (!mkdtemp(temp))
    return 3;
  try {
    JournalHeader h{id<EnvironmentId>(1),
                    id<AuditStreamId>(2),
                    3,
                    {1024 * 1024, 4 * 1024 * 1024},
                    std::nullopt};
    auto root =
        unwrap(RetainedState::create(std::make_unique<NativeJournalDirectory>(
                                         unwrap(NativeJournalDirectory::open(temp))),
                                     "audit", h, {256 * 1024 * 1024, 100000}));
    AuditLog log{*root};
    ContextStore context{log};
    Json::Array items;
    const auto context_entries = 64 + history;
    for (unsigned long i = 0; i < context_entries; ++i)
      items.push_back(Json::object(
          {{"role", Json{"user"}}, {"content", Json{std::string(128, 'x')}}}));
    context.append(std::move(items), "fixture");
    const auto packet = Json::object({{"fixture", Json{std::string(128, 'x')}}});
    for (unsigned i = 0; i < history; ++i)
      log.record(ApplicationChannel::log, packet);
    auto sink = std::make_unique<LocalTimingSink>();
    if (enabled)
      local_timing_sink = sink.get();
    std::ofstream output(prefix + ".samples.jsonl");
    if (!output)
      throw std::runtime_error("sample output open failed");
    const auto valid = [](LocalTick a, LocalTick b) {
      if (!a.valid || !b.valid || b.wall < a.wall || b.cpu < a.cpu)
        throw std::runtime_error("invalid benchmark clocks");
    };
    const auto initial = root->journal_usage();
    for (unsigned i = 0; i < samples; ++i) {
      const auto before = root->journal_usage();
      const auto begin = local_tick();
      log.record(ApplicationChannel::log, packet);
      const auto end = local_tick();
      valid(begin, end);
      const auto after = root->journal_usage();
      output << "{\"action\":\"append\",\"history\":" << history
             << ",\"records\":" << before.indexed_records
             << ",\"record_delta\":" << after.indexed_records - before.indexed_records
             << ",\"journal_bytes\":" << before.prefix.end_offset
             << ",\"byte_delta\":" << after.prefix.end_offset - before.prefix.end_offset
             << ",\"wall_ns\":" << end.wall - begin.wall
             << ",\"cpu_ns\":" << end.cpu - begin.cpu << "}\n";
      const auto pbegin = local_tick();
      std::size_t bytes;
      {
        LocalSpan prepare{"fixture.prepare_subset", "ContextStore::items + dump_json"};
        auto request = Json::object(
            {{"model", Json{"fixture"}}, {"input", Json{context.items()}}});
        bytes = unwrap(dump_json(request)).size();
        prepare.observe(after.indexed_records, bytes);
        prepare.linkage(context.head());
        prepare.outcome("success");
      }
      const auto pend = local_tick();
      valid(pbegin, pend);
      output << "{\"action\":\"prepare_subset\",\"history\":" << history
             << ",\"records\":" << after.indexed_records
             << ",\"context_entries\":" << context_entries
             << ",\"request_bytes\":" << bytes
             << ",\"wall_ns\":" << pend.wall - pbegin.wall
             << ",\"cpu_ns\":" << pend.cpu - pbegin.cpu << "}\n";
    }
    // Explicit primitive overhead, same clocks and iterations in both modes.
    const auto b = local_tick();
    for (unsigned i = 0; i < 2000; ++i) {
      LocalSpan span{"primitive", "fixture"};
      span.outcome("success");
    }
    const auto e = local_tick();
    valid(b, e);
    output << "{\"action\":\"primitive_2000\",\"history\":" << history
           << ",\"wall_ns\":" << e.wall - b.wall << ",\"cpu_ns\":" << e.cpu - b.cpu
           << ",\"initial_records\":" << initial.indexed_records << "}\n";
    local_timing_sink = nullptr;
    if (enabled && !sink->persist(prefix + ".telemetry.jsonl"))
      return 4;
    output.close();
    if (!output)
      throw std::runtime_error("sample output write failed");
    root.reset();
    std::filesystem::remove_all(temp);
    return 0;
  } catch (...) {
    std::filesystem::remove_all(temp);
    return 5;
  }
}
