# E2EE for independent Arconauts: research before selection

2026-10-07. Frumentarius research, not an approved protocol/dependency or a claim
that multiplayer exists. G9 remains a proposal. No acquired code was built or
executed; no credentials, provider calls, networking implementation, or account
enrollment were used. The root colleague owns consolidation and journal entries.

## Recommendation to discuss

Separate three products before selecting cryptography: live two-peer conversation,
live rooms with more members, and asynchronous mail. They can share identities and
application envelopes; they need not share key-management mechanisms. A pinned,
mutually authenticated TLS 1.3 connection is a credible smallest first product.
An encrypted connection to a relay that decrypts messages is not E2EE. Two TLS
connections terminated at a room hub do not meet the operator's requirement.

For the first product, prefer a maintained TLS implementation over constructing
an authenticated handshake from primitives. Discuss OpenSSL 3.5 LTS against Mbed
TLS's maintained LTS branch, including actual package/build and byte costs. This
research does not select either. Do not implement a private cryptographic protocol
merely because our default is to own machinery: the concrete exception worth
discussing is the established authenticated cryptographic channel; Arco owns
identity policy, framing, custody, admission, and collaboration semantics.

If distributed rooms or durable offline mail are near-term requirements, do not
present direct TLS as the completed security architecture. MLS supplies group
keying, but leaves application identity and distributed delivery decisions to us.
It can run over P2P transport; it is not itself a NAT traversal or consensus system.

## What existing G9 gets right, and what remains unspecified

The proposal distinguishes authentic reception from model inclusion and execution,
requires both directions to authenticate, disables early data and plaintext
fallback, and does not equate Tailscale reachability with application E2EE.
Its warnings against static `crypto_kx` as a complete forward-secret protocol are
correct. Its 64 KiB frame cap is a proposed engineering bound, not a measured one.

Concrete gaps requiring design, not more certification:

1. **Revocation during a live connection.** Editing an allowlist rejects future
   handshakes only unless all connections/admissions consult the new policy.
   Define an atomic activation point that closes revoked channels, blocks queued
   inclusion, and records whether previously accepted messages remain eligible.
   For groups, removing a member needs a successfully processed group-key change;
   changing a GUI roster or removing a socket is not cryptographic removal.
2. **Identity hierarchy.** Human/operator identity, device key, Arco session,
   model participant, and transport connection are different things. A model name
   or JSON `sender` field is not an authenticated principal. Decide whether one
   device key can authorize several independent sessions; bind session claims to
   the authenticated connection and enforce separate authorization scopes.
3. **Pin semantics.** Pinning the full certificate deliberately rejects a renewed
   certificate with the same key. Pinning SPKI permits that renewal, but needs
   separate validity and authorized-rotation rules. A self-signed peer enrollment
   needs an explicit trust-store/verification policy, not an unconditional
   certificate-verification callback. Specify precisely what is compared.
4. **Protocol binding.** An authenticated TLS peer might speak a different service.
   Reserve an Arco protocol identifier (e.g. ALPN) and bind protocol/version,
   identity, and intended room in the authenticated application opening. Reject
   disagreement before user text is admitted. Do not use a socket address as identity.
5. **Persistence versus secret deletion.** An immutable audit of all traffic keys,
   ratchet secrets, or raw secret-bearing protocol state would defeat forward
   secrecy. Audit public identity, decisions, counters, outcomes and message bytes;
   exclude private keys/traffic secrets from ordinary journals, complaints, model
   context, crash payloads, refit handoffs, and tracing. Retained plaintext messages
   intentionally remain exposed to later local audit compromise. Say that plainly.
6. **Application ordering and authority.** Deduplication keyed by authenticated
   device/session/room/message identity must survive reconnection. Membership or
   credential revocation can make an old signed/encrypted envelope unauthorized
   now, even if its cryptography verifies. Reception never grants tool authority.
   Sender-unknown after missing ACK is compatible with retrying the same immutable
   ID under a specified deduplication protocol; it is not a mandate to stop forever
   or permission to invent a new ID and execute twice. Specify this reconciliation.
