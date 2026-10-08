# Provider capture write amplification: first delivery

Operator replaced blanket original-byte preservation objectives with measured runtime objectives. This unit addresses the native CodingEngine provider response capture path (arconaut-bzn), not process-output or standalone colleague capture.

## Policy and implementation

Transport diagnostics aggregate into blocks of at most 64 KiB. The current attempt owns one pending block. Flush occurs when full, at successful provider completion, or on a caught provider exception/cancellation before operation settlement. No destructor performs I/O. A sink exception poisons the accumulator: it never repeats that potentially uncertain write. Live response preview consumes incoming callbacks immediately rather than waiting for a diagnostic block to fill.

A process crash can discard the pending partial diagnostic block. There is deliberately no timer or promise of a durable copy of every observed transport byte. A stalled provider can keep a partial block pending until completion/cancellation; live preview is not delayed by that policy. Existing durable attempt/unknown-effect handling is unchanged. Completed blocks still enter the current session log; this unit does not set a disk retention lifetime or implement pruning of completed diagnostic records.

## Direct results

Release build/release/blackbird built successfully. stream_capture and coding_stream_capture pass. Unit and actual native engine fixtures each deliver 100,000 one-byte callbacks with exactly two capture sink/source-log appends, rather than one append per callback. This measures append count, not physical fsync count or a production latency percentile.

Direct coverage includes oversized callbacks split into bounded blocks, empty/repeated flush, partial completion/error flush, abandoned diagnostic tail without destructor I/O, sink failure without replay, provider retry attempt separation, byte/record capacity settlement, immediate live preview, interrupted response, and validate_restart after interruption. Capacity fixtures now consume full blocks to continue exercising record exhaustion; retry fixture expects one combined failed-attempt stream record instead of two callback records.

Initial full coding test exposed its obsolete per-callback record-count expectation (fixed), then stopped with unsupported in retained_output_test. That section does not call the provider until its later timeout-recovery scenario; exact failing operation remains unlocated. Do not claim the broad test passes or that this is proven pre-existing. A focused coding_stream_capture entry point covers the affected native paths independently.

Allowance: 45-minute unit, hardening at most 25 minutes; initial implementation/checks followed by one source/direct-oracle recheck. Both focused checks passed on that recheck; no third assurance layer or broad suite rerun.

## Remaining scope

arconaut-bzn remains in progress: measure actual sync/CPU and delivery latency under representative streams; decide completed-diagnostic retention lifetime; separately assess process/colleague captures if relevant. Broader coding-test unsupported failure remains explicit. Other five performance/reopen tickets are untouched. No external provider/network calls were needed for these checks.
