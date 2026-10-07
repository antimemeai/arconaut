# G2b: explicit failed-custom-workflow presentation repair

Unit arconaut-7iy.2, root-authored fresh working context; original unit allowance
2026-10-07T05:09:25.050731Z through06:39:25.050731Z. No imported attempt
or verified successor authority. Scope B4 only; G1 cooperative backstop and G2a
context budget remain accepted, not recertified here.

## Grounding and design consequences

`CodingEngine::turn` cancels staged context/policy/restart on failure. Only
interrupted/capacity failures append `ContextStore::stop_outputs`; custom Lua
errors can retain unanswered calls. `validate_protocol` refuses the next request.
Ordinary `/lua` runs a separate workflow, so it is the explicit repair seam.

The CLM primary-source study (immutable upstream18dc11115f50f261233c5bba7937834491e307e8,
restoration and URLs in2026-10-01-context-language-models-study.md) separates edited
presentation from original events and actual requests. Its resume code drops
unanswered calls. Consequence here: retain original calls and append distinctly
unknown placeholders instead of removing evidence or pretending execution failed.
Candea/Fox and recovery source consequences in2026-10-07-backstop-recovery-study.md
reject arbitrary effect replay. Repair dispatches no old effects and grants no new
backstop/attempt custody.

Native `locally_quiescent()` refuses general-exec containment for the rest of the
lifetime. This unit does not relax that G1 rule. G2b only repairs presentation and
checks no owned Child is live. Escaped children/remote effects can remain unknown;
repair does not establish lifetime quiescence, reset the lifetime, activate a
successor, restart, unpause or establish execution success. Startup's unresolved
non-provider admission gate remains unchanged.

## Delivered interface

`arco.call('context_repair',{base=arco.context().base})`, through an explicit new
recovery Lua workflow. Strict current base; only unanswered call IDs present at
workflow admission are eligible. Candidate full protocol validation precedes one
native context append. Empty missing-call IDs, duplicate calls/orphan outputs,
new active-workflow calls, nested tool use and live owned children are refused.
Existing outputs/originals stay unchanged. Repeated repair on complete context is
a no-op. Operation admission/result and context publication use existing audit
machinery; no new dependency or production language.

Each new output says `workflow_result_missing`, `effect_outcome: unknown`,
`replayed: false`, and directs inspection of retained actual results. A locally
completed tool with a missing Lua result and a never-dispatched call both receive
conservative unknown presentation placeholders, not invented effect observations.

## Direct evidence and bounds

Initial focused release oracle failed `external_unknown:0` before implementation.
The new workflow_repair test exercises actual exec-before-Lua-error (marker remains
exactly one byte), withheld result, undispatched call, next-request rejection,
explicit repair followed by provider request, existing known result preservation,
original preservation, repeat/no-op, stale base, cancellation, current-workflow
call refusal, actual live owned `/bin/sleep` refusal, duplicate malformed context
nonpublication and retained ContextStore reload.

Hardening declared05:14:30UTC,25minute maximum/deadline05:39:30UTC. Layer1 interface,
remediation and direct checks; layer2 one source/test recheck added live child and
existing-result oracles and checked final affected tests. End05:22:24UTC (~8min),
no third assurance. Mac release passed; Linux Clang18/libstdc++13 debug/release/
ASan+UBSan passed only workflow_repair. No old pilot/B1/U0-U9/full-suite reruns.

Initial background Linux local driver was interrupted after configuration/partial
build, not treated as a test result. Inspected retained log and actual remote
process state, then finished only the affected target in the same remote source
snapshot `/tmp/arconaut-linux-r65uTV3b`. Final test-only changes rechecked only
workflow_repair in those profiles. Local captures:
`context/giga-campaign/g2b/linux-repair.log`, `linux-final.log`, original incomplete
`context/linux/run-keegdiry/checks.log`. No absent final boundary retroactively
settled. Build-source snapshot and actual logs remain ignored, not published audits.

## Actual useful work

`context/giga-campaign/g2b/actual` ran custom Lua that appended two calls, executed
`printf observed > .../observed; sleep 3` with one-second deadline, then deliberately
failed before appending results. Exec timed out with unknown effect; marker written
is not containment/success evidence. Explicit `/lua` repair reported repaired2,
unknown/no-replay. Selected ordinary `programs/turn.lua`; actual OpenAI Codex
`gpt-6.1-sol`, medium, then wrote `docs/WORKFLOW_REPAIR_OPERATING.md` with native
write_file. Original failed program, timeout output and admission identities remain
at their paths. Actual allowed reopen checked two retained unknown placeholders.

Outcome useful but imperfect: eight reads mixed byte/line ranges and were correctly
rejected. Model eventually advanced independent documentation from available tool
contracts and honestly recorded its missing source read. That limitation is kept
in the delivered guide, with follow-up arconaut-k48; it does not authorize replay
of the old effects. Ten requests:23301 input +1380 output =24681 total tokens,
1920 cached input tokens reported; account cost unavailable. No speedup claim.

## Publication/activation state

Candidate includes scoped C++/Lua-facing interface, direct test and guides. Initial
build used a source snapshot at8a0e959 to exclude unrelated dirty TUI work. Root
subsequently integrated accepted UI through932ca08 while this unit ran; final native
replacement includes that accepted baseline plus this unit. Unrelated journal/bead
changes are protected. Candidate publication is not acceptance. Actual same-session
RRC observation, final bead/queue update and checked non-force master fast-forward
remain next actions at this checkpoint. Unit/hardening deadlines do not reset.
