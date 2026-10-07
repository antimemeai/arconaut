# Local performance fleet — 2026-10-07

Operator priorities: dramatically reduce durable reservations; startup below
100 ms even with large context/history through intelligent loading. Provider
latency is separate. These are targets, not delivered claims.

Three Blackbird instances run gpt-6.1-sol/medium in isolated candidate checkouts.
Each has a fixed 25-minute unit allowance, successful 12-round turn segments,
managed context, one code review, direct checks, commits/pushes and persistent
resource/stack collection. Original P1 allowance is unchanged.

Current status: **one active autodev**, durable-state-r1-2026-10-07. It owns the
new compact-state/publication/recovery unit, seeded with the delivered allocator
and projection changes in a candidate branch. Beads and debug-only timing surfaces
are merged; earlier launch descriptions below are historical.

| Lane | Ownership | Discriminator |
| --- | --- | --- |
| arconaut-fsp / candidate/local-performance-2026-10-07 | Opt-in local timing, representative append/context/request hooks and scaling fixture | Local wall/CPU, disabled-path cost and matched short/long histories |
| arconaut-02g / candidate/issuer-ranges-2026-10-07 | Durable issuer high-water ranges and volatile allocation cursor | Reservation/sync counts and uniqueness after failures, reopen, namespace changes and exhaustion |
| arconaut-n11 / candidate/startup-demand-loading-2026-10-07 | Indexed restoration or derived snapshots/demand-loaded originals | Prompt **and audited-request** readiness on large histories; authoritative recovery and original repair |

Reservation ranges must be acknowledged durably before returning IDs. Reopen
burns unused IDs rather than reusing them. The persisted counter is the durable
upper bound, not the transient allocation cursor. Stream-capture batching is a
separate follow-up if it cannot fit this whole allocator unit.

Startup distinguishes retained-log size from live-context size. An early banner
does not satisfy the target. Derived accelerators must reject stale, corrupt or
truncated state and preserve unknown-effect fences. File timestamps alone do
not constitute full integrity validation. Measure the exact path/trust model,
workload, byte/record counts and cache conditions; report the remaining dominant
cost if the first bounded slice misses 100 ms.

New lanes start from master14513f7. P1 owns timing, issuer owns allocation,
startup owns restoration. Different functions of the same file may change;
Root integrates those hunks serially and resolves their interactions. Read
actual relevant LevelDB/reference source, write the short plan, then implement.
References remain study-only. No automatic master integration or primary build
replacement; the operator session and old paused campaigns remain untouched.

The new candidate pool has two reusable slots; existing P1 has one. The external
board records three active jobs and two preserved legacy holds. Its limit is
five and global admission stays paused; explicitly adopted runs are active.
Single-input references, append suffix/indexing and workflow-review repairs stay
queued rather than expanding these unit scopes.

Private context/ run directories retain missions, deadlines, process observations,
audit, raw profiles and eventual completion markers. Candidate notes retain code,
direct checks and aggregate measurements. A bound is not a success criterion.

Update14:01UTC: P1 stopped after four segments; useful code/checks/review fixes and
measurements preserved in pushed94bc2d4, inactive pending integration. All221
sampled identities absent on reconciliation; its slot released. Operator then
requested native Beads design/autodev. candidate/native-beads-2026-10-07 now uses
that freed slot under a fresh independent B1 unit, fixeddeadline1791383181. Current
three active BBs: issuer, startup, Beads. See BEADS_DESIGN.md; no added worktree.

Update14:39UTC: zero active. Beads delivered/pushed47bb9be; P1 candidate94bc2d4
awaits integration. Issuer/startup stopped at four-segment cap without completion;
partial sources preserved/pushed02ff9ac and8e35f55, inactive. Startup's ten large
fixture runs all failed, so no valid latency target claim. Slots reconciled and
released. arconaut-kut tracks continuation semantics; expired allowances were
not renewed. Historical active counts above describe launch observations only.

Update15:03UTC: operator explicitly directs continued reservation/startup work.
Beads/timing surfaces merged; BLACKBIRD_DEBUG defaultsOFF and ordinary recipe
excludes developer instrumentation/fixtures. Two active BBs in reused slots,
candidate/issuer-ranges-r2-2026-10-07 and candidate/startup-loading-r2-2026-10-07,
start currentmaster1131185 plus preserved partial source02ff9ac/8e35f55. Fixed
25-minute renewed units have deadlines1791386844/1791386846. Shared driver has
no arbitrary segment cap; completion or same deadline stops it. These deliberate
development runs use explicit debug profiling with private per-process native
span files plus OS observations; normal operator startup never enables profiling.

Update15:35UTC: both renewed units completed before their fixed deadlines.
Issuer e0e0b98 issued14610 IDs with15 durable reservations/syncs versus14610
for a width1 reference; matched issue intervals0.063762s versus88.372045s.
Five affected checks passed; one review finding fixed and directly rechecked.
Stream-capture cadence remains unchanged; these figures are not startup claims.
Startup35413f3 validates historical JSON while avoiding materialization of unused
candidate fields. Five successful warm runs per variant on335,783,054 history
bytes gave median audited-request readiness1394.550ms baseline /265.961ms
candidate; usable prompt180.675ms candidate. Target<100ms is NOT met. Replay
160.891ms and engine/admission85.965ms remain dominant; large live context was
not measured. Three affected checks passed after one review/fix/recheck.
Both branches pushed, clean, inactive; primary runtime unchanged. Manager and
collector exit0; all160 issuer /214 startup sampled identities absent via libproc.
Native slots and board reservations released, private profiles/artifacts retained.
No new allowance, extra certification round, or implicit master merge.

Operator explicitly launches durable-state-r1-2026-10-07 after the cross-domain
source study. candidate/durable-state-r1-2026-10-07 seeded1072613 combines reviewed
allocator/projection with currentmasterbc69391 and operator memory clarification.
Root resolves independent CMake targets and preserves both journal histories.
One native BB gpt-6.1-sol/medium, reusedslot0, absolute deadline1791391681,
25min TOTAL including provider/build/review waits; no segment cap. One coherent
owner for current-state publication and tail recovery, with direct state/fault
oracles and one review/fix/recheck. Substantial memory welcome when intentional
and justified; arbitrary1MiB/32MiB goals withdrawn. Explicit development captures
persist resources/stacks/native spans, normal DEBUGOFF runtime unchanged.
No master merge/primary replacement; design note precedes implementation, using
the actual pinned source study rather than a fresh survey. Global admission paused,
this specifically authorized job adopted. Old expired runs remain stopped.
