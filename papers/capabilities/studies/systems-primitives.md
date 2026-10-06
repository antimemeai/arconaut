# Systems primitives: state, computation and replacement

These nine references supply machinery, not nine more coding agents. Their matrix
cells marked S mean composition is required. Read their linked rows for exact pinned
sources, actions, inspected tests and applicability limits. No reference runtime or
suite was executed for this study.

SBCL and SLY make native compilation, live definition changes, object inspection,
condition restarts and addressed evaluation ordinary operations. They demonstrate
that compiled execution need not imply an edit/build/restart cycle for every program
change. They do not supply Arconaut's activation boundary. SBCL images preserve global
state, unwind stacks, replace stream state and require thread quiescence; even another
build of the same runtime is incompatible. SLY recordings are selective in-memory
debugging observations, with a noted multiple-client limitation. Neither establishes
a complete original agent audit or a compiled-core refit handoff.

Erlang/OTP offers a different continuity mechanism: old/current bytecode can coexist,
with explicit suspended state migration. Global calls move to current code while old
frames may remain. A third load or destructive purge can kill old-code processes;
soft purge is the discriminating refusal mechanism. This is useful evidence for
versioned executable programs and explicit state schemas. It is not native VM rebuild
continuity, and asynchronous cast returning ok is not recipient completion.

Guile Fibers' first-class choice/wrap operations compose IO, messages, timers and
continuations with less callback scaffolding. Its run lifecycle makes initial-thunk
completion and draining background work distinct. A concrete source concern matters:
[cancel-other-operations](../../../quarantine/guile-fibers/fibers/operations.scm#L140)
uses a named loop at index zero without advancing. The heap-growth test places its
losing timer first and does not cover later losing alternatives. A follow-up oracle
should observe every cancellation callback for every possible winner position. This
is a source observation, not a reproduced runtime failure. Continuations remain inside
the Guile process; no durable transfer across a rebuilt runtime is supplied.

IPyKernel provides a clear independent computation service with persistent namespaces,
addressed execution replies, rich errors, optional threaded subshells and a separate
control channel. History retrieval is shell code/output history, not all original
provider/program IO. Signal interruption varies by platform/thread and foreign code;
a restart flag is not namespace serialization. For Arconaut, the central question is
how to pause only its client work while other consumers continue, and how to reacquire
handles after a harness refit.

Brush supplies native shell embedding, live functions, descriptor IO, custom builtins
and process-group jobs. Shell history is mutable command text; it is not audit. Its
background compatibility cases explicitly skip implicit wait-on-exit scenarios, and
wall timing uses SystemTime rather than a monotonic clock. Process handles, shell
state and durable agent operation identities must remain distinct.

s6 supplies independently supervised daemons, readiness events, STOP/CONT and FD custody.
It is well aligned with putting work that cannot tolerate refit pauses under OS daemon
management. Its signals do not settle an agent's provider requests or serialize agent
state. A shared service must not be stopped simply because one consumer refits. The
logger only sees its input stream and can change line framing; complete original audit
requires capture at the actual boundaries, not a general logger's name or durability
claim.

SpacetimeDB supplies standing tables, reducers, subscriptions, recovered scheduled rows
and actual MCP schema/SQL/call interfaces with caller identity. Its service boundary
fits consumption by multiple agents. Published module/schema updates have compatibility
and durability controls; manual migration is explicitly unimplemented in this snapshot.
Durable database schedules are not arbitrary live Python/OS heap preservation or agent
context handoff. This is a service design reference, not a selected dependency.

Squirreling is a read/query composition engine rather than standing storage. Its useful
idea is lazy row/cell evaluation: queries can avoid unselected expensive API/model
calls. Abort must distinguish partial output from completed success. Its JS host is
not selected for our core; the transferable idea is a demand-driven structured query
interface whose external work remains observable and attributable.

The common design question is which state is ordinary serializable data, which is a
live process/continuation, and which belongs to an independent service. A refreshed
Arconaut baseline needs these identities and activation boundaries explicitly. None
of these runtime names alone provides the full outpost/refit or original-audit contract.
