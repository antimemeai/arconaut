# Native decision models

First delivery: one `decision_model` interface in model tools, Lua `blackbird.call`
and `/decision JSON`. Jev is the initial adapter. No Python/SDK runtime, dependency
or automatic confidence/approval policy. Use the existing C++ Child/curl transport
pattern; auth travels through stdin, never command arguments or the audit.

Written subplan, 2026-10-07: native request/response validation and lazy credential
loading, audited CodingEngine operation, model tool/slash discovery, direct fake
transport/native Lua tests, one small live batched smoke and one review/recheck.
One 25-minute implementation period, <=j2 build. Do not expand into caches,
automatic harness routing, statistical calibration, plugin discovery or a laboratory.

Grounding: current [HTTP API](https://docs.typesafe.ai/api),
[models](https://docs.typesafe.ai/models), Choice/Score/Noul pages and workspace
JEV.md, plus inspected owned codex-tools/jev.py and native OpenAI/Beads adapters.
Live docs checked 2026-10-07. Choice has up to 255 alternatives; Score 2–10 levels;
state/instructions/criteria preserve documented structured shapes. Same request
question IDs and answer types/options/levels must match. Retain actual model,
probabilities, confidence where provided and usage. Pinned models must match;
aliases may resolve to a concrete version. Typed output is not truth/permission.

The model/Lua request supplies provider (default `jev`), model (default pinned
`jev-1.13.0`), state and questions, optional timeout_seconds. No credential or
endpoint in model arguments. Native configuration controls transport executable,
byte limits and credential file. Credentials resolve lazily from TYPESAFE_API_KEY,
then jev, then BLACKBIRD_JEV_ENV_FILE. The local launcher may select its workspace
parent .env when present, exporting only the path. Direct binary installs use env
or explicit path. Never read secrets at startup or copy them into session data.

One batch is one admitted audited effect. Exact request/response bytes and typed
result link to the operation attempt. Cancellation/transport ambiguity stays
explicit; no hidden retry/cache. Workflows may compose probabilities and perform
an explicit later call. Pre-effect admission and recovery fences remain in the
existing CodingEngine operation path; Jev does not become a conversational provider.

Direct checks: malformed requests reject before any transport; missing/mismatched
answers/options/levels/model and invalid numbers reject; exact valid batch returns
unchanged judgments; secret absent from argv/audit; real Child stdin transport,
HTTP/oversize/cancellation failures; Lua call shares effect linkage. Live smoke is
transport/use evidence only, not calibration. Normal build DEBUG OFF/profiling OFF.

Use from a Lua workflow:

```lua
local result = blackbird.decide {
  state = {message = "Compilation failed with mismatched types."},
  questions = {
    failed = {
      type = "noul",
      instructions = "Does state.message report a failed compilation?"
    }
  }
}
local probability = result.response.answers.failed.noul
-- The workflow chooses what to do with this judgment.
```

The model sees the same argument shape through `decision_model`. The operator can
submit `/decision {"state":{"message":"Compilation failed"},"questions":{"failed":{"type":"noul","instructions":"Does state.message report compilation failure?"}}}`.
Native callers use `DecisionModels::evaluate`; CodingEngine supplies admitted
effects, attempt linkage, and raw request/observed-response capture. Direct native
callers supply their own observer and effect ownership.

Default timeout is 30 seconds; explicit values range from 1 to 3,600 seconds.
Host request/response byte bounds default to 1 MiB each; these are transport bounds,
not claims about provider token capacity. HTTP, incomplete transport, invalid
responses and cancellation report errors. Raw observed stdout, including failure
bodies, is retained; quiet curl stderr is discarded. No request is automatically
retried. Score legends are checked against string criteria; structured criteria
retain the provider's string legend with exact level keys.

The first adapter is Jev only. Configured credential_file precedes
BLACKBIRD_JEV_ENV_FILE after environment key resolution. Credentials never become
model arguments. This implementation is a candidate until explicitly integrated;
it does not replace the workspace development client.
