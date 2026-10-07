# Opt-in Lua packages and thematic feed HUD

G8 delivers a lean Lua seam, not a plugin SDK, service manager or binary ABI.
There is no automatic directory scan, network polling, account setup or connector
advertising. Ordinary Arco works with zero packages. Packages are trusted Lua
code, not sandboxed third-party plugins; top-level effects cannot be rolled back.
The shipped modules are inert on import and tested for zero disabled fetches.
Read `packages/github-releases.json` for metadata without importing its source.
Installed (source available), enabled, configured and received are distinct facts.

Install the retained modules explicitly through `program_config`, preserving any
existing effective modules/defaults when proposing the complete configuration:

- `packages`: source `programs/packages.lua`.
- `github_releases`: source `packages/github_releases.lua`.

Successful workflow boundary publishes sources; a later workflow uses
`arco.module`. Lua-only source needs no native rebuild/RRC. Import itself does not
fetch; the model/operator decides when to receive data and how to consume it.

```lua
local P = arco.module('packages')
local G = arco.module('github_releases')
local feed = P.new({id='cpp-lua', revision='1',
  topic='C++/Lua toolchain releases', max_items=2})
feed:install(G)                         -- metadata/callback registration only
feed:enable('github-releases', {repository='llvm/llvm-project', limit=2})
local receipt = feed:receive('github-releases', arco) -- one native audited fetch
local text = feed:display('github-releases')         -- no inference/context/work
arco.call('write_file', {path='context/toolchain-hud.md', content=text})
-- Show text in a separate terminal, or ordinary Arco presentation; no TUI pane.
-- Choose an actual ID from this snapshot, not the example below.
local selected = feed:select('github-releases', '404597416')
feed:include('github-releases', selected.id, arco.append) -- explicit context sink
local event = feed:event('github-releases', selected.id,
  'Inspect release relevance to our declared compiler; do not upgrade anything.')
-- Explicitly publish a source-owned events.json for a selected station.
-- Preparing the event is NOT native admission, execution or completion.
arco.call('write_file', {path='context/feed-adapter/events.json',
  content=arco.json.encode(arco.array({event}))})
```

Create the adapter directory before writing: native `write_file` does not create
parent directories. Check every native result (`assert(not result.error)`) before
claiming publication; a returned error is not a thrown exception. The example
paths assume existing parent directories.

For a live adapter use atomic replacement (temporary file plus rename), not an
in-place edit visible to its station. See [STATION](STATION.md) for admission,
source-owned backlog, boundary pause/control, unknown/no-replay semantics and
snapshot bounds. Native source/id identity deduplicates a prepared event when
explicitly routed to that session; package instances do not own durable dispatch.
Native `workflow_returned` does not certify output or remote effects.

## Receipt, display, context and action are separate

`receive` validates the whole snapshot before replacing the preceding view;
`original_ref` points to exact retained native exec output. Failure/cancellation
propagates, leaves the prior snapshot available with `last_receive=unknown`,
and never retries. Source errors can be inspected with `read_process_output`.
A missing/truncated body is not a successful empty feed.

`display` returns an ASCII text HUD of external project developments: source item,
declared version, link, supplied publication date, and freshness/original locator.
It removes terminal controls/non-ASCII, is derived and repairable, and is not a
runtime-status dashboard. Reading/rendering the text doesn't add it to context.

`context` prepares one labelled UNTRUSTED user item; `include` explicitly sends
it to the supplied append callback. The local included counter counts only
callbacks that returned, not arbitrary sink correctness or provider consumption.
Actual request inclusion is established by native request/context observations.

`event` prepares an explicitly prompted native station input and checks the
combined source/id/cursor/prompt limits. It does not dispatch. The `actions`
counter stays zero because the package performs no work admissions. Only native
station status/audit establishes actual admission/outcome; neither a display
counter nor a prepared event is an action receipt.

## Contract and limits

An explicitly supplied adapter has `metadata={id,version=1}`, optional pure
`configure(config)`, and `fetch(config,api)` returning `items`, `original_ref`,
`fetched_at`. Items have bounded `id`, declared `revision`, `title`, `link`, and
optional `published`/`updated`. A profile names a topic, ID/revision and1..32 item
limit. At most16 packages per instance. Snapshots are workflow-local; export a
view/receipt explicitly when needed. No credentials are stored by this seam.

Public GitHub releases use the official list endpoint, at most8 items/512KiB,
one25-second HTTPS curl request, no redirects, curlrc, retries or authentication.
Rate limits/access failures are visible native attempts, not empty success.
Release tag is a source-declared version, **not a digest of release-note edits**;
repeated tags do not schedule body-edit work. This is a bounded current snapshot,
not a complete event stream, cursor acknowledgement or missed-release recovery.
Missing wall-clock fetch timestamp is marked unavailable; native audit retains
attempt/timing identity. Fetch again explicitly when useful, not on every paint.
No automatic freshness/cadence guarantee or cross-instance quota governor.

LLVM is a thematic C++ toolchain watch, not a request to adopt/upgrade a compiler.
No vulnerability coverage, arXiv parser, external posting, hosted room, graphical
HUD pane, auto-scheduler or third-party package safety claim in this slice.
