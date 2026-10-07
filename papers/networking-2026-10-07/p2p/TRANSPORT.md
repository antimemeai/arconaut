# P2P transport, reachability and independent lifetimes

2026-10-07 frumentarius study. Source inspection and acquisition only: no dependency adopted, reference executable run, network service provisioned, or performance measured. Read current workspace Blackbird, Arconaut AGENTS/FOUNDATION, the G9 security decision and the phux study. The operator asks for n independent Arcos sharing development facts and communication; terminal attachments to one agent are useful adjacent tooling, not that semantic system.

## What the evidence changes

**Separate five decisions:** endpoint identity/authorization; reachability and discovery; encrypted transport; durable application admission; shared-development semantics. A choice in one does not settle the others. Starting with authenticated direct TCP/TLS over known reachable hosts remains a defensible discussion candidate. It does not oblige us to build NAT traversal, group security or shared-file conflict resolution at once. Nor should that convenience silently become the eventual multiplayer architecture.

**Important correction to broad phux assumptions:** its current relay terminates QUIC/TLS on both legs. Not parsing application frames does not make a relay blind to plaintext. Its relay is valuable reference tooling, but cannot supply Arco's E2EE requirement unchanged. Direct phux and shared terminal access have a different endpoint/trust boundary from independent Arco peer communication.

**Bytes remain precious:** bound queued plaintext/ciphertext, retained unacknowledged data, partial frames, streams, connections, discovery attempts and retry timers together. A bounded socket buffer with an unbounded application queue is not bounded. Transport delivery is not durable acceptance or model consumption.

## Reachability: ICE is a separate operational subsystem

