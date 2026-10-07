# Multiplayer research: consequences before design

2026-10-07. Frumentarii acquired and studied primary literature and immutable
reference archives across transport, game networking and E2EE. This synthesis
uses their source findings to revise the earlier G9 proposal. It is discussion
input, not a wire specification, dependency selection or implementation claim.

Read [transport](p2p/TRANSPORT.md), [multiplayer state](multiplayer/REPORT.md) and
[E2EE](e2ee/REPORT.md), with their source/provenance/restoration manifests.
Unavailable literature is listed explicitly; no claim it was read.

## The coherent system we need to describe

An Arco is an independent actor with local context, provider credentials, audit
and execution ownership. Multiplayer connects actors and humans; it does not
replicate a whole harness or make one Arco governor of shared services.

| Contract | Corpus consequence | Design obligation |
| --- | --- | --- |
| Identity | TLS endpoint identity is not operator, device, session, model or room membership. | Bind those identities/scopes explicitly; a JSON sender is not authority. |
| Reachability | QUIC does not provide ICE, NAT traversal or room enrollment. | Choose initial deployment topology separately from eventual relay/discovery. |
| E2EE | phux relay terminates TLS per hop; DAVE has downgrade paths; TLS/Noise key updates are not compromise recovery. | Define endpoint threat model and forbid silent plaintext/downgrade. Select maintained crypto only after discussion. |
| Durable messages | Transport ACK, durable recipient custody, context inclusion and external action differ. | Commit immutable IDs/bytes/audience; acknowledge durable admission; retain per-recipient custody and conflicts. |
| Reconnect | Unknown delivery need not stop forever. Exact-ID retransmission can be safe under durable dedup. | Define reconciliation and dedup retention coverage; never translate a retry into a new effect attempt. |
| State | Game prediction/rollback cannot replay inference or external effects. | Deterministic reducers operate on observations; real effects retain ownership and uncertainty. |
| Presence | Game snapshot techniques fit replaceable visual state. | Bound/coalesce progress, show staleness; never interpolate custody or completion. |
| Shared work | CRDT convergence is not execution ownership or authorization. | Exchange immutable artifacts/base revisions; target owner enforces apply/conflicts and real job fences. |
| Groups | MLS does not decide P2P concurrent commits or application membership. | Explicit epochs, invitation/removal authority, partial recipients and commit reconciliation. |
| Audit | Recording traffic/private keys defeats forward secrecy; local plaintext history remains exposed to host compromise. | Exclude secrets from ordinary audit/context/refit; retain public decisions/message facts with honest security claims. |
| Resource bounds | libdatachannel send(false) may already have buffered; per-stream caps do not bound aggregate memory. | Count all pending/buffered/retained bytes, admission/handshake work and dependency waits; preserve control progress. |
| Refits | Runtime/connection incarnation differs from persistent participant/message identity. | Quiesce only the refitting actor; preserve durable custody; reject stale callbacks without stopping shared services. |

## What to carry forward from the references

ngtcp2's explicit packet/clock/ACK-buffer custody suggests a controllable native
transport boundary; it also leaves pacing, sockets and TLS integration to us.
MsQuic reduces that work but its pinned official platform statement excludes Mac
qualification by implication. WebRTC/libdatachannel combines ICE/DTLS/SCTP at a
substantially broader dependency/lifecycle cost. libjuice alone is connectivity,
not encryption. These are options, not recommendations to adopt all of them.

GameNetworkingSockets/ENet traffic classes inform separation of chat/control and
bulk artifacts. GGPO makes the limit of rollback explicit. Automerge sync assumes
a reliable ordered pairwise stream and tracks peer-known heads/dependencies; it
does not provide room trust or a shared execution lock. MLS++/libdave show group
key mechanics and the additional delivery/epoch machinery a genuine room needs.

## Discussion sequence

1. Establish the use case: live rooms, addressed model mail, shared problem/file
   proposals and shared builds; choose the first useful slice without hardcoding n=2.
2. Write the identity/authority/threat and retention model, including enrollment,
   live revocation, key rotation, secret exclusions and malicious authorized peers.
3. Write message custody/inclusion/action, room epoch/conflict and artifact/job
   contracts together. Name reconnect and uncertainty outcomes before wire fields.
4. Choose transport/reachability and maintained cryptographic implementation against
   those contracts, Mac/Linux delivery, byte cost and actual operational needs.
5. Develop the bounded implementation plan and direct tests: hostile identity,
   revocation during live work, crash before/after durable ACK, duplicate/conflict,
   reconnect, n=3 partition, artifact flood and actor refit while another works.

First direct-pair TLS on reachable hosts is a credible experiment, not the complete
architecture for groups/offline mail. No decision or permission question is forced
by today's corpus; the operator has deferred multiplayer while core work settles.
