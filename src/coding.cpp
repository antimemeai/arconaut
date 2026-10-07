#include "arconaut/coding.hpp"
extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#include <algorithm>
#include <cerrno>
#include <charconv>
#include <chrono>
#include <cstring>
#include <set>
#include <thread>

namespace arconaut {
namespace {
enum class JsonTag { array, object, number, null };
char markers[4]; // array, object, exact number, null (registry keys, no string
                 // allocation).
void tag(lua_State *L, JsonTag kind) {
  lua_rawgetp(L, LUA_REGISTRYINDEX, &markers[static_cast<int>(kind)]);
  lua_setmetatable(L, -2);
}
bool tagged(lua_State *L, int index, JsonTag kind) {
  if (!lua_getmetatable(L, index))
    return false;
  lua_rawgetp(L, LUA_REGISTRYINDEX, &markers[static_cast<int>(kind)]);
  const bool equal = lua_rawequal(L, -1, -2) != 0;
  lua_pop(L, 2);
  return equal;
}
void push_json(lua_State *L, const Json &j) {
  std::visit(
      [&](const auto &v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
          lua_newtable(L);
          tag(L, JsonTag::null);
        } else if constexpr (std::is_same_v<T, bool>)
          lua_pushboolean(L, v);
        else if constexpr (std::is_same_v<T, std::string>)
          lua_pushlstring(L, v.data(), v.size());
        else if constexpr (std::is_same_v<T, JsonNumber>) {
          lua_createtable(L, 1, 0);
          lua_pushlstring(L, v.text.data(), v.text.size());
          lua_rawseti(L, -2, 1);
          tag(L, JsonTag::number);
        } else if constexpr (std::is_same_v<T, Json::Array>) {
          lua_newtable(L);
          for (std::size_t i = 0; i < v.size(); ++i) {
            push_json(L, v[i]);
            lua_rawseti(L, -2, static_cast<lua_Integer>(i + 1));
          }
          tag(L, JsonTag::array);
        } else {
          lua_newtable(L);
          for (const auto &[key, value] : v) {
            lua_pushlstring(L, key.data(), key.size());
            push_json(L, value);
            lua_rawset(L, -3);
          }
          tag(L, JsonTag::object);
        }
      },
      j.value());
}
std::string lua_string(lua_State *L, int index) {
  if (lua_type(L, index) != LUA_TSTRING)
    throw Error{ErrorCode::invalid_range};
  std::size_t length = 0;
  const char *value = lua_tolstring(L, index, &length);
  return {value, length};
}
// Caller reserves the full traversal stack first. Only raw, nonallocating Lua
// reads occur while C++ owners exist; no metamethods/string-key interning here.
struct JsonRead {
  std::size_t depth;
  std::size_t *nodes;
};
Json read_json(lua_State *L, int index, JsonRead budget) {
  const auto depth = budget.depth;
  auto &nodes = *budget.nodes;
  if (depth > 128 || ++nodes > 500000)
    throw Error{ErrorCode::capacity};
  index = lua_absindex(L, index);
  switch (lua_type(L, index)) {
  case LUA_TNIL:
    return Json{};
  case LUA_TBOOLEAN:
    return Json{lua_toboolean(L, index) != 0};
  case LUA_TSTRING:
    return Json{lua_string(L, index)};
  case LUA_TNUMBER: {
    char buffer[128];
    std::to_chars_result result;
    if (lua_isinteger(L, index))
      result = std::to_chars(buffer, buffer + sizeof(buffer), lua_tointeger(L, index));
    else
      result = std::to_chars(buffer, buffer + sizeof(buffer), lua_tonumber(L, index));
    if (result.ec != std::errc{})
      throw Error{ErrorCode::invalid_range};
    return Json{JsonNumber{std::string{buffer, result.ptr}}};
  }
  case LUA_TTABLE: {
    if (tagged(L, index, JsonTag::null))
      return Json{};
    if (tagged(L, index, JsonTag::number)) {
      lua_rawgeti(L, index, 1);
      auto number = lua_string(L, -1);
      lua_pop(L, 1);
      return Json{JsonNumber{std::move(number)}};
    }
    const auto length = lua_rawlen(L, index);
    const bool array = tagged(L, index, JsonTag::array) ||
                       (!tagged(L, index, JsonTag::object) && length != 0);
    if (array) {
      std::size_t count = 0;
      lua_pushnil(L);
      while (lua_next(L, index)) {
        if (!lua_isinteger(L, -2) || lua_tointeger(L, -2) < 1 ||
            static_cast<unsigned long long>(lua_tointeger(L, -2)) > length)
          throw Error{ErrorCode::invalid_range};
        ++count;
        lua_pop(L, 1);
      }
      if (count != length)
        throw Error{ErrorCode::invalid_range};
      Json::Array values;
      values.reserve(length);
      for (std::size_t i = 0; i < length; ++i) {
        lua_rawgeti(L, index, static_cast<lua_Integer>(i + 1));
        values.push_back(read_json(L, -1, {depth + 1, &nodes}));
        lua_pop(L, 1);
      }
      return Json{std::move(values)};
    }
    Json::Object fields;
    lua_pushnil(L);
    while (lua_next(L, index)) {
      auto key = lua_string(L, -2);
      auto value = read_json(L, -1, {depth + 1, &nodes});
      fields.emplace_back(std::move(key), std::move(value));
      lua_pop(L, 1);
    }
    std::sort(fields.begin(), fields.end(),
              [](const auto &a, const auto &b) { return a.first < b.first; });
    return Json::object(std::move(fields));
  }
  default:
    throw Error{ErrorCode::unsupported};
  }
}
Json error_json(Error e) {
  return Json::object({{"error", Json{error_name(e.code)}},
                       {"detail", Json{JsonNumber{std::to_string(e.detail)}}}});
}
template <class T> T parse_id(std::string_view s) {
  if (s.size() != 32)
    throw Error{ErrorCode::invalid_identity};
  IdentityBytes out{};
  for (std::size_t i = 0; i < 16; ++i) {
    unsigned n = 0;
    const auto r = std::from_chars(s.data() + 2 * i, s.data() + 2 * i + 2, n, 16);
    if (r.ec != std::errc{} || r.ptr != s.data() + 2 * i + 2)
      throw Error{ErrorCode::invalid_identity};
    out[i] = static_cast<std::byte>(n);
  }
  return unwrap(T::from_bytes(out));
}
void validate_protocol(const Json::Array &items) {
  std::set<std::string> seen, pending;
  for (const auto &item : items) {
    const auto *type = item.find("type");
    if (type == nullptr)
      continue;
    const auto &s = type->string();
    if (s == "function_call") {
      const auto &call = string_field(item, "call_id");
      if (!seen.insert(call).second)
        throw Error{ErrorCode::conflict};
      pending.insert(call);
    }
    if (s == "function_call_output") {
      if (pending.erase(string_field(item, "call_id")) != 1)
        throw Error{ErrorCode::conflict};
    }
  }
  if (!pending.empty())
    throw Error{ErrorCode::conflict};
}
} // namespace
Json OpenAiCodingProvider::respond(
    const Json &request, const std::function<void(std::string_view)> &capture) {
  auto config = config_;
  config.response_observer = capture;
  config.cancelled = cancelled;
  auto login = unwrap(codex_login(config));
  auto raw = unwrap(openai_http(config, login, "responses", &request));
  return unwrap(completed_response(raw));
}
struct CodingEngine::Runtime {
  CodingEngine &engine;
  lua_State *state = nullptr;
  Json scratch;
  std::string error;
  std::optional<Error> failure;
  std::array<char, 512> bridge_error{};
  void failed(Error why, const char *message) noexcept {
    failure = why;
    const auto count = std::min(std::strlen(message), bridge_error.size() - 1);
    std::memcpy(bridge_error.data(), message, count);
    bridge_error[count] = '\0';
  }
  explicit Runtime(CodingEngine &e) : engine(e) {
    state = luaL_newstate();
    if (state == nullptr)
      throw Error{ErrorCode::allocation};
    *static_cast<Runtime **>(lua_getextraspace(state)) = this;
    lua_sethook(state, interrupt_hook, LUA_MASKCOUNT, 1000);
    lua_pushcfunction(state, install);
    lua_pushlightuserdata(state, this);
    if (lua_pcall(state, 1, 0, 0) != LUA_OK) {
      lua_close(state);
      state = nullptr;
      throw Error{ErrorCode::allocation};
    }
  }
  ~Runtime() {
    if (state != nullptr)
      lua_close(state);
  }
  enum Op {
    request_op,
    call_op,
    context_op,
    edit_op,
    originals_op,
    restore_op,
    append_op,
    decode_op,
    encode_op,
    display_op,
    array_op,
    present_op,
    restarting_op,
    stats_op,
    manage_op,
    inspect_op
  };
  static int install(lua_State *L) {
    auto *self = static_cast<Runtime *>(lua_touserdata(L, 1));
    // Host effects go through the owned audit boundary, including module reads.
    const luaL_Reg libraries[] = {{LUA_GNAME, luaopen_base},
                                  {LUA_COLIBNAME, luaopen_coroutine},
                                  {LUA_TABLIBNAME, luaopen_table},
                                  {LUA_STRLIBNAME, luaopen_string},
                                  {LUA_MATHLIBNAME, luaopen_math},
                                  {LUA_UTF8LIBNAME, luaopen_utf8},
                                  {nullptr, nullptr}};
    for (const auto *library = libraries; library->name != nullptr; ++library) {
      luaL_requiref(L, library->name, library->func, 1);
      lua_pop(L, 1);
    }
    lua_pushnil(L);
    lua_setglobal(L, "dofile");
    lua_pushnil(L);
    lua_setglobal(L, "loadfile");
    for (int i = 0; i < 4; ++i) {
      lua_newtable(L);
      lua_rawsetp(L, LUA_REGISTRYINDEX, &markers[i]);
    }
    lua_newtable(L);
    constexpr const char *names[] = {"request",    "call",    "context", "edit",
                                     "originals",  "restore", "append",  "decode",
                                     "encode",     "display", "array",   "present",
                                     "restarting", "stats",   "manage",  "inspect"};
    for (int i = 0; i < 16; ++i) {
      lua_pushlightuserdata(L, self);
      lua_pushinteger(L, i);
      lua_pushcclosure(L, bridge, 2);
      lua_setfield(L, -2, names[i]);
    }
    lua_newtable(L);
    lua_getfield(L, -2, "decode");
    lua_setfield(L, -2, "decode");
    lua_getfield(L, -2, "encode");
    lua_setfield(L, -2, "encode");
    lua_setfield(L, -2, "json");
    lua_setglobal(L, "arco");
    constexpr const char *print_program =
        "function print(...) local v={} for i=1,select('#',...) do "
        "v[i]=tostring(select(i,...)) end "
        "arco.display(table.concat(v,'\\t')..'\\n') end";
    if (luaL_loadstring(L, print_program) != LUA_OK || lua_pcall(L, 0, 0, 0) != LUA_OK)
      return lua_error(L);
    return 0;
  }
  static int bridge(lua_State *L) {
    auto *self = static_cast<Runtime *>(lua_touserdata(L, lua_upvalueindex(1)));
    const auto op = static_cast<Op>(lua_tointeger(L, lua_upvalueindex(2)));
    if (op == array_op) {
      luaL_checktype(L, 1, LUA_TTABLE);
      lua_pushvalue(L, 1);
      tag(L, JsonTag::array);
      return 1;
    }
    if (!lua_checkstack(L, 1024))
      return luaL_error(L, "Lua traversal stack exhausted");
    self->failure.reset();
    bool failed = false;
    {
      try {
        std::size_t nodes = 0;
        switch (op) {
        case stats_op:
          self->scratch = self->engine.stats();
          break;
        case restarting_op:
          self->scratch = Json{self->engine.restart_note_.has_value()};
          break;
        case request_op:
          self->scratch = self->engine.request(
              lua_isnoneornil(L, 1) ? Json::object({}) : read_json(L, 1, {0, &nodes}));
          break;
        case call_op: {
          auto name = lua_string(L, 1);
          auto args = lua_type(L, 2) == LUA_TSTRING
                          ? unwrap(parse_json(lua_string(L, 2)))
                          : read_json(L, 2, {0, &nodes});
          self->scratch = self->engine.call(std::move(name), std::move(args));
          break;
        }
        case manage_op:
          self->scratch = self->engine.context_.manage(read_json(L, 1, {0, &nodes}));
          break;
        case inspect_op:
          self->scratch = self->engine.context_.inspect(read_json(L, 1, {0, &nodes}));
          break;
        case context_op:

          self->scratch = self->engine.context_.view();
          break;
        case edit_op:
          self->scratch = self->engine.context_.edit(read_json(L, 1, {0, &nodes}));
          break;
        case originals_op:
          self->scratch = self->engine.context_.originals();
          break;
        case restore_op:
          self->engine.context_.restore(lua_string(L, 1));
          self->scratch = self->engine.context_.view();
          break;
        case append_op: {
          auto items = read_json(L, 1, {0, &nodes});
          self->engine.context_.append(items.array(), "lua.synthetic");
          self->scratch = self->engine.context_.view();
          break;
        }
        case decode_op:
          self->scratch = unwrap(parse_json(lua_string(L, 1)));
          break;
        case encode_op:
          self->scratch = Json{unwrap(dump_json(read_json(L, 1, {0, &nodes})))};
          break;
        case display_op: {
          auto s = lua_string(L, 1);
          self->engine.log_.original({"display", s, Json{}});
          if (self->engine.display)
            self->engine.display(s);
          self->scratch = Json{};
          break;
        }
        case present_op:
          self->engine.present(read_json(L, 1, {0, &nodes}));
          self->scratch = Json{};
          break;
        case array_op:
          break;
        }
      } catch (const Error &e) {
        self->failed(e, error_name(e.code));
        failed = true;
      } catch (const std::bad_alloc &) {
        self->failed({ErrorCode::allocation}, "allocation");
        failed = true;
      } catch (const std::exception &e) {
        self->failed({ErrorCode::external_unknown}, e.what());
        failed = true;
      }
    }
    // No owning automatic C++ objects live across Lua allocation/error jumps.
    if (failed) {
      lua_pushstring(L, self->bridge_error.data());
      return lua_error(L);
    }
    push_json(L, self->scratch);
    return 1;
  }
  static void interrupt_hook(lua_State *L, lua_Debug *) {
    auto *self = *static_cast<Runtime **>(lua_getextraspace(L));
    if (self->engine.cancelled && self->engine.cancelled()) {
      self->failure = Error{ErrorCode::interrupted};
      luaL_error(L, "Turn interrupted");
    }
  }
  Json eval(std::string_view source) {
    const int base = lua_gettop(state);
    if (luaL_loadbufferx(state, source.data(), source.size(), "arco-program", "t") !=
            LUA_OK ||
        lua_pcall(state, 0, 1, 0) != LUA_OK) {
      const char *message = lua_tostring(state, -1);
      error = message == nullptr ? "Lua error" : message;
      engine.log_.original(
          {"lua.error", error,
           Json::object(
               {{"generation", Json{hex_identity(engine.generation_.bytes())}}})});
      lua_settop(state, base);
      throw failure.value_or(Error{ErrorCode::external_unknown});
    }
    if (!lua_checkstack(state, 1024)) {
      lua_settop(state, base);
      throw Error{ErrorCode::allocation};
    }
    std::size_t nodes = 0;
    try {
      auto result = read_json(state, -1, {0, &nodes});
      lua_settop(state, base);
      return result;
    } catch (...) {
      lua_settop(state, base);
      throw;
    }
  }
};
CodingEngine::CodingEngine(AuditLog &log, ContextStore &context,
                           CodingProvider &provider, std::string model)
    : log_(log), context_(context), provider_(provider), model_(std::move(model)),
      identity_(session_identity(log)),
      generation_(unwrap(log.root().issue<DefinitionGenerationId>())) {
  if (context_.head().empty())
    context_.append({}, "initial");
}
CodingEngine::~CodingEngine() = default;
void recover_coding_session(RetainedState &root) {
  struct ProviderCustody final : CustodyVerifier {
    RetainedState &root;
    std::vector<OperationAttemptId> abandoned;
    explicit ProviderCustody(RetainedState &value) : root(value) {}
    Result<void> verify(std::span<const AttemptState> attempts) override {
      for (const auto &attempt : attempts) {
        const DecisionEvent *decision = nullptr;
        for (const auto &fact : root.committed_facts())
          if (const auto *candidate = std::get_if<DecisionEvent>(&fact.event.body);
              candidate && candidate->decision == attempt.admission.decision)
            decision = candidate;
        if (!decision)
          return Result<void>::failure({ErrorCode::external_unknown});
        const auto metadata = parse_json(std::string_view{
            reinterpret_cast<const char *>(decision->continuation.data()),
            decision->continuation.size()});
        if (!metadata.has_value())
          return Result<void>::failure({ErrorCode::external_unknown});
        const auto *operation = metadata.value().find("operation");
        if (!operation || !std::holds_alternative<std::string>(operation->value()) ||
            operation->string() != "provider")
          return Result<void>::failure({ErrorCode::external_unknown});
        const auto *input = metadata.value().find("input");
        const auto *generation = metadata.value().find("generation");
        const auto *revision = metadata.value().find("revision");
        const auto encoded_input =
            input ? dump_json(*input)
                  : Result<std::string>::failure({ErrorCode::corrupt});
        const std::string_view admitted_input{
            reinterpret_cast<const char *>(attempt.admission.input.data()),
            attempt.admission.input.size()};
        bool linked = false;
        for (const auto &fact : root.committed_facts())
          if (const auto *invocation = std::get_if<InvocationEvent>(&fact.event.body);
              invocation && invocation->invocation == attempt.admission.invocation)
            linked = invocation->decision == decision->decision &&
                     invocation->definition == decision->definition &&
                     invocation->input == attempt.admission.input;
        if (!linked || !encoded_input.has_value() ||
            encoded_input.value() != admitted_input || !generation || !revision ||
            !std::holds_alternative<std::string>(generation->value()) ||
            !std::holds_alternative<std::string>(revision->value()) ||
            generation->string() != hex_identity(decision->definition.bytes()) ||
            revision->string() != hex_identity(decision->context.bytes()))
          return Result<void>::failure({ErrorCode::external_unknown});
        abandoned.push_back(attempt.admission.attempt);
      }
      return Result<void>::success();
    }
  } custody{root};
  unwrap(root.reconcile(custody));
  try {
    std::vector<RetainedEvent> observations;
    for (const auto attempt : custody.abandoned) {
      const std::string message =
          "Provider request abandoned during prior process; outcome unknown. "
          "No response accepted and no request replayed.";
      const auto bytes = std::as_bytes(std::span{message.data(), message.size()});
      observations.push_back({{},
                              AttemptObservationEvent{attempt,
                                                      AttemptPhase::terminal,
                                                      AttemptDisposition::unknown,
                                                      {bytes.begin(), bytes.end()}}});
    }
    if (!observations.empty())
      (void)unwrap(root.append(root.cursor(), {}, observations));
  } catch (...) {
    root.block_admission();
    throw;
  }
}

