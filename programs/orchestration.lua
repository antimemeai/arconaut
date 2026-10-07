-- Scoped synchronous orchestration; no hidden workers, restart or replay.
-- Load as retained named module, or through an audited source read.
local M = {}
local function copy(v, seen)
  if type(v) ~= "table" then
    return v
  end
  seen = seen or {}
  assert(not seen[v], "cyclic outcome")
  seen[v] = true
  local t = {}
  for k, x in pairs(v) do
    assert(type(k) == "string" or type(k) == "number", "outcome key")
    assert(
      type(x) ~= "function" and type(x) ~= "thread" and type(x) ~= "userdata",
      "outcome value"
    )
    t[k] = copy(x, seen)
  end
  seen[v] = nil
  return t
end
local function integer(v)
  return type(v) == "number" and v >= 1 and v % 1 == 0 and v < math.huge
end
local function identity(v)
  return type(v) == "string" and #v > 0 and #v <= 256
end
local statuses = { completed = true, refused = true, failed = true, unknown = true }
local dispositions = { completed = true, not_dispatched = true, unknown = true }
function M.new(profile)
  assert(
    type(profile) == "table" and identity(profile.owner),
    "explicit owner required"
  )
  assert(
    integer(profile.max_participants) and integer(profile.max_attempts),
    "finite positive limits required"
  )
  local owner, maxp, maxa =
    profile.owner, profile.max_participants, profile.max_attempts
  local rows, count, attempts, active, paused = {}, 0, 0, nil, nil
  local S = {}
  local function view(row)
    return copy(row)
  end
  local function invoke(row, action)
    assert(not active, "synchronous owner already active")
    assert(type(action) == "function", "action required")
    if paused then
      row.status = "paused"
      row.reason = paused
      return view(row)
    end
    if attempts >= maxa then
      row.status = "limited"
      row.reason = "attempt budget"
      return view(row)
    end
    attempts = attempts + 1
    row.attempts = row.attempts + 1
    row.status = "running"
    row.remote_disposition = "unknown"
    active = row.id
    -- Callbacks use ordinary blackbird.call; native lifetime/cancellation remains owner.
    -- Catching an exception NEVER establishes nonexecution or remote cancellation.
    local ok, result =
      pcall(action, { owner = owner, id = row.id, attempt = row.attempts })
    active = nil
    if ok then
      local valid = type(result) == "table"
        and statuses[result.status]
        and dispositions[result.remote_disposition]
      valid = valid
        and (result.status ~= "completed" or result.remote_disposition == "completed")
      valid = valid
        and (result.status ~= "unknown" or result.remote_disposition == "unknown")
      local copied, payload = pcall(copy, result)
      if valid and copied then
        row.status = result.status
        row.remote_disposition = result.remote_disposition
        row.result = payload
      else
        row.status = "unknown"
        row.result = nil
        row.reason = "malformed outcome"
      end
    else
      row.status = "unknown"
      row.reason = tostring(result)
    end
    row.history[#row.history + 1] = {
      attempt = row.attempts,
      status = row.status,
      remote_disposition = row.remote_disposition,
      result = row.result,
      reason = row.reason,
    }
    return view(row)
  end
  function S:run(id, action)
    assert(identity(id), "participant identity required")
    assert(not rows[id], "duplicate participant identity")
    assert(not active, "synchronous owner already active")
    assert(type(action) == "function", "action required")
    assert(count < maxp, "participant budget")
    count = count + 1
    local row = {
      id = id,
      owner = owner,
      attempts = 0,
      status = "admitted",
      remote_disposition = "not_dispatched",
      history = {},
    }
    rows[id] = row
    return invoke(row, action)
  end
  function S:retry(id, action)
    local row = assert(rows[id], "unknown participant")
    assert(
      row.remote_disposition == "not_dispatched" and row.status == "refused",
      "retry requires explicit refused/not_dispatched; reconcile unknowns separately"
    )
    row.reason = nil
    row.result = nil
    return invoke(row, action)
  end
  function S:branch(condition, yes_id, yes_action, no_id, no_action)
    assert(type(condition) == "boolean", "explicit boolean branch")
    if condition then
      return self:run(yes_id, yes_action)
    end
    return self:run(no_id, no_action)
  end
  function S:get(id)
    return view(assert(rows[id], "unknown participant"))
  end
  function S:join(ids)
    assert(type(ids) == "table" and #ids > 0, "nonempty selected join")
    local n = 0
    for k, id in pairs(ids) do
      assert(integer(k) and identity(id), "dense identity selection required")
      n = n + 1
    end
    assert(n == #ids, "sparse join selection")
    for i = 1, n do
      assert(ids[i] ~= nil, "sparse join selection")
    end
    local results, seen, settled = {}, {}, true
    for _, id in ipairs(ids) do
      assert(not seen[id], "duplicate join identity")
      seen[id] = true
      local row = assert(rows[id], "unknown join participant")
      results[#results + 1] = view(row)
      if row.status ~= "completed" then
        settled = false
      end
    end
    return {
      owner = owner,
      status = settled and "completed" or "unsettled",
      results = results,
    }
  end
  function S:pause(reason)
    assert(identity(reason), "explicit pause reason")
    paused = reason
    -- Does not interrupt an active native call or claim its remote effects stopped.
    return self:state()
  end
  function S:state()
    return {
      owner = owner,
      participants = count,
      attempts = attempts,
      active = active,
      paused = paused,
      max_participants = maxp,
      max_attempts = maxa,
      concurrency = 1,
      scope = "current workflow only; no replay/admission inheritance",
    }
  end
  return S
end
return M
