# G9: encrypted independent peers — decision required

2026-10-07. Active unit `arconaut-7iy.9`, original allowance
07:25:14.674231..08:55:14.674231 UTC. Research candidate only: no peer transport,
cryptographic code, dependency adoption, native build or activation. G9 is BLOCKED
on the operator decision below; this document is useful preparation, not acceptance.

## Source-grounded subplan and actual finding

Purpose: two independently owned Arco sessions exchange addressed encrypted room
messages, then each performs useful work from explicitly included peer evidence.
Independent contexts, audits, provider credentials and execution ownership remain
local. Chat is the first slice; shared builds/files are not prerequisites.

Read `PARTICIPANTS_AND_MODES` lines116–156, integration SYNTHESIS lines79–142,
the teams source report lines1–105, `STATION`, `src/station.cpp` lines1–145,
`src/openai.cpp` transport references and CMake dependency declarations. Existing
requirements explicitly leave cryptography and key lifecycle unselected. CMake
links owned libraries, Threads and Lua, not OpenSSL/libsodium. Provider HTTPS
uses an external curl child; that does not select or expose a native TLS server.
Station provides durable-before-dispatch source/id admission and unknown/pause,
not authenticated remote provenance, encrypted reception or receive acknowledgments.
A peer-supplied `source` must never become authentication merely by reaching station.

Implementation allowance: only after a decision, remaining original unit time,
with at most 25 minutes hardening inside the 90-minute whole-unit bound. Today's
blocked research delivery uses at most20 minutes. Documentation hardening declared
07:29 UTC with deadline07:34 UTC: layer1 remediate source/contract/oracle gaps;
layer2 one recheck/fix. No third assurance layer or runtime suite reruns.
Direct failing oracle today: no selected E2EE implementation or native authenticated
peer path exists. Future executable oracles below are NOT executed tests.
Actual next action: ask for the specific security/dependency decision, retaining
this candidate and the original deadline. Do not fill the gap with plaintext chat.

## Recommendation for operator decision (not selected)

Approve discussing/adopting **OpenSSL libssl/libcrypto for an opt-in TLS1.3 direct
peer transport**, with mutual authentication and out-of-band pinned certificate
identities. TLS ends inside the two Arco processes, not at a plaintext hub. The
initiator/listener role describes one connection, not an orchestration hierarchy.
No custom primitives, custom handshake, shell TLS pipeline or production Python.
Exact maintained OpenSSL release, supported host packages/build provenance and
license review must be recorded before adoption; reading3.5.0 documentation below
is not selecting that version for production. No installation performed.

Benefit: established handshake, ephemeral key agreement and authenticated records;
we own framing, custody, addressing and admission rather than invent cryptography.
Cost: new C library/build distribution and security-update obligation on Mac/Linux;
certificate/private-key provisioning, nonblocking native I/O, handshake/record
errors and shutdown ownership. A small feature still requires these costs.
Tradeoff: larger API/dependency surface than libsodium, but much less owned
security-protocol construction. Peer certificates are not ordinary website names;
no reliance on public web CAs to establish room membership.

Alternative: libsodium `crypto_kx` plus authenticated encryption has a smaller C API
and explicit directional keys. Its documentation requires the peer public key to
be known. It is not a complete authenticated ephemeral handshake, framing,
anti-replay store or membership protocol. Long-lived static KX keys alone do not
supply forward secrecy against later key compromise. Ephemeral authentication
would require additional protocol design; do not silently construct it here.
Owned primitives are not recommended. Noise would still require a concrete
implementation/dependency decision and additional qualification, not a shortcut.

**Requested decision:** permit the OpenSSL dependency approach and explicit
out-of-band peer enrollment for the direct two-peer first slice, or choose an
alternative. Also confirm the limited threat/metadata model below. Until then
G9 stays blocked; no implementation/adoption by implication.

## Proposed security and lifecycle boundary

Threats: active network eavesdropper/modifier, impostor endpoint, replay/duplicate
application messages, untrusted peer text and disconnect after acceptance.
Not protected: compromised endpoint or its private key, local plaintext audits,
malicious authorized peer recommendations, denial of service, traffic analysis.
Local provider requests necessarily expose selected peer content to the selected
model provider; E2EE protects peer transport, not inference performed at endpoints.
Network address, length and timing remain observable. No anonymity claim.

Enrollment: exchange and verify full certificate fingerprints through an existing
trusted channel; configure an explicit allowlist mapping certificate identity to
peer identity and room/work membership. No automatic trust-on-first-use. Rotation
and revocation require explicit allowlist change; unknown/changed keys fail closed.
A private key stays native-owned in a restricted local file, never Lua/model/tool
output. Optional integration must remain off without configuration. Certificate
validity, proof of key possession, protocol constraints and exact pin match must
all succeed before application data is admitted; do not write a verify callback
that unconditionally overrides certificate errors. The precise pinned-certificate
trust-store policy still needs implementation design after operator selection.

TLS1.3 only, mutual authentication on the initial handshake, no 0-RTT, no session
resumption for the first slice. Server requires both peer verification and failure
when no peer certificate is supplied. Both roles enforce the configured remote
identity. Disable plaintext fallback, downgrade and unauthenticated retry.
Neither endpoint may send or include an application message before authentication.

Start with direct configured TCP endpoints on accessible hosts (existing Tailscale
can provide reachability, not substitute for E2EE). No rendezvous service, NAT
traversal, relay, offline delivery or group-key scheme in this slice. A future
byte-forwarding relay must not terminate endpoint encryption. Network reachability
and packet-capture permissions are to be observed, not assumed.

