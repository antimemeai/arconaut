# Backstop recovery: source and literature consequences

Read for the operator's explicit request; design in
[BACKSTOP_RECOVERY](../docs/BACKSTOP_RECOVERY.md). No reference executed or adopted.

## Primary literature

Candea and Fox's [Crash-Only Software (HotOS2003)](https://www.usenix.org/legacy/events/hotos03/tech/full_papers/candea/candea_html/)
separates externally controlled component lifetime from persistent state and
emphasizes containment and recovery-aware interactions. Sections2–3 drive an
independent backstop, preserved state and bounded lifetime observations. This is
an architectural consequence, not a claim that Arco currently satisfies their
component contracts. We do not adopt transparent retry of arbitrary tool effects.
Local PDF: recovery/candea-fox-crash-only-2003.pdf (ignored; source URL above).

Candea et al.'s [Microreboot (OSDI2004)](https://www.usenix.org/legacy/events/osdi04/tech/full_papers/candea/candea_html/index.html)
discusses separation of data/process recovery and dependency-aware recovery groups.
Consequence: restart the affected harness/workflow while retaining originals and
leaving shared services alone. A native process restart cannot establish whether
remote effects succeeded; resource-specific reconciliation remains necessary.
Local PDF: recovery/candea-microreboot-2004.pdf (ignored).

## Acquired implementation sources read

- quarantine/auto-harness/autoharness/session/resume.py, format_briefing: completed,
  active, failed/do-not-retry, questions and next-step briefing. Transfer this small
  recovery packet shape; labels remain assertions until supported by actual records.
- quarantine/auto-harness/autoharness/context/recovery.py: output-recovery counter
  and reset, plus generic exception retry. Transfer bounded per-cause recovery;
  reject generic exception retry for arbitrary effects and reset-on-normal-output
  as the sole criterion for useful recovery.
- quarantine/agentchat/lib/supervisor/agent-supervisor.sh: lifecycle/runner separation,
  stop forwarding, wait and forced termination, state outside runner. Transfer
  separation. PID existence and leader-only kill are inadequate for our quiescence
  claim; do not adopt auth handling, libraries or source code.
- Existing Codex logical fork/physical base study remains in HISTORY_CAPACITY_SUBPLAN.
  Use explicit source lineage rather than pretending a new audit settled its parent.

Existing quarantine manifests identify these acquired reference archives; sources
are historical references, not active workspace instructions.

## Our current seams and gaps

src/main.cpp runs qualified provider-only startup reconciliation; unresolved local
operations block ordinary reopen. SESSION_RECOVERY_SUBPLAN correctly distinguishes
provider unknowns from tool effects. A fresh assessor can inspect independent
source without treating a blocked predecessor as operationally settled.

src/native_process.hpp Child destructor kills its group only while leader pid is
positive; collect sets pid=-1 on reaped leader. There is no exposed persistent
registry/complete descendant quiescence API. Parent termination or session exclusion
alone must not become a boolean “nothing in flight.” Detached OS services and
escaped children require distinct disposition.

External evotools/scripts/arco-campaign.lua is provider-only, uses os.execute for
native launcher and records supervisor PID. It stops on capacity. It neither owns
structured quiescence/attempt state nor performs model assessment/selected pivot.
Its transient classifier still expects legacy io curl codes whereas native retry
now uses provider_transport; eventual bridge integration must align typed failure
classification without retrying unrelated io errors or bypassing new controls.

Current .14.3 integrated workflow reserve/maintenance implementation is in progress
in the shared checkout. This study deliberately does not edit that source or claim
its tests pass. Native protected settlement, explicit successor destination and
bounded old-original access are inputs to the backstop. Avoid duplicating their
state machines in a text-log parser.

## Disposition

Propose an independently audited recovery conversation after confirmed local
quiescence, retained unresolved-effect locators, explicit selected handoff and
bounded outcome-driven pivots. Implement a whole useful recovery path, not a
restart-status facade. Ordinary successful exit, explicit pause and healthy long
work are not stuck incidents. No new dependency/provider adopted.
