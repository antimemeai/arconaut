local helper_path = assert(rawget(_G, "helper_path"), "runner must supply helper_path")
-- Standalone M.feedback regression. Runner supplies helper_path.
-- Sources: programs/useful_work.lua:4-9,38-50;
-- tests/useful_work_test.lua:18,27-28; tests/useful_work_test.cpp:5-18;
-- docs/USEFUL_WORK_CANDIDATES.md:66-70.
local stats_calls = 0
local current_stats
blackbird = {
  stats = function()
    stats_calls = stats_calls + 1
    assert(current_stats, "unexpected stats request")
    return current_stats
  end,
  call = function()
    error("feedback must not dispatch tools")
  end,
}
local M = assert(loadfile(assert(helper_path, "runner must supply helper_path")))()

-- Distinct sizes avoid unspecified tie order. Lexical sorting would put 9
-- above 1000; truncation before sorting would omit both largest entries.
local entries = {
  { id = "tiny-tagged", item_bytes = 9 },
  { id = "third-native", item_bytes = 100 },
  { id = "fourth-tagged", item_bytes = 80 },
  { id = "first-tagged", item_bytes = 1000 },
  { id = "second-native", item_bytes = 250 },
}
local original_ids = {}
local original_sizes = {}
for i, e in ipairs(entries) do
  original_ids[i], original_sizes[i] = e.id, e.item_bytes
end
local expected_ids = { "first-tagged", "second-native", "third-native" }
local expected_sizes = { 1000, 250, 100 }
-- Independent oracle: do not use M.number to validate its own sorting.
local function numeric(v)
  if type(v) == "number" then
    return v
  end
  assert(type(v) == "table" and type(v[1]) == "string", "invalid size representation")
  return assert(tonumber(v[1]), "invalid lexical size")
end
local function check(f, s)
  local largest = assert(f.largest_entries, "missing largest_entries")
  assert(#largest == 3, "must return exactly three entries")
  local count = 0
  for k in pairs(largest) do
    assert(k == 1 or k == 2 or k == 3, "unexpected largest_entries key")
    count = count + 1
  end
  assert(count == 3, "exactly largest3, no hidden extra entries")
  for i = 1, 3 do
    assert(largest[i].id == expected_ids[i], "wrong top-three ID/order at " .. i)
    assert(numeric(largest[i].bytes) == expected_sizes[i], "wrong size at " .. i)
    if i > 1 then
      assert(
        numeric(largest[i - 1].bytes) > numeric(largest[i].bytes),
        "not descending numerically"
      )
    end
  end
  assert(f.input_bytes == s.input_bytes, "input_bytes must pass through unchanged")
  assert(
    f.last_request_bytes == s.last_request_bytes,
    "last_request_bytes must pass through unchanged"
  )
  assert(
    f.usage_scope == s.request_scope,
    "request_scope must pass through as usage_scope"
  )
  assert(f.usage == s.usage, "usage must pass through; absence is not zero")
  for i, e in ipairs(entries) do
    assert(
      e.id == original_ids[i] and e.item_bytes == original_sizes[i],
      "input entries mutated/reordered"
    )
  end
end

-- Explicit stats must bypass blackbird.stats; absent provider usage remains nil.
local missing = {
  entries = entries,
  input_bytes = 1439,
  last_request_bytes = 2048,
  request_scope = "last request in this process",
}
local f = M.feedback(missing)
check(f, missing)
assert(
  f.usage == nil,
  "missing usage must remain nil, never zero or a fabricated table"
)
assert(stats_calls == 0, "explicit stats must not consult runtime")

-- Default path exercises native and lexical counters in the opposite roles,
-- and preserves a populated usage object, including its native zero counter.
local usage = { input_tokens = 37, output_tokens = 0 }
current_stats = {
  entries = entries,
  input_bytes = 1439,
  last_request_bytes = 2048,
  usage = usage,
  request_scope = "mock provider request",
}
check(M.feedback(), current_stats)
assert(stats_calls == 1, "default feedback must fetch stats exactly once")

-- Runtime stats may also lack usage and last-request data entirely.
current_stats = { entries = entries, input_bytes = 1439 }
f = M.feedback()
check(f, current_stats)
assert(
  f.usage == nil and f.last_request_bytes == nil and f.usage_scope == nil,
  "unavailable fields must not be synthesized as zero"
)
assert(stats_calls == 2, "one stats request per default call")

-- Preserve the existing explicit unavailable sentinel as well.
local unavailable = {
  entries = entries,
  input_bytes = 0,
  last_request_bytes = 0,
  usage = "unavailable",
  request_scope = "last request",
}
check(M.feedback(unavailable), unavailable)
assert(stats_calls == 2, "explicit sentinel case must bypass runtime")
print("M.feedback regression passed: numeric largest3, counters, missing usage")