Native lifetime owns socket/handshake/deadlines/cancellation and session custody.
Refit pauses this Arco's client activity only; it must not stop the other peer.
Explicit pause stays paused. Reopen must not silently inherit predecessor
admission/attempt authority or retransmit uncertain sends. Socket closure is not
proof that remote work succeeded or failed.

## Envelope/admission consequences (proposed, not wire specification)

Bounded framed UTF-8 JSON: protocol version, room, work_id, message_id, sender
incarnation, destination participant and text. Authenticated channel identity is
attached by native reception; an envelope's sender label cannot override it.
Address/membership mismatch rejects before model inclusion. Work_id is shared
coordination metadata, not a lease, success result or capability to execute tools.
Proposed bounds: frame 64KiB, text 32KiB, finite admission table and pending bytes;
reject malformed/oversized frames before uncontrolled allocation. Exact persistence
representation should reuse immutable retained bytes rather than clone history.

Separate observations: locally queued; native send attempt; recipient durably
accepted; included in recipient context/request; workflow returned; artifact
observed. An authenticated acceptance acknowledgment means only durable reception,
not model consumption or tool completion. A disconnect before acknowledgment leaves
send outcome unknown, not permission to replay. Same identity/room/message_id with
same payload is a duplicate, not another admission; changed payload is a conflict.
Admission-table exhaustion blocks visibly; no eviction of unknown effects to make
room. Peer input cannot directly invoke station control or masquerade as operator
steer. Inclusion is attributed user/data content under local workflow policy, not
a new system/developer instruction. Station's existing source/id suppression is
useful but does not cover these authenticated envelope distinctions by itself.

## Direct implementation oracles and useful delivery

1. Wrong pin, missing client cert, expired/revoked identity, changed key, plaintext
   endpoint and downgrade: zero accepted/included application messages on either
   side. Mutate encrypted traffic: authentication failure, never plaintext fallback.
2. Capture transport while a unique message marker passes between two actual Arcos:
   marker absent from network payload, present in recipient retained receive bytes.
   Capture alone is not an authentication or general security proof; case1 targets
   active interception. Capture contents remain ignored/private.
3. Matching messages duplicated and conflicting payload under one ID, including
   reopen: one durable admission, conflict explicitly retained, no second workflow.
   Cut connection after receive but before acknowledgment: sender unknown remains
   unknown; original paths and attempt IDs survive, no automatic resend.
4. A and B have different actor/session/context identities. A sends a source/work
   question; B explicitly includes it, produces an independently scoped review,
   and sends addressed evidence; A includes the reply and writes a useful work
   decision with source/check identities. Retained requests demonstrate inclusion;
   resulting file bytes demonstrate delivery, not a vague "peers started" status.
5. Pause B while A runs: B's queued text does not start work; A can continue
   independently. One controlled reconnect/refit preserves B's local identity and
   unknown sends without terminating A or claiming crash containment.
6. Message floods/oversize/truncation and repeated failure reach finite byte/count/
   time bounds, with exact unavailable/blocked outcomes. No retry-loop budget reset
   from handshake success, model text or a fresh incarnation.

After selection, layer1 implements/fixes these direct fault classes and useful
work path; layer2 one affected recheck/fix within the remaining25-minute ceiling.
No every-provider comparison, old campaign rerun or third security-certification
layer. If incomplete at the bound, archive source/check evidence inactive.

## Primary source/restoration and concrete consequences

Study-only bytes acquired with curl2026-10-07; no source code executed/adopted.
Local files are ignored under `quarantine/g9-security/`. Restore via each URL
and verify SHA256; the libsodium master URL is mutable, so mismatch requires
explicit new source identity rather than pretending exact restoration succeeded.

- RFC8446: https://www.rfc-editor.org/rfc/rfc8446.txt
  `rfc8446.txt`, SHA256
  `47871bc8820a2c3b6ea89f061055577058862cf543686b82d10131239702b3bd`.
  Read bounded E.2/E.3/E.5 passages and authentication/0-RTT references.
  Consequences: authenticated record integrity is not an application admission
  receipt; disable 0-RTT/reconnect replay; metadata length/timing exposure is explicit.
- OpenSSL3.5.0 verification modes:
  https://raw.githubusercontent.com/openssl/openssl/openssl-3.5.0/doc/man3/SSL_CTX_set_verify.pod
  `SSL_CTX_set_verify.pod`, SHA256
  `8cdb8cd333fc2719acc5c8b1e57f8b2facb471177949a6553701111983065bff`.
  Read DESCRIPTION/NOTES through FAIL_IF_NO_PEER_CERT. Consequences: client
  SSL_VERIFY_NONE can continue despite verification failure; server PEER alone
  does not require a client certificate. Missing-cert/wrong-pin oracles are mandatory.
- libsodium key exchange:
  https://raw.githubusercontent.com/jedisct1/libsodium-doc/master/key_exchange/README.md
  `libsodium-key-exchange.md`, SHA256
  `1f7cf0a1fb3e4644b459607fbaaef66e2f62bcca6063f523d706edaa1fa5b2eb`.
  Read examples/purpose/usage. Consequence: directional key derivation presupposes
  known peer keys; do not mistake it for authenticated membership or a full protocol.
- Existing teams/source studies, restoration manifest under
  `papers/integrations-2026-10-06/teams/`: Commonly persists before socket broadcast;
  Agent Room durable replay is not context consumption. Consequence: received,
  included and acted-on observations must remain distinct; no imported replay policy.

Actual costs/outcomes: three successful bounded HTTP source acquisitions; source
hashes above; no model/provider/reviewer call, build, runtime security test or account
billing measurement. Research delivery does not resolve the missing decision.
