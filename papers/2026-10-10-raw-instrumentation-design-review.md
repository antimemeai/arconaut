# Raw instrumentation: one adversarial design and plan review

Owner resolution2026-10-10: accepted R1–R5 and completed the single finding-driven
design/plan correction. The final design adds the32-byte recorder origin prefix
and physical limits; reverse armed wakeups with one socketpair/receiver per
execution context; separate control lanes and held per-owner terminal credit;
reserved recovery credit plus explicit external replenishment; and bounded
rotating lane service. Open-lane absence is an error and closure precedes unlink.
The plan now exercises those exact claims, including multiple parked owners and
failure exhaustion. This resolves the written omissions, not implementation
qualification: those dynamic production checks await approved source work.
No second reviewer or review of this report was added.

2026-10-10. This is the single design/plan attack requested for the bounded
research unit. It does not qualify code, review another review, authorize product
implementation, or add an assurance layer. No source/build/test/reference runtime
was changed or executed by this review.

Reviewed the proposed [design](../docs/INSTRUMENTATION_RAW_DESIGN.md),
[plan](../docs/INSTRUMENTATION_PLAN.md), [raw surfaces](2026-10-10-raw-instrumentation-surfaces.md),
and both [transport](2026-10-10-raw-transport-design-study.md) and
[recorder](2026-10-10-raw-recorder-design-study.md) studies. Inspected existing
owned journal framing/append and participant/command observation, backpressure
and cleanup paths. Findings below address concrete missing protocol operations,
not a demand for more literature or a new generic framework.

## Assessment

The process split and retained publication/reclamation rule are sound design
choices for the stated process-crash guarantee. The current proposal is not yet
a complete implementable protocol: disk identity/reconciliation, producer
parking, terminal reservations and recovery from exhausted failure capacity
need the following fixes. Fair lane service should be explicit too. None calls
for metrics transforms in a writer.

The host-before-flush limitation is correctly exposed. Ordinary volatile shared
memory cannot protect an acquired-but-unpublished fact, nor a published pending
record against host/power loss. Changing that requires a stronger acquisition or
durable-return contract. It is not one of the fixable protocol defects below.

## R1 — Persist source identity and byte position in the actual raw frame

**Blocking design omission.** The producer envelope specifies `record_bytes`,
`site_id` and lane-local `ordinal`. One recorder multiplexes many lanes into one
raw audit stream. Its proposed physical frame is the existing32-byte frame,
which has physical sequence and batch-first, but no lane epoch or source byte
position. Two lanes can both emit ordinal1. On recorder restart, disk records
must identify which lane's `D` can advance, including a sync-success/ack-loss
case. An identity in a runtime object or a separate lane description cannot
associate each persisted multiplexed record with that object.

Source: [journal.hpp](../include/blackbird/journal.hpp:8) defines the frame/header
fields and existing kinds; [journal.cpp](../src/journal.cpp:208) writes kind,
physical sequence, batch-first, payload length and CRC. Neither persists lane
identity. [journal_writer.cpp](../src/journal_writer.cpp:330) explicitly accepts
only source/semantic drafts today. The draft already proposes a new raw kind;
the missing part is its exact payload format and recovery rule.

**Required change:** specify recorder-side source metadata for every raw frame
or complete lane-prefix frame: lane epoch, first source byte cursor, complete
end cursor, and the association with exact unchanged producer records/ordinals.
Specify widths, arithmetic checks and how contiguous replay reconstructs `D`.
Specify lane closure's final cursor/ordinal and setup/retirement ordering so a
missing named object is accepted only for a durably closed/drained lane.
Metadata/framing stays in the recorder; producer bodies remain unchanged.

The `u32 record_bytes` envelope and existing `u32 max_payload/max_batch_bytes`
also need an explicit representability rule. A larger successor lane alone
cannot admit a single record beyond that format. Either select wider native
producer lengths and recorder container fragmentation, or explicitly constrain
each acquisition before it occurs and retain arbitrary existing large originals
through a qualified locator. Do not turn a known complete event into silently
independent smaller observations or truncate it.

**Direct oracle:** two lanes use the same ordinal and different bytes; kill the
recorder after sync before either/both acknowledgements, restart from only
catalog/disk/named memory, and check each reconstructed byte cursor and exact
record identity. Include segment rollover, a preserved bad tail, and arithmetic
limits. Expected source identity/bytes are independent of the recorder decoder.

## R2 — Define the reverse preservation/credit wake protocol

**Blocking design omission.** The selected `armed`/doorbell protocol wakes the
recorder after producer publication. It does not wake a producer that parked
because `P-D == C`, or one executing `await_preserved`. The documents say wait
at a safe boundary but give no actual condition registration, final `D` check,
wait primitive, acknowledgement notice, or recorder-death/cancellation path.
An acknowledgement store by itself does not wake an OS descriptor wait; a
process-shared `atomic::wait` is expressly not assumed.

**Required change:** specify a slow-path preservation wait using a real supported
OS notification/control mechanism, with registration before final acquire `D`
check, cumulative acknowledgement semantics, independent death/restart handling
and cancellation that does not discard published bytes. No producer hot-path
registry or periodic poll should be introduced. A waiter must either see enough
`D`, have a pending wake, or be registered for the required future wake.

