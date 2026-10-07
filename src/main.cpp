#include "arconaut/coding.hpp"
#include "arconaut/backstop.hpp"
#include "arconaut/terminal.hpp"
#include <atomic>
#include <csignal>
#include <fstream>
#include <iostream>
#include <map>
#include <sys/random.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace arconaut;
namespace {
std::atomic_bool interrupted{false};
static_assert(std::atomic_bool::is_always_lock_free);
extern "C" void interrupt_handler(int) {
  interrupted.store(true, std::memory_order_relaxed);
}
template <class T> T random_id() {
  IdentityBytes bytes{};
  if (getentropy(bytes.data(), bytes.size()) != 0)
    throw Error{ErrorCode::io, errno};
  return unwrap(T::from_bytes(bytes));
}
std::string safe(std::string_view text) {
  std::string out;
  constexpr char hex[] = "0123456789abcdef";
  for (std::size_t i = 0; i < text.size(); ++i) {
    const auto c = static_cast<unsigned char>(text[i]);
    const bool c1 = c == 0xc2 && i + 1 < text.size() &&
                    static_cast<unsigned char>(text[i + 1]) >= 0x80 &&
                    static_cast<unsigned char>(text[i + 1]) <= 0x9f;
    if ((c < 32 && c != '\n' && c != '\t') || c == 127 || c1) {
      out += "\\x";
      out += hex[c >> 4U];
      out += hex[c & 15U];
      if (c1) {
        const auto next = static_cast<unsigned char>(text[++i]);
        out += "\\x";
        out += hex[next >> 4U];
        out += hex[next & 15U];
      }
    } else
      out += text[i];
  }
  return out;
}
} // namespace
int main(int argc, char **argv) {
  try {
    const char *home = std::getenv("HOME");
    if (home == nullptr)
      throw Error{ErrorCode::invalid_range};
    std::filesystem::path session =
        std::filesystem::path{home} / ".local/state/arconaut/default";
    std::filesystem::path workflow = ARCONAUT_WORKFLOW;
    std::string model = "gpt-6.1-sol", once, seed_path, backstop_path;
    Json backstop_mission;
    bool one = false, inspect = false, plain = false;
    std::string effort = "medium";
    bool model_option = false, effort_option = false, workflow_option = false;
    bool resume_requested = false, discovery = false;
    auto discovery_root = session.parent_path();
    for (int i = 1; i < argc; ++i) {
      const std::string_view arg = argv[i];
      if (arg == "--help") {
        std::cout
            << "arco [--session DIRECTORY] [--model NAME] [--workflow FILE] [--once "
               "PROMPT] [--audit-last] [--effort low|medium|high|xhigh] "
               "[--plain] [--list-sessions [ROOT]] [--seed-session JSON] "
               "[--backstop MISSION_JSON (requires --once)] "
               "[--resume-continue|--resume-once]\nInteractive: /context, "
               "/originals, /restore "
               "ENTRY, /lua CODE, /model NAME, /effort LEVEL, /workflow FILE, "
               "/session, "
               "/restart NOTE, /paste (until /send), /quit or /exit\n";
        return 0;
      }
      if (arg == "--list-sessions") {
        discovery = true;
        if (i + 1 < argc && !std::string_view{argv[i + 1]}.starts_with("--"))
          discovery_root = argv[++i];
        continue;
      }
      if (arg == "--resume-continue" || arg == "--resume-once") {
        resume_requested = true;
        if (arg == "--resume-once")
          one = true;
        continue;
      }
      if (arg == "--plain") {
        plain = true;
        continue;
      }
      if (arg == "--audit-last") {
        inspect = true;
        continue;
      }
      if (i + 1 == argc)
        throw Error{ErrorCode::invalid_range};
      if (arg == "--backstop")
        backstop_path = argv[++i];
      else if (arg == "--seed-session")
        seed_path = argv[++i];
      else if (arg == "--session")
        session = argv[++i];
      else if (arg == "--effort") {
        effort = argv[++i];
        effort_option = true;
      } else if (arg == "--workflow") {
        workflow = argv[++i];
        workflow_option = true;
      } else if (arg == "--model") {
        model = argv[++i];
        model_option = true;
      } else if (arg == "--once") {
        once = argv[++i];
        one = true;
      } else
        throw Error{ErrorCode::invalid_range};
    }
    if (!backstop_path.empty()) {
      if (!one || resume_requested || inspect || discovery || !seed_path.empty() ||
          once.empty() || once.starts_with("/"))
        throw Error{ErrorCode::conflict};
      backstop_mission = unwrap(parse_json(read_file(backstop_path, 32768)));
      if (string_field(backstop_mission, "mission").empty())
        throw Error{ErrorCode::invalid_range};
    }
    if (discovery && !seed_path.empty())
      throw Error{ErrorCode::conflict};
    if (discovery) {
      std::cout << unwrap(dump_json(list_sessions(discovery_root))) << '\n';
      return 0;
    }
    Json seed;
    if (!seed_path.empty()) {
      if (inspect || discovery || resume_requested ||
          std::filesystem::exists(session / "audit"))
        throw Error{ErrorCode::conflict};
      seed = unwrap(parse_json(read_file(seed_path, 1024 * 1024)));
      ContextStore::validate_successor_seed(seed);
    }
    std::filesystem::create_directories(session);
    if (::chmod(session.c_str(), 0700) != 0)
      throw Error{ErrorCode::io, errno};
    const auto path = session / "audit";
    const JournalCapacity capacity{512ULL * 1024 * 1024, 200000};
    std::unique_ptr<RetainedState> root;
    auto directory = std::make_unique<NativeJournalDirectory>(
        unwrap(NativeJournalDirectory::open(session.string())));
    if (std::filesystem::exists(path)) {
      if (!seed_path.empty())
        throw Error{ErrorCode::conflict};
      std::array<std::byte, journal_header_size> bytes{};
      std::ifstream file{path, std::ios::binary};
      file.read(reinterpret_cast<char *>(bytes.data()),
                static_cast<std::streamsize>(bytes.size()));
      if (!file)
        throw Error{ErrorCode::incomplete};
      const auto header = unwrap(decode_journal_header(bytes));
      root =
          unwrap(RetainedState::open(std::move(directory), "audit", header, capacity));
      unwrap(root->confirm_recovery());
      if (!inspect) {
        recover_coding_session(*root);
      }
    } else {
      std::uint64_t name_space = 0;
      if (getentropy(&name_space, sizeof(name_space)) != 0 || name_space == 0)
        throw Error{ErrorCode::io};
      JournalHeader header{random_id<EnvironmentId>(),
                           random_id<AuditStreamId>(),
                           name_space,
                           {32 * 1024 * 1024, 96 * 1024 * 1024},
                           std::nullopt};
      root = unwrap(
          RetainedState::create(std::move(directory), "audit", header, capacity));
    }
    if (inspect) {
      const auto usage = root->journal_usage();
      const auto remaining = usage.remaining_bytes();
      std::cout << "Audit observed headroom: "
                << (remaining ? std::to_string(*remaining) + " bytes"
                              : "bytes unavailable")
                << ", " << usage.remaining_records()
                << " indexed records; no admission permission or handoff reserve\n";
      for (const auto &fact : root->committed_facts()) {
        const auto *admission = std::get_if<AttemptAdmissionEvent>(&fact.event.body);
        if (!admission ||
            !unwrap(root->attempt(admission->attempt)).reconciliation_required)
          continue;
        std::string operation = "unclassified";
        for (const auto &related : root->committed_facts()) {
          const auto *decision = std::get_if<DecisionEvent>(&related.event.body);
          if (!decision || decision->decision != admission->decision)
            continue;
          const auto metadata = parse_json(std::string_view{
              reinterpret_cast<const char *>(decision->continuation.data()),
              decision->continuation.size()});
          if (metadata.has_value())
            if (const auto *name = metadata.value().find("operation");
                name && std::holds_alternative<std::string>(name->value()))
              operation = name->string();
        }
        std::cout << "Unfinished " << safe(operation) << " attempt "
                  << hex_identity(admission->attempt.bytes()) << " (outcome unknown)\n";
      }
      std::size_t shown = 0;
      const auto facts = root->committed_facts();
      for (std::size_t i = facts.size(); i > 0 && shown < 12; --i) {
        const auto &fact = facts[i - 1];
        const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
        if (record == nullptr || record->channel != ApplicationChannel::log ||
            fact.event.dependencies.empty())
          continue;
        const std::string_view meta{
            reinterpret_cast<const char *>(record->payload.data()),
            record->payload.size()};
        std::cout << safe(meta) << '\n';
        for (const auto reference : fact.event.dependencies) {
          const auto bytes = unwrap(root->source(reference));
          const std::string_view data{reinterpret_cast<const char *>(bytes.data()),
                                      bytes.size()};
          std::cout << safe(data) << '\n';
        }
        ++shown;
      }
      return 0;
    }
    AuditLog log{*root};
    ContextStore context{log};
    if (!seed_path.empty())
      context.seed_successor(seed);
    SessionStore session_store{log};
    auto settings = session_store.settings();
    if (model_option)
      settings.model = model;
    if (effort_option)
      settings.effort = effort;
    if (workflow_option || settings.workflow.empty())
      settings.workflow =
          std::filesystem::absolute(workflow).lexically_normal().string();
    (void)read_file(settings.workflow);
    session_store.save(settings);
    model = settings.model;
    effort = settings.effort;
    workflow = settings.workflow;
    bool resume_turn = resume_requested && session_store.resume(context);
    if (resume_requested && !resume_turn && one)
      return 0;
    std::atomic_bool restart_pending{false};
    OpenAiCodingProvider provider;
    CodingEngine engine{log, context, provider, model};
    engine.effort(effort);
    struct sigaction action{};
    action.sa_handler = interrupt_handler;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, nullptr) != 0)
      throw Error{ErrorCode::io, errno};
    std::atomic_bool cancelled{false};
    engine.cancelled = [&] {
      return interrupted.load(std::memory_order_relaxed) || cancelled.load();
    };
    TerminalUI ui{"Arconaut · " + model + " · " + effort + " · " + session.string()};
    const bool tui = !plain && !one && isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
    auto emit = [&](std::string_view text) {
      if (tui)
        ui.text(text);
      else
        std::cout << safe(text) << std::flush;
    };
    auto refresh_info = [&] {
      try {
        save_session_info(session, session_store.settings(), session_identity(log));
      } catch (...) {
        emit("Session discovery snapshot save failed; audit settings remain "
             "authoritative.\n");
      }
    };
    refresh_info();
    engine.display = emit;
    engine.process_output = emit;
    engine.status = [&](std::string_view text) {
      if (tui) {
        ui.status(text);
        if (text != "provider")
          ui.text("\n[" + std::string{text} + "]\n");
      } else
        std::cerr << '[' << safe(text) << "]\n";
    };
    engine.operation_completed = [&](std::string_view text) {
      if (tui)
        ui.operation_completed(text);
      else
        std::cerr << '[' << safe(text) << "]\n";
    };
    auto perform = [&](std::string_view prompt) {
      interrupted.store(false, std::memory_order_relaxed);
      try {
        if (prompt == "/help") {
          emit("/model NAME · /effort low|medium|high|xhigh · /context · /stats · "
               "/originals\n"
               "/compact JSON · /inspect JSON · /restore ENTRY · /lua CODE · /workflow "
               "FILE · /session · /restart NOTE\n"
               "/clear · /quit or /exit\n"
               "Paste multiline text; Enter submits, Alt-Enter adds a line.\n"
               "Type during work to queue another prompt. Ctrl-C stops and clears the "
               "queue.\n");
        } else if (prompt == "continue" && resume_turn) {
          resume_turn = false;
          engine.turn({"", read_file(workflow)});
        } else if (prompt == "/session") {
          const auto identity = session_identity(log);
          emit(unwrap(dump_json(Json::object(
                   {{"actor", Json{hex_identity(identity.actor.bytes())}},
                    {"conversation", Json{hex_identity(identity.conversation.bytes())}},
                    {"workflow_id", Json{hex_identity(identity.workflow.bytes())}},
                    {"model", Json{model}},
                    {"effort", Json{effort}},
                    {"workflow", Json{workflow.string()}}}))) +
               "\n");
        } else if (prompt.starts_with("/restart ")) {
          engine.validate_restart();
          session_store.restart(prompt.substr(9));
          restart_pending.store(true);
        } else if (prompt.starts_with("/workflow ")) {
          auto candidate = session_store.settings();
          candidate.workflow = std::filesystem::absolute(std::string{prompt.substr(10)})
                                   .lexically_normal()
                                   .string();
          (void)read_file(candidate.workflow);
          session_store.save(candidate);
          workflow = candidate.workflow;
        } else if (prompt == "/context")
          emit(unwrap(dump_json(context.view())) + "\n");
        else if (prompt == "/sessions")
          emit(unwrap(dump_json(list_sessions(session.parent_path()))) + "\n");
        else if (prompt == "/stats")
          emit(unwrap(dump_json(engine.stats())) + "\n");
        else if (prompt.starts_with("/compact "))
          emit(unwrap(dump_json(context.manage(unwrap(parse_json(prompt.substr(9)))))) +
               "\n");
        else if (prompt.starts_with("/inspect "))
          emit(
              unwrap(dump_json(context.inspect(unwrap(parse_json(prompt.substr(9)))))) +
              "\n");
        else if (prompt == "/originals")

          emit(unwrap(dump_json(context.originals())) + "\n");
        else if (prompt.starts_with("/restore ")) {
          context.restore(prompt.substr(9));
          emit("Original restored.\n");
        } else if (prompt.starts_with("/model ")) {
          auto candidate = session_store.settings();
          candidate.model = prompt.substr(7);
          session_store.save(candidate);
          engine.model(candidate.model);
          model = candidate.model;
        } else if (prompt.starts_with("/effort ")) {
          auto value = std::string{prompt.substr(8)};
          auto candidate = session_store.settings();
          candidate.effort = value;
          session_store.save(candidate);
          engine.effort(value);
          effort = std::move(value);
        } else if (prompt.starts_with("/lua "))
          engine.turn({"", prompt.substr(5)});
        else if (prompt.starts_with("/"))
          emit("Unknown command: " + std::string{prompt} +
               ". /help lists commands; /quit or /exit closes Arco.\n");
        else if (!prompt.empty())
          engine.turn({prompt, read_file(workflow)});
        if (prompt != "/sessions")
          refresh_info();
        if (auto note = engine.take_restart_note(); note && !restart_pending.load()) {
          engine.validate_restart();
          session_store.restart(*note);
          restart_pending.store(true);
        }
        if (tui)
          ui.title("Arconaut · " + model + " · " + effort + " · " + session.string());
      } catch (const Error &e) {
        if (tui)
          ui.failed();
        emit("\n" + std::string{error_name(e.code)} + " (" + std::to_string(e.detail) +
             "); context/audit retained\n");
        if (root->state() != JournalWriterState::live)
          throw;
        if (!backstop_path.empty() && e.code != ErrorCode::interrupted &&
            !engine.cancelled()) {
          const auto outcome = run_backstop(engine, log, session, backstop_mission,
                                           provider, session_store.settings(), engine.cancelled, emit);
          emit("\nBackstop: " + unwrap(dump_json(outcome)) + "\n");
          if (string_field(outcome, "phase") == "useful-work-observed")
            return;
          throw;
        }
        if (one)
          throw;
      }
      if (interrupted.load(std::memory_order_relaxed))
        cancelled.store(true);
    };
    if (one) {
      perform(resume_turn ? "continue" : once);
      std::cout << '\n';
      return restart_pending.load() ? 75 : 0;
    }
    if (tui) {
      ui.text("/help lists commands. Workflow reloads each turn.\n");
      for (const auto &item : context.items()) {
        const auto *role = item.find("role");
        if (!role || !std::holds_alternative<std::string>(role->value()))
          continue;
        const auto *content = item.find("content");
        if (!content)
          continue;
        std::string text;
        if (const auto *value = std::get_if<std::string>(&content->value()))
          text = *value;
        else if (const auto *parts = std::get_if<Json::Array>(&content->value()))
          for (const auto &part : *parts)
            if (const auto *part_text = part.find("text");
                part_text && std::holds_alternative<std::string>(part_text->value()))
              text += part_text->string();
        ui.text("\n" + role->string() + "\n" + text + "\n");
      }
      ui.run(
          perform, cancelled, [&] { return restart_pending.load(); },
          resume_turn ? "continue" : "", session / "ui-state.json");
      return restart_pending.load() ? 75 : 0;
    }
    std::cout << "Arconaut · " << model << " · " << effort << " · " << session.string()
              << "\n/help lists commands; /quit or /exit exits.\n";
    if (resume_turn)
      perform("continue");
    if (restart_pending.load())
      return 75;
    std::string line;
    while (std::cout << "\narco> " && std::getline(std::cin, line)) {
      if (line == "/quit" || line == "/exit")
        break;
      if (line == "/paste") {
        line.clear();
        std::string part;
        while (std::getline(std::cin, part) && part != "/send") {
          line += part;
          line += '\n';
        }
      }
      cancelled.store(false);
      perform(line);
      if (restart_pending.load())
        break;
    }
    return restart_pending.load() ? 75 : 0;

  } catch (const Error &e) {
    std::cerr << "arco: " << error_name(e.code) << " (" << e.detail
              << "). Inspect --audit-last; use a fresh --session for unresolved prior "
                 "work.\n";
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "arco: " << safe(e.what()) << '\n';
    return 1;
  }
}
