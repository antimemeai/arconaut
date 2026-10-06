# coscientist

Public supporting data plus a toy command-routing implementation; CALCULATE/RANDOM/STOP are the actually shipped example tools.

Role: research supplementary data and simple agent example. Runtime: Python.

Pinned source: [https://github.com/gomesgroup/coscientist](https://github.com/gomesgroup/coscientist); revision/version `417e82b85743f88719ba7cf8838220fcb872e2ae`.

A minimal synchronous OpenAI text-command loop with injected callable tools; accompanying chemistry CSV artifacts.

Owns in-memory history and example callable dispatch only; full experimental robotics/search system from the article is not this simple implementation.

Inspection: entire simple_implementation loop/tool bodies/launch composition and repository scope README

Limits of this study: No full research system or first-party test suite exists in inspected inventory. No code/chemistry actions/providers executed; paper results are not runtime conformance proof.

## Actions

### Coscientist.run / get_next_message

Surface: operator/API.

Input: Prompt,max_steps, injected command_name/callable/prompt objects and config model.

Result: Mutated history/log messages; no returned durable run handle.

Lifecycle: Synchronous bounded model/dispatch iterations.

Authority: Host chooses callable dictionary; provider authority external.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5).

### CALCULATE / RANDOM / STOP

Surface: model text commands.

Input: Python expression; two integer bounds; ignored STOP input.

Result: Expression result/error string; random integer; termination exception.

Lifecycle: One command-leading line per text reply; literal substring count feedback.

Authority: Host eval authority unrestricted; no actual chemistry/robot tool is supplied.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

## Capabilities

### filesystem

**S — Files** (source): No native filesystem tool; eval can call arbitrary Python expression with host/module authority. That generic escape capability is not a designed file operation contract.

Evidence: [e4](#evidence-e4).

### processes

**S — OS programs** (source): No process/job API; unrestricted expression evaluation can invoke imported/host operations when composed. No lifetime/interrupt/quiescence semantics supplied.

Evidence: [e4](#evidence-e4).

### code-actions

**L — Code actions** (source): CALCULATE directly evals arbitrary expression; no cell/kernel/schema or incremental source-edit mechanism. Count-by-substring routing can misclassify incidental command text.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### persistent-kernel

**? — Kernel** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### standing-database

**? — Standing DB** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### workflow-programming

**S — Workflows** (source): Host injects callable tool objects and prompt metadata; model sequences one command per response. Fixed loop and example are not hot turn-program redefinition.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5).

### multi-model

**? — Models** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### live-collaboration

**? — Peer chat** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### concurrent-work

**? — Concurrency** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### steering-interrupt

**L — Steer/interrupt** (source): STOP is model-controlled termination exception and max_steps bounds iterations; no provider cancellation or operator live steering exists in inspected full simple loop.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### turn-redefinition

**? — Turn program** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### compaction

**? — Compaction** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### context-repair

**? — Repair** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### original-audit

**L — Original audit** (source): Logging records role/text for history messages; history is in memory. No immutable raw provider calls/tool arguments/results, execution events or durable restore contract.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### audit-query

**? — Audit query** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### hot-change

**? — Hot change** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### rebuild-continuity

**? — Rebuild continuity** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### remote-services

**I — Remote** (source): OpenAI ChatCompletion call consumes configured model. Research web/robotics services are not present in shipped toy tool roster.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5).

### self-improvement

**? — Self-improve** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### complaints

**? — Complaints** (inspection): Not established in the explicitly inspected entire simple_implementation loop/tool bodies/launch composition and repository scope README; no universal absence claim.

### authority

**L — Authority** (source): Injected callables and eval run with Python host authority; no approval/isolation gate in full simple implementation. STOP and max_steps do not bound expression effects.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### evaluation

**— — Evaluation** (inspection): The supplied runtime is a toy command example; accompanying paper data is not an executable independent evaluation/oracle in this implementation. README scopes supporting data explicitly.

Evidence: [e1](#evidence-e1).

### time-order

**L — Time/order** (source): Synchronous message/command loop preserves list order and logs it; no durable IDs/generations/causal ledger.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Very small inspectable example of tools described by objects and sequential feedback.
- Clear public supplementary-data boundary.

## Material limits

- Full experimental Coscientist implementation cannot be inferred from toy example/data.
- Substring command detection and unrestricted eval have no robust action/authority lifecycle.

## Arconaut design questions

- How can flexible operator-injected actions carry typed identity/lifetime without relying on incidental text matching?

## Evidence

### Evidence e1

[quarantine/coscientist/README.md:1–17](../../../quarantine/coscientist/README.md#L1): Repository labels itself supporting information with chemistry data and a simple implementation; not the full apparatus.

### Evidence e2

[quarantine/coscientist/simple_implementation/coscientist.py:5–39](../../../quarantine/coscientist/simple_implementation/coscientist.py#L5): Injected callable tools generate prompt descriptions, model receives in-memory message history and model text is logged/appended.

### Evidence e3

[quarantine/coscientist/simple_implementation/coscientist.py:41–91](../../../quarantine/coscientist/simple_implementation/coscientist.py#L41): Substring tool counts enforce zero/one/many feedback; first command-leading line dispatches one string argument and output becomes user message; StopIteration terminates.

### Evidence e4

[quarantine/coscientist/simple_implementation/tools.py:10–55](../../../quarantine/coscientist/simple_implementation/tools.py#L10): CALCULATE uses unconstrained eval returning str/result or exception; RANDOM parses two integers; STOP raises custom StopIteration.

### Evidence e5

[quarantine/coscientist/simple_implementation/run.py:5–34](../../../quarantine/coscientist/simple_implementation/run.py#L5): Shipped example only composes CALCULATE/RANDOM/STOP with fixed gpt-4-0314 and ten-step limit.

