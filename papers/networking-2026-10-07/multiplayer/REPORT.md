# Multiplayer state and game-networking frumentarius

2026-10-07. Research and design input, not an adopted specification or dependency.
No reference was compiled, executed, or connected to operator accounts. No product
implementation or campaign restart was performed. Root owns synthesis, journal,
manifest integration and Git publication. Sources below were acquired and read,
including the named implementation paths rather than only project descriptions.

## Result to carry into design

Use independently authoritative Arco actors, authenticated addressed communication,
selected immutable artifact exchange and explicit coordination over contested
resources. Do not replicate the entire harness into a deterministic multiplayer
simulation. Treat live views as disposable projections and accepted messages as
retained facts. These are proposals inferred from the sources and current operator
brief, not claims that existing Arco already supports them.

A minimal useful n=2 scenario: two independent Arcos with independent provider
credentials, context and audit; their humans/models exchange room messages, one
shares a versioned problem/file proposal, the other builds it in its own environment
and sends evidence. Each chooses when to include received material under local
workflow policy. No shared execution owner or automatic context merge is necessary.

## Grounding in current Arco

Read `docs/PARTICIPANTS_AND_MODES.md`, FOUNDATION's multiplayer/outpost sections,
and `papers/2026-10-07-peer-security-decision.md`. Operator requirements are P2P,
E2EE, IRC-style live interaction, optional deeper file/problem/build sharing and
model mail between live sessions. n=2 is an example rather than a ceiling. Spawn
lineage does not define communication topology. Kernels, databases and build
services can be shared externally: Arco consumes them and does not govern them.
Refit quiesces one actor's activity, not another actor or a shared service.

The G9 proposal separates acceptance, context inclusion and action correctly. Its
"unknown is not permission to replay" rule must distinguish message custody from
external effects: retransmitting the *same* persistently identified, byte-identical
message is safe once recipient deduplication is specified and qualified; creating
another message/attempt or blindly rerunning the requested tool is not. Reconnect
can ask for custody status before resending. Lack of ACK remains unknown, never
proof of non-delivery. If dedup history cannot cover an old ID, refuse/explicitly
resolve rather than treating it as fresh.

## What game networking teaches, and where it stops

Glenn Fiedler's [deterministic lockstep](https://gafferongames.com/post/deterministic_lockstep/)
requires exactly reproducible simulation and waits for required inputs. Applying
that to LLM requests or shell effects would be a category error: different model
responses and external effects cannot be reconstructed by replaying shared input.
Local reducers over captured observations can be deterministic; providers and
program execution remain outside that reducer. A slow peer must not hold every
other peer's reasoning hostage.

