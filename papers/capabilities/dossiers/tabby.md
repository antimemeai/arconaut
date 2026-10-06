# tabby

Tabby is primarily a reusable inference/retrieval/answer service plus editor completion and preview-edit assistant. Its client named agent does not establish an autonomous model-native shell/tool continuation loop.

Role: inference/retrieval service and editor coding assistant. Runtime: Rust, TypeScript, JavaScript.

Pinned source: [https://github.com/TabbyML/tabby](https://github.com/TabbyML/tabby); revision/version `21b29048d7bcf6b94f9f482f2d0fd05efadfd19f`.

Rust Tokio/Axum inference/answer service with Arc trait backends and DB; TypeScript language-server/editor client produces previews under single edit mutex.

Server owns inference/retrieval/thread storage and supplies compatible APIs; editor agent consumes it and delegates actual workspace mutations to editor. Pochi link is separate agent, not engine hidden in these paths.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### serve / download

Surface: operator CLI.

Input: configured models/device/server arguments or model ID

Result: running inference/answer service or downloaded model

Lifecycle: Tokio process; startup config load; download internals untraced

Authority: operator/service owns resources

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### POST /v1/chat/completions / /v1beta/chat/completions

Surface: service API.

Input: compatible chat request messages/model/user fields

Result: SSE provider chunks/error

Lifecycle: forwards to configured backend; connection stream lifecycle, no execution-loop settlement

Authority: authorized client/user headers; model no OS tool authority

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6).

### AnswerService.answer / ThreadService.create_run

Surface: answer/thread API.

Input: user,thread messages,query/options/attachments/model_name

Result: thread/user/assistant IDs,reading/attachments/content delta events

Lifecycle: fixed retrieval/related-question/final-answer pipeline; persists selected state as it streams

Authority: user source access policy; service performs predetermined retrieval

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### tabby/chat/edit

Surface: editor request.

Input: location,command,previewChanges format

Result: edit token + preview stream or feature/length/mutex error

Lifecycle: one current edit under global mutex, selected-context template provider request

Authority: operator/editor client, cancellation aborts request

