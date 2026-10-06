# Hosted workspace agents: OpenHands and Open SWE

Study completed 2026-10-01 against the revisions in the
[registry](../registry.json). These snapshots have very different implementation
boundaries. OpenHands is now a control center around external engines. Open SWE
contains substantial application middleware and execution machinery, while its
base agent loop comes from dependencies. No acquired program or test was run.
The dossiers record exact source ranges, exposed actions and inspected test oracles.

| Reference | Inspected implementation | Concrete agency and collaboration | Record and lifetime boundary |
| --- | --- | --- | --- |
| [OpenHands](../dossiers/openhands.md) | TypeScript/JavaScript Agent Canvas, launcher and client adapters; Python agent server, SDK and automation engine are separately resolved packages | Actual conversation creation, parent/child task launch, operator steering, profile changes, fork and condensation requests | Browser claims and client state; external execution/history algorithms unavailable in this snapshot |
| [Open SWE](../dossiers/open-swe.md) | Python service, middleware, sandbox adapters, transcript engine and background runner; Deep Agents/LangGraph supply the base loop | Actual model tools to inspect, send to, cancel and resolve other threads; per-role models, editable skills and owner settings | Transactional transcript records, bounded observers/output, durable steering queue, application-owned sandbox lifecycle and continuing commands |

## OpenHands: a real client surface with an external engine

The current source is not the old Python execution engine. Its launcher chooses
an external agent server from a local SDK checkout, matching Git reference or
versioned package; editable local packages are explicitly reinstalled. This is
a useful development arrangement, but a new launcher invocation does not prove
live harness refit, identity handoff or settlement of old programs. Claims about
provider execution, persistent kernels, original engine audit and condensation
algorithms remain outside the inspected source boundary.

The child-conversation bridge is actually wired to incoming WebSocket tool events.
It can create a worktree under the parent workspace and launch a child task. If
isolated creation fails, it retries with the shared parent workspace and discloses
that fallback. This is a material authority/isolation change, rather than proof
that every child has an independent filesystem. A local launch can return the
start-task ID before a conversation exists. Cloud polling likewise can reach its
deadline with a still-pending result or no conversation URL. A successful launch
result must therefore be interpreted as an accepted operation, not finished work.

Duplicate suppression uses a browser-local claim written before launch. That
prevents a repeat in the inspected successful single-client path, as the mocked
test requires. Corrupt or unavailable storage deliberately allows execution to
proceed; the read/write claim is not atomic across tabs. An active goal also
suppresses the follow-up chat result because sending it would cancel that goal.
The child can have launched while the parent receives only a toast. These source
limits distinguish operation receipt, model-visible result and durable ownership.

Ordinary send requests resolve the actual runtime endpoint/key and call the
external client with execution enabled. Cancellation, compaction, automation and
model/profile changes similarly have concrete client surfaces. They do not expose
the server's activation boundary or guarantee that a cancellation has stopped
descendant effects. Forking at a selected event cursor is implemented for local
conversations; cloud rejects this path, and older server contracts can copy the
whole history. The client copies its metadata without establishing complete
original-record lineage.

The process helper makes a useful distinction: a process with `killed=true` can
still be running until it has an exit code or signal code. Its test attacks that
exact case. The signal helper itself sends a signal without waiting, however.
Neither a client cancellation response nor a successful signal is an Arconaut
refit barrier. The recorded tests predominantly mock the external engine; they
establish dispatch, fallback disclosure, deduplication and request arguments,
not the external implementation's behavior.

## Open SWE: substantial orchestration above a delegated loop

The service wires a Deep Agents graph with main/child models, backend, tools,
skills and middleware. Child graphs omit the root-only queue and routing hooks.
This is considerably more than a UI, but the dependent base loop and its precise
semantics should not be presented as source owned by this snapshot.

The model-facing `manage_thread` path checks actor access and exposes real
send/cancel/resolve behavior. Sending to a busy thread uses the service's queue;
a start request can select an admitted model/effort. Thread inspection joins
state, run, plan, approvals and pending input. This supports live colleagues
with separate thread state, rather than merely alternating provider names in
one chat. Owner/private-thread settings and skills can also be changed through
actual model tools. Those are normal-operation agency; they are not a governed
harness autoresearch protocol.

Steering messages acquire user identity and queue IDs before injection. Middleware
builds an input snapshot and then removes the selected messages while retaining
new arrivals observed on reread. The inspected fake-store test introduces a
message during fetch and requires the first to be delivered and second retained.
It is a useful particular concurrency witness. Consumption and context update
are separate operations, however: a crash between removal and graph publication
can lose the intended delivery. The source does not establish a transaction over
every simultaneous writer or an acknowledgement by the recipient model.

The application owns sandbox creation, provisioning and refresh. It publishes
the ready binding only after successful provisioning, and the inspected failure
test requires an unready backend after a provisioning error. Existing unreachable
sandboxes are preserved rather than universally discarded. These are useful
custody rules, but application ownership of this fabric is different from
Arconaut consuming shared services administered elsewhere.

The background runner is actual source machinery. It starts a separate process
group, records task/status paths, bounds output, uses a monotonic deadline and
escalates termination before waiting for the root. This is stronger than setting
a cancelled flag. It still does not prove arbitrary descendants have stopped.
Task tracking and monitoring are attached after launch, so a tracking failure
can return failure while work is already live. Cancellation handlers request
interruption and annotate the transcript; they can dispatch queued follow-up
work. They provide no global barrier covering all provider and program effects.

## Transactional transcript is not complete original audit

Open SWE's transcript engine locks a thread with a PostgreSQL advisory lock and
writes events, attachments, projection state and version receipts in one
transaction. Deduplication and version tests check literal stored record counts
and versions. This is a valuable transaction boundary for the data it receives.
It does not extend backward to every original provider byte or outward to an
external program's effects.

The observer deliberately excludes hidden streams, bounds captured deltas and
errors, and assigns normalized identities. Tool output is capped both for model
context and retained storage. Its asynchronous writer logs append failures and
marks queue entries done without retry; final draining has a timeout followed by
writer cancellation. These are material limits on completeness even when the
database transaction itself is correct. The capped-output tests assert the
stored literal length, providing a direct witness for the limit rather than
evidence that the original output is retained elsewhere.

Manual compaction and compaction lifecycle events exist, but the summarization
algorithm is inherited. Authorized retrieval of retained tool output supports
some repair; output beyond the storage cap cannot be recovered through that
surface. The incident-report flow finalizes domain evidence before storing a
report and digest. It is a business workflow, not the requested universal model
complaint capturing harness state and linking a bead to an external table.

## Questions transferred to Arconaut

Use the distinctions these sources make visible: accepted request versus running
work; queued input versus model acknowledgement; original input versus bounded
projection; transaction receipt versus complete audit; and requested stop versus
settled effects. Define a useful operation handle independently of a chat message.

The shared-service boundary must remain explicit. Arconaut may consume the
workspaces, kernels, databases and queues involved without becoming their fabric
governor. Its refit should settle only its own active programs/provider work and
pause its own standing work, preserving shared services used by other clients.
Neither inspected application supplies that complete contract. These comparisons
inform the coming specification; they select no framework or dependency.