7. **Bounds during authentication.** Handshake input bytes, simultaneous sockets,
   pending admissions, invitation attempts, incomplete frames, timeouts and retry
   budgets need limits before allocating application-sized buffers. An unverified
   sender should not obtain an expensive model call or unbounded complaint stream.

These are design consequences inferred from Arco's retained-state and agency
requirements, not assertions that the current code implements them.

## Protocol options and their actual properties

| Option | What it solves | What Arco must still solve | Fit and cost |
|---|---|---|---|
| TLS 1.3, mutual pinned certificates, fresh ephemeral handshake | Authenticated live ordered channel; forward secrecy for the specified handshake/key deletion | Enrollment and rotation; room authorization; relay topology; durable IDs/ACKs; nonblocking lifetime; local retained plaintext | Strong first two-peer option. Native C dependency, certificates, maintained host distribution. Standard interoperability does not make arbitrary TLS apps Arco-compatible. |
| Noise, fixed vetted interactive pattern | Compact two-party authenticated key establishment/transport with chosen pattern guarantees | Acceptable static keys; protocol binding; framing; nonce/replay handling; persistence; membership; concrete maintained implementation | Good peer-channel vocabulary; avoid 0-RTT/handshake application payloads. Small primitives do not remove integration risk. Noise-C acquired for study is inactive since 2023, not a vibrant production adoption candidate. |
| MLS (RFC 9420) | Asynchronous group key agreement, membership epochs, FS and PCS under stated conditions | Credential authentication; invite authorization; commit ordering/fork recovery; routing; durable state; bounded stale keys; provider disclosure | Credible future rooms foundation. MLS++ is active native C++ but brings crypto/build surface and distributed group-state complexity. |
| PQXDH plus ratcheted pairwise messaging | Asynchronous pairwise setup and per-message evolution | Prekey publication/consumption, identity verification, bounded skipped keys, multid-device routing, state durability/rollback | Relevant for offline mail. libsignal has substantial ecosystem/API/license mismatch for a tiny MIT C++ harness. Study rather than adopt by analogy. |
| libsodium primitives/secretstream | Cryptographic operations and protected stream given an appropriate shared key | Authenticated ephemeral key agreement and whole protocol above it | Active small C building blocks; not a complete selected peer protocol. Static key exchange plus encryption is insufficient. |

