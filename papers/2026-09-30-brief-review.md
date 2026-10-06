# Working brief and design discussion: review dispositions

2026-09-30. Parent integration record. The independently written reports below
reviewed the [working brief](../docs/BEHAVIOR.md) and the candidate originally at
docs/DESIGN.md, now [foundation design discussion](2026-09-30-foundation-design-discussion.md).
The operator clarified the phase during this pass: develop/review/discuss the brief,
then specify in pen from the ground up. Accordingly the candidate moved to papers
as discussion input. No architecture/language/exact contract was adopted and no
implementation plan, product code, runtime test, or model comparison was produced.

The reviewers attacked the parent integration, including across the conceptual
units they had researched. This is not independent certification of every source
study, exhaustive correctness, or a second audit receipt apparatus.

## Independent reviews and disposition

| Review / finding | Concrete fault | Integrated correction |
| --- | --- | --- |
| [Runtime R1](2026-09-30-design-review-runtime.md) | A former conversation owner could dispatch after an ownership label changed | Epoch checks at supported admission/dispatch; close and settle outgoing work before both handoff and return; late observations do not grant continuation authority |
| Runtime R2 | Main gate had no precise cohort, exceptions, or reopening | Named main cohort and effect gate; ingress/observation/control/outpost startup remain available; reopen after successor state/ownership/attachments accepted |
| [Integration R1](2026-09-30-design-review-integration.md) | Query text did not preserve the result actually seen; watches could recursively feed delivery bookkeeping | Retain request/actual result and finite source cursor; default workload feeds exclude audit-observation/delivery bookkeeping globally; originals remain finitely queryable |
| Integration R2 | A lagging view could be treated as authoritative live state | Operational decisions use authoritative ledger; projected cursor/lag are explicit |
| Integration R3 | A stuck controller could strand active provider cancellation/close | Independent active transport handle/worker registered before dispatch; semantic definitions can remain in controller; local completion is distinct from unknown remote outcome |
| [Workflow R1](2026-09-30-design-review-workflows.md) | Late tool results after compaction could orphan or misorder their calls | Per-request continuation branches and protocol obligations; explicit next-context choice/combination and ordinary provider structural checks |
| Workflow R2 | Crash after action admission but before continuation persistence could duplicate an effect under a new ID | Durable supported-decision state and stable planned-action IDs before admission; recovery inspects linked results; arbitrary lost frames stop for reconciliation |
| Workflow R3 | Failed migration could have real external effects despite preserving the old object | Qualify usable-old-state guarantee to isolated/transactional owned-state conversion; other effects remain recorded and need reconciliation |
| Workflow R4 | “Compiled changes use refit” contradicted live compiled Lisp policy | Distinguish live compiled-definition activation from controller/native executable replacement |

All nine findings were valid and integrated. Each independent reviewer reread their
corresponding fixes and recorded resolution within the original review scope. The
watch issue's mutual-feedback refinement is part of Integration R1, not a new receipt
or distinct independent validation layer. Direct future fault scenarios are in the
review reports and brief; none was executed in this pass.

The revised brief also incorporates decision/action recovery and late continuation
structure as proposed behavior. The discussion paper retains acknowledged limits:
SBCL not available on local PATH; pause/descendant scope varies by platform; owner
crash/replacement is not established; exact HTTP capture/independent close needs
feasibility evidence; recording can have an unconfirmed crash tail; no language or
model-ergonomics superiority is measured. A source-supported proposal is not a
shipping guarantee.

## Handoff to discussion

The brief now covers ordinary repository work, a live room, redefining a turn,
managed compaction, paused-standing-work refit, messy audit, and one autodroit
improvement. It supplies purpose, explicit operator intent, proposed semantics,
direct outcomes, and a short discussion agenda. The exploratory candidate shows
how those might compose and exposes the difficult choices without settling them.

Discuss scope and the working experience next. Then develop the authoritative
specification from entities/identity, lifetimes/ownership, time/order, state/effects,
and observations upward. Reviewed specification precedes implementation planning
and legacy disposition. The quarantined June implementation remains unchanged.

## Subsequent discussion clarification: consumer and fabric ownership

The operator subsequently corrected the premise that Arconaut owns resident kernels,
databases, and computational hosts. These may be shared services consumed by many
Arconauts; their governor/control-plane/scaling belongs to the eventual meta-project
fabric. The [current brief](../docs/BEHAVIOR.md) and
[intent record](../docs/FOUNDATION.md#discussion-decision-arconaut-consumes-the-computational-fabric)
now state that boundary and limit refit pause to Arconaut-owned work/client activity.

The candidate paper is explicitly retained as a pre-clarification proposal. The
review dispositions above concern its historical contract defects, not acceptance
of its resource ownership or a new review of the corrected consumer architecture.
The ground-up specification must address service/workload references, reconnect,
shared-state observations, and actual client exchanges; service custody/child-wait
questions are not automatically Arconaut's responsibility. No fabric governor or
scaling implementation is a prerequisite for early Arconaut self-development.
