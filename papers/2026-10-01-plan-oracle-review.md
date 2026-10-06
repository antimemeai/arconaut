# Plan review: retained publication and context/provider oracles

2026-10-01. Reviewed `docs/TESTING_PLAN.md` and `docs/IMPLEMENTATION_PLAN.md`
against the resolved core contracts and this lane's context review. Scope is
independence/completeness of publication, admission, recovered decisions, context
and provider evidence, including the declared fault profile. No plan edits,
acquired execution, credential access or installations.

**Judgment: the route is actionable and the principal oracles are independent.
Two bounded omissions should be corrected before execution.** Neither needs
another research phase or new architecture.

## 1. Persistence schedules must not silently assume prefix persistence

Testing plan T1, line 69, asks to model admissible persisted prefixes and sync
failures. Cutting execution at every source/commit/sync boundary is useful, but
does not by itself explore persistence reordering or partial persistence among
unsynchronized writes. A later commit frame or segment name can survive while an
earlier pending payload does not, unless the chosen backend establishes stronger
ordering. The reviewed specification explicitly handles interior damage and binds
commit to source frames; a prefix-only fake device can mask an implementation that
trusts a surviving commit marker without all its source/dependency material.

**Required adjustment.** U1's selected fault profile should define admissible
surviving writes, including reordered/partial pending writes and segment-name
publication where the backend permits them. Drive recovery directly with those
states: missing earlier source plus surviving later commit, complete unsynchronized
batch, and acknowledged earlier batch with damaged/uncommitted later material.
The predicate must independently require the appropriate acknowledged sources and
transitions, and forbid trusting an incomplete batch. If a profile intentionally
models only prefix loss, explicitly restrict its claim rather than calling it
generic persistence fault coverage. Runtime power-loss claims still need the named
environment already required by the plan.

This is ordinary direct recovery testing, not a second audit or tests for tests.
Grounding: the custody study's framing/sync distinction and the readiness report's
Pillai application-crash-consistency method distinguish persistence ordering from
execution ordering; neither establishes prefix-only behavior for current APFS.

## 2. Provider incoming originals need their own seam comparison

T6's acceptance and detailed passage explicitly compare outgoing final bytes to
the credential-free server, then test streaming events/tool-result structure.
They do not explicitly compare the server's independently emitted response bytes
to the retained incoming originals before decoding. U1's known-byte journal tests
do not exercise this transport-to-capture seam.

**Counterexample.** An adapter captures reconstructed parsed events, discarding
whitespace, unknown fields, invalid byte tails or a truncated event. Its tool
actions and next request can still be correct, and its outgoing request capture
can be exact. The stated T6 examples could pass while original provider capture
violates the core contract and loses material needed for future study/repair.

**Required adjustment.** Give the local server a predetermined response byte
sequence and controlled fragmentation/termination. Compare the retained incoming
body/observable transport representation and offsets directly to what that server
emitted within the declared boundary, separately from the expected decoded events
and continuation actions. Include malformed/truncated tails and recording failure
mid-stream; retained prefix and uncertainty must match the actual observation
boundary. No network-packet or provider-internal claim is required. Existing server,
journal and decoder seams suffice.

## What already works

Recovered effects use an external dispatch count and retained decision identities;
fresh IDs cannot masquerade as recovery. Context tests use seeded indispensable
requirements and inspect real selected request structures, old edit bases and CAS
outcomes. Decoders compare explicit roles/IDs/content rather than only roundtrip.
Outgoing provider bytes are checked at an independently observing server. The
bounded model is separated from production algorithms, and sanitizers/coverage are
properly weaker complementary evidence. No circular oracle was found in these
contracts.

U1's semantic admission tests precede real U3 IPC/process recovery checks; U2's
deterministic context checks precede Lua/provider composition. Each unit explicitly
requires written sub-plan, direct red examples, code, green checks and adversarial
review. Dependency choices are timed to use, and qualification belongs to a bounded
unit. The plan does not defer CLM to a later polish phase or substitute a successful
chat for failure evidence. Resolving the two omissions permits this lane to proceed
to the ordered units; broadening the corpus again is unnecessary.

## Focused reread after integration

2026-10-01. Reread T1's revised persistence schedules and T6's incoming capture
comparison. **Both findings are resolved at plan scope; no remaining finding in
this review's publication/admission/context/provider invariant.**

T1 now explicitly covers admissible reordered/partial unsynchronized writes and
directory/segment publication, including surviving commits with missing source
material and acknowledged batches followed by damaged pending material. Recovery
must validate all sources/dependencies; prefix-only models limit their stated
scope. Process death remains distinct from qualified host power loss.

T6 now compares predetermined server response bytes and offsets against retained
incoming originals before decoding, including unknown fields, fragmentation,
malformed/truncated tails and recording failure. It separately checks observed
prefix/uncertainty and decoded behavior. Its note that server writes need not equal
transport chunk boundaries correctly limits the oracle to observable representation.

These are ready implementation/test obligations. This reread ran no runtime test
and establishes no host or provider qualification. The ordered unit/sub-plan/red/
code/green/review process can proceed without further changes from this lane.
