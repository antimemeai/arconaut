# Independent review of the frumentarii synthesis

2026-09-30. Bounded review of current intent translation and research conclusions.
Read FOUNDATION, README, AGENTS, the synthesis, the autodroit report, and the
relevant workflow, systems, and standing-state reports. Independently inspected
the precise GEPA evaluation-service and Self-Harness acceptance paths to check
the evaluator claims. Did not repeat archive verification, run reference code,
or conduct another survey. The reviewer authored the standing-state report;
review independence here concerns the parent's synthesis and intent translation,
not an independent certification of that earlier report.

## Finding R1 — Preserve the full audit requirement through compaction and retention

**Medium priority; clarification needed before this synthesis drives design.**
The integrated brief discusses before/after context and durable recording, but
does not explicitly distinguish reduction of active context from retention of
the underlying captured evidence. The synthesis's compaction scenario asks for
“useful” retained data and “retained sources”; its audit scenario asks to
reconstruct recorded work and identify gaps. These are valuable, but a design
could satisfy those formulations by keeping selected sources and reporting that
other captured material was discarded. That would lose the operator's complete
audit requirement for later study.

Locations: [FOUNDATION, research consequences](../docs/FOUNDATION.md#research-consequences),
lines 80–92 at review time; [synthesis, scenarios](2026-09-30-frumentarii-synthesis.md#scenarios-to-shape-the-research-design),
lines 139–151. The more precise distinction already appears in the
[standing-state report](2026-09-30-standing-state-frumentarii.md#comprehensive-audit-as-the-primary-research-surface)
and the systems report's compaction scenario; it should survive the integrated
handoff without requiring future readers to recover it from a subordinate report.

**Concrete consequence:** a summary omits a tool result or a failed experiment,
the active conversation continues correctly, and its surviving records are
internally consistent. Later autodroit analysis cannot determine why the old
model behaved as it did because the original material was removed. Neither a
successful continuation nor an explicit deletion marker recovers that evidence.

**Requested correction:** state that active-context selection, compaction, and
derived query views must not silently prune the original captured audit material.
Retain the actual pre-transformation evidence and transformation inputs/outputs;
use references to locate retained bytes, not substitute for them. Any retention
or recording-loss policy needs explicit design treatment against the operator's
requirement. In the messy-run experiment, compare known emitted provider/program
input and output with retained original material, including failed/abandoned
work; self-consistency of the surviving log is not a completeness oracle. This
does not demand unknowable provider internals or serialization of an unobserved
runtime heap, and it selects no storage architecture.

## Other reviewed claims

No other substantive contradiction or premature implementation decision found
within this review's scope:

- FOUNDATION preserves self-improvement as a core principle, hot turn semantics,
  managed compaction, hot reload, expressive shared workflows, model ergonomics,
  live peers, and minimal routine approval friction. It accurately preserves
  optional Rust, distrust of JS/TS and Go, general JVM exclusion, and the
  conditional preference to minimize compiled components until rebuild
  continuity is demonstrated. README and AGENTS agree with that brief.
- The runtime comparison is explicitly provisional. It keeps function
  replacement, state migration, executable replacement, and OS-resource/model
  continuity distinct. The small native owner is an alternative, and even that
  owner's replacement remains in scope. A surviving image, restored conversation,
  or reconnecting UI is not presented as proof of full continuity.
- The autodroit report bounds evaluator claims appropriately. In the inspected
  GEPA source, HTTP evaluation reaches the same evaluation service/budget as
  direct calls; the server registers train/validation examples and excludes
  optional test examples. This supports the narrow stated interface claim,
  not confidentiality of arbitrary application data or audit completeness.
  In Self-Harness, the acceptance path compares repeat identities/denominators
  and split averages. The report correctly limits this: it does not certify
  identical case sets, sound graders, generalization, or continuity.
- The synthesis's Prime, Dagger, Goose, Science, Restate, and Letta conclusions
  remain within the corresponding reports' inspected boundaries. Benchmark
  outcomes and experiment mechanisms are not promoted into promises of better
  live coding work. Changing the evaluator/audit is treated as an intervention;
  neither a score nor the model's endorsement becomes an adoption oracle.

Exact source checked for the evaluator questions:
[GEPA evaluation service at `3f160c2`](https://github.com/gepa-ai/gepa/blob/3f160c295000dd31db3d438c3d17553c23cc5f81/src/gepa/oa/eval_server.py),
[Self-Harness acceptance at `2720dbb`](https://github.com/qzzqzzb/Self-Harness/blob/2720dbb3f52283684f4b85a1065d642df1779dd8/acceptance/scripts/run_acceptance_gate.py).
Other source claims were checked for faithful synthesis of the bounded reports,
not re-audited implementation by implementation. This review is not a behavioral
validation or an architecture approval. Only this review file was written.

## Finding disposition

R1 accepted and resolved by the parent on 2026-09-30. FOUNDATION now explicitly
requires retention of complete captured originals independently of active context,
compaction, and derived views, including failed/abandoned work and transformation
inputs/outputs. The synthesis's compaction and messy-audit scenarios preserve that
distinction and compare known emitted inputs/outputs against retained bytes.
Retention or recording-loss policy remains a design question subject to the
operator's comprehensive audit requirement. No storage mechanism was selected.