[ICE RFC 8445](https://www.rfc-editor.org/rfc/rfc8445.html) specifies candidate gathering, pair checking, role conflict handling and nomination. STUN discovers/checks address mappings; it does not guarantee a direct path or authenticate an Arco room member. [STUN RFC 8489](https://www.rfc-editor.org/rfc/rfc8489.html) and [TURN RFC 8656](https://www.rfc-editor.org/rfc/rfc8656.html) give distinct discovery and relay machinery. TURN allocations, permissions and channels have lifetimes and refresh work. TURN credentials grant relay use, not participant identity or authority to run a tool.

Discovery advertisements should carry location hints bound to a known peer identity. A malicious rendezvous may substitute addresses, reveal metadata or make a peer unreachable; it must not be able to substitute the peer key. Private/local addresses in candidate lists expose topology. Deciding whether to announce them is product policy, not an accidental logging default. Peer-controlled addresses also create an outbound-probing boundary: restrict allowed address classes and attempt budgets rather than giving arbitrary remote input unlimited scanning ability.

[Consent freshness RFC 7675](https://www.rfc-editor.org/rfc/rfc7675.html), sections 5.1–5.2, requires stopping application sends when consent expires (30 seconds) and fresh credentials to regain consent after loss. Keepalive, path viability and authorization are different observations. An Arco refit should stop its own activity without killing the other participant or a shared relay. If quiescence drops connectivity, reconnect creates a new transport incarnation; durable message identity must survive it.

[Tailscale's engineering account](https://tailscale.com/blog/how-nat-traversal-works) supplies an especially useful integration warning: hole punching and the main protocol normally need coordinated access to the same UDP socket. A QUIC library owning an opaque socket cannot automatically be coupled to a separate ICE library. Existing Tailscale can supply the first deployment's reachability externally; this avoids adopting its implementation language into Arco. It does not establish Arco room authorization or portable zero-setup reachability.

## Transport alternatives to discuss

| Approach | Concrete benefit | Machinery and costs that remain | Fit for the first useful slice |
|---|---|---|---|
| Direct TCP + mature TLS implementation | Familiar stream framing; native Mac/Linux integration; accessible hosts/Tailscale already available | Pinned mutual identity, enrollment, application bounds/admission, reconnect; one stream can couple bulk transfer to chat latency | Strong small candidate when only reliable chat/evidence is needed; not selected by this study |
| Native QUIC: ngtcp2 + TLS backend | Independently ordered streams, migration, explicit application socket/timer ownership | UDP reachability; TLS integration; pacing, expiry scheduling, stream-buffer lifetime and application scheduling | Worth investigating once concurrent artifacts/live control make stream isolation valuable |
| Native QUIC: MsQuic | Async general-purpose C API and C++ wrapper; integrated platform IO | Callback/lifetime discipline, queue bounds, TLS packaging, actual Mac support qualification | Serious reference/candidate, with platform caveat below |
| WebRTC data channels: libdatachannel | Native C++ + browser interoperability; ICE/DTLS/SCTP integration; reliable or partial-reliable channels | Authenticated signaling/fingerprint identity, TURN operations, dependency graph, channel buffering, reconnection policy | Attractive only if browser peers/ICE integration justify its broader stack |
| Optional external connectivity/tooling | Keep installs lean; operator supplies VPN/SSH/phux infrastructure | Precisely identify the encryption endpoints and external failure/status boundary | Useful now; never substitute terminal sharing for independent-peer semantics |

[QUIC RFC 9000](https://www.rfc-editor.org/rfc/rfc9000.html), sections 2/4/9, supplies independent ordered byte streams, per-stream and connection flow control, and validated migration. It supplies no application message framing, NAT rendezvous, durable custody or room membership. Streams avoid transport ordering dependency across streams, while congestion and connection credit remain shared. Reserve a small control budget and cap bulk windows; stream count alone does not guarantee responsive control. Migration preserves a live connection; restart/resume must re-establish one. Do not assign durable message identities from QUIC stream IDs.

[QUIC TLS RFC 9001](https://www.rfc-editor.org/rfc/rfc9001.html), section 9.2, makes early-data replay a live concern. Keep execution-triggering messages out of 0-RTT. Ordinary transport retransmission of bytes inside one connection is not a second application action; reconnect submission of a message requires stable deduplication/custody semantics.

[QUIC DATAGRAM RFC 9221](https://www.rfc-editor.org/rfc/rfc9221.html), section 5, gives no transport retransmission or explicit flow-control signaling, while still using congestion control. Ephemeral cursor/presence samples may eventually fit; build requests, reviews, chat retained as work evidence and effect acknowledgments belong on reliable application paths. Old presence can be discarded; old effect uncertainty cannot.

## Native reference inspection

### ngtcp2: explicit control with real integration work

Pinned `b9fc4d55ce6035a4cc76dc456ced6371e98f9214`, committed 2026-10-04. C11 transport core; current repository examples are C++23, so do not pretend they are drop-in C++20 production code. The core itself has no external dependencies; crypto helpers/examples require a TLS backend. Current README labels OpenSSL >=3.5 integration experimental. MIT license. This is active reference source, not selected software.

Read [programmer guide](https://github.com/ngtcp2/ngtcp2/blob/b9fc4d55ce6035a4cc76dc456ced6371e98f9214/doc/source/programmers-guide.rst), Read/write packets, Stream data ownership, Timers, Connection migration. Application supplies packets and timestamps, schedules expiry and pacing, and retains sent stream bytes until `acked_stream_data_offset`. That aligns with immutable payload slices and deterministic transport simulation, but puts substantial lifecycle work in Arco. Transport ACK only releases library buffer use; it is not durable application admission.

Read `fuzz/read_write_pkt.cc`, `fuzz/read_write_handshake_pkt.cc`, `fuzz/decode_frame.cc` and seed corpus. Callback failures are fuzz inputs, an instructive fault class beyond malformed bytes. These are crash/parser/lifecycle mechanisms, not an oracle for peer custody or useful work. No upstream tests executed.

### MsQuic: async convenience, Mac distinction

Pinned `8249f718b9f0018a9e151367fbea11b5e86adb8c`, committed 2026-10-05. General-purpose C QUIC implementation with C++ wrappers; MIT. Its pinned [Platforms.md](https://github.com/microsoft/msquic/blob/8249f718b9f0018a9e151367fbea11b5e86adb8c/docs/Platforms.md) officially lists Windows/Linux; Other configurations are experimental. Darwin builds/framework workflows exist in the same snapshot. Building on Mac is not the same as an upstream support commitment. The document's OpenSSL discussion also contains older version/fork wording: resolve exact supported TLS configuration from build code/release policy before any adoption; do not repeat it as current universal QUIC truth.

[StreamSend contract](https://github.com/microsoft/msquic/blob/8249f718b9f0018a9e151367fbea11b5e86adb8c/docs/api/StreamSend.md): a successful nonblocking call queues data; its buffer/descriptor lifetime lasts until SEND_COMPLETE. That event releases ownership obligations, not model inclusion. [StreamReceiveComplete](https://github.com/microsoft/msquic/blob/8249f718b9f0018a9e151367fbea11b5e86adb8c/docs/api/StreamReceiveComplete.md) distinguishes normal and multi-receive completion modes and warns duplicate completion can race with a new receive. Arco would need explicit retained-slice ownership and cancellation fencing, not ad hoc callbacks into Lua.

Read `src/test/lib/HandshakeTest.cpp`: random loss, resume rejection, address replacement/drop helpers. Reuse the fault classes, not the test suite as evidence for our product. No throughput/latency/RSS claim made here.

### libjuice: small connectivity API is not secure transport

Pinned `cb523763255f241442da1fbdd72c498f33a54b35`, committed 2026-10-06; C, MPL-2.0. [README](https://github.com/paullouisageneau/libjuice/blob/cb523763255f241442da1fbdd72c498f33a54b35/README.md) states one UDP component, default-route/all-local-address policy, STUN/TURN and consent freshness; server can be compiled out. It also lists TCP candidates while stating UDP-only limitations. Treat this as an unresolved documentation/feature-surface distinction, not a promise of TCP/TLS relay fallback. Verify the specific required path before selection.

Read `src/agent.c` consent expiry, selected-pair deadlines and compile-time `JUICE_DISABLE_CONSENT_FRESHNESS` branches. Builds can remove a safety-relevant protocol behavior, so qualification must bind build options. `test/turn.c` uses public demo services; do not execute copied tests with those services as a production readiness oracle. No application encryption, authenticated room identity or durable message protocol is supplied by an ICE agent alone.

### libdatachannel: useful integration, queues remain our responsibility

Pinned `773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12`, committed 2026-09-26; C++17/C API, MPL-2.0. Read `BUILDING.md`, `DOC.md`, `include/rtc/channel.hpp`, `src/impl/sctptransport.cpp`, `src/impl/dtlstransport.cpp`, `test/turn_connectivity.cpp`. Data-channel-only builds can disable media/WebSockets; TLS, ICE and SCTP integration still remain. Archive submodule placeholders do not constitute a complete buildable dependency closure.

`Channel::send` returning false may mean buffered, not rejected. SCTP send code pushes messages into `mSendQueue` and updates buffered amount; the caller must impose queue policy. Naively retrying a false return duplicates a queued message. TURN test forces relayed candidate selection and asserts channel state; its early hello callbacks mostly print content, so stronger exact-payload/admission oracles are needed. Later negotiated-message checks are useful but do not supply durable receipt semantics.

DTLS verifies the presented certificate fingerprint through the configured verifier. [WebRTC security RFC 8827](https://www.rfc-editor.org/rfc/rfc8827.html), sections 4/6.5, explains the identity/signaling boundary: a fingerprint received only through a signaling service trusts that service for peer identity. An Arco identity binding must remain verifiable even if rendezvous substitutes descriptions. DTLS ending at actual peers can traverse an opaque TURN relay; a relay terminating DTLS is a different trust design.

## phux: compare implemented mechanisms, not historical headings

Existing immutable phux source is `0fd4e511621f50c70c857ef0300b90fcd34a295f`; acquired by prior study, not duplicated here. Read ADRs 0038, 0115, 0120 and the file `docs/adr/0116-workload-auth-is-mtls.md` (its internal title says 0114), plus `crates/phux-relay/src/runtime.rs`, `crates/phux-server/src/transport/quic.rs`, TLS verifier code. Historical 0038 bearer/pin policy is not the whole current workload identity design.

[Current relay runtime](https://github.com/no-phux/phux/blob/0fd4e511621f50c70c857ef0300b90fcd34a295f/crates/phux-relay/src/runtime.rs) accepts a QUIC consumer connection and opens another QUIC tunnel stream; `bridge_consumer` splices decoded stream bytes. The current workload-mTLS ADR explicitly describes per-hop TLS and route authority. Thus relay can observe plaintext even though it does not interpret phux frames. Current runtime also refuses extra streams on relay routes, contrary to ADR 0115's proposed arbitrary stream forwarding. Source implementation wins for actual capability claims.

Direct QUIC stream-per-terminal, bounded handshake/first-stream admission, source-share limits and stream credit accounting are valuable patterns. Server QUIC transport optionally verifies workload credentials against a registry and attaches channel-derived identity. SSH bootstrap delivers the pin/token through SSH, opens a bounded-linger listener and falls back to terminal-over-SSH if UDP fails. It deliberately does not implement NAT traversal. Bootstrap after listener loss is a separate reconnect task. Arco can support this external tool without embedding Rust or adopting its relay security boundary.

## Concrete design consequences for n peers

- Keep participant identity, session incarnation, room/work membership, durable message ID and transport connection ID distinct. Pairwise connections need no global leader; two simultaneous dials need a deterministic tie-break without dropping admitted work.
- A full mesh costs n(n-1)/2 connections. Bound per-peer and total bytes/attempts; do not hardcode n=2. Relay-assisted P2P remains viable when ciphertext still ends at peers; relay operator owns service availability, not Arco contexts.
- Separate small reliable chat/control from bulk artifacts; bulk manifests can name immutable content and exact length/digest. A sender's file name must not authorize recipient filesystem traversal or overwrite. Build facts require source/toolchain identity and scoped result semantics, addressed by the shared-development research lane.
- Refitting one participant quiesces its local requests/programs and peer client activity; shared service lifetimes remain independent. Reopen starts a new connection generation while preserving durable custody and explicit pause.
- Presence can be a replaceable projection with visible age. Chat/evidence admission must survive process loss before acknowledgment; model consumption is recorded separately. A disconnected peer is unavailable, not proven dead, and cannot transfer tool authority merely by naming itself.
- Reject a contradictory payload under the same authenticated message ID. A delayed application acknowledgment may be retried/queryable; an uncertain effect cannot be re-executed automatically because a transport timed out. This is compatible with transport-level retransmission.
- Live rooms and offline model mail differ: offline mail adds persistent ciphertext retention, expiry/revocation consequences and asynchronous key lifecycle. Keep the first discussion's live-peer slice honest.

## Direct fault classes and test oracles to put into the design

| Fault/injection | Oracle for the real claim |
|---|---|
| Wrong peer pin/key, substituted rendezvous description, missing mutual identity | Zero application admission; explicit unauthenticated failure; no plaintext fallback |
| Relay operator logs traffic or becomes malicious | Endpoint ciphertext only; altered/replayed envelope rejected; plaintext marker test is supplementary to peer-authentication tests |
| Crash after recipient durable write, before ACK | On reopen, exactly one admitted original; sender pending/unknown resolved by stable ID; no second workflow |
| Crash before durable write, after transport ACK | No application accepted status invented from the transport ACK |
| Slow artifact consumer plus chat/control | Observed finite buffer high-water and control progress within designed budget; bound connection credit as well as stream buffers |
| App callback fails, queue full, short frame/header truncation, oversized declared length | Exact rejection/backpressure outcome; no uncontrolled allocation; no lost custody |
| Duplicate dials, partition, reorder, loss, NAT rebinding, relay failover | Same identities/admitted messages retained; no stale-generation callback activates a replacement session |
| Consent loss, TURN expiry, UDP blocked | Sends stop/reachability visibly changes; reconnect budget finite; alternate path authenticates the same identity |
| A refits while B is working | B's execution/provider request continues; A does not terminate shared relay or kernel |
| Presence flood from authenticated peer | Replaceable state bounded; effects and durable evidence neither starved nor silently evicted |

Use an explicit clock and scripted packet/callback schedule for deterministic transport/admission simulation, plus actual two-host checks for platform/real-network failure paths. Fuzz framing with spec-derived limits and callback-failure injection. Do not call upstream crash fuzzing a custody oracle, or spend a third layer certifying the recheck. Proposed tests above are not executed evidence.

## What to decide next

Before multiplayer launch: identity/enrollment and peer authority; live-room versus offline-mail scope; exact durable acceptance/uncertainty contract; first reachable-host deployment; bounded control/artifact channel policy; chosen maintained crypto implementation and its Mac/Linux build. Discuss adoption explicitly. Start with two useful independent Arcos exchanging scoped source-review/build evidence, then one forced reconnect/refit. NAT traversal and browser interoperability should be justified by actual deployment need, not included to make the stack look complete.

Research limits: no NAT behavior, interoperability, performance, certificate configuration or runtime test executed. Four source archives acquired and inspected selectively; no assertion that their full implementations are defect-free. ICE RFC PDF acquisition initially returned 404 at two URLs; text was acquired and read. PDFs for RFC 9000/8831 acquired. There is no operator acquisition blocker for this study.
