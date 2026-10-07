# Local performance fleet — 2026-10-07

Operator priorities: dramatically reduce durable reservations; startup below
100 ms even with large context/history through intelligent loading. Provider
latency is separate. These are targets, not delivered claims.

Three Blackbird instances run gpt-6.1-sol/medium in isolated candidate checkouts.
Each has a fixed 25-minute unit allowance, successful 12-round turn segments,
managed context, one code review, direct checks, commits/pushes and persistent
resource/stack collection. Original P1 allowance is unchanged.

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