His [snapshot interpolation](https://gafferongames.com/post/snapshot_interpolation/)
separates the originating simulation from its remote visual approximation. Useful
transfer: a fleet HUD can discard superseded presence/progress updates, explicitly
mark staleness and recover from a compact current snapshot. Unsuitable transfer:
interpolating success, custody, tool settlement or file content. A UI must not
smooth away uncertainty or render a guessed job result as an observation.

His [state synchronization](https://gafferongames.com/post/state_synchronization/)
uses sequence numbers, bounded packet work and priorities that accumulate to
avoid starving less urgent objects. Transfer those scheduling questions to chat,
control and artifact transfers. Keep facts exact; lossy approximation belongs
only to expendable presentation. No 60 Hz simulation loop is needed for Arco.

[Valve's Source networking](https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking)
is client/server authority over a game world. Authority remains useful at resource
scope: the process actually running a build owns its observations; a workspace
owner validates mutations. There is no reason to appoint one Arco owner of all
other Arcos' contexts or tools. Prediction/lag compensation may improve a local
input display but cannot make a remote side effect happen or undo it.

## Read reference implementations

Immutable source URLs and archive hashes are in `reference-manifest.json` here.
All paths below are relative to `quarantine/multiplayer-study/`; GitHub links are
pinned to acquired commits, not moving branches. These are study references, not
selected dependencies.

| Reference and read paths | Concrete source finding | Arco consequence |
| --- | --- | --- |
| [GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets/tree/d534c19aa760df3fb75fd20db13ba1932b8a5463): `README_P2P.md`, `include/steam/isteamnetworkingsockets.h:419`, `tests/test_connection.cpp:670`, `README.md` | Independent message lanes have per-lane numbering, cross-lane order is not promised; strict priority and same-priority weights serve different purposes. Real network loopback tests exercise lanes because internal-buffer loopback bypasses the mechanism. Symmetric connect handles simultaneous initiation. P2P still needs signaling, STUN and sometimes relay; naming/authentication/matchmaking are outside transport. Detailed P2P doc now describes native ICE default and optional WebRTC while README's summary still names WebRTC. | Separate control/chat/artifact classes; no artifact head-of-line blocking of interactive control. Test at the actual network seam. Settle simultaneous dialing explicitly. Do not infer a complete room/identity protocol from encrypted transport or a README feature bullet. |
| [ENet](https://github.com/lsalzman/enet/tree/5a9c537fd464b3c6d3c55e1d3bd47588faf71b42): `docs/design.dox`, `protocol.c:196`, `protocol.c:550` | Per-channel sequence state, reliable predecessor blocking, superseded unreliable packet discard, retransmission and bounded windows solve transport issues independently. | Durable messages need reliability; presence can be replaceable. Transport sequence numbers are not durable application IDs across reconnect. ENet is not itself our E2EE/membership solution. |
| [GGPO](https://github.com/pond3r/ggpo/tree/7ddadef8546a7d99ff0b3530c6056bc8ee4b9c0a): `src/lib/ggpo/sync.cpp:144`, `backends/synctest.cpp:100`, `README.md` | Rollback loads saved state and re-invokes advance-frame callbacks; the sync test checks saved-frame checksums after replay. | Valuable oracle pattern for a *pure* reducer: same captured input gives same derived state. Do not replay provider/tool callbacks when reconstructing a participant. The README's Windows build scope is another reason not to treat this reference as turnkey Arco machinery. |
| [Automerge](https://github.com/automerge/automerge/tree/e2452ea1ea9a4008e94f435d60e893bfeb8e0f38): `rust/automerge/src/sync.rs:1,167,386`, `sync/state.rs`, sync tests in `sync.rs` | Sync explicitly assumes reliable in-order stream, tracks per-peer heads and in-flight state, supports reset/resync after peer data loss. Dependency requests are restricted to peer-advertised heads; asking a peer to supply unrelated third-peer orphan dependencies can stall useful synchronization. | Anti-entropy needs connection-specific state and a defined reset. Do not let a missing unrelated artifact/dependency deadlock room traffic. CRDT convergence is not a transaction lock, authorization policy or external-effect guarantee. |

The acquired Automerge archive is 66.8 MB: acquisition is not a suggestion that
Arco carry this machinery. The reader must compare representation, history cost,
compaction, FFI and deployment obligations before any later adoption discussion.
GNS likewise supplies much more transport machinery than basic two-peer chat.

## Literature: exact claims and design use

[Lamport, Time, Clocks, and the Ordering of Events (1978)](https://lamport.azurewebsites.net/pubs/time-clocks.pdf)
was acquired and read for happened-before and logical-clock ordering. A local
sequence identifies source order; explicit causal references identify dependencies.
A logical-time plus identity sort gives a deterministic presentation tie-break,
not evidence of physical order or a global consensus decision. Clock skew cannot
be allowed to choose whose file mutation wins. A room need not promise one total
order unless a concrete operation requires it.

[Viotti and Vukolić, Consistency in Non-Transactional Distributed Storage Systems](https://arxiv.org/abs/1512.00168)
is useful for naming guarantees separately: convergence, read-your-writes,
monotonic observation and causal consistency are different contracts. Application:
local chat displays a pending send immediately, custody is separately reported,
and replies retain their parent reference even if arrival order differs. Neither
TCP ordering nor an eventually equal file set makes the entire session linearizable.
The publisher/author download returned 403; an arXiv PDF was acquired instead.

[Kleppmann and Howard, Byzantine Eventual Consistency](https://arxiv.org/abs/2012.00472)
was read especially for invariant confluence. Its coordination-free construction
applies to merge-safe invariants, not all database operations. Our inference:
append-only conversation/proposal facts can be mergeable; "exactly one executor
owns this mutable directory" is not made merge-safe by picking a CRDT. Independent
partitioned claimants can each appear valid while their combination violates
exclusivity. Give one service/owner authority or require coordination for that
resource; do not label a gossip claim a safe lease.

[Kleppmann, Making CRDTs Byzantine Fault Tolerant (2022)](https://martin.kleppmann.com/papers/bft-crdt-papoc22.pdf)
was read for authenticated operations, convergence assumptions and connectivity.
It explicitly needs a communication path between correct peers not entirely
controlled by faulty intermediaries for eventual delivery. Thus eventual delivery
is conditional on eventual connectivity, custody retention and permitted membership;
no encryption primitive prevents a relay from withholding ciphertext. Author/source
identity and structurally valid operations do not make submitted content safe or
true. Authenticity is distinct from authorization and model-context policy.

[Ink & Switch, Local-first software](https://www.inkandswitch.com/essay/local-first/)
was read as the primary architectural essay. Its local ownership/offline aims fit
independent Arco state. Its CRDT examples guide shared-document experiments but
do not imply automatic tool coordination. The PDF acquisition was blocked by
HTTP 403; the HTML was read. `SHOPPING_LIST.md` records that limited acquisition
gap rather than claiming possession.

## Proposed state classes and ownership

| Class | Owner and sharing contract | Partition/reconnect behavior |
| --- | --- | --- |
| Actor identity/context/provider credentials/local audit | Actor-local; room refers to identity and selected evidence, never imports credentials or silently splices full contexts. | Local work continues under policy; peers show stale/unknown. Refit retains actor identity while a new runtime/connection incarnation prevents old network state reuse. |
| Live room message/model mail | Origin identity + durable message ID + explicit destination/audience, exact retained bytes and per-recipient custody. | Pending/accepted/included/acted are distinct. Retry exact ID under retained dedup; absence of remote settlement remains unknown. Offline mailbox is separately chosen scope. |
| Presence/typing/progress projection | Origin-authoritative current version; replaceable bounded data, observed-local freshness. | Drop/coalesce old samples. Deadline produces "not recently observed", not proof of death or safe resource takeover. |
| File/problem/source proposal | Immutable blob/hash, base revision, provenance and explicit apply target. | Both sides can propose independently. Base mismatch produces a real merge/conflict decision; never last-arrival-wins source overwrite. |
| Build/tool job | Execution host/service authoritative for its attempt and observations, code revision + environment + command evidence. | Disconnect does not cancel or certify job. Query/reconcile same attempt; new execution requires explicit policy after uncertain outcome. |
| Shared mutable checkout/database/kernel | External service or designated resource owner enforces concurrent operations. Arcos are clients. | No "distributed filesystem" claim from chat/file replication. Enforce fences at the actual mutation target, not solely in peer UIs. |

A command to a peer is a request under that peer's local policy, not an authenticated
remote shell by default. Operator low-friction preferences belong in configured
capabilities, not a per-message approval dialogue. Peer text must remain attributed
data and never impersonate local operator/system instructions.

## Topology alternatives and n=2 to n

1. **Direct authenticated pairs / small mesh.** Strong first experiment: simple
   end-to-end path, no opaque third-party room state. For n peers full mesh has
   n(n−1)/2 connections and sender fanout n−1; local queues and membership must not
   be hardcoded to two. Sender records custody per intended recipient. Simultaneous
   connection attempts converge to one selected channel without double admission.
2. **P2P with untrusted rendezvous and ciphertext relay.** Improves reachability
   while keeping actor independence. Relay custody is not destination custody;
   connectivity, queue quotas and metadata exposure need explicit bounds. No
   assumption that a relay establishes membership or guarantees delivery. Direct
   path change must retain application IDs and authenticated identity continuity.
3. **Sparse overlay / gossip for replicated facts.** Potentially scales room facts
   and artifacts but requires end-to-end attributable origin, bounded dedup,
   anti-entropy, membership epochs and missing-dependency rules. A forwarded message
   cannot inherit the forwarder's identity as its author. It is unnecessary for
   first two-peer chat and substantially expands the hostile-peer surface.
4. **Shared external coordination/workspace service.** Useful for contested jobs
   or real collaborative editing, while Arco remains P2P for communication. It is
   a resource-scoped authority, not the harness's universal controller. Do not
   smuggle this into the first slice merely to claim "shared build".

Recommendation for discussion: first direct-pair messages and selected immutable
artifacts; versioned envelopes, separate traffic classes and destination sets allow
n later. Defer one replicated mutable workspace and arbitrary overlay forwarding
until their semantic needs are established. This recommends sequencing, not a
hardcoded topology ceiling or refusal to support richer workflows.

Group membership is a distinct protocol fact. For n>2 define an epoch, who may
invite/remove, when a membership change takes effect, and which recipients a send
intended. Avoid "delivered to room" as a scalar: show the intended recipient set,
per-recipient status and partial delivery. Security-lane research owns group-key
choices; this report does not select pairwise encryption or MLS.

## Tight message lifecycle sketch

An application ID is stable across reconnect/refit; connection and runtime
incarnations are separate fields. Local origin commits the message and intended
recipients before attempting transmission. Recipient authenticates the transport
and author, verifies room/audience/bounds, then atomically records ID→payload custody
before acknowledging. Same ID/same bytes returns custody; same ID/different bytes
is a conflict. Inclusion records the actual context revision/request, not only a
UI notification. Acting names a separately owned attempt. No checkpoint may make
unknown effects disappear to free queue space.

For direct streams, sender order is enough for most live chat. Include reply-to
and selected artifact dependencies; require dependency resolution before an
operation that needs them. Do not block unrelated conversation behind a missing
artifact. For later forwarding, authenticate origin and causal references through
the final recipient, not only relay connection. Negotiated protocol/capability
versions must fail unsupported operations explicitly, never downgrade to plaintext
or silently omit required semantics.

Finite bytes, message size, in-flight work, pending admissions and retained-dedup
coverage are configuration, observable on both sides. A malicious peer cannot
force indefinite body allocation, infinite dependency waits or unlimited fanout.
Backpressure signals overload without ACKing dropped durable work. Coalescing only
applies to replaceable facts. Control/chat budgets remain serviceable during bulk
transfer; strict priority needs rate bounds or it can starve artifact progress.

## Direct fault-class oracles for the eventual plan

These are proposed checks, not executed qualifications. Implement a small event
model and deterministic scheduler at the real owned application seam; do not
wrap it in a second certification system. Crypto tests belong to the security lane.

| Fault injected | Direct expected observation/oracle |
| --- | --- |
| Drop ACK after recipient's durable record; reconnect and resend identical ID | Exactly one recipient admission and no repeated workflow action; origin changes unknown→accepted from queried/returned custody. Count actual dispatches independently. |
| Crash before/after custody commit and before/after origin marks ACK | Recovery state matches durability boundary; ACK never precedes retained custody. Original message bytes and IDs survive, partial tails handled by existing audit policy. |
| Same ID with different body/author/room; stale session clone | Conflict/refusal, not another accepted message or overwritten original. Dedup scope binds authenticated identity and room; stale incarnation cannot gain current authority. |
| Reordered cross-peer replies/clock skew | Exact source order retained where promised; unresolved parent visible; no invented global timestamp authority or missing-parent success. |
| Sender reboot/sequence wrap/disconnect/path replacement/simultaneous dial | Stable application IDs; new connection incarnation; one logical channel selected; no duplicate room entry or admission. |
| n=3 partition A↔B while C sees both, followed by healing | Intended audience and per-recipient status retained; independent work continues; equivalent accepted sets produce equivalent shared projection. No exclusivity lease granted on timeout. |
| Bulk artifact saturates link; control/chat arrives | Measured bound for control admission under configured load, finite resident/pending bytes, eventual artifact service once connectivity/load permit; no token throughput claim substituted. |
| One peer sends malformed lengths, endless missing dependencies, stale updates | Refuse before large allocation; finite dependency wait; unrelated chat progresses; stale projection cannot supersede current origin version. |
| Two proposals modify same file from same base | Both proposals preserved; applying second checks base/current state, reports conflict or a correctly defined merge; no silent lost update. |
| Build accepted remotely then link dies; recipient refits | Actual execution count and owner attempt show whether work continued/paused; origin stays unknown until reconciled. Never rerun merely because socket closed. |
| Actor compacts received chat or changes workflow while a request is in flight | Original remains retrievable; final request audit identifies actual included version. No peer mutation alters an already-issued local request. |
| Membership removal/revocation races queued messages | Configured epoch policy decides admission consistently; removed peer cannot receive future authorized content. Historical observations are not falsely erased. |

Test pure projection replay against an independently simple reducer over captured
facts; external tools are spies with counted effects and explicit unknown outcomes.
Run actual two-host interoperability over Neuroses when implementation exists,
including link interruption and restart. Offline simulation is not evidence that
provider credentials, NAT traversal or deployed crypto work. Conversely a live
"hello" is not evidence for crash custody or duplicate suppression.

## Operator decisions that actually change design

- First room: direct reachable peers only, or NAT/relay as part of first delivery?
  Direct over existing reachability still uses selected endpoint encryption.
- Does live-session mail retain a sender queue across absence/refit, and for how
  long? Offline store-and-forward service is extra scope, not implied by live mail.
- Is file/build collaboration proposal-and-apply initially sufficient, or must two
  participants edit one mutable working tree live? The latter requires a resource
  authority/conflict contract and substantially different failure handling.
- Should room rendering have a deterministic causal presentation or must every
  participant see one committed total order? Chat usually does not need consensus.
- Who controls n>2 membership, and what happens to queued sends on epoch change?
- Which remote capabilities permit addressed requests to launch work automatically?
  Profile-level policy can preserve low friction while keeping effects attributable.

The first executable spec should answer those, define custody/dedup/reconnect and
encode exact owner/action invariants before transport code. It must not grow into
a certification doom loop: direct defect tasks, one recheck/fix, no third layer.