Evidence: [e16](#evidence-e16), [e18](#evidence-e18), [e19](#evidence-e19), [e22](#evidence-e22).

### tabby/chat/edit/resolve

Surface: editor request.

Input: preview location,accept/discard/cancel

Result: bool; WorkspaceEdit or controller abort

Lifecycle: reconstructs desired preview lines then applies via host editor

Authority: operator/host owns final workspace mutation

Evidence: [e20](#evidence-e20), [e23](#evidence-e23).

### tabby/chat/smartApply

Surface: editor request.

Input: location,text patch

Result: bool/edit preview or error

Lifecycle: range heuristic then possible LLM fallback and edit stream under mutex

Authority: operator/editor host

Evidence: [e21](#evidence-e21), [e24](#evidence-e24).

### fetchFileContent / ReadFileRequest

Surface: editor context primitive.

Input: URI and optional range

Result: synced/delegated/local text or undefined

Lifecycle: reads context before edit; does not expose general model filesystem tool

Authority: language-server/editor authority

Evidence: [e17](#evidence-e17).

### ConfigFile.watch / updated

Surface: hot configuration primitive.

Input: client config file change

Result: new/old configuration event

Lifecycle: reload on file add/change; active edit already captured config

Authority: operator/client host; not executable reload

Evidence: [e18](#evidence-e18), [e25](#evidence-e25).

### DbConn.create_thread / list_threads / message updates

Surface: storage primitive.

Input: user/thread/message IDs,content/attachments

Result: rows/IDs or DB error

Lifecycle: durable mutable application records; no general model SQL channel

Authority: owned service database

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

## Capabilities

### filesystem

**S — Files** (source): Editor context reads synced/delegated/local files and accepted previews use host WorkspaceEdit; no arbitrary model read/write tool in answer route.

Evidence: [e17](#evidence-e17), [e20](#evidence-e20).

### processes

**— — OS programs** (inspection-limit): Inspected inference/answer/editor service supplies no model command-execution lifecycle; local server inference infrastructure is not agent shell authority.

### code-actions

**L — Code actions** (source): Editor tabby/chat/edit and smartApply generate a preview/token then accept/discard WorkspaceEdit. These are proposed code edits; model-authored executable programs composing native tools are not established by inspected edit/completion/answer paths.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24).

### persistent-kernel

**— — Kernel** (inspection-limit): Reference inference service/editor assistant has no inspected execution-kernel consumer contract; model inference process is not operator language kernel.

### standing-database

**S — Standing DB** (source): Server persists thread/message/attachment state through DB operations and authorized retrieval context. Internal application storage is not model general database execution.

Evidence: [e7](#evidence-e7), [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### workflow-programming

**S — Workflows** (source): REST/LSP and trait backend API enable host composition; editor template/preset context plus fixed answer pipeline. Model-authored workflow/turn program not established.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e16](#evidence-e16), [e19](#evidence-e19).

### multi-model

**L — Models** (source): Configured inference model default/support fallback and answer model_name selection are actual routing; no independent agents or mixed-model conversation authority established.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e8](#evidence-e8).

### live-collaboration

**— — Peer chat** (inspection-limit): Thread sharing/application users and separate Pochi link do not establish live agent-to-agent collaboration; inspected component role is inference/retrieval/editor assistance.

### concurrent-work

**S — Concurrency** (source): Tokio/Axum service supplies independent request streams and joined document enrichment; editor edits explicitly global-mutex serialized. No multi-agent work scheduler established.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e8](#evidence-e8), [e18](#evidence-e18), [e21](#evidence-e21).

### steering-interrupt

**L — Steer/interrupt** (source): Editor cancellation/stop/cancel maps to request abort; no ongoing model-task steering/queue or all owned inference process quiescence contract traced.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e21](#evidence-e21).

### turn-redefinition

**— — Turn program** (inspection-limit): Fixed answer-request/retrieval and one-shot editor edit paths are service contracts, not an exposed autonomous agent turn scheduler.

### compaction

**? — Compaction** (inspection-limit): No semantic context compaction/projection mechanism established in inspected answer request construction/thread persistence/editor template paths.

### context-repair

**S — Repair** (source): Persistent thread data and editor reread context support host reconstruction, but no native model original-context fetch/repair protocol established.

Evidence: [e10](#evidence-e10), [e12](#evidence-e12), [e17](#evidence-e17).

### original-audit

**L — Original audit** (source): Thread deltas persist into mutable content/attachments; coarse completion logger records operation event, not original prompts/chunks/tool/OS effects. Edits/debug templates may log locally but no complete immutable audit.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e11](#evidence-e11), [e13](#evidence-e13), [e19](#evidence-e19).

### audit-query

**S — Audit query** (source): DB thread listing/message records are queryable application state; not immutable originals or complete event history.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13).

### hot-change

**L — Hot change** (source): Client config file data refresh and health-based feature registration are wired; server main startup loads config once. No compiled executable handoff or affected-workflow fence established.

Evidence: [e2](#evidence-e2), [e14](#evidence-e14), [e18](#evidence-e18), [e25](#evidence-e25).

### rebuild-continuity

**— — Rebuild continuity** (inspection-limit): Server/editor client lacks inspected agent continuity/refit lifecycle; API clients could compose continuity externally but it is not supplied here.

### remote-services

**I — Remote** (source): Compatible service API and remote provider backend trait with reusable Arc inference/retrieval components suit an externally consumed service role. Editor host owns workspace mutation.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e6](#evidence-e6), [e7](#evidence-e7), [e20](#evidence-e20).

### self-improvement

**— — Self-improve** (inspection-limit): No self-directed coding-agent improvement loop in inspected service/editor role; standalone Pochi is a different reference.

### complaints

**? — Complaints** (inspection-limit): No native model grievance filing or independently tracked state bundle established in inspected API/editor/thread paths.

### authority

**I — Authority** (source): User policy filters repository/doc retrieval sources; editor host accepts/discards changes and uses synced/delegated files. No provider model command authority inferred.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e17](#evidence-e17), [e20](#evidence-e20), [e23](#evidence-e23).

### evaluation

**S — Evaluation** (source): Fake answer/retrieval test only asserts stream count; local model/SSE goldens snapshot seeded text. Neither proves edit correctness, complete audit or broad agent behavior; not run.

Evidence: [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29).

### time-order

**S — Time/order** (source): Thread IDs/create/update timestamps and per-stream persist-before-yield order provide application sequencing, not global effect/audit causal order.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

## Inspected test oracles

- [quarantine/tabby/ee/tabby-webserver/src/service/answer.rs](../../../quarantine/tabby/ee/tabby-webserver/src/service/answer.rs): Fixed retrieval/answer pipeline shape Oracle: Fake auth/chat/code/doc/context and in-memory DB assert six stream events; no live semantic retrieval/edit oracle. Read, **not executed**.
- [quarantine/tabby/crates/tabby/tests/goldentests_chat.rs](../../../quarantine/tabby/crates/tabby/tests/goldentests_chat.rs): Local inference SSE response snapshots Oracle: Would spawn local model/server and compare collected seeded text; platform-specific and requires inference artifact. Not run; snapshots do not assert coding task success. Read, **not executed**.

## Useful mechanisms

- Actual compiled Rust service core, with editor client cleanly consuming API and owning UI/preview choices.
- Authorized source retrieval and stored thread attachments expose relevant service/context boundary.
- Explicit edit token, cancellation and accept/discard WorkspaceEdit lifecycle.

## Material limits

- Not an autonomous coding harness in inspected paths; Pochi tasks are another repository.
- Model selection, shareable threads and internal DB must not become multi-agent collaboration or model standing SQL claims.
- Global editor edit mutex limits concurrent preview work.
- Mutable thread state/coarse completion events lack full original audit.
- Client config refresh does not imply server hot compiled refit or active agent continuity.

## Arconaut design questions

- What inference/retrieval APIs should Arconaut consume from independent services, preserving strong systems implementation without becoming their governor?
- Keep request tokens and UI preview/edit lifecycle separate from model-native execution/workflow authority.
- Retain service request/response provenance in Arconaut audit even when provider/reference stores only mutable thread content.

## Evidence

### Evidence e1

[quarantine/tabby/README.md:17–36](../../../quarantine/tabby/README.md#L17): Tabby describes self-hosted coding assistant; Pochi task agent is linked separately, not this service engine.

### Evidence e2

[quarantine/tabby/crates/tabby/src/main.rs:53–73](../../../quarantine/tabby/crates/tabby/src/main.rs#L53): Rust Tokio main loads config once, prepares owned root and dispatches serve/download CLI.

### Evidence e3

[quarantine/tabby/crates/tabby/src/serve.rs:289–326](../../../quarantine/tabby/crates/tabby/src/serve.rs#L289): Axum mounts configured completion and chat SSE endpoints; absent completion has explicit NOT_IMPLEMENTED route.

### Evidence e4

[quarantine/tabby/crates/tabby/src/routes/chat.rs:38–85](../../../quarantine/tabby/crates/tabby/src/routes/chat.rs#L38): Chat endpoint forwards request to trait backend, streams chunks and logs coarse ChatCompletion event; no native tool execution/continuation in this route.

### Evidence e5

[quarantine/tabby/crates/tabby-inference/src/chat.rs:44–72](../../../quarantine/tabby/crates/tabby-inference/src/chat.rs#L44): Configured remote model processing fills default or falls back unsupported models and adapts provider-specific fields.

### Evidence e6

[quarantine/tabby/crates/tabby-inference/src/chat.rs:110–145](../../../quarantine/tabby/crates/tabby-inference/src/chat.rs#L110): Backend ChatCompletionStream invokes configured compatible/Azure chat create/create_stream; provider selection is not peer collaboration.

### Evidence e7

[quarantine/tabby/ee/tabby-webserver/src/service/answer.rs:67–133](../../../quarantine/tabby/ee/tabby-webserver/src/service/answer.rs#L67): Answer stream reads authorized context, fixed pipeline decides code needs, lists capped files and retrieves snippets.

### Evidence e8

[quarantine/tabby/ee/tabby-webserver/src/service/answer.rs:135–214](../../../quarantine/tabby/ee/tabby-webserver/src/service/answer.rs#L135): Answer pipeline filters authorized doc sources, concurrently enriches attachments, optionally asks related questions and constructs final chat request with model selection.

### Evidence e9

[quarantine/tabby/ee/tabby-webserver/src/service/answer.rs:216–264](../../../quarantine/tabby/ee/tabby-webserver/src/service/answer.rs#L216): Answer emits content deltas from one final chat stream and logs coarse completion event; no arbitrary tool-call loop traced.

### Evidence e10

[quarantine/tabby/ee/tabby-webserver/src/service/thread.rs:156–220](../../../quarantine/tabby/ee/tabby-webserver/src/service/thread.rs#L156): Thread create_run loads messages, requires last user, creates assistant message ID before answer and emits paired identities.

### Evidence e11

[quarantine/tabby/ee/tabby-webserver/src/service/thread.rs:222–267](../../../quarantine/tabby/ee/tabby-webserver/src/service/thread.rs#L222): Stream persists delta text and overwrites selected attachments/relevant questions before yielding each item.

### Evidence e12

[quarantine/tabby/ee/tabby-db/src/threads.rs:23–101](../../../quarantine/tabby/ee/tabby-db/src/threads.rs#L23): Persistent thread messages have content/attachments/timestamps and DB list/create operations with user filtering.

### Evidence e13

[quarantine/tabby/ee/tabby-db/src/threads.rs:179–197](../../../quarantine/tabby/ee/tabby-db/src/threads.rs#L179): Attachment updates overwrite current JSON data and updated_at; mutable application data, not immutable original event history.

### Evidence e14

[quarantine/tabby/clients/tabby-agent/src/chat/index.ts:29–54](../../../quarantine/tabby/clients/tabby-agent/src/chat/index.ts#L29): Editor chat capability dynamically registers/unregisters when server health exposes chat model.

### Evidence e15

[quarantine/tabby/clients/tabby-agent/src/server.ts:86–122](../../../quarantine/tabby/clients/tabby-agent/src/server.ts#L86): Language-server host constructs/registers chat edit, smart apply and command/completion features.

### Evidence e16

[quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts:45–55](../../../quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts#L45): LSP requests wire edit command listing, edit creation and edit resolution.

### Evidence e17

[quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts:105–140](../../../quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts#L105): Context file read uses synced documents, delegated workspace read or local filesystem fallback.

### Evidence e18

[quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts:143–186](../../../quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts#L143): Inline edit requires preview format/synced document and available chat; document length limit and global edit mutex, cancellation requests abort.

### Evidence e19

[quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts:248–314](../../../quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts#L248): Template composes context into one chat request, passes mutex abort signal, returns edit token and supports current-edit stop/cancel.

### Evidence e20

[quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts:364–391](../../../quarantine/tabby/clients/tabby-agent/src/chat/inlineEdit.ts#L364): Accept/discard resolution reconstructs preview text and applies WorkspaceEdit through editor host.

### Evidence e21

[quarantine/tabby/clients/tabby-agent/src/chat/smartApply.ts:38–131](../../../quarantine/tabby/clients/tabby-agent/src/chat/smartApply.ts#L38): SmartApply exposes separate request, computes range or asks LLM fallback then edit generation under same mutex/abort.

### Evidence e22

[quarantine/tabby/clients/tabby-agent/src/protocol.ts:395–404](../../../quarantine/tabby/clients/tabby-agent/src/protocol.ts#L395): Native protocol edit request name is tabby/chat/edit and returns ChatEditToken.

### Evidence e23

[quarantine/tabby/clients/tabby-agent/src/protocol.ts:475–489](../../../quarantine/tabby/clients/tabby-agent/src/protocol.ts#L475): Native tabby/chat/edit/resolve supports accept/discard/cancel at preview location.

### Evidence e24

[quarantine/tabby/clients/tabby-agent/src/protocol.ts:549–558](../../../quarantine/tabby/clients/tabby-agent/src/protocol.ts#L549): Native SmartApply protocol name tabby/chat/smartApply returns bool or typed feature/document/mutex errors.

### Evidence e25

[quarantine/tabby/clients/tabby-agent/src/config/configFile.ts:137–149](../../../quarantine/tabby/clients/tabby-agent/src/config/configFile.ts#L137): Client config file watcher reloads data and emits update on changed content; no deferred workflow fence.

### Evidence e26

[quarantine/tabby/ee/tabby-webserver/src/service/answer.rs:629–704](../../../quarantine/tabby/ee/tabby-webserver/src/service/answer.rs#L629): Answer test uses fake chat/code/doc/context/auth and in-memory DB, asserting six streamed items.

### Evidence e27

[quarantine/tabby/crates/tabby/tests/goldentests_chat.rs:49–69](../../../quarantine/tabby/crates/tabby/tests/goldentests_chat.rs#L49): Golden harness would launch real local Tabby inference server/model with kill-on-drop process.

### Evidence e28

[quarantine/tabby/crates/tabby/tests/goldentests_chat.rs:91–132](../../../quarantine/tabby/crates/tabby/tests/goldentests_chat.rs#L91): Golden oracle collects SSE text then snapshot asserts, not coding correctness or full audit.

### Evidence e29

[quarantine/tabby/crates/tabby/tests/goldentests_chat.rs:136–161](../../../quarantine/tabby/crates/tabby/tests/goldentests_chat.rs#L136): Seeded Metal platform golden examples request generic Python/regex answers; tests not executed.

