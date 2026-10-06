# Python operator interfaces: Aider, GPTMe and original Open Interpreter

Source study completed 2026-10-01 against the revisions recorded in the
[registry](../registry.json). These are three separate execution arrangements,
not interchangeable examples of a Python harness. The Rust Open Interpreter
reference has its own row and native-family study. No acquired programs or tests
were executed. Findings below are source traces and inspected test oracles;
potential failures are identified as source-inferred.

| Reference | Actual computation | Model agency and operation surface | History and lifetime boundary |
| --- | --- | --- | --- |
| [Aider](../dossiers/aider.md) | Synchronous model/edit/reflection loop; fresh shell/PTY command; separate summarizer thread | Selected-file edit formats, suggested commands, operator slash commands, sequential architect/editor models | Formatted optional chat/request logs; bounded model summary; no standing interpreter or live peer room traced |
| [GPTMe](../dossiers/gptme.md) | Synchronous CLI step; continuing shell/background jobs; thread/subprocess children; global IPython | Executable Python, callable functions/hook registry, actual save/patch, child model/context/profile and steering | Main messages retained beside compacted views; recovery events periodically replaced by current checkpoint; Python/capture global state |
| [Open Interpreter, Python](../dossiers/open-interpreter-python.md) | Continuing owned Jupyter and shell objects; synchronous response generator; optional async server around a response thread | Model code blocks, mutable computer/language/settings objects, file operations and optional saved/imported Python skills | Current message JSON and trimmed context; no original effect ledger or managed context lineage traced; interruption varies by language |

## Aider: a small edit and reflection policy

The actual edit path parses model search/replace blocks, attempts each write and
reports failures for reflection. A failed match can trigger a search among other
selected files. Successful edits are already written when later edit failures
are reported. Git bookkeeping, lint, shell and tests then feed the bounded
reflection policy; this is not a transaction over the entire proposed change.
The architect path invokes a separate editor model with a fresh chat and merges
bookkeeping afterward. It is sequential role specialization, not independently
working colleagues who can exchange live messages.

Shell suggestions and general automatic confirmation have different authority.
Suggested command execution asks for explicit confirmation. In the inspected
`--yes` path, an `explicit_yes_required` prompt selects a negative response.
This matters for an operator expecting an autonomous coding agent: the flag does
not authorize the complete command path. Actual shell execution streams merged
output and waits; the inspected pipe/PTY paths do not supply continuing job
ownership, a deadline or a process-group settlement protocol.

History reduction supplies a particularly useful failure case. The background
worker begins with an empty replacement. If every summary model fails, it catches
`ValueError` and warns without publishing a replacement. The join path can then
assign that empty default if the source snapshot still matches. **Source-inferred:
completed history can be discarded on summary failure.** The inspected history
tests check literal summary shaping and a successful fallback model; they do not
exercise this worker publication failure.

The benchmark restores the original task tests before invoking the selected
language runner and checks its exit code. That is a useful independent witness
for the particular task suite. It does not make ordinary repair reflection a
governed harness-improvement experiment, nor does formatted diagnostic logging
retain the original provider stream and every abandoned attempt. See the
[Aider dossier](../dossiers/aider.md) for source ranges and inspected tests.

## GPTMe: normal executable agency, with distinct state scopes

The Python tool invokes a continuing IPython instance, exposes registered host
helpers and permits Python imports. The hook registry can be changed through
runtime APIs. Hook invocations take snapshots of enabled callbacks; hooks can
add generation context, affect tool-use data and run at step/turn boundaries.
These are concrete ingredients for normal model agency over behavior. They are
supporting machinery for replaceable turns/workflows; the inspected CLI loop is
still a host program. Runtime callback registration does not by itself implement
Arconaut's default activation after the current turn and affected workflows.

