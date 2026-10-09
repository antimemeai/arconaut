local M = dofile("programs/packages.lua")
local G = dofile("packages/github_releases.lua")
local calls = 0
local adapter = {
  metadata = { id = "test", version = 1 },
  fetch = function()
    calls = calls + 1
    return {
      items = {
        {
          id = "r1",
          revision = "v1",
          title = "compiler release",
          link = "https://example.com",
          published = "today",
        },
      },
      original_ref = "native-attempt",
      fetched_at = "now",
    }
  end,
}
local p =
  M.new({ id = "cpp-lua", revision = "1", topic = "C++/Lua toolchain", max_items = 3 })
p:install(adapter)
assert(calls == 0, "discovery must not initialize")
assert(not pcall(function()
  p:receive("test")
end), "disabled fetch refused")
assert(calls == 0, "disabled package never launches")
p:enable("test", {})
local receipt = p:receive("test")
assert(calls == 1 and receipt.items == 1)
local hud = p:display("test")
assert(hud:find("compiler release", 1, true))
assert(
  p:observations().included == 0 and p:observations().actions == 0,
  "display is not inclusion/action"
)
local item = p:select("test", "r1")
item.title = "tampered"
assert(p:select("test", "r1").title == "compiler release", "immutable selection")
local message = p:context("test", "r1")
assert(message.role == "user" and message.content:find("UNTRUSTED", 1, true))
assert(p:observations().included == 0, "prepared context not included")
p:include("test", "r1", function(x)
  assert(x[1].role == "user")
end)
assert(p:observations().included == 1 and p:observations().actions == 0)
local event =
  p:event("test", "r1", "Inspect toolchain relevance without changing dependencies.")
assert(event.source == "package/test" and event.id == "r1@v1")
assert(p:observations().actions == 0, "prepared station event is not dispatch")
p:receive("test")
assert(p:observations().received == 2, "receipt distinct from revision")
p:disable("test")
assert(not pcall(function()
  p:receive("test")
end))
assert(calls == 2)
local bad = M.new({ id = "test", revision = "1", topic = "test", max_items = 1 })
bad:install({
  metadata = { id = "bad", version = 1 },
  fetch = function()
    return { items = { { id = "x" }, { id = "y" } } }
  end,
})
bad:enable("bad", {})
assert(not pcall(function()
  bad:receive("bad")
end), "oversized intake refused")
assert(not pcall(function()
  G.configure({ repository = "bad/../../x" })
end), "repository cannot inject argv/url")
local cfg = G.configure({ repository = "llvm/llvm-project", limit = 2 })
local fake = {
  call = function(name, args)
    assert(name == "exec" and args.argv[1] == "curl" and args.timeout_seconds == 30)
    assert(
      args.argv[#args.argv]
        == "https://api.github.com/repos/llvm/llvm-project/releases?per_page=2"
    )
    return { exit_code = 0, output = "[]", output_bytes = 4, omitted_bytes = 0, output_ref = "ref" }
  end,
  json = {
    encode = function(v)
      return tostring(v)
    end,
    decode = function()
      return {
        {
          id = 123,
          tag_name = "v1",
          name = "LLVM\27[31m",
          html_url = "https://github.com/llvm/llvm-project/releases/tag/v1",
          updated_at = "rev",
          published_at = "date",
          draft = false,
        },
      }
    end,
  },
}
local r = G.fetch(cfg, fake)
assert(r.items[1].id == "123" and r.original_ref == "ref")
fake.call = function()
  return { exit_code = 22, output = "rate limited", output_ref = "unknown" }
end
assert(not pcall(function()
  G.fetch(cfg, fake)
end), "transport failure never parses success")
print("package receipt/display/context/action separation: passed")
local fail = false
local refresh = M.new({ id = "cpp", revision = "1", topic = "C++", max_items = 2 })
refresh:install({
  metadata = { id = "refresh", version = 1 },
  fetch = function()
    if fail then
      error("unknown fetch")
    end
    return {
      items = { { id = "a", revision = "v1", title = "safe\27[31m", link = "https://example.com" } },
      original_ref = "old",
      fetched_at = "unavailable",
    }
  end,
})
refresh:enable("refresh", {})
refresh:receive("refresh")
assert(not refresh:display("refresh"):find("\27", 1, true), "terminal controls removed")
fail = true
assert(not pcall(function()
  refresh:receive("refresh")
end))
assert(refresh:status("refresh").last_receive == "unknown", "failed refresh visible")
assert(
  refresh:select("refresh", "a").revision == "v1",
  "failed refresh preserves prior snapshot"
)
assert(refresh:display("refresh"):find("Last refresh: unknown", 1, true))
assert(not pcall(function()
  refresh:event("refresh", "a", string.rep("x", 32768))
end), "combined station prompt bounded")
local cycle = {}
cycle.self = cycle
assert(not pcall(function()
  refresh:enable("refresh", cycle)
end), "cycle refused")
assert(refresh:status("refresh").enabled, "invalid configuration preserves effective")
-- Model native JSON representation: numbers are exact tagged lexeme tables.
fake.json.encode = function(v)
  if type(v) == "table" then
    return v[1]
  else
    return tostring(v)
  end
end
fake.call = function()
  return { exit_code = { "0" }, output = "[]", omitted_bytes = { "0" }, output_ref = "native-ref" }
end
fake.json.decode = function()
  return {
    {
      id = { "123" },
      tag_name = "v1",
      html_url = "https://github.com/llvm/llvm-project/releases/tag/v1",
      draft = false,
    },
  }
end
assert(G.fetch(cfg, fake).items[1].id == "123", "native exact JSON id")
fake.call = function()
  return { exit_code = { "0" }, output = "{}", omitted_bytes = { "0" }, output_ref = "native-ref" }
end
assert(not pcall(function()
  G.fetch(cfg, fake)
end), "object is not empty feed")
print("package unknown refresh/native scalar/capacity cases: passed")
