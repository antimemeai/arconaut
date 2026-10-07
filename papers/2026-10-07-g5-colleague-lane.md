# G5 independent colleague lane

Start 2026-10-07 04:24:59 UTC; whole-unit deadline 05:19:59 UTC (55 minutes;
outer process ceiling60). Candidate `candidate/giga-colleagues`, baseline8c5fcc3.
No shared core ABI/process-lifetime work; Root integrates serially after exit.

## Source-grounded subplan

Deliver a callable one-request context-only colleague contract in C++, using the
accepted native OpenAI Responses transport and installed Claude CLI via LocalTools
exec. Context consists only of explicit ID/text selections, not automatic parent
history. Participant addressing is independent of provider choice. Admit and retain
before dispatch; retain actual upstream bytes and distinguish local failure from
remote uncertainty. No automatic replay, implicit session selection or continuation.
Missing authentication blocks live heterogeneous completion, not independent coding.

Direct failing oracle: no colleague target existed on baseline; first compile found
an ambiguous local helper name; corrected without changing core. Direct tests use
counted fake dispatches, malformed replies, forbidden profiles, admission failure,
and an actual exec timeout with partial bytes. Useful task: diagnose accepted
`completed_response` semantics using selected source, not a ceremonial review.

### Research consequences

Primary literature: Anthropic, *How we built our multi-agent research system*,
https://www.anthropic.com/engineering/multi-agent-research-system (read2026-10-07,
local capture `context/g5/anthropic-multi-agent.html`). Separate contexts and explicit
objective/output/tool/scope boundaries inform selected-context JSON and the single
bounded request. Their discussion of coding coordination limits argues against
inventing a scheduler or shared room in this unit. No effectiveness claim transferred.

Primary CLI documentation https://code.claude.com/docs/en/cli-reference and actual
installed `claude --help` establish print JSON, explicit model, disabling tools,
settings sources, MCP selection and session persistence. These are tool restrictions,
not an OS sandbox or a guarantee against model prompt injection. No SDK adopted.

Read quarantined AgentPool `src/agentpool/delegation/teamrun.py` and
`utils/model_capabilities.py`: model/capability choices and errors are explicit;
missing capability data must not become invented support. Thus actual model and usage
remain null when absent. Read coordination-services source study and NTM mechanisms:
record-before-send and no replay of uncertain submission; a delivery acknowledgement
is not execution. Admission capture failure prevents counted dispatch; timeout and
malformed/partial upstream replies remain unknown. No quarantined code copied/run.

## Hardening bound

Layer one began04:33UTC, allowance at most25min, deadline04:58UTC inside whole unit.
Concrete remediation: malformed request must refuse before dispatch; wrong upstream
JSON types must yield unknown, not escape as successful CLI execution; expose actual
structured CLI authentication failures even with nonzero exit; retain partial bytes
and prohibit replay. Add direct cases, format/analyze only owned units; affected
native Mac release/sanitizer and Linux contract tests, not unrelated fullsuite.
Layer two: ONE scoped recheck of these changes, fix findings inside the same allowance.
No third layer, certification or reviews of reviews. At bound unresolved candidate
stays inactive with exact blocker. Settled core checks are not repeated.

## Direct observations and layer-two findings

Mac release and ASan/UBSan contract tests passed; scoped Clang-Tidy on the three
owned translation units had no diagnostics. Linux Clang18.1.3/libstdc++ build with
ASan/UBSan and the same contract oracle passed on Neuroses in an isolated `/tmp`
directory. Only affected colleague tests ran; no mutation/fullsuite campaign.
First layer's expanded test initially failed because the old Claude fixture lacked
new required type/subtype fields. Corrected the fixture to the actual CLI result
shape, then the direct cases passed. This was a fixture mismatch, not hidden success.

Layer two began04:38:41UTC. ONE scoped source recheck found: input capacity validation
occurred after potentially expensive context traversal; Claude model attribution
had an inconsistent scalar/map shape; known failure usage was unnecessarily omitted;
CLI captures lacked owner-only directory permissions. Fixed by validating capacity
before traversal, retaining `model_usage` separately and setting scalar actual_model
only for exactly one model, preserving reported failure usage, and restricting the
new capture directory before writing. Run only affected build/tests for these fixes.
No third assurance layer or provider retry is planned.

Actual OpenAI source diagnosis completed through `arco-colleague` in
`context/g5/live-openai-1`: requested/actual `gpt-6.1-sol`, input2128/output1487,
total3615 tokens reported. It established from selected source that baseline
`completed_response` permits structurally completed empty/refusal/tool outputs.
Adapter now requires assistant output_text, rejects tool-call output as unknown,
and distinguishes provider refusal from task completion. Direct cases implement
these source-derived expectations. This is useful diagnosis, not a review verdict.

Actual heterogeneous attempts: Claude CLI returned401 expired OAuth (despite
`auth status` loggedIn); Kimi Code `kimi-code/k3` returned403 subscription denial;
installed OpenCode's listed MiMo free endpoint returned403 service policy refusal;
configured OpenRouter/MiMo returned401 expired API key. All raw outcomes retained
under `context/g5`. Kimi skill, installed CLI help and provider list were read;
OpenCode model/auth listing and primary run/config/agent source were studied to
make bounded no-tool probes with isolated config and deny permissions. OpenCode
was NOT added to production because live access failed and its extra inference/
configuration/lifetime semantics need qualification before a bounded adapter claim.
No authentication/package install, credential publication, purchase or new provider
SDK was attempted. No successful cross-provider interaction or second combination
is claimed. Access remains the exact G5 live-completion blocker.

### Disposition

Layer-two affected release/ASan/UBSan tests and scoped Linux sanitizer build/contract
oracle passed. Existing capture-directory CLI check refused with exit2 before any
provider dispatch. Scoped layer-two Clang-Tidy has no diagnostics. Stop here: no
third source review, global suite or provider rechecks. Release `arco` was built
locally as requested for compiled work but no primary binary/harness was replaced;
`arco-colleague` is the independently exercised executable.

Useful native source slice retained on candidate for Root's serial integration;
not promoted or activated into the main harness. Broader G5 remains BLOCKED on
actual non-OpenAI provider access, not declared complete. No known remaining defect
from this bounded adapter work; live Claude completion/profile behavior beyond the
observed authentication refusal remains unqualified. An operator-restored account
or a different actually configured working endpoint is needed for real cross-provider
completion and a second available combination. Do not repeat the failed requests
implicitly. No standing/detached colleague process is left by this lane.

Root artifacts relative to lane checkout:
- `context/g5/lane-metadata.json` start/deadline and hardening bound;
- `context/g5/live-openai-1/{admission,upstream_result,result}.json`, `raw.bin`;
- `context/g5/live-claude-1/` failed authenticating CLI capture;
- `context/g5/kimi-events.jsonl`, `kimi-stderr.txt` subscription refusal;
- `context/g5/{mimo,openrouter-mimo}-events.jsonl` configured endpoint failures;
- `context/g5/layer2-{release,asan}-test.log`, `linux-layer2.log`, `layer2-tidy.log`.

Capture files are ignored local operator data, not published source; this report
summarizes actual outcomes without credentials/raw private audit. The native common
API allows arbitrary caller addresses independent of chosen implemented provider;
new providers can add transport/decoder branches without changing participant
addressing. No universal upstream capability or arbitrary-provider support is claimed.
Core integration assumption: Root keeps accepted LocalTools/OpenAI semantics and
reconciles recovery-lane lifetime fixes once. This slice never owns/refactors that
invariant and does not detach or claim remote cancellation settlement.