**Direct oracle:** force a lane exactly full; pause the producer at every
register/recheck/sleep cut and the recorder around sync/`D`/notice. The producer
must resume after actual preservation without any unrelated user event.
Exercise cancellation, stale/full notice, recorder death and restart, and
multiple waiting execution contexts without one stealing another's only wake.

## R3 — Make terminal/cleanup reservation independent of ordinary queue fullness

**Blocking owner-integration omission.** The design correctly forbids waits
under owner locks and overlapping reservations across async yields, but also
says to reserve terminal/failure/cleanup. Those are not the same lifetime. A
running child can exit or need cancellation while its ordinary output lane is
full; the owner cannot postpone child termination/reap until recorder progress.
It must still retain the actual terminal/control outcomes. Saying “reserve the
bounded synchronous region” does not provide that independently available
capacity for an already-live asynchronous owner.

Source: [command_jobs.cpp](../src/command_jobs.cpp:540) stops reading ordinary
output at capacity while [its exit/control loop](../src/command_jobs.cpp:585)
continues observing child state, forced close, kill and reap. The
[foreground cancellation path](../src/command_jobs.cpp:707) joins cleanup before
propagating cancellation. [participants.cpp](../src/participants.cpp:117) can
currently reject acquired capture bytes; its later drain retains the front
only if owner retention succeeds. These are precisely the owner boundaries
the proposal is intended to repair, not emitter-only details.

**Required change:** before spawn/dispatch, assign bounded terminal/cleanup
capacity that ordinary data cannot consume. Define ownership through yields and
the complete possible terminal/control burst, without creating publication
holes in the ordinary lane. A dedicated owner terminal lane/cell is one option;
the design must select and describe the option. Additional external control
requests also need pre-admission credit. Distinguish stopping a future read from
already acquiring its bytes/status.

**Direct oracle:** saturate ordinary raw output, stop the recorder, then cancel
and reap a real child and let a participant return/fail. Cleanup must complete,
and exact terminal/failure bytes must remain independently retained for later
preservation. Force multiple control outcomes, not just one exit record.

## R4 — Define recovery when every failure reservation is occupied

**Blocking recovery omission.** Reserve a failure slot before each fallible I/O
attempt; consume it on failure; when none remains, stop all attempts. With one
available slot, one EIO occupies it. When disk recovers, the recorder cannot
attempt the write needed to preserve that failure and free the slot: that write
itself is fallible and needs a reserved slot. More finite preallocated slots
merely postpone the same state.

**Required change:** describe the fenced recovery transition. For example,
external setup provisions and durably discovers a fresh retained failure lane
before authorizing another attempt; if provisioning fails, stay fenced and
retain all originals. Alternatively choose another explicit nonrecursive
mechanism that preserves each newly failed attempt. Do not silently permit an
unrecorded recovery failure, overwrite the old failure, or retry forever.
Clarify failure-lane draining and failures of those drain attempts without a
recursive call to the ordinary emitter.

**Direct oracle:** fill every ordinary and failure reservation through injected
write/sync failures, restore storage, then execute the specified recovery
transition. All original records and each actual failed attempt must survive,
and a working recorder must eventually resume after the stated replenishment.

## R5 — Specify fair finite-prefix selection across lanes

**Progress omission.** A finite batch prevents an advancing producer from
extending that batch forever. It does not prevent the recorder from repeatedly
choosing that same busy lane and starving a quiet lane with one terminal record.
The documents do not currently select a fair lane scan/batch rule.

**Required change:** choose a rotating lane scan or another simple rule that
services every ready registered lane within a bounded number of selection
rounds, subject to actual storage progress. Keep batching based on already-ready
bytes, without a timer or waiting for batch fill.

**Direct oracle:** continuously publish on one hot lane while another publishes
one terminal event. Observe preservation of the latter within the selected
round bound; also exercise lane registration/retirement during busy draining.

## Attacks that did not produce findings

The unconditional acquire/release `armed` exchanges defeat the opposing
store/load lost-wakeup race: producer exchange before recorder arm transfers the
publication through RMW modification order; the reverse order supplies the
notice. Draining old notices before arm/final readiness check avoids consuming
the only new notice after deciding to sleep. A conditional producer flag load
would invalidate that argument and should remain prohibited.

Named memory left linked, with exact names registered before activation, covers
the last-mapper-exit case while the OS survives. A launcher retaining only pipe
endpoints would not cover drained recorder-private bytes; the shared retained
lane is justified by this actual failure path.

Holding `D` stationary through write/sync failure and reconciling a resynced
disk prefix before ack correctly avoids premature reuse. Recorder-side CRC,
commit framing and necessary source-position metadata are storage integrity and
transport work, not metric transforms; they must remain outside the producer.

The plan properly separates SIGKILL from host/power failure, does not claim TSan
qualifies process-shared atomic IPC, and does not mistake primitive timing for
the performance of the selected full protocol. Storage naming durability and
previous-tail sector disturbance remain explicit qualification requirements,
not facts established by the design review.

## Finding-driven completion

Root should fix these protocol gaps in the existing design/plan and make one
finding-driven recheck within the already declared bound. A new review campaign
is unnecessary. The corrected artifact can be discussed for approval while
keeping power-loss acquisition/durable-return policy an explicit operator
decision. Product implementation then executes the direct oracles in the plan;
this text review neither runs nor replaces them.
