# Performance wrap-up delivery

Operator scope: finish the seven assessed tasks; do not extend stream-capture
measurement. Set provider transport diagnostic lifetime to30days. No dependency
adoption, general certification pass or speed promise.

## Changed behavior

Coding tests now exercise current admission sizes and compact history access.
Three capacity fixtures had historical repeated-input assumptions; the batch
fixture constructs its large input inside a small Lua program, isolating admission
from retaining that program. Retained-output inspection uses fact_count/fact rather
than the unsupported contiguous-history accessor. Full coding test passes.

CodingEngine stores request input once per invocation. Its admission wire is a
schema2 identity reference; native execution/recovery/inspection resolves and shares
that input. Schema1 remains readable. Updated executables must be used for new
sessions containing schema2; old binaries are not a rollback path for them.

Json values share immutable backing. Mutable array/object borrows force separation;
a later copy of an exposed container recursively respects exposed descendants.
Construction now allocates a backing Value/control block, including scalar nodes;
that is an explicit cost of avoiding repeated deep payload copies. No overall
allocation or CPU improvement is inferred from this representation alone.
Retained facts and source descriptors use an owned32-way persistent radix sequence.
Snapshots pin one root and mutation copies only changed paths/leaves. The direct
33000-record case copies0 records for a snapshot and40 for an append plus a change
to the first leaf. Pinned history survives owner release without an eager tail copy.
Borrowed mutable sequence references end at the next copy/mutation. Context edits
omit duplicate candidate.entries, pending publication moves its staged state, and
successful checkpoints prune non-live original handles/capture handles.

Catalog renewal streams old entries and the suffix delta instead of collecting a
history-sized RAM map/vector. It still rewrites historical catalog bytes. Scratch
cleanup resumes bounded passes; cursor lifetime is this process. Compact ledger-only
activity requests checkpoints; repeated publication failure stops new admission at
the finite resident-tail budget while retaining settlement room. /checkpoint retries
maintenance explicitly; /stats and errors surface the failure.

Reopen behavior is finite and visible: automatic full replay is limited to64MiB;
--rebuild-session enables the existing512MiB/200000-record slow path. Usable compact
state avoids that full replay. Missing derived state is distinct from damaged audit
or unknown effects. Linked histories are explicitly refused by this launcher.
See docs/SESSION_REOPEN.md for the actual matrix.

Phase samples identify durable append waits in the local command. Unchanged
session-info.json no longer gets rewritten. Program source and generation activation
now share one durable batch before execution, removing their separate commit/sync
boundary without weakening pre-effect recording. No polling timeout was reduced.

New provider stream blocks live outside the audit and expire after30days. Expired
reads refuse; bounded capture/reopen cleanup unlinks files. --expire-diagnostics
provides bounded explicit cleanup for idle sessions. No daemon is installed.
Legacy raw streams embedded in finite older audits are not rewritten. Process and
standalone colleague captures remain separate; further stream cost measurements
were explicitly excluded.

## Integration and direct checks

Release/OFF built. Twenty-one affected checks pass across the initial run and the
same failed-case recheck/fixes: coding, context/context_delta, JSON, persistent
sequence, diagnostics, retained codec/state/crash/environment, session recovery/
store, request storage, saved state, cold history, checkpoint/catalog/scratch,
terminal/editor/PTTY and journal batch. Initial program-source oracle incorrectly
assumed no durable ID reservation; corrected it to compare actual linked record
sequences and same-batch ordering. The new maintenance callback destructor was
fixed to use an owner lifetime token when a context outlives its journal owner.
No third source review or unrelated test sweep. New diagnostics and JSON source
pass focused clang-tidy; changed lines/new source formatted, diff check passes.

Profile phase/reopen checks and raw observations are in the separate reopen report.
macOS CPU accounting was corrected from raw Mach ticks with the actual timebase.
The phase probes do not establish a before/after speedup.

Published Release SHA25647282d5c978dbed9c4a7eb6e01a1aa0c30e810910b63a1f91e011ea882a88d99
is shared by blackbird, blackbird-ui and compatibility aliases. Two actual default
launcher starts (no executable override) completed their audited local query and
exited0. First frames805.103/443.370ms, command markers33.162/45.976ms after input;
these slow starts are retained, not explained away. A final native profile sample
had681.124ms before native entry and766.925ms to first frame, then40.629ms to the
local marker. Seven retained batches fell inside its command.perform span (twelve
including startup), versus eight in the earlier command samples. No provider request
was made. No under100ms readiness result follows.
Raw records: docs/measurements/2026-10-08-published-wrapup.json and
2026-10-08-seven-batch-command.json. Existing operator sessions and untracked art
were not changed.
