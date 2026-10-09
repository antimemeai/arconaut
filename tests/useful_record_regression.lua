-- Standalone M.record regression. Run with useful_work_test and helper_path.
-- No real writes or subprocesses: all helper effects are mocked below.
assert(type(helper_path) == "string", "runner must provide helper_path")
local active
blackbird = {
  binary = {
    encode = function(value)
      active.encodes = active.encodes + 1
      assert(value == active.request, "encode the actual record")
      return "BBM2-MOCK-RECORD"
    end,
  },
  stats = function()
    error("resources supplied; stats must not be needed")
  end,
  call = function(name, args)
    active.calls[#active.calls + 1] = name
    if name == "write_file" then
      active.writes = active.writes + 1
      assert(args.path == active.path, "write the designated request path")
      assert(args.content == "BBM2-MOCK-RECORD", "write encoded request")
      return active.write_result
    elseif name == "exec" then
      active.executions = active.executions + 1
      local argv = args.argv
      assert(type(argv) == "table" and #argv == 4, "exact CLI argv length")
      assert(
        argv[1] == active.executable
          and argv[2] == "mock-pool"
          and argv[3] == "record"
          and argv[4] == active.path,
        "CLI must use the designated executable, pool and request"
      )
      assert(
        args.timeout_seconds == 30 and args.output_max_bytes == 4096,
        "bounded CLI execution"
      )
      return active.exec_result
    end
    error("unexpected tool: " .. tostring(name))
  end,
}
local M = assert(loadfile(helper_path))()
assert(type(M.record) == "function", "helper must expose M.record")

local function attempt(label, write_result, exec_result, executable)
  active = {
    writes = 0,
    executions = 0,
    encodes = 0,
    calls = {},
    path = "mock-requests/" .. label .. ".bbm",
    executable = executable or "build/release/blackbird-candidate",
    write_result = write_result,
    exec_result = exec_result,
    request = {
      question = "record dispatch regression",
      action = "mock persistence",
      outcome = label,
      references = {},
      resources = { test = true },
    },
  }
  local result = M.record("mock-pool", active.path, active.request, executable)
  assert(
    active.writes == 1 and active.encodes == 1,
    label .. ": exactly one request serialization/write, no retries"
  )
  return result, active
end

-- Missing written is unconfirmed even when there is no explicit error.
local unconfirmed = {}
assert(unconfirmed.written == nil and unconfirmed.error == nil)
local result, observed = attempt("unconfirmed", unconfirmed, { exit_code = 0 })
assert(
  observed.executions == 0
    and #observed.calls == 1
    and observed.calls[1] == "write_file",
  "unconfirmed write must not invoke CLI"
)
assert(
  type(result) == "table"
    and result.error == "request write not confirmed"
    and result.write_result == unconfirmed,
  "return rejection and original write result"
)

-- A confirmed write permits exactly one invocation and returns its result.
local success = { exit_code = 0, output = "mock recorded" }
result, observed = attempt("confirmed", { written = true }, success)
assert(
  observed.executions == 1
    and #observed.calls == 2
    and observed.calls[1] == "write_file"
    and observed.calls[2] == "exec",
  "confirmed write must invoke CLI exactly once, after persistence"
)
assert(
  result == success and result.exit_code == 0,
  "return actual successful CLI result"
)

-- Unknown effect is propagated, not replayed (nor rewritten).
local unknown =
  { timed_out = true, effect_outcome = "unknown", output_ref = "mock-output" }
result, observed = attempt("unknown", { written = true }, unknown, "mock-candidate")
assert(
  observed.executions == 1
    and #observed.calls == 2
    and observed.calls[1] == "write_file"
    and observed.calls[2] == "exec",
  "unknown delivery must never be retried"
)
assert(
  result == unknown
    and result.timed_out == true
    and result.effect_outcome == "unknown"
    and result.output_ref == "mock-output",
  "return unknown outcome intact"
)
print("M.record regression passed: unconfirmed=0 CLI; confirmed=1; unknown=1, no retry")
