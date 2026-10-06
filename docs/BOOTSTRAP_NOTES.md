# Bootstrap implementation notes

2026-10-02. Operator selected OpenAI and directed implementation now, with Kimi
challenge later. Implement one useful coding path, using the existing root journal.

Own the JSON representation/parser so opaque Responses items, encrypted reasoning,
tool-call IDs, exact numeric lexemes and extensible fields survive context edits.
Use OpenAI Responses with local item replay and store:false; preserve output items
in order and append function_call_output by call_id. Source grounding:
https://developers.openai.com/api/docs/guides/function-calling
https://developers.openai.com/api/docs/guides/conversation-state

Use the installed curl executable for HTTPS, invoked with an argument vector and
private input/config descriptors; credentials must not appear in argv or audit.
No TLS implementation, HTTP library or Python production wrapper. Model is runtime
configuration. Operator selected the existing Codex/ChatGPT login on 2026-10-02.
The installed native Codex 0.160.0 is used only as the authentication authority:
initialize/initialized/getAuthStatus; never thread/start or turn/start. It owns token
refresh and shared credential persistence. Arco pins account identity across the
RPC and file reload, and does not write/rotate Codex credentials. Credentials stay
outside argv, context and request audit. Explicit CODEX_HOME is passed only in the
child environment. Keychain-only storage is not implemented in this first bridge;
this operator's existing file-backed login is the qualified path.

Existing Codex credentials use the matching subscription endpoint from studied
Codex source (model-provider-info/src/lib.rs to_api_provider), not API-key routing.
Live qualification: api.openai.com/v1/models refused this credential with HTTP403;
chatgpt.com/backend-api/codex/models returned10 models. The newly documented public
Sign in with ChatGPT partner OAuth flow uses api.openai.com/v1/responses, but has
different client registration/permissions; importing this existing Codex session
does not establish that entitlement. No separate partner registration is introduced.
Source: https://learn.chatgpt.com/docs/auth and
https://developers.openai.com/siwc/token-sharing-open-source/models-and-inference

Responses require stream:true/store:false. Live Codex completion output can be empty
while response.output_item.done carries final items. Retain those complete opaque
items in output_index order; reject duplicate/gapped/conflicting finals and streams
without response.completed. Never reconstruct reasoning or tool calls from text
deltas. HTTP errors retain status metadata, not credential-bearing transport config
or unfiltered auth errors. No automatic transport retries are introduced; a401 can
request guarded Codex refresh, with attempts accounted for by the workflow owner.

Use installed Lua5.4.8 with narrow owned C bindings for the workflow, tools and
context operations. No binding framework; Lua stack/longjmp boundaries must not
cross live C++ owning locals. Native process/file execution and terminal client
stay owned C++. curl handles transport rather than governing turn semantics.

Audit application packets through a typed generic application record with source
dependencies in the existing ledger. Context edits keep immutable candidate/base/
outcome and originals; CAS failure cannot overwrite the head. Derive restored
context only from typed context records, never arbitrary tool-output magic bytes.
Final requests/responses and tool inputs/outputs are retained. Audit before effects;
recovered attempts never redispatch. Single-journal capacity/uncertainty closes new
work visibly; no journal continuation or surviving-worker reconnection is added now.

Direct checks: JSON independent expected values and malformed/Unicode/limit cases;
context stale edit, repair from originals and opaque item preservation; real file
edit and child command output; scripted Responses tool roundtrip with exact next
request; bounded live OpenAI coding exercise when credentials are available.

Coding-loop slice: add wire kind14 ApplicationRecordEvent (typed channel + unique
record ID + metadata bytes), sharing root identity/dependency/durability checks.
Keep raw provider/tool streams in source frames, referenced by the typed record.
CLM revisions use explicit base/current/candidate/outcome; original entry mapping
is owned separately from presentation and reconstructed only from context-channel
facts. Prepare context caches before append acknowledgement; stale/invalid edits
retain candidates and never replace the current head. No arbitrary source packet
is interpreted as context authority.

Native local tools are synchronous bootstrap clients: file read/write/exact edit,
argv/shell execution with bounded captured output and timeout. The root dispatch
seam admits every tool/provider operation before effects; record terminal outcomes
and raw originals before incorporating results. Reopen refuses any nonterminal
operation without inventing custody reconciliation. Lua controls request/tool
iteration and can inspect/edit/restore context or run arbitrary transformations;
load a fresh workflow environment each turn, retain its effective source/generation.
A thin line terminal supplies /context, /restore, /model and /quit initially.
Meaningful direct checks: actual stale/invalid context edits and reopen/restore;
actual child output/file edit; scripted model/tool roundtrip; real model source edit
and build/test in a scratch Arco fixture. Broader interface/refit follows useful loop.

Lua binding qualification uses5.4.8: table traversal reserves1024 stack slots before
C++ ownership, uses raw nonallocating access only, and stages callback return values
in runtime-owned storage before allocating/pushing into Lua. Exceptions are caught
before lua_error. JSON null, empty arrays and exact number lexemes have distinct
registry-backed Lua tags; new empty arrays use arco.array({}). Fresh VM/program per
turn, effective source and generation retained. arco.request(options) can redefine
assembly/model/tools/reasoning; subscription stream/store constraints are explicit.
Local tools capture prior/proposed file bytes before writing and output chunks before
buffering, through root journal sources. Failed provider transport ends with an
explicit unknown disposition; no automatic replay/retry of a truncated tool call.

Lua's ordinary host effects use `arco.call` and display uses `arco.display`
(including `print`). Pure base/coroutine/table/string/math/utf8 libraries are
available. Raw io/os/package/debug and file-loading globals are absent: module
source can be read with the audited read_file tool and evaluated with `load`.
This preserves operator/model agency without silently skipping audit records;
it is not a hostile-code isolation sandbox. A direct fixture first failed with
raw libraries present, then checks their absence and retained display.
