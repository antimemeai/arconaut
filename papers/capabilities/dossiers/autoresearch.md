# autoresearch

A narrow, explicit edit/run/measure/keep protocol with a fixed evaluator is useful for autodroit, but its agent governance is authored instructions.

Role: research protocol and GPU experiment program. Runtime: Python, Markdown protocol.

Pinned source: [https://github.com/karpathy/autoresearch](https://github.com/karpathy/autoresearch); revision/version `228791fb499afffb54b46200aca536f79142f117`.

External coding agent edits a Python training program and launches separate GPU processes; repo contains no autonomous provider loop.

Consumes an externally supplied agent/GPU environment; does not supply harness/provider/service ownership.

Inspection: Read full program instructions, README role, train timing/failure/eval loop and bits-per-byte evaluator.

Limits of this study: No GPU training, agent invocation or benchmark reproduction; no authored agent engine or test suite in snapshot.

## Actions

### edit/commit/run/measure/keep/discard loop

Surface: documented agent workflow.

Input: Candidate train.py, tag and hypothesis

Result: Commit, val_bpb/VRAM, keep/discard/crash TSV row

Lifecycle: External agent runs iterations; shell/provider identity lives outside repo.

Authority: External agent under authored edit scope

Evidence: [protocol](#evidence-protocol), [loop](#evidence-loop).

### train.py

Surface: experiment program.

Input: Model/optimizer/training definitions and prepared GPU data

Result: Training log and printed BPB/VRAM/time metrics

Lifecycle: One GPU process, measured five-minute training budget excludes warmup.

Authority: Host command execution

Evidence: [train](#evidence-train), [result](#evidence-result).

### evaluate_bpb

Surface: evaluation API.

Input: Model, tokenizer and batch size

Result: Validation bits per byte

Lifecycle: After candidate training; no harness activation.

Authority: Fixed evaluator calls editable model loss

Evidence: [eval](#evidence-eval).

## Capabilities

### filesystem

**D — Files** (documentation): External coding agent is instructed to edit only train.py and read fixed evaluator; repo supplies no file tool implementation.

Evidence: [protocol](#evidence-protocol).

### processes

**D — OS programs** (documentation): External agent launches uv run train.py and is instructed to kill >10-minute runs; lifecycle control is the host agent.

Evidence: [loop](#evidence-loop).

### code-actions

**S — Code actions** (source): Candidate is executable training code, independently run against fixed data/evaluator; no programmable model action runtime.

Evidence: [train](#evidence-train).

### persistent-kernel

**— — Kernel** (role): persistent_kernel: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### standing-database

**— — Standing DB** (role): standing_database: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### workflow-programming

**D — Workflows** (documentation): Markdown specifies a perpetual candidate/measure/keep loop; host agent must execute it.

Evidence: [loop](#evidence-loop).

### multi-model

**— — Models** (role): multi_model: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**— — Concurrency** (role): concurrent_work: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### steering-interrupt

**— — Steer/interrupt** (role): steering_interrupt: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): Overwritten run.log and summary TSV plus git candidates do not retain complete original agent/tool/provider IO or all failed attempts.

Evidence: [loop](#evidence-loop).

### audit-query

**D — Audit query** (documentation): Model reads run.log and results.tsv; no standing queryable audit database.

Evidence: [loop](#evidence-loop).

### hot-change

**— — Hot change** (role): hot_change: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### rebuild-continuity

**— — Rebuild continuity** (role): rebuild_continuity: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### remote-services

**— — Remote** (role): remote_services: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### self-improvement

**S — Self-improve** (source): Dedicated candidate code and evaluate/keep workflow can improve training, not automatically the host harness; model selection/governance is instruction-level.

Evidence: [protocol](#evidence-protocol), [loop](#evidence-loop), [eval](#evidence-eval).

### complaints

**— — Complaints** (role): complaints: this reference supplies research protocol and GPU experiment program; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**D — Authority** (documentation): Protocol authorizes autonomous edits inside one file after initial setup; library/evaluator restrictions are instructions, not enforcement.

Evidence: [protocol](#evidence-protocol), [loop](#evidence-loop).

### evaluation

**S — Evaluation** (source): BPB against held validation data and crash/time rules provide an external metric; evaluator trusts editable model loss behavior, not a harness correctness TCK.

Evidence: [eval](#evidence-eval), [train](#evidence-train).

### time-order

**L — Time/order** (source): Training budget accumulates time.time deltas after warmup with CUDA synchronization; operator kill deadline is host instruction and wall-clock jumps are not handled here.

Evidence: [train](#evidence-train), [loop](#evidence-loop).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Tiny editable surface and explicit fixed measurement make experimental progress intelligible.
- Keep/discard/crash vocabulary records unsuccessful experiments as part of research.

## Material limits

- No agent runtime, continuous self-hosted refit or invariant-preserving oracle supplied.
- Fixed prepare.py does not prevent a malicious/incorrect editable model from returning misleading loss; scientific score is narrower than system correctness.

## Arconaut design questions

- How can autodroit keep model-directed experiment freedom while retaining independent fault-class oracles and failed attempts?
- Which experiment objective is external to the changing harness and cannot be silently altered by it?

## Evidence

### Evidence protocol

[quarantine/autoresearch/program.md:20–50](../../../quarantine/autoresearch/program.md#L20): Editable train.py and read-only prepare.py/evaluation constraints; baseline first, metric and simplicity selection.

### Evidence loop

[quarantine/autoresearch/program.md:90–114](../../../quarantine/autoresearch/program.md#L90): External agent commits candidate, runs to log, reads metric, records TSV and keeps/resets; ten-minute kill is an instruction.

### Evidence train

[quarantine/autoresearch/train.py:535–605](../../../quarantine/autoresearch/train.py#L535): Training loops with CUDA synchronization, NaN/loss fast-fail and measured budget excluding warmup.

### Evidence eval

[quarantine/autoresearch/prepare.py:344–370](../../../quarantine/autoresearch/prepare.py#L344): BPB computes loss per target byte over validation tokens with fixed sequence length; editable model produces per-token loss.

### Evidence result

[quarantine/autoresearch/train.py:607–630](../../../quarantine/autoresearch/train.py#L607): Final printed metric, time, VRAM, tokens and model shape feed external selection.

