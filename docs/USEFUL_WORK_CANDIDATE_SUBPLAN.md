# Unit2: bounded candidates and contrastive Lua work

Source base: reconstruction/cpp-lua-2026-10-06, after c0a4a22. Unit1 and B1
closed. No primary checkout switching. One owned serial slot by default, explicit
configured cap (pilot limit4, explicitly configurable); allocation only on demand, FIFO overflow. No new dependency.

Product: a small C++ `arco-candidate` CLI used via ordinary audited exec, plus a
self-contained editable Lua workflow. Branches/commits retain candidate ancestry;
private local experiment records reference source/Lua/context POLICY/settings,
hypotheses, independently specified discriminators, baseline/candidate attempts,
results/artifacts and observed activation. Source archive is NOT activation.

Before affected checks: wrong-slot switching could destroy unrecorded source or
run against changing code. Test full-pool queue, dirty refusal, active-run lock,
unknown refusal, recorded serial reuse, branch/artifact survival after retirement,
and independent activation record. Never count parent exit as all descendants
stopped: after *every* run require explicit evidence of all checkout users stopped.
Detached/escaped processes are outside manager ownership; operator/model must
inspect them and settle explicitly. No automatic unknown replay/recovery. Git
mutation transitions persist before effects; interrupted transition requires
explicit evidence-linked reconciliation. Per-slot locks plus global state lock;
run drops global lock so other slots can work. Results remain private local;
publish concise selected reports only. Retirement requires clean recorded source
and declared artifacts already copied outside checkout. No worktree per candidate.

Lua: positional audit watermark helper avoids the actual reserved `end` parser
failure; explicit tagged-number conversion avoids table arithmetic. A bounded
selector scans at most a declared page budget, returning continuation/unknown
rather than pretending exhaustive search. Contrast records require hypothesis,
predeclared discriminator, baseline, candidate, measured outcome, disposition.
Cost feedback uses actual stats/usage, never token estimates or zero for missing
usage; persistent operator steer is read at workflow boundaries. Economy presents
causes/tradeoffs/options, not a judge or forced compaction. Workflow effective
source is retained by native runtime each turn; steer changes apply at next
boundary, explicit interrupt remains ordinary runtime cancellation.

Grounding: Inspect AI _pool.py/test_condense_linear.py => original bytes and
occurrences, no O(history) reparse for bounded pages; AdaMAST evidence.py => linked
observations not mutable blocking verdict gate. ModularRSI k_roll.py/tests =>
freeze complete source/helpers/settings in comparisons, missing identity invalidates
comparison; no adoption/copy-per-roll. Rethinking Evaluation 2607.12227v4 methods
3.1-3.3 => matched total feedback/attempt budgets, fresh transfer, fixed sequential
refinement can be competitive. No presumption evolution speeds work up.

Allowance: one direct red and affected green per mechanism, <=2 inconclusive
retries then change/defer. One narrow consequential independent review plus one
access retry/fallback. Debug/release/ASan affected checks; Neuroses portable batch
once at meaningful native boundary. Release/arco build and quiet RRC before actual
native activation, then ordinary live behavior check. No B1 recertification.

Then predeclare bounded useful inspection tasks and edit surface/participant policy
before assignment: unchanged harness extra attempts vs editable Lua policy, equal
source/access/feedback/resource caps; fresh subsequent transfer. Record acceptance,
failures/reverts/unknowns, operator attention, end-to-end and phase latencies,
available EVERY-provider usage (missing unavailable), compute and identities.
Small feasibility pilot, no statistical acceleration claim. Commit/push completed
units and candidate branches; bd notes/backup/journal at milestones.