The child path explicitly initializes selected tools, model, profile and context
inside separate context-local registries. Its offline echo test drives a real
child completion, then requires a subsequent actual parent shell call to work
with the original hook/tool state. That is stronger than comparing two mocked
configuration dictionaries. However, the Python interpreter singleton and
replacement of `sys.stdout`/`sys.stderr` remain process-global. **Source-inferred:
the traced child isolation does not establish separate Python variables or
capture ownership for simultaneous Python clients.** Those need their own
independent witnesses.

Shell promotion is an actual transfer of custody. When a POSIX foreground budget
expires, the existing shell and worker are registered as a background job; a
replacement shell permits subsequent work, and another thread publishes the
original job's eventual output. The inspected test requires prompt return,
continuing job liveness, and both before/after output at eventual exit. This is
a useful primitive for standing computation. These jobs remain harness-owned;
external shared service consumption would require a different ownership boundary.

Cancellation remains weaker than settlement. Thread cancellation publishes a
cancelled result before the next cooperative checkpoint stops chat. ACP mode
marks the result without that checkpoint. The subprocess path terminates and
waits for the root; the shell helper escalates from an interrupt to termination
without a final wait in that helper. A response saying cancelled therefore cannot
serve as the refit barrier.

Context management keeps an original main message history and separate compacted
views, dual-writes subsequent messages, persists the selected view and permits a
return to master. Trimmed output can carry master byte-range references. Tests
read the actual main/view files and reload them, providing direct witnesses for
retained message repair. That should remain distinct from full original audit:
the CLI buffers a whole model/tool step before persistence, Python can return a
`Message` early and omit captured streams, and the recovery log discards events
before its latest checkpoint. State recovery and original event custody are
different guarantees. See the [GPTMe dossier](../dossiers/gptme.md).

## Original Open Interpreter: persistent computation without a settled stop

The Python implementation re-renders mutable prompts, language definitions and
computer settings before model requests. Code dispatch validates the selected
language and confirmation, then executes through a cached language object.
Python uses an owned Jupyter kernel and shell uses a continuing subprocess with
reader threads and injected completion markers. This is actual stateful
computation rather than a prompt suggesting that variables persist.

The common stop interface does not imply that every language stops. Shell
inherits a no-op `stop`; termination requests close streams without a wait. The
Python listener requests an interrupt but does not join the listener or prove
effects have stopped. Its traced IOPUB path treats an idle status as completion
without checking a parent request identity there. The async server sets flags
and synchronously joins its response thread; a blocking request can therefore
hold up the async host. **Source-inferred:** these paths do not establish the
quiescence needed before replacing the harness.

Mutable settings are applied sequentially and can partially change before an
error, without a candidate/activation transaction. The optional skill mechanism
can write/import Python functions, but file existence after saving a skill is
not a performance or correctness oracle. The computer AI helper's parallel work
temporarily rebinds the same interpreter's messages and system prompt; this is
not a collection of isolated live model colleagues.

The persistence path excludes selected review/active-line events, truncates
program output in stored messages and overwrites current JSON after a response.
Context trimming has no traced original/repair lineage. Basic Python/shell tests
assert actual outputs; mocked response tests check confirmation and truncation.
They do not attack descendant process survival, request attribution or failed
context publication. See the [original Python dossier](../dossiers/open-interpreter-python.md).

## Questions transferred to Arconaut

The strongest transferable mechanisms are executable normal-operation agency,
separate original/context views, real continuing-job custody and independent
task-test material. None of these three establishes Arconaut's complete compiled
refit/outpost contract. None selects Python as the implementation language.

Before specifying the host boundary, distinguish the authority to install a
program from the time at which its definition becomes active. Require an original
record before context replacement and before effects are exposed as complete.
Test callback failures, all-summary-model failure, simultaneous interpreter
clients, cancellation followed by a continuing effect, and context repair after
an overzealous transformation. Those experiments should use witnesses outside
the implementation under test, rather than treating the agent's own status or
summary as proof.
