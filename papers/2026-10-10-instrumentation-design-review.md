# Instrumentation proposal: system-source pressure test

2026-10-10. Research-only review of `docs/INSTRUMENTATION.md` against the current native system census at `e85a845340d600b40b07b255040bf57d1b4e51d1`. One review of the proposed design; no product edits, implementation qualification, tests, provider calls or review-of-review. This report does not modify the proposal.

The proposal now correctly separates instrumentation producers from operations/evals consumers, and hook mechanics from quantity semantics and domains. It locates measurement acquisition at semantic owners rather than only wrappers. The current `src/*.cpp`, helper headers and seven shipped Lua programs are represented in the union census. That owner coverage is supportable; it does not establish that every semantic boundary is enumerated. Three precise corrections remain.

## 1. Audit-retention conditional loophole — corrected during this review

The version first read correctly said completeness is unconditional, but its proposed rule retained newly acquired ephemeral evidence **when historical reproduction is required**, followed by a prohibition against discarding observations acquired **as required evidence**. Those qualifiers allowed an implementation to classify an actually acquired observation as optional after acquisition and omit it. That would repeat the previous incomplete-audit mistake under another name.

Concrete source evidence: worker execution endpoints, a past RSS sample, short read/write progress, syscall signal result and queue transitions cannot be reacquired later. Current `Participants::Impl::work` (`participants.cpp:114–126`) can reject raw capture; `CommandJobs` read/control paths (`command_jobs.cpp:497–575`) hold actual outcomes only transiently; diagnostics expiry is a known deficiency. The proposal must not give those deficits another route through conditional retention. Optional activation decides whether an expensive diagnostic fact is acquired; it does not decide whether to erase an observation after actual acquisition.

Proposed replacement for the first retention paragraph:

> Audit retains every actual instrumentation acquisition and its observation, the applicable versioned definition/mapping, and actual emission attempts/outcomes, including unsuccessful work. Existing authoritative source facts can be shared when they contain the observation and its association; no duplicate raw-metrics journal is needed merely to repeat them. Newly acquired ephemeral evidence enters complete audit custody rather than depending on a later decision to reproduce history. A past resource sample, worker clock endpoint or partial-progress observation cannot be recovered by polling later.

Replace the later phrase “or observations actually acquired as required evidence” with “or observations actually acquired.” Exact underlying fact granularity belongs to the agreed system/audit model; this correction must not be interpreted as an unsolicited requirement for exhaustive CPU instruction tracing.

The already stated need to settle reservation/crash/exhaustion behavior with nls.10 is appropriate. It must remain a qualification prerequisite for those implementation units, not an excuse to implement a lossy first version while calling the result complete.

Root changed this paragraph during the review and requested a targeted read of that changed text. The current proposal adds newly acquired ephemeral evidence and failed acquisition outcomes to authoritative custody unconditionally, explicitly prohibits discarding acquired measurement evidence, and names activation boundaries rather than inventing earlier history. This resolves the conditional-retention finding. Concrete custody/crash/exhaustion behavior remains an acknowledged open design decision.

## 2. Foundation handle/identity/resource admission is not explicitly enumerated

The native-values row covers shared value/radix allocation and codec/bridge work; the foundation row covers clocks and native synchronization. Neither states handle registry acquire/validate/release, slot generation retirement, wrong environment/incarnation/registry, stale handle, registry capacity and byte-buffer copy refusal. They are real resource/identity surfaces even though their owning source unit appears elsewhere.

Source: `foundation.hpp:156–226` implements HandleRegistry create/acquire/validate/release; generation at UINT64_MAX retires the slot rather than making it reusable. `foundation.cpp:82–92` bounds and copies ByteBuffer, with distinct capacity/allocation outcomes. These should not become one generic “invalid handle” counter: wrong environment, wrong incarnation, wrong registry and stale slot/generation are separate observed reasons.

Proposed additional census row:

> Native identity/handle/byte-buffer owners: `foundation.hpp`, `foundation.cpp` | O/S/W/R/P | Registry create/admission, slot acquire/release/retirement, validation outcome by native reason, live/capacity snapshots, identity exhaustion, and requested/copied/refused byte-buffer work. Acquire attempts and successful live ownership are distinct; generation retirement does not emit release/reacquire success or reset counters.

Alternatively add the same explicit facts to the existing native-values row. The purpose is source completeness, not adding a hook to every pure helper or making identities aggregate labels.

## 3. Instrument retirement needs an explicit lock-order restriction

The proposal correctly waits for in-flight calls before freeing detached instrument storage and forbids arbitrary receivers/Lua under product locks. It leaves unstated whether detach/retirement can wait while holding the owner's lock or from inside an instrument invocation. Those choices can deadlock even with small native observations.

Concrete owners: CommandJobs' collector/read/control/drain all share `Impl::mutex`, while shutdown joins workers (`command_jobs.cpp:311`, `759`, `932`, `1017`). Participants workers and owner drain/save share a mutex; shutdown/join must not retain a lock needed by the worker (`participants.cpp:104`, `263`, `595`). Terminal cleanup takes `ui.mutex_` and closes wakeup descriptors (`terminal.cpp:1260–1271`). Emitter handles following join do not alone establish safe lock ordering.

Proposed addition to the lifecycle paragraph:

> Detach and retirement do not wait for producers while holding a product/resource mutex, an emitter/collection mutex needed by those producers, or from within the retiring instrument's own invocation. Owner shutdown first prevents new entries, then joins/settles outside those locks, then retires storage. Activation/collection snapshots follow a stated lock order or owner-thread message protocol.

This is a concrete mechanism constraint, not a request to select RCU, add generic callback timeouts, or claim every retirement is nonblocking. The same restriction belongs to snapshot registration removal and programmable projection teardown.

## Supported coverage and preserved open decisions

The proposal includes journal write/sync/publication/recovery, archives/indexes and resident retirement; actual file/subprocess/PTY custody; command output/input/control/foreground wait; participant request/direction/capture queues; terminal custody and UI transfers; COW/value/codec work; platform resources and clock domains. It correctly distinguishes app background from SIGSTOP/relaunch and POSIX foreground group; query snapshots from transitions; externally reported usage from measured work; replay work from repeated product execution; and failed capture from a successful audit.

Its open choices are honestly stated: physical allocation/accounting, OS resource attribution, metric retention/custody bridge, distribution representation, native stream/snapshot delivery, release/diagnostic selection, programmable projection expressiveness and logical-clock integration. Nothing in this review selects their implementations or an evaluator. No additional assurance pass is proposed; integrate the concrete corrections and present the design for operator discussion.
