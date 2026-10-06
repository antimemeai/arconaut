---@param value string
---@return integer
local function count_bytes(value)
  return #value
end

assert(count_bytes("a\0b") == 3)
local worker = coroutine.create(function()
  local value = coroutine.yield("waiting")
  return value
end)
local ok, value = coroutine.resume(worker)
assert(ok and value == "waiting")
ok, value = coroutine.resume(worker, "continued")
assert(ok and value == "continued")
assert(coroutine.status(worker) == "dead")
