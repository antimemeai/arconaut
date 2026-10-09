# AFK instrumentation and reliability unit — 2026-10-09

Operator attaches c54 and 05n to nls and authorizes instrumentation and bug work
while AFK. Evaluation definitions, scoring, runs and comparisons wait for the
operator discussion. Steering and further trajectory-view work remain deferred.
Candidate work continues on command-jobs; no master merge or runtime activation
is implied. Source checkpoints and pushes remain required.

Three independently owned conceptual units start at 22:31:23 UTC. Each has a
90-minute ceiling through 00:01:23 UTC, including at most 25 minutes hardening:
remediation/fix followed by one affected recheck/fix. No new review campaign,
library, live provider probe, mutation run or performance study. Shared builds
use at most six workers; root owns builds and final clean-staged-tree hook.

1. nls.2: inspect provider/tool/program/participant capture paths, write a finite
   capture matrix, and add bounded operational metadata to the existing native
   audit. Keep requested and observed model separate, retain reported usage with
   explicit absent/malformed status, link nested work/participants, preserve retry
   identities and expose capture loss. No inferred billing, success, cross-host
   ordering or clock synchronization; no copied transcripts or second store.
2. c54: check the existing Claude review against current source and primary
   provider envelopes. Repair missing-result reported errors and allocation after
   dispatch with direct envelope and exact terminal-disposition oracles. Disposition
   remains unknown whenever remote effects cannot be determined. Record rejected
   findings without another provider review.
3. 05n: inspect abandoned native file/command/mixed attempts and historical
   evidence read-only. Implement only source-grounded recovery with explicit
   uncertainty and validated linkage, no command replay or old-PID adoption.
   Unsupported historical formats remain unsupported. Invalid custody/linkage
   remains fenced. Exercise actual recovery/reopen without operator-session edits.

Grounding precedes code. Direct instrumentation cases use actual engine dispatch
and committed audit pages: exact requested/actual model and usage values, missing
or malformed values, retry attempts, nested parent identities, participant request
identity, capture refusal/loss and reopen/pinned-prefix stability. Existing source
inspection must stay bounded and avoid reading cold result payloads for metadata.
Mac debug/release and applicable sanitizer cases plus portable Linux checks cover
changed fault classes. The mandatory full debug/C++/Lua hook runs before the final
source checkpoint; no suppression or hook bypass. Results and Beads updates are
recorded before checkpoint so closure does not require an extra code campaign.

Integrated direct debug cases are green: session_recovery3.57s,
operational_metadata0.77s, colleague_engine0.90s, participants0.29s and
command_jobs_engine3.11s (8.65s total). Separate colleague/provider_colleague
red/green evidence is retained. Nested actual parent, bounded metadata, reported
usage, retry identities, participant loss and saved/pinned reopen now have direct
oracles. One instrumentation review found valid long participant fields could
make metadata capture alter a completed outcome; bounded null/status projection
and a counted successful9000-byte request fix it. Encoding refusal enters loss
accounting. No new dependency, provider probe or evaluation.

The hardening allowance includes the review/remediation already begun at
22:46:20UTC: at most25minutes through23:11:20UTC, within each original90minute
unit. One affected profile recheck/fix plus mandatory clean-staged-tree full debug
and owned C++/Lua lint completes integration. No new assurance campaign.

Mac affected nine-case release/ASAN passes11.16s/16.51s. The final bounded-field
projection rechecks pass3/3 release4.47s and ASAN6.95s; final TSAN9/9 passes23.32s.
Neuroses Linux first snapshot and final projection recheck continue; captures are
under context/linux and are historical until their commands complete. Full debug
and complete owned C++/Lua analysis are enforced by the ordinary checkpoint hook;
this note does not claim that pending gate passed. Closing nls.2/c54 records the
implemented direct-oracle result; a failed gate still prevents publication.
05n stays open for unsupported historical payloads and the unidentified original
session/error. Beads supported backup records these dispositions. Evaluations wait.

Final stable Linux snapshot passes3/3 debug2.25s, release1.93s, ASAN3.31s,
including actual long participant metadata, pinned compact reopen and command
capture projection (context/linux/run-t614eecp/checks.log). Earlier nine-case
Linux debug/release/ASAN passes3.74s/3.19s/5.82s. Remote workspaces cleaned.

First mandatory gate passes all85 debug tests in74.49s, then clang-tidy finds
two unchecked optional accesses in new recovery fixtures: a custom require does
not establish analyzer-visible control flow for the later dereference. Stop that
failed gate, add explicit presence guards without weakening outcome/linkage
assertions or suppressing diagnostics, and rerun the same mandatory hook. Product
source stays unchanged; no repeat host campaign. The same23:11:20UTC hardening
ceiling applies. Final gate results are established by the guarded source commit,
not by this earlier pending note. No hook bypass or new suppression.

## Final disposition at the hardening bound

The25minute source hardening window ends23:11:20UTC. No further instrumentation
repair is made after that bound. The final mandatory gate completes C++ analysis
and rejects one remaining easily-swappable-parameters diagnostic in the added
participants_test callback (label/raw). Full debug85/85 passes78.83s; all planned
Mac/Linux dynamic cases pass. Complete C++/Lua gate has NOT passed for this draft;
no source activation or qualified instrumentation delivery is claimed.

Preserve the full integrated state in Git stash "AFK instrumentation draft at
declared hardening bound" and context/nls-afk/integrated-before-split. The reviewable
nonexecuting [instrumentation overlay](../papers/2026-10-09-operational-instrumentation-draft.patch)
is retained in papers and is not applied to this checkpoint. nls.2 remains open.
Applying/repairing that held candidate needs explicit resumed operator scope; the
old allowance is not reset. Evaluation selection/execution remains deferred.

As independent useful delivery, package only the already source-checked colleague
and native recovery fixes. Their code is unchanged from the passing affected
checks; remove the held instrumentation overlay from the active source tree.
The ordinary every-commit full debug/C++/Lua gate qualifies that coherent bug-only
checkpoint. No new review, host/profile campaign, dependency or hardening repair.
05n retains its historical payload/original-session residual; no main activation.
