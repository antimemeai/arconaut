-- Account-free public GitHub release snapshots. Optional, never auto-polled.
-- curl and native exec/JSON are existing capabilities, not new dependencies.
local M = { metadata = { id = "github-releases", version = 1 } }
function M.configure(c)
  assert(
    type(c.repository) == "string"
      and #c.repository <= 160
      and c.repository:match("^[%w_-]+/[%w_.-]+$"),
    "owner/repository required"
  )
  assert(
    type(c.limit) == "number" and c.limit % 1 == 0 and c.limit >= 1 and c.limit <= 8,
    "limit 1..8"
  )
  return { repository = c.repository, limit = c.limit }
end
function M.fetch(config, api)
  local c = M.configure(config)
  assert(api and api.call and api.json, "native API required")
  local r = api.call(
    "exec",
    {
      argv = {
        "curl",
        "--disable",
        "--fail",
        "--silent",
        "--show-error",
        "--proto",
        "=https",
        "--max-time",
        "25",
        "--max-filesize",
        "524288",
        "-H",
        "Accept: application/vnd.github+json",
        "-H",
        "X-GitHub-Api-Version: 2026-03-10",
        "https://api.github.com/repos/"
          .. c.repository
          .. "/releases?per_page="
          .. c.limit,
      },
      timeout_seconds = 30,
      output_max_bytes = 524288,
    }
  )
  assert(
    api.json.encode(r.exit_code) == "0"
      and not r.timed_out
      and api.json.encode(r.omitted_bytes) == "0",
    "GitHub fetch failed/unknown/truncated; output_ref="
      .. tostring(r.output_ref)
      .. "; no automatic retry"
  )
  assert(type(r.output) == "string" and r.output:match("^%s*%["), "JSON array required")
  local releases = api.json.decode(r.output)
  assert(type(releases) == "table" and #releases <= c.limit, "release snapshot shape")
  local items = {}
  for _, v in ipairs(releases) do
    assert(type(v) == "table", "release object required")
    -- Native decode preserves JSON numeric lexemes as tagged tables.
    local id = api.json.encode(v.id)
    assert(
      type(id) == "string"
        and #id <= 20
        and id:match("^[1-9]%d*$")
        and type(v.tag_name) == "string"
        and type(v.html_url) == "string",
      "release schema"
    )
    assert(
      v.html_url:sub(1, #("https://github.com/" .. c.repository .. "/releases/"))
        == "https://github.com/" .. c.repository .. "/releases/",
      "canonical repository release link"
    )
    assert(v.draft == false, "public release required")
    -- GitHub does not supply a dependable release content revision in this
    -- contract. Published identity/tag is a source-declared version, NOT a hash
    -- of body edits. Snapshot originals remain available for exact comparison.
    local revision = v.tag_name
    assert(#revision > 0 and #revision <= 128, "release version bound")
    local title = (type(v.name) == "string" and #v.name > 0) and v.name or v.tag_name
    items[#items + 1] = {
      id = id,
      revision = revision,
      title = title,
      link = v.html_url,
      published = type(v.published_at) == "string" and v.published_at or nil,
      updated = type(v.updated_at) == "string" and v.updated_at or nil,
    }
  end
  return {
    items = items,
    repository = c.repository,
    original_ref = r.output_ref,
    fetched_at = "unavailable (see native attempt timing)",
  }
end
return M
