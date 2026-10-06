# sly

Addressed live evaluation, native compile/load, debugger restarts and inspectable recordings offer model-ergonomics ideas; this is not an agent loop.

Role: Emacs/Lisp live development client and RPC server. Runtime: Emacs Lisp, Common Lisp.

Pinned source: [https://github.com/joaotavora/sly](https://github.com/joaotavora/sly); revision/version `3ffa216d0818972f7a7fea38a566a6b570349f3b`.

Emacs frontend talks to SLYNK inside a long-lived Lisp; communication can use worker threads or single-thread styles.

Client consumes a Lisp runtime; SLYNK evaluates with that runtime authority.

Inspection: Read RPC dispatch/replies/authentication, eval/compile/load APIs, recordings and sticker tests.

Limits of this study: No complete backend-specific restart/interrupt review; no model provider, complete audit store or refit coordinator.

## Actions

### interactive-eval / eval-and-grab-output

Surface: supplied primitive.

Input: Lisp source string and package

Result: Printed values, captured output or condition

Lifecycle: Persistent Lisp process; request continuation settles.

Authority: RPC client with server authority

Evidence: [actions](#evidence-actions), [eval](#evidence-eval).

### compile-file-for-emacs / compile-string-for-emacs

Surface: supplied primitive.

Input: Path or source, load flag, source positions and policy

Result: Compiler notes and compilation/load result

Lifecycle: In-place development; hook-defined completion.

Authority: RPC client

Evidence: [compile](#evidence-compile).

### :emacs-interrupt / :emacs-channel-send

Surface: supplied primitive.

Input: Thread or channel ID and message

Result: Interruption/channel event or invalid-channel/thread reply

Lifecycle: Independent connection dispatch; backend determines cancellation.

Authority: Connected client

Evidence: [dispatch](#evidence-dispatch).

### compile-for-stickers

Surface: supplied primitive.

Input: Instrumented/original source and sticker IDs

Result: Notes, armed state, later value/condition recordings

Lifecycle: Live instrumentation; in-memory retention.

Authority: Connected development client

Evidence: [record](#evidence-record).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**S — Code actions** (source): Executable Lisp forms with values and output capture over addressed RPC.

Evidence: [actions](#evidence-actions).

### persistent-kernel

**S — Kernel** (source): Evaluation occurs inside the same Lisp process and package environment; no serialization promise for active requests.

Evidence: [eval](#evidence-eval).

### standing-database

**— — Standing DB** (role): standing_database: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### workflow-programming

**S — Workflows** (source): Configurable evaluation wrappers and compilation hooks supply programmable live-development composition.

Evidence: [eval](#evidence-eval).

### multi-model

**— — Models** (role): multi_model: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**S — Peer chat** (source): Addressed RPC/channel/thread messaging provides a primitive; no model conversation or participant lifecycle supplied.

Evidence: [dispatch](#evidence-dispatch).

### concurrent-work

**S — Concurrency** (source): Worker-thread routing is available under the selected communication style; backend and style determine behavior.

Evidence: [auth](#evidence-auth).

### steering-interrupt

**S — Steer/interrupt** (source): Thread-targeted interrupt message has a separate dispatch path; backend semantics are not universally established.

Evidence: [dispatch](#evidence-dispatch).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): Sticker recordings capture instrumented values/conditions in memory, with a noted multiple-client limitation; not a complete provider/program IO audit.

Evidence: [record](#evidence-record).

### audit-query

**S — Audit query** (source): Inspectable in-memory recordings provide a debugging primitive; retention and complete original IO require other machinery.

Evidence: [record](#evidence-record).

### hot-change

**S — Hot change** (source): Compile/load/eval can alter definitions in the existing process; no after-turn activation coordinator.

Evidence: [compile](#evidence-compile).

### rebuild-continuity

**? — Rebuild continuity** (inspection scope): Compile/load replaces Lisp definitions, but no executable rebuild/re-inhabitation handoff was established in inspected RPC and compilation paths.

### remote-services

**S — Remote** (source): Host/port Lisp RPC client/server, localhost default and optional shared secret; no transport/reconnect-work continuity inferred.

Evidence: [server](#evidence-server).

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Authenticated or local client can evaluate Lisp with server process authority; per-command agent approval is outside role.

Evidence: [auth](#evidence-auth).

### evaluation

**S — Evaluation** (source): Inspected sticker tests distinguish placed/armed states and fail when invalid compilation incorrectly arms a sticker.

Evidence: [compile](#evidence-compile).

### time-order

**— — Time/order** (role): time_order: this reference supplies Emacs/Lisp live development client and RPC server; an agent policy, model interface and context lifecycle are outside its supplied role.

## Inspected test oracles

- [quarantine/sly/test/sly-stickers-tests.el](../../../quarantine/sly/test/sly-stickers-tests.el): Instrumentation lifecycle after valid or invalid live compilation. Oracle: Explicit ert-fail if compiled definitions do not arm selected stickers or invalid instrumentation arms any sticker; this is UI/instrumentation state, not raw audit completeness. Read, **not executed**.

## Useful mechanisms

- Thread and continuation identities make live evaluation/debugging inspectable.
- Evaluation wrappers, diagnostics, object inspectors and restarts are strong ergonomic patterns.

## Material limits

- Runtime process and active stack lifetime remain backend-dependent.
- Recordings are selective and in-memory; multi-client recording state is explicitly unfinished.

## Arconaut design questions

- Which inspector/restart operations should the model use directly as ordinary programs?
- How do we retain original IO while exposing curated debug recordings without confusing their scopes?

## Evidence

### Evidence server

[quarantine/sly/slynk/slynk.lisp:935–950](../../../quarantine/sly/slynk/slynk.lisp#L935): Localhost default listener may accept multiple connections when configured.

### Evidence auth

[quarantine/sly/slynk/slynk.lisp:1013–1055](../../../quarantine/sly/slynk/slynk.lisp#L1013): Optional .sly-secret authentication and communication-style selection.

### Evidence dispatch

[quarantine/sly/slynk/slynk.lisp:1273–1323](../../../quarantine/sly/slynk/slynk.lisp#L1273): Requests address threads and continuation IDs; interrupts and channel messages dispatch independently.

### Evidence eval

[quarantine/sly/slynk/slynk.lisp:1966–2004](../../../quarantine/sly/slynk/slynk.lisp#L1966): Evaluation is wrapped by configurable handlers and replies with ok/abort plus original continuation ID.

### Evidence actions

[quarantine/sly/slynk/slynk.lisp:2079–2124](../../../quarantine/sly/slynk/slynk.lisp#L2079): Interactive evaluation and output capture execute in the live Lisp environment.

### Evidence compile

[quarantine/sly/slynk/slynk.lisp:2852–2911](../../../quarantine/sly/slynk/slynk.lisp#L2852): Compile-file hook dispatch and compile-string return collected diagnostics and can load definitions.

### Evidence record

[quarantine/sly/contrib/slynk-stickers.lisp:19–95](../../../quarantine/sly/contrib/slynk-stickers.lisp#L19): Stickers record values/conditions/time in in-memory global vectors; source notes multiple-client limitation.

