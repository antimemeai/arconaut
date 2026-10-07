local M = dofile("programs/orchestration.lua")
local calls = 0
local function completed()
  calls = calls + 1
  return { status = "completed", remote_disposition = "completed", text = "useful" }
end
local function check(v, m)
  assert(v, m)
end
local s = M.new({ owner = "test", max_participants = 6, max_attempts = 8 })
check(s:run("a", completed).status == "completed", "sequential completion")
check(not pcall(function()
  s:run("a", completed)
end), "duplicate refused")
check(calls == 1, "duplicate never dispatches")
s:run("unknown", function()
  calls = calls + 1
  error("transport interrupted")
end)
check(s:get("unknown").remote_disposition == "unknown", "exception preserves unknown")
check(not pcall(function()
  s:retry("unknown", completed)
end), "unknown retry refused")
check(calls == 2, "unknown never replayed")
s:run("refused", function()
  calls = calls + 1
  return { status = "refused", remote_disposition = "not_dispatched" }
end)
check(s:retry("refused", completed).status == "completed", "safe explicit retry")
local changed = s:get("unknown")
changed.remote_disposition = "not_dispatched"
check(not pcall(function()
  s:retry("unknown", completed)
end), "snapshot cannot authorize replay")
check(
  s:branch(false, "wrong", completed, "right", completed).id == "right",
  "selected branch"
)
local j = s:join({ "a", "unknown", "right" })
check(j.status == "unsettled" and #j.results == 3, "join preserves unresolved outcome")
s:pause("operator cancel")
check(s:run("paused", completed).status == "paused", "explicit pause")
check(calls == 5, "pause does not dispatch")
check(not pcall(function()
  s:retry("refused", completed)
end), "pause cannot undo successful dispatch")
local bounded = M.new({ owner = "bounded", max_participants = 2, max_attempts = 1 })
bounded:run("one", completed)
check(bounded:run("two", completed).status == "limited", "attempt bound")
check(calls == 6, "limit prevents dispatch")
check(not pcall(function()
  bounded:run("three", completed)
end), "participant bound")
local malformed = M.new({ owner = "malformed", max_participants = 2, max_attempts = 2 })
check(malformed:run("bad", function()
  return { status = "completed" }
end).status == "unknown", "malformed outcome conservative")
check(not pcall(function()
  malformed:retry("bad", completed)
end), "malformed retry refused")
local nested = M.new({ owner = "nested", max_participants = 2, max_attempts = 2 })
nested:run("outer", function()
  check(not pcall(function()
    nested:run("inner", completed)
  end), "no untracked nested dispatch")
  check(
    nested:join({ "outer" }).status == "unsettled",
    "living participant cannot join as complete"
  )
  nested:pause("during call")
  return { status = "completed", remote_disposition = "completed" }
end)
check(nested:run("after", completed).status == "paused", "pause during call persists")
check(not pcall(function()
  s:join({ [1] = "a", [3] = "right" })
end), "sparse join rejected")
check(not pcall(function()
  s:join({ "a", "a" })
end), "duplicate join rejected")
local payload = {
  status = "completed",
  remote_disposition = "completed",
  nested = { text = "original" },
}
local aliases = M.new({ owner = "aliases", max_participants = 3, max_attempts = 3 })
aliases:run("retained", function()
  return payload
end)
payload.nested.text = "mutated"
check(
  aliases:get("retained").result.nested.text == "original",
  "input payload detached"
)
local snapshot = aliases:get("retained")
snapshot.history[1].result.nested.text = "other"
check(
  aliases:get("retained").history[1].result.nested.text == "original",
  "history snapshot detached"
)
check(aliases:run("contradiction", function()
  return { status = "unknown", remote_disposition = "not_dispatched" }
end).remote_disposition == "unknown", "contradictory unknown conservative")
local finite = M.new({ owner = "loop", max_participants = 1, max_attempts = 2 })
local rejected = 0
local function refusal()
  rejected = rejected + 1
  return { status = "refused", remote_disposition = "not_dispatched" }
end
finite:run("call", refusal)
finite:retry("call", refusal)
check(
  finite:retry("call", refusal).status == "limited" and rejected == 2,
  "repeated safe refusal bounded"
)
check(not pcall(function()
  finite:retry("call", refusal)
end), "limited loop remains stopped")
check(not pcall(function()
  M.new({ owner = "bad", max_participants = math.huge, max_attempts = 1 })
end), "infinite limit rejected")
print(
  "orchestration direct oracles passed; dispatches="
    .. calls
    .. "; bounded refusals="
    .. rejected
)
