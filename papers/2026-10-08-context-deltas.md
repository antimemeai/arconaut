# Context mutation costs: first delivery

Operator objective arconaut-2lx: remove history-sized copying and cumulative serialization where they do not serve current session behavior. No requirement to preserve every original byte is inferred.

## Delivered

1. Remove ContextStore's write-only HistoryRecord vector. Each mutation previously serialized another whole context packet into this mirror and copied the accumulated mirror vector; constructor also populated it. History inspection already walks a pinned journal reader, so the mirror has no consumer.
2. New append packets carry format=append-delta-v1 and only newly introduced entries in originals; they no longer serialize the full live entries array. Replay reconstructs the next view from the previous view plus those entries. Legacy full-entries packets remain readable; unknown tagged formats and hybrid tagged/full packets are rejected.
3. Append and managed publication prepare only fresh original-map nodes and capture handles. They reserve capture vector slots geometrically before publication, then merge map nodes and move handles instead of cloning the entire originals map and capture-order vector. The existing visible-entry preparation and protection callback remain.

These changes do not eliminate all live-context copies. Edited/managed snapshots remain full snapshots, including redundant candidate representation in edit packets. Native retained-state forward metadata copies, capture/original index lifetime, checkpoint cost and pinned history-snapshot tail copying remain explicit scope.

## Measurements

Fixed live anchor:65,536content bytes. One small appended assistant entry:

| Historical originals before small append | New packet bytes | Reconstructed old-format packet bytes | Single append ms in final affected run |
| --- | ---: | ---: | ---: |
| 1 |315|66,006|4.171|
| 21 |315|66,006|4.240|
| 81 |315|66,006|5.241|

Old-format bytes are computed by rebuilding the former packet representation from the same actual next view; they are not a separate baseline executable run. The new format removes65,691 serialized packet bytes per append in this fixture. Timings are individual diagnostic samples, not controlled speedup ratios or latency distributions. Earlier runs included scheduling/I/O outliers. No heap-allocation count or arbitrary-history constant-time claim.

## Direct checks and the one recheck

Release build/release/blackbird built. context, cold_history, saved_state and context_delta pass. Direct coverage includes edit/rejected stale CAS, managed operations and tool groups via context, checkpoint/pinned archived readers, new deltas mixed with legacy packets, reopen equality, unchanged append bytes with increasing history, pre-publication protection failure leaving visible context/original indexes unchanged, and rejection of unknown/hybrid formats. git diff --check passed.

The one recheck traced another cost: per-mutation RetainedState::history_snapshot copies the resident fact tail. A candidate moved that snapshot creation to explicit inspection. It broke a currently tested capability: ContextStore's pinned historical reader is used after the root is released. A fresh snapshot would dereference the released root (caught as a context-test segfault in history_snapshot via originals).

That candidate was withdrawn, not activated or accepted. The failed design and observation are retained here and in the local audited edits. Existing eager pinned snapshots were restored; affected checks passed after that finding was fixed, within the same recheck allowance. Do not silently retry the lazy approach or call it safe. A future design must explicitly own a surviving history authority/reader or deliberately revise that finite reader-lifetime contract with operator acceptance.

Allowance:45-minute unit, at most25-minute hardening; implementation/direct checks and one recheck with its fixes. No third review/assurance pass. Earlier capture and request-storage checks were not rerun for this unit.

arconaut-2lx remains in progress for the remaining costs named above. Catalog maintenance and bounded reopen policy remain their independent tickets.