TLS traffic key updates do not provide recovery after the current traffic secret
is stolen; they do not introduce new independent entropy. A fresh authenticated
handshake is different. Record anti-replay applies within one connection; durable
application retransmission across connections needs its own rule. Ordinary TLS
also exposes traffic shape. [RFC 8446 §E.2](https://www.rfc-editor.org/rfc/rfc8446.html#appendix-E.2)

Noise XX exchanges static keys, but the application decides whether a remote key
is acceptable. Rekey evolves a cipher key without a new DH exchange. Use a fixed
pattern/ciphersuite, bind negotiation in the prologue, and admit application text
only after handshake completion and identity acceptance. Those are proposed Arco
profile choices, not something the framework automatically supplies.
[Noise specification §§6, 7.7, 11.3, 14](https://noiseprotocol.org/noise.html)

MLS epochs separate membership states. New members do not obtain prior messages
from protocol keying alone. Removal and honest fresh updates can protect future
epochs; an Update proposal alone does not establish PCS before commitment is
processed. Secret deletion is essential, including old/forked states. These
properties do not undo plaintext deliberately retained by an endpoint.
[RFC 9420 §§9.2, 16.6](https://www.rfc-editor.org/rfc/rfc9420.html#section-16.6)

In a P2P MLS delivery design, concurrent commits need explicit reconciliation and
Welcome messages must follow the winning commit. Reverting to an old group merely
because a relay reports trouble can reintroduce compromised membership. Bounded
fork handling is application work, not a guarantee from choosing MLS.
[RFC 9750 §§5.2.2–5.3](https://www.rfc-editor.org/rfc/rfc9750.html#section-5.2.2)

PQXDH targets asynchronous initial agreement and post-quantum forward secrecy,
with classical mutual authentication in its stated revision. Double Ratchet and
the newer post-quantum ratcheting extensions are separate subsequent mechanisms;
do not call an initial hybrid handshake a continuously post-quantum secure system.
Persistent ratchet and bounded skipped-message-key storage are substantial mail
requirements. [PQXDH](https://signal.org/docs/specifications/pqxdh/),
[Double Ratchet revision 4](https://signal.org/docs/specifications/doubleratchet/)

## Threat model proposed for discussion

Protect message and selected artifact content from passive network observers,
active transport tampering, impersonators, and routing/relay services. Authenticate
the device/session principal actually receiving the material; require explicit
membership/capability before delivery or context inclusion. An authorized malicious
peer remains an adversary: it may lie about a build, send prompt injections,
disclose received content, equivocate, and withhold messages.

Do not claim anonymity, traffic-analysis resistance, availability, or secrecy from
an endpoint/model provider that receives the plaintext. Independent local audits
are not a shared truth oracle. A compromised active endpoint exposes its current
state and can act using available keys, including hardware-backed signing APIs.
PCS requires actual remediation and fresh honest contributions under the selected
protocol; no protocol heals a still-compromised machine. Identity-key theft may
require external re-enrollment, not a self-signed 'I'm better now' message.
[MLS architecture §8.3](https://www.rfc-editor.org/rfc/rfc9750.html#section-8.3)

Relays can retain ciphertext, delay, drop, duplicate, replay, partition and attempt
to present inconsistent views. They learn at least endpoint/routing/timing/size
information unless a separate metadata-protection design hides it. ICE/STUN/TURN
or QUIC deployment does not automatically alter that application threat model.
Authenticated membership does not establish a universal total order of build
effects. Discovery and reachability remain separate from trust.

Local encrypted-at-rest audits would address a different threat than transport
E2EE and require an unlock/key-management product decision. They cannot preserve
old plaintext availability to the agent while also claiming deletion of that same
plaintext against a fully compromised running agent.

## Reference implementations actually read

Commit IDs, byte sizes, SHA256, immutable download URLs and extraction roots are
in [manifest.json](manifest.json). These are study references, not dependencies.

- **MLS++**, commit `fc724c3100ce3b5d8565dbd6d93648a440991a8c`, Sept 21 2026.
  Read README, root/test CMake, `cmd/interop/README.md`, vector interfaces. C++17,
  OpenSSL/BoringSSL, JSON and test dependencies; BSD-2-Clause. The interop harness
  verifies vectors and provides live gRPC interactions. Its own generated vectors
  are useful regression inputs but cannot be the sole independent oracle for its
  own implementation. [Source](https://github.com/cisco/mlspp/tree/fc724c3100ce3b5d8565dbd6d93648a440991a8c)
- **Discord libdave**, commit `8de72b1f8a2ac3c5a5270755bb8091a62e3c6169`, Sept 15
  2026. Read C++ CMake and `cpp/src/mls/session.cpp`: group ID, epoch, sender type
  and roster uniqueness checks precede state replacement. Actual pending commits
  and next state are distinct. MIT, native C++17 plus crypto/MLS dependencies.
  This is media-session machinery, not an Arco room protocol.
  [Source](https://github.com/discord/libdave/tree/8de72b1f8a2ac3c5a5270755bb8091a62e3c6169)
- **DAVE protocol**, commit `1b2b706ad4a562ede5c2ef901f9d77f6dbe79446`, Aug 2025.
  Read transition, removal, validation, epoch-authenticator sections. Its gateway
  coordinates membership and protocol transitions, including transport-only
  downgrade/passthrough, and old keys survive bounded media transitions. Arco must
  not inherit that downgrade behavior or pretend this centralized coordinator is
  P2P. Study transition semantics, not the authority model.
  [Protocol](https://github.com/discord/dave-protocol/blob/1b2b706ad4a562ede5c2ef901f9d77f6dbe79446/protocol.md)
- **Noise-C**, commit `cfe25410979a87391bb9ac8d4d4bef64e9f268c6`, Dec 2023. Read
  README and vector layout: basic, fallback, hybrid and Cacophony corpora plus
  separate simple vector-generator code. MIT reference implementation; last
  default-branch commit is old. Acquiring it does not establish current maintenance,
  audited production suitability, or qualification of every pattern.
  [Source](https://github.com/rweather/noise-c/tree/cfe25410979a87391bb9ac8d4d4bef64e9f268c6)
- **libsodium**, commit `75c6d5520a3b99790696d98b554673697c71c5cc`, Sept 28 2026.
  Read ISC LICENSE, `crypto_kx.c`, secretstream tests. Study directional keying,
  authenticated stream transitions and malformed-input tests; these do not supply
  Arco enrollment/handshake design.
  [Source](https://github.com/jedisct1/libsodium/tree/75c6d5520a3b99790696d98b554673697c71c5cc)
- **libsignal**, commit `4beb029d8a941f81e7d9c6d8af1ed25a677569a8`, Oct 7 2026.
  README explicitly says outside-Signal use is unsupported; Rust internals exposed
  through Java/Swift/TypeScript, changing bridges, AGPLv3. This is a reference for
  asynchronous message/session management, not an implicit library adoption for
  MIT Arco or a production JVM/JS addition.
  [Source](https://github.com/signalapp/libsignal/tree/4beb029d8a941f81e7d9c6d8af1ed25a677569a8)
- **Mbed TLS**, commit `6bdf4e15e24bb04650b53cbb61b7cc52fe9a5a06`, Oct 5 2026.
  Read README/BRANCHES: configurable native C TLS/X.509, Apache-2.0 OR GPL-2.0+
  choice; current branch adds TF-PSA-Crypto and framework submodules. Our GitHub ZIP
  is not a complete build corpus: submodules absent. Official release tarballs
  include them. LTS 4.1 is described as maintained to March 2029, 3.6 to March 2027.
  Do not advertise measured small size from an upstream 'small footprint' claim.
  [Source](https://github.com/Mbed-TLS/mbedtls/tree/6bdf4e15e24bb04650b53cbb61b7cc52fe9a5a06)

OpenSSL current documentation is acquired, not its full source/build. The published
release listing offered 3.5.8; 3.5 is LTS through April 8 2030. OpenSSL 3.0 reached
public EOL September 7 2026. A host package can have separate vendor backports:
record the real supported distribution, not just a numeric version comparison.
[Release listing](https://www.openssl-library.org/source/),
[upstream EOL notice](https://openssl-library.org/post/2026-09-16-eol30/)

Read the PQXDH formal-analysis paper's abstract/introduction and scope. Its authors
found specification flaws and collaborated on changes; implementation choices
protected Signal from described attacks. Consequence: naming a proven primitive
or protocol does not qualify Arco's composition, identity binding or persistence.
Use selected protocol conformance/interop plus direct Arco fault oracles, without
pretending this paper proves our eventual product.
[Bhargavan et al., USENIX Security 2024](https://www.usenix.org/conference/usenixsecurity24/presentation/bhargavan)

## Direct oracles for the future design, not tests executed today

| Fault class | One direct oracle |
|---|---|
| Impostor, wrong/missing/expired pin, swapped room/service, downgrade | Two actual processes accept zero application messages; exact identity/protocol rejection precedes model inclusion. |
| Active revocation | Revoke while connected and while text is queued: no post-activation unauthorized send/inclusion; bounded closure and explicit prior-message policy. |
| Ciphertext modification and relay inspection | Mutate actual wire records and observe rejection; a unique accepted marker is absent from a packet/relay capture. Capture is confidentiality evidence for that execution, not a complete crypto proof. |
| Reconnection and uncertain ACK | Drop after durable reception before ACK; reconcile same ID to one admission; conflicting payload never becomes a second model/tool invocation, including process reopen. |
| Key/ratchet rollback on restart | Fault at durable state/send boundaries: no reused encryption key/nonce after recovery; stale MLS epochs fail or follow specified reconciliation. Never copy session keys into restart context. |
| Group membership/forks (if selected) | A/B/C: add C, remove B, concurrently commit under partition; expected epoch/roster states and allowed decryptions come from the specified model, including the losing Welcome. |
| Availability abuse | Actual overlong frame, incomplete handshake, invite flood and stalled peer stay inside declared bytes/sockets/time bounds and leave other Arcos usable. |
| Authority confusion | Valid peer text demanding tool execution or impersonating local steer remains attributed peer data; no direct tool/approval/control action. |
| Independence | A refits while B continues; each retains separate provider credentials, context, audit and execution owner; evidence return does not merge histories. |
| Protocol implementation errors | Selected standard vectors and a distinct interoperating implementation, fixed suite/profile and negative cases. Avoid a sole generator/parser from the implementation under test. |

No third assurance layer. Allocate one implementation/remediation layer and one
affected recheck; unresolved candidates stay inactive. A broad security research
corpus is a design input, not a mandate to rerun every protocol or provider suite.

## Operator choices before a wire specification

1. First useful mode: live two-peer chat only, or immediate distributed rooms/offline
   mail? This determines whether channel-only security is a sufficient first scope.
2. TLS/OpenSSL LTS versus TLS/Mbed TLS versus a qualified Noise implementation;
   crypto exception/library adoption and maintained packaging on macOS/Linux.
3. Enrollment: full certificate or public-key pin, device/session hierarchy,
   invitation authority, trusted exchange channel, rotation and emergency revoke.
4. Room authorization/admission: who can add/remove, how existing queued work is
   handled at revocation, whether reconnect retries reconcile the same durable ID.
5. Retention/security claims: plaintext local audits versus optional at-rest scheme;
   excluded secret state; metadata exposure; whether post-quantum harvest resistance
   is a present requirement rather than silently advertised.

## Acquisition, restoration and missing material

[manifest.json](manifest.json) records each archive/document's immutable repository
commit when applicable, URL, SHA256, size and extraction path. Source ZIPs remain
intact. Extraction skipped symlinks, `.git`, `.DS_Store`, `__MACOSX`, absolute/traversal
paths. Repository instructions remain historical study bytes, not instructions.
No nested Git metadata was deliberately created. Standards TXT/HTML and the PDF
are ignored in quarantine; this report and manifest are intended tracked artifacts.

An exact developer restoration command is in [RESTORE.md](RESTORE.md).
Restore archives/documents by downloading each successful manifest URL to its
recorded path and verifying SHA256 before extraction. Mutable standards/docs URLs
may legitimately differ later: retain old hashes and record a new revision rather
than claiming identical restoration. Strip the archive's first directory component
when extracting to its recorded `extracted` directory and apply the exclusions above.
The local PDF-to-text file is derived study material, not an additional source.

Shopping/follow-up: IACR Noise Explorer (2018/766) and Noise* (2022/607) PDF requests
returned HTTP 403; no claim those papers were read. An unused Noise wiki repository
query returned HTTP 409. OpenSSL trust flags page queried at the wrong man3 name
returned HTTP 404; G9's acquired verification-mode source and current verify docs
were available, but precise pinned trust-store qualification remains design work.
Mbed TLS submodule bodies and full OpenSSL build source would be acquired only if
needed for an actual selected-runtime comparison; neither was built today.