Json CodingEngine::operation(std::string_view name, const Json &input,
                             const std::function<Json(OperationAttemptId)> &body) {
  if (cancelled && cancelled())
    throw Error{ErrorCode::interrupted};
  const auto began = std::chrono::steady_clock::now();
  if (status) {
    std::string description{name};
    for (const auto key : {"path", "command", "argv"}) {
      const auto *value = input.find(key);
      if (value) {
        description += " · " + unwrap(dump_json(*value));
        break;
      }
    }
    status(description);
  }
  auto &root = log_.root();
  const auto decision = unwrap(root.issue<DecisionId>());
  const auto invocation = unwrap(root.issue<InvocationId>());
  const auto attempt = unwrap(root.issue<OperationAttemptId>());
  const auto serialized = unwrap(dump_json(input));
  const auto raw = std::as_bytes(std::span{serialized.data(), serialized.size()});
  const std::vector<std::byte> bytes{raw.begin(), raw.end()};
  const auto context = parse_id<ContextRevisionId>(context_.head());
  const auto meta =
      Json::object({{"operation", Json{std::string{name}}},
                    {"input", input},
                    {"revision", Json{context_.head()}},
                    {"generation", Json{hex_identity(generation_.bytes())}}});
  const auto continuation = unwrap(dump_json(meta));
  const auto continuation_bytes =
      std::as_bytes(std::span{continuation.data(), continuation.size()});
  // Fresh identities: one ordered batch, no authoritative partial admission.
  const std::array<RetainedEvent, 3> admitted{
      RetainedEvent{
          {},
          DecisionEvent{decision,
                        identity_.actor,
                        identity_.conversation,
                        identity_.workflow,
                        generation_,
                        context,
                        {invocation},
                        {continuation_bytes.begin(), continuation_bytes.end()}}},
      RetainedEvent{{}, InvocationEvent{invocation, decision, generation_, bytes}},
      RetainedEvent{{}, AttemptAdmissionEvent{attempt, invocation, decision, bytes}}};
  (void)unwrap(root.append(root.cursor(), {}, admitted));
  struct Boundary final : EffectBoundary {
    const std::function<Json(OperationAttemptId)> &fn;
    Json result;
    std::optional<Error> error;
    explicit Boundary(const std::function<Json(OperationAttemptId)> &f) : fn(f) {}
    Result<void> dispatch(const EffectIntent &intent) override {
      try {
        result = fn(intent.attempt);
        return Result<void>::success();
      } catch (const Error &e) {
        error = e;
        return Result<void>::failure(e);
      } catch (const std::bad_alloc &) {
        error = Error{ErrorCode::allocation};
        return Result<void>::failure(*error);
      } catch (const std::exception &) {
        error = Error{ErrorCode::external_unknown};
        return Result<void>::failure(*error);
      }
    }
  } boundary{body};
  const auto report = unwrap(root.dispatch(attempt, boundary));
  unwrap(report.recording);
  if (!report.dispatched)
    throw Error{ErrorCode::conflict};
  Json result =
      boundary.error ? error_json(*boundary.error) : std::move(boundary.result);
  const bool timed_out = boundary.error && boundary.error->code == ErrorCode::io &&
                         boundary.error->detail == ETIMEDOUT;
  if (name == "exec") {
    result.object().emplace_back("output_ref", Json{hex_identity(attempt.bytes())});
    if (timed_out) {
      result.object().emplace_back("timed_out", Json{true});
      result.object().emplace_back("effect_outcome", Json{"unknown"});
    }
  }
  const auto output = unwrap(dump_json(result));
  log_.original({"operation.result", output,
                 Json::object({{"attempt", Json{hex_identity(attempt.bytes())}},
                               {"operation", Json{std::string{name}}}})});
  const auto observed = std::as_bytes(std::span{output.data(), output.size()});
  auto disposition =
      boundary.error ? AttemptDisposition::failure : AttemptDisposition::success;
  if (boundary.error && (name == "provider" || name == "exec") &&
      (boundary.error->code == ErrorCode::interrupted ||
       boundary.error->code == ErrorCode::io ||
       boundary.error->code == ErrorCode::incomplete ||
       boundary.error->code == ErrorCode::external_unknown ||
       boundary.error->code == ErrorCode::provider_transport))
    disposition = AttemptDisposition::unknown;
  if (!boundary.error && name == "exec" &&
      field(result, "exit_code").number().text != "0")
    disposition = AttemptDisposition::failure;
  (void)unwrap(
      root.submit({{},
                   AttemptObservationEvent{attempt,
                                           AttemptPhase::terminal,
                                           disposition,
                                           {observed.begin(), observed.end()}}}));
  if (operation_completed) {
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - began)
                             .count();
    const auto outcome =
        timed_out && name == "exec"                  ? "timed out; outcome unknown"
        : disposition == AttemptDisposition::success ? "completed"
        : disposition == AttemptDisposition::unknown ? "stopped; outcome unknown"
        : boundary.error && boundary.error->code == ErrorCode::interrupted
            ? "interrupted (recorded failure)"
            : "failed";
    operation_completed(std::string{name} + " " + outcome + " · " +
                        std::to_string(elapsed) + "ms");
  }
  if (boundary.error && name != "provider" && process_output)
    process_output("\n[" + std::string{name} + ": " + error_name(boundary.error->code) +
                   "]\n");
  if (boundary.error &&
      (boundary.error->code == ErrorCode::interrupted || name == "provider" ||
       root.state() != JournalWriterState::live))
    throw *boundary.error;
  return result;
}
Json CodingEngine::request(Json options) {
  if (restart_note_)
    throw Error{ErrorCode::busy};
  auto items = context_.items();
  validate_protocol(items);
  auto request = Json::object(
      {{"model", Json{model_}},
       {"instructions",
        Json{"You are Arconaut, a programmable coding colleague for an expert "
             "operator. Use local file/process tools without approval ceremony. "
             "Context is ordinary editable data: inspect it, transform it with Lua, "
             "repair it from originals. A timed-out tool returns a result: inspect its "
             "retained output and choose a new attempt or another approach; never "
             "blindly repeat an effect with unknown outcome. Keep tool call/result "
             "linkage valid. Governing "
             "Lua workflow changes activate next turn. For compiled changes, build "
             "build/release/arco and request restart with a note listing changes, "
             "checks and next steps; the same session will resume with continue. "
             "Bound hardening before starting: list concrete remediation tasks, "
             "direct checks and a time/resource allowance. Layer one: remediate "
             "and fix. Layer two: one recheck and fix its findings within that "
             "allowance. Never a third assurance layer: no reviews of reviews, "
             "certifying rechecks, unrelated full-suite reruns or silent budget "
             "resets. Settled checks stay settled absent relevant change or new "
             "defect evidence. If blocked at the bound, retain the unsafe candidate "
             "inactive and continue independent useful work; do not call it safe. "
             "Work carefully and verify "
             "actual results. Host cwd: " +
             std::filesystem::current_path().string()}},
       {"reasoning", Json::object({{"effort", Json{effort_}}})},
       {"input", Json{std::move(items)}},
       {"tools", tool_definitions()},
       {"parallel_tool_calls", Json{false}},
       {"include", Json{Json::Array{Json{"reasoning.encrypted_content"}}}},
       {"store", Json{false}},
       {"stream", Json{true}}});
  if (!std::holds_alternative<Json::Object>(options.value()))
    throw Error{ErrorCode::invalid_range};
  unsigned max_attempts = 5;
  unsigned base_ms = 1000;
  unsigned cap_ms = 16000;
  if (const auto *policy = options.find("retry_policy")) {
    if (!std::holds_alternative<Json::Object>(policy->value()))
      throw Error{ErrorCode::invalid_range};
    for (const auto &[key, value] : policy->object()) {
      if (!std::holds_alternative<JsonNumber>(value.value()))
        throw Error{ErrorCode::invalid_range};
      unsigned n = 0;
      const auto &text = value.number().text;
      const auto parsed = std::from_chars(text.data(), text.data() + text.size(), n);
      if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size())
        throw Error{ErrorCode::invalid_range};
      if (key == "max_attempts" && n >= 1 && n <= 100)
        max_attempts = n;
      else if (key == "base_ms" && n <= 60000)
        base_ms = n;
      else if (key == "cap_ms" && n <= 60000)
        cap_ms = n;
      else
        throw Error{ErrorCode::invalid_range};
    }
  }
  for (auto &[key, value] : options.object()) {
    if (key == "retry_policy")
      continue;
    auto &fields = request.object();
    auto found = std::find_if(fields.begin(), fields.end(),
                              [&](const auto &f) { return f.first == key; });
    if (found == fields.end())
      fields.emplace_back(key, std::move(value));
    else
      found->second = std::move(value);
  }
  const auto *store = std::get_if<bool>(&field(request, "store").value());
  const auto *stream = std::get_if<bool>(&field(request, "stream").value());
  if (store == nullptr || *store || stream == nullptr || !*stream)
    throw Error{ErrorCode::unsupported};
  validate_protocol(field(request, "input").array());
  last_request_bytes_ =
      Json{JsonNumber{std::to_string(unwrap(dump_json(request)).size())}};
  usage_ = Json{};
  if (display)
    display("\n[request " + last_request_bytes_.number().text + " JSON bytes]\n");
  const auto origin = context_.head();
  // One live request group; each failed attempt stays independently recorded.
  const auto retry_group =
      hex_identity(unwrap(log_.root().issue<OperationAttemptId>()).bytes());
  provider_.cancelled = cancelled;
  Json response;
  unsigned delay_ms = std::min(base_ms, cap_ms);
  for (unsigned ordinal = 1;; ++ordinal) {
    if (cancelled && cancelled())
      throw Error{ErrorCode::interrupted};
    const auto journal = log_.root().journal_usage();
    if (journal.approaching() || journal.extent_error) {
      const auto remaining = journal.remaining_bytes();
      const auto warning =
          "audit headroom: " +
          (remaining ? std::to_string(*remaining) + " bytes" : "bytes unavailable") +
          ", " + std::to_string(journal.remaining_records()) +
          " indexed records remaining; observation only, no handoff reserve; "
          "context compaction does not reclaim audit history";
      if (status)
        status(warning);
      if (display)
        display("\n[" + warning + "]\n");
    }
    ResponsePreview preview;
    previewed_.clear();
    try {
      response = operation("provider", request, [&](OperationAttemptId attempt) {
        log_.original(
            {"provider.request", unwrap(dump_json(request)),
             Json::object({{"attempt", Json{hex_identity(attempt.bytes())}},
                           {"revision", Json{origin}},
                           {"generation", Json{hex_identity(generation_.bytes())}},
                           {"retry_group", Json{retry_group}},
                           {"ordinal", Json{JsonNumber{std::to_string(ordinal)}}}})});
        return provider_.respond(request, [&](std::string_view raw) {
          log_.original({"provider.stream", raw,
                         Json::object({{"attempt", Json{hex_identity(attempt.bytes())}},
                                       {"revision", Json{origin}}})});
          for (const auto &delta : preview.feed(raw)) {
            previewed_[delta.item_id] += delta.text;
            if (display)
              display(delta.text);
          }
        });
      });
      break;
    } catch (const Error &e) {
      const bool transient_http =
          e.code == ErrorCode::external_unknown &&
          (e.detail == 408 || e.detail == 429 || e.detail == 500 || e.detail == 502 ||
           e.detail == 503 || e.detail == 504);
      const bool transient_curl =
          e.code == ErrorCode::provider_transport &&
          (e.detail == 5 || e.detail == 6 || e.detail == 7 || e.detail == 16 ||
           e.detail == 18 || e.detail == 28 || e.detail == 52 || e.detail == 55 ||
           e.detail == 56 || e.detail == 92 || e.detail == 95 || e.detail == 96);
      if (ordinal >= max_attempts || !(transient_http || transient_curl) ||
          log_.root().state() != JournalWriterState::live)
        throw;
      const auto message =
          "provider attempt " + std::to_string(ordinal) + " stopped (" +
          error_name(e.code) + ":" + std::to_string(e.detail) +
          "); partial output not accepted; retry " + std::to_string(ordinal + 1) + "/" +
          std::to_string(max_attempts) + " in " + std::to_string(delay_ms) + "ms";
      log_.original({"provider.retry", message,
                     Json::object({{"retry_group", Json{retry_group}}})});
      if (status)
        status(message);
      if (display)
        display("\n[" + message + "]\n");
      const auto until =
          std::chrono::steady_clock::now() + std::chrono::milliseconds{delay_ms};
      while (std::chrono::steady_clock::now() < until) {
        if (cancelled && cancelled())
          throw Error{ErrorCode::interrupted};
        std::this_thread::sleep_for(std::chrono::milliseconds{25});
      }
      delay_ms = std::min(cap_ms, delay_ms * 2);
    }
  }
  if (const auto *usage = response.find("usage");
      usage && std::holds_alternative<Json::Object>(usage->value())) {
    Json::Object selected;
    auto select = [](const Json &source, std::string_view key, Json::Object &dest) {
      const auto *v = source.find(key);
      if (!v || !std::holds_alternative<JsonNumber>(v->value()))
        return;
      const auto &n = v->number().text;
      if (!n.empty() &&
          std::all_of(n.begin(), n.end(), [](char c) { return c >= '0' && c <= '9'; }))
        dest.emplace_back(std::string{key}, *v);
    };
    for (const auto key : {"input_tokens", "output_tokens", "total_tokens"})
      select(*usage, key, selected);
    for (const auto &[group, key] :
         {std::pair{"input_tokens_details", "cached_tokens"},
          std::pair{"output_tokens_details", "reasoning_tokens"}}) {
      if (const auto *details = usage->find(group)) {
        Json::Object kept;
        select(*details, key, kept);
        if (!kept.empty())
          selected.emplace_back(group, Json::object(std::move(kept)));
      }
    }
    if (!selected.empty()) {
      usage_ = Json::object(std::move(selected));
      if (display)
        display("\n[provider usage " + unwrap(dump_json(usage_)) + "]\n");
    }
  }
  if (context_.head() != origin)
    throw Error{ErrorCode::conflict};
  context_.append(field(response, "output").array(), "provider.branch:" + origin);
  return response;
}
Json CodingEngine::call(std::string name, Json arguments) {
  if (name == "provider")
    throw Error{ErrorCode::invalid_range};
  return operation(name, arguments, [&](OperationAttemptId attempt) {
    if (name == "restart") {
      auto note = string_field(arguments, "note");
      if (note.empty() || note.size() > 65536)
        throw Error{ErrorCode::invalid_range};
      restart_note_ = std::move(note);
      return Json::object({{"scheduled", Json{true}}});
    }
    if (name == "read_process_output") {
      const auto &reference = string_field(arguments, "output_ref");
      bool known = false;
      std::string raw;
      for (const auto &fact : log_.root().committed_facts()) {
        const auto *record = std::get_if<ApplicationRecordEvent>(&fact.event.body);
        if (!record || record->channel != ApplicationChannel::log)
          continue;
        const auto packet = unwrap(parse_json(
            std::string_view{reinterpret_cast<const char *>(record->payload.data()),
                             record->payload.size()}));
        const auto &metadata = field(packet, "metadata");
        const auto *old_attempt = metadata.find("attempt");
        if (!old_attempt || old_attempt->string() != reference)
          continue;
        const auto &label = string_field(packet, "label");
        if (label == "operation.result") {
          const auto *op = metadata.find("operation");
          if (op && op->string() == "exec")
            known = true;
        }
        if (label == "process.output") {
          known = true;
          for (const auto dependency : fact.event.dependencies) {
            const auto bytes = unwrap(log_.root().source(dependency));
            raw.append(reinterpret_cast<const char *>(bytes.data()), bytes.size());
          }
        }
      }
      if (!known)
        throw Error{ErrorCode::invalid_range};
      // Retrieval supports byte ranges only, never file line semantics.
      if (arguments.find("line_start") || arguments.find("line_end"))
        throw Error{ErrorCode::invalid_range};
      return process_output_presentation(raw, arguments);
    }
    if (name == "context_manage") {
      auto proposal = field(arguments, "proposal");
      // Explicit invocation-time snapshot convenience. Supplied bases remain strict
      // CAS.
      if (std::holds_alternative<Json::Object>(proposal.value()) &&
          !proposal.find("base"))
        proposal.object().emplace_back("base", Json{context_.head()});
      return context_.manage(proposal);
    }
    if (name == "rageshake") {
      const auto &observation = string_field(arguments, "observation");
      if (observation.empty() || observation.size() > 65536)
        throw Error{ErrorCode::invalid_range};
      const auto references = arguments.find("references")
                                  ? field(arguments, "references")
                                  : Json::object({});
      if (!std::holds_alternative<Json::Object>(references.value()) ||
          unwrap(dump_json(references)).size() > 65536)
        throw Error{ErrorCode::invalid_range};
      std::set<std::string> active;
      for (const auto &fact : log_.root().committed_facts()) {
        if (const auto *a = std::get_if<AttemptAdmissionEvent>(&fact.event.body))
          active.insert(hex_identity(a->attempt.bytes()));
        if (const auto *o = std::get_if<AttemptObservationEvent>(&fact.event.body);
            o && o->phase == AttemptPhase::terminal)
          active.erase(hex_identity(o->attempt.bytes()));
      }
      Json::Array active_ids;
      for (const auto &id : active) {
        if (active_ids.size() == 64)
          break;
        active_ids.push_back(Json{id});
      }
      auto detail = Json::object(
          {{"observation", Json{observation}},
           {"references", references},
           {"active_attempts", Json{std::move(active_ids)}},
           {"active_attempt_count", Json{JsonNumber{std::to_string(active.size())}}},
           {"revision", Json{context_.head()}},
           {"generation", Json{hex_identity(generation_.bytes())}},
           {"actor", Json{hex_identity(identity_.actor.bytes())}},
           {"conversation", Json{hex_identity(identity_.conversation.bytes())}},
           {"workflow", Json{hex_identity(identity_.workflow.bytes())}},
           {"model", Json{model_}},
           {"effort", Json{effort_}},
           {"audit_end",
            Json{JsonNumber{std::to_string(log_.root().committed_facts().size())}}},
           {"reporting_attempt", Json{hex_identity(attempt.bytes())}},
           {"repair_required_now", Json{false}}});
      const auto complaint = log_.original(
          {"complaint.detail", unwrap(dump_json(detail)),
           Json::object({{"attempt", Json{hex_identity(attempt.bytes())}}})});
      // Only a non-sensitive locator goes to beads (and hence issue backups).
      // The observation and provider/context history remain in the local audit.
      auto delivery =
          call("exec",
               Json::object(
                   {{"argv",
                     Json{Json::Array{
                         Json{"bd"}, Json{"create"},
                         Json{"Arco model complaint " + complaint}, Json{"--type"},
                         Json{"bug"}, Json{"--description"},
                         Json{"Local audit complaint " + complaint + "; conversation " +
                              hex_identity(identity_.conversation.bytes()) +
                              ". Inspect complaint.detail locally. Advisory; no "
                              "immediate repair required."},
                         Json{"--json"}}}},
                    {"timeout_seconds", Json{JsonNumber{"20"}}},
                    {"output_max_bytes", Json{JsonNumber{"4096"}}}}));
      const auto *exit = delivery.find("exit_code");
      const bool delivered = exit && exit->number().text == "0";
      auto result = Json::object({{"complaint", Json{complaint}},
                                  {"retained_locally", Json{true}},
                                  {"bead_created", Json{delivered}},
                                  {"delivery", delivery},
                                  {"sink", Json{"external consumer not configured"}},
                                  {"repair_required_now", Json{false}}});
      log_.original({"complaint.delivery", unwrap(dump_json(result)),
                     Json::object({{"complaint", Json{complaint}}})});
      return result;
    }
    if (name == "audit_inspect")
      return log_.inspect(field(arguments, "query"));
    if (name == "context_inspect")
      return context_.inspect(field(arguments, "query"));
    if (name == "context_stats")

      return stats();
    if (name == "context_view")
      return context_.view();
    if (name == "context_edit")
      return context_.edit(field(arguments, "candidate"));
    if (name == "context_originals")
      return context_.originals();
    if (name == "context_restore") {
      context_.restore(string_field(arguments, "entry"));
      return context_.view();
    }
    if (name == "lua")
      return runtime_->eval(string_field(arguments, "code"));
    LocalTools tools{[&](std::string_view label, std::string_view raw) {
      log_.original({label, raw,
                     Json::object({{"attempt", Json{hex_identity(attempt.bytes())}}})});
      if (label == "process.output" && process_output)
        process_output(raw);
    }};
    tools.cancelled = cancelled;
    auto result = tools.run(name, arguments);
    if (name == "exec" && process_output)
      process_output("\n[exit " + field(result, "exit_code").number().text + "]\n");
    return result;
  });
}
Json CodingEngine::stats() const {
  auto result = context_.stats();
  const auto journal = log_.root().journal_usage();
  auto number = [](std::uint64_t n) { return Json{JsonNumber{std::to_string(n)}}; };
  result.object().emplace_back(
      "audit",
      Json::object(
          {{"scope",
            Json{
                "physical observation; not admission permission or reserved headroom"}},
           {"journal", Json{hex_identity(journal.prefix.journal.bytes())}},
           {"prefix_end", number(journal.prefix.end_offset)},
           {"observed_extent",
            journal.observed_extent ? number(*journal.observed_extent) : Json{}},
           {"max_file_bytes", number(journal.limit.max_file_bytes)},
           {"remaining_bytes",
            journal.remaining_bytes() ? number(*journal.remaining_bytes()) : Json{}},
           {"max_records", number(journal.limit.max_records)},
           {"indexed_records", number(journal.indexed_records)},
           {"remaining_records", number(journal.remaining_records())},
           {"writer_state", number(static_cast<unsigned>(journal.state))},
           {"approaching", Json{journal.approaching()}},
           {"extent_error",
            journal.extent_error
                ? Json::object({{"code", Json{std::string{
                                             error_name(journal.extent_error->code)}}},
                                {"detail", Json{JsonNumber{std::to_string(
                                               journal.extent_error->detail)}}}})
                : Json{}}}));
  result.object().emplace_back("last_request_bytes", last_request_bytes_);
  result.object().emplace_back("usage", usage_);
  result.object().emplace_back(
      "request_scope", Json{"last request in this process; null if unavailable"});
  return result;
}
void CodingEngine::validate_restart() const { validate_protocol(context_.items()); }
void CodingEngine::effort(std::string value) {
  if (value != "low" && value != "medium" && value != "high" && value != "xhigh")
    throw Error{ErrorCode::invalid_range};
  effort_ = std::move(value);
}
void CodingEngine::present(const Json &item) {
  std::string text;
  for (const auto &part : field(item, "content").array())
    if (string_field(part, "type") == "output_text")
      text += string_field(part, "text");
  const auto *id = item.find("id");
  auto found = previewed_.find(id ? id->string() : "");
  if (found != previewed_.end()) {
    if (!text.starts_with(found->second)) {
      if (display)
        display("\n[Final response differs from streamed preview]\n");
    } else
      text.erase(0, found->second.size());
    previewed_.erase(found);
  }
  if (!text.empty()) {
    log_.original({"display", text, Json{}});
    if (display)
      display(text);
  }
}
void CodingEngine::turn(TurnInput input) {
  restart_note_.reset();
  const auto prompt = input.prompt;
  const auto workflow = input.program;
  generation_ = unwrap(log_.root().issue<DefinitionGenerationId>());
  log_.original(
      {"program.source", workflow,
       Json::object({{"generation", Json{hex_identity(generation_.bytes())}}})});
  log_.record(ApplicationChannel::program,
              Json::object({{"generation", Json{hex_identity(generation_.bytes())}},
                            {"activation", Json{"turn-boundary"}}}));
  if (!prompt.empty())
    context_.append({Json::object({{"role", Json{"user"}},
                                   {"content", Json{std::string{prompt}}}})},
                    "operator");
  runtime_ = std::make_unique<Runtime>(*this);
  context_.begin_workflow();
  try {
    (void)runtime_->eval(workflow);
    if (cancelled && cancelled())
      throw Error{ErrorCode::interrupted};
    auto settlement = context_.finish_workflow(true);
    if (settlement != Json{} && display)
      display("Managed context: " + unwrap(dump_json(settlement)) + "\n");
    if (restart_note_)
      validate_protocol(context_.items());

  } catch (const Error &e) {
    (void)context_.finish_workflow(false);
    restart_note_.reset();
    if (e.code == ErrorCode::interrupted &&
        log_.root().state() == JournalWriterState::live) {
      std::set<std::string> pending;
      for (const auto &item : context_.items()) {
        const auto *type = item.find("type");
        if (!type)
          continue;
        if (type->string() == "function_call")
          pending.insert(string_field(item, "call_id"));
        if (type->string() == "function_call_output")
          pending.erase(string_field(item, "call_id"));
      }
      Json::Array results;
      for (const auto &call : pending)
        results.push_back(Json::object(
            {{"type", Json{"function_call_output"}},
             {"call_id", Json{call}},
             {"output", Json{"{\"error\":\"turn_interrupted\",\"detail\":\"Consult "
                             "audit for effect outcome; no automatic retry\"}"}}}));
      if (!results.empty())
        context_.append(std::move(results), "interrupt.linkage");
    }
    throw;
  } catch (...) {
    (void)context_.finish_workflow(false);
    restart_note_.reset();
    throw;
  }
}
} // namespace arconaut
