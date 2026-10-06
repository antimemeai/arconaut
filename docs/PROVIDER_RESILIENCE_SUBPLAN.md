# Live provider resilience (.14.2), capacity follow-on (.14.3)

Fresh session after old audit admission exhaustion; no old effects replayed. The old
file stat is 536870704 bytes under a 536870912-byte configured ceiling (208 bytes
remaining), not a successful final boundary/quiet replacement. Pilot d2b89d7 is
published; all12 accepted, REVISE unconditional bounded-first; B1/.14.1 closed.

## Source consequences and order

CodingEngine::operation admits and records each provider attempt before dispatch;
request currently throws on its first failed attempt. Raw chunks are immutable
provider.stream records. Acceptance/context insertion occurs only after respond
returns. Child::collect conflates curl exit codes with errno in io(detail), so
retrying io92 blindly is not a sound classifier. openai_http must distinguish curl
transport exits from OS/config errors. Audit recording errors must propagate rather
than become retryable transport errors.

Read acquired Codex codex-client/src/retry.rs lines1-115: separate transport/build/
policy errors, retry status selection, exponential delay, per-attempt telemetry.
Transfer typed failures, bounded delays and visible waiting; do not adopt its crate
or whole-request loop as a workflow replay. No new library. Begin retries first,
then capacity handling uses the resulting attempt growth model.

## Implementation and direct faults

Append a transport error category without renumbering existing codes. Whitelist
curl transient exits (DNS/connect/timeout/send/receive/HTTP2/HTTP3), retry HTTP
408/429/500/502/503/504, never auth/config/cancellation/semantic parse failures.
Each retry is a new admitted operation, same frozen request/revision, explicit
retry group/ordinal on original request. Reset previews between attempts; partial
text is visible but never inserted or dispatched. Default bounded 5 attempts,
1s exponential waits capped16s. Lua request retry_policy allows smaller/larger
bounded attempts/delays, removed before wire serialization. Poll cancellation at
<=25ms during wait and before dispatch. Do not re-enter Lua or repeat any tool.
Test midstream recover, exhausted outage, permanent errors, wait cancellation,
unique attempt lineage and exactly one accepted tool dispatch. Mac/Linux affected
checks, independent review when available, release build and actual quiet RRC.

## Capacity design work remains open

main.cpp fixes512MiB/200000records; journal_writer preflights admission, retains
all records/payloads in memory and replays the whole file on reopen. Context
compaction does not reduce this native history. Before changing machinery, measure
bounded frame sizes/growth from existing audit and replay costs in isolated reads.
Expose headroom before exhaustion and preserve room to record a clean handoff;
explicit new-session lineage must retain old locators. Compare segmentation with
explicit successor journals rather than blindly raising the cap. No truncation,
move, cap increase, or claim of settlement of the exhausted old session.
