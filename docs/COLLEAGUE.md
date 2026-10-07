# Callable native colleague slice

Build in this checkout:

```sh
cmake --preset release
cmake --build build/release --target arco-colleague colleague_test
ctest --test-dir build/release -R '^colleague$' --output-on-failure
build/release/arco-colleague request.json context/new-colleague-call
```

The binary is independent of the harness. An ordinary audited `exec` tool can invoke
it; no harness restart or dynamic tool registry is necessary. A library caller uses
`prepare_colleague`, `call_colleague` and `native_colleague_transport` from
`include/arconaut/colleague.hpp`. Injected transports support counted direct tests.
The capture callback must retain bytes or throw; admission capture failure prevents
dispatch. Do not use a no-op capture in a production caller.

Example request (all shown fields required):

```json
{
  "request_id": "diagnosis-1",
  "from": "arco-openai",
  "to": "source-colleague",
  "provider": "claude",
  "model": "sonnet",
  "task": "Identify the source-defined failure cases and concrete expected results.",
  "context": [{"id": "source:version:file:lines", "text": "selected source bytes"}],
  "profile": {
    "name": "context-only",
    "provenance": "operator/local",
    "timeout_seconds": 90,
    "tools": "none",
    "requests": 1
  }
}
```

Use provider `openai` with a configured Codex subscription model, or `claude` with
an installed/authenticated Claude CLI model/alias. Caller identity is not a provider
pairing gate: either can address either, including the same provider. No claim of
Kimi/MiMo/Grok support is made by this adapter. Unknown provider/profile keys refuse
rather than silently degrade. Provider services/CLIs are consumed, not governed;
no SDK or library is adopted. Installed/account-discovered does not imply working
authentication. This lane's Claude live attempt failed with expired OAuth.

## Effective limits and actual capabilities

One synchronous context-only request per invocation; 64KiB serialized request;
unique selected-context IDs; timeout1..3600 seconds; inherited native transport
response/output capacity16MiB. Context may be empty. Nonempty selected texts only.
Profile name/provenance are explicit caller metadata, not an authorization policy.
There is no hidden provider-pair ceiling. Tool execution, workspace access, nesting,
detachment, continuation, concurrent rooms, usage/currency budgets, capability
catalogue, model obedience and a sandbox are NOT delivered. Unknown usage is null,
not zero spend. Timeouts bound native request/CLI collection, not a transaction over
all external inference or auth-refresh time. The outer audited caller must retain
its own admission and unknown disposition if the independent binary is interrupted.

OpenAI uses existing native Codex auth/HTTP/SSE completion and advertises no tools.
Claude uses literal argv, print JSON, no tools/skills, empty settings sources and
MCP configuration, custom system prompt and no saved conversation continuation.
No parent history or project files are automatically supplied by Arco. External CLI
ambient behavior is not OS isolation; flags cannot guarantee model instructions
are obeyed or prevent an independently changed upstream client from misbehaving.

## Results, retention and recovery

Reply includes request_id, reversed from/to addresses, provider, requested_model,
actual_model (string or null), usage (provider JSON or null), effective profile,
status and remote_disposition. Claude's actual model map is separately retained as
model_usage; actual_model is null if the map is absent/ambiguous. Never substitute
a requested alias for observed model. Full upstream result retains richer metadata.

- `completed`: structurally complete upstream answer with nonempty text. It is not
  a judgment of correctness. OpenAI output must be assistant output_text, not a tool.
- `refused/not_dispatched`: invalid input, unsupported provider/profile or capacity.
- `refused/completed`: upstream completed response contains a refusal, not an answer.
- `failed`: observed nonzero CLI exit or explicit structured error. Local exit does
  not establish cancellation of remote work; remote disposition may remain unknown.
- `unknown`: timeout, transport/parse failure, malformed types or incomplete result.
  Partial prose, zero exit alone, and an empty completed provider response are not
  completed colleague answers. Inspect retained bytes; do not automatically retry.

Capture directory must be new; existing directory refuses before dispatch. It is
owner-only. admission.json records prepared request/argv before dispatch, raw.bin
retains received stream/chunks, upstream_result.json retains structured transport
result, decoded_result.json retains CLI result where available, result.json records
observed disposition. Files/capture failures stop rather than fabricate retained
bytes. A killed binary may leave only admission/partials: treat that as unknown.
These are local inspection captures, not a new crash-transactional core journal;
raw bytes are flushed but no power-loss durability guarantee is claimed. Calls
through the harness's existing audited exec seam keep its independent outer audit.
Credential acquisition stays inside the existing transport/installed client. Never
put secrets in selected context; Claude prompt bytes appear in process arguments.

The CLI exits0 only for completed,1 for other outcomes,2 for usage/existing directory.
Exit status alone does not settle remote effects. Reading old captures is recovery;
there is no replay/resume command. A new request requires an explicit caller choice
with retained prior uncertainty, not a background retry loop.
