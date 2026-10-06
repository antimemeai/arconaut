# chat-system

Chat System supplies protocol-neutral messaging and listener callbacks plus group/stream helper primitives. Important exposed contracts have silent no-ops and IRC lifecycle/TLS limits.

Role: supporting Rust multi-protocol chat transport library. Runtime: Rust.

Pinned source: [https://github.com/rexlunae/chat-system](https://github.com/rexlunae/chat-system); revision/version `8fefcaea4f51ca5cd62e58a739f617ed0912522b`.

Tokio async clients/listeners/callbacks; caller supplies agent runtime; manager serial awaits.

Consumes external chat protocols or hosts listeners; owns transport connections/helper buffers, not models/shared computation fabric.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### GenericMessenger::new / initialize / disconnect

Surface: Rust API.

Input: tagged MessengerConfig protocol/backend settings

Result: initialized boxed backend or error

Lifecycle: initialize builds connection; disconnect backend-specific

Authority: caller supplies transport credentials/config; no model actor built in

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e10](#evidence-e10).

### Messenger::send_message / send_message_with_options / receive_messages

Surface: Rust API.

Input: recipient/content or reply/thread/silent/media options

Result: message-ID string or Vec<Message>/error

Lifecycle: protocol await/poll; default options can be discarded; IRC IDs not receipt

Authority: caller or composed agent program; remote platform rules

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### Messenger::set_typing / set_status / set_text_status / add_reaction / remove_reaction / get_profile_picture / set_profile_picture

Surface: Rust API.

Input: channel/bool, presence/text, message/channel/emoji, user/URL

Result: Result<()> or optional profile URL

Lifecycle: optional trait defaults silently succeed/None; not universal remote effect

Authority: caller account; unsupported operations not indicated by success

Evidence: [e4](#evidence-e4).

### Messenger::search_messages / edit_message / delete_message / pin_message / unpin_message / get_channel_members

Surface: Rust API.

Input: SearchQuery filters or message/channel/content

Result: Vec<Message>, Result<()>, member Vec

Lifecycle: default empty/no-op; platform-specific support must be inspected separately

Authority: caller account; current API does not negotiate support

Evidence: [e4](#evidence-e4).

### MessengerManager::initialize_all / disconnect_all / receive_all / broadcast / get

Surface: Rust API.

Input: configured messenger list, recipient/content or name

Result: aggregated messages, per-backend results, optional handle

Lifecycle: serial awaits; receive errors swallowed to tracing; not concurrent fan-out

Authority: program owns backend list; backend credentials differ

Evidence: [e5](#evidence-e5).

### Server::add_listener / ChatServer::run / shutdown

Surface: Rust service API.

Input: listeners and async Message→optional-reply callback

Result: listener-bound service and callbacks

Lifecycle: accept tasks concurrent; shutdown alive barrier excludes active connection children

Authority: caller owns listen address/callback authority; no model request engine

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### GroupChatConfig::is_group_allowed / should_respond / session_key / strip_prefix

Surface: Rust supporting API.

Input: group/user/name/text; activation/isolation config

Result: boolean routing choice, stable key, stripped text

Lifecycle: pure helpers; caller must enforce and persist context

Authority: not automatically wired policy/context governor

Evidence: [e16](#evidence-e16).

### StreamBuffer::push / finish / should_flush / flush

Surface: Rust supporting API.

Input: text and edit/chunk/buffer config

Result: SendNew|EditExisting text plan

Lifecycle: monotonic local flush interval; no native provider/transport call

Authority: caller performs sends/edits and tracks remote ID

Evidence: [e17](#evidence-e17).

## Capabilities

### filesystem

**— — Files** (inspection-limit): Not a coding filesystem agent; inspected transports and message helpers do not grant workspace tools.

### processes

**— — OS programs** (inspection-limit): No process lifecycle agent in traced IRC/manager/server paths; optional subprocess backends untraced.

### code-actions

**— — Code actions** (inspection-limit): Model-authored native tool program runtime outside messaging-library role.

### persistent-kernel

**— — Kernel** (inspection-limit): Language interpreter/kernel outside role.

### standing-database

**— — Standing DB** (inspection-limit): General model-accessible standing query DB outside transport role; platform storage not inferred.

### workflow-programming

**S — Workflows** (source): Rust async message callbacks and transport APIs compose custom workflows; no owned workflow scheduler/model loop.

Evidence: [e5](#evidence-e5), [e13](#evidence-e13).

### multi-model

**— — Models** (inspection-limit): No model/provider selection or inference; supports chat transports, not model collaboration by itself.

### live-collaboration

**L — Peer chat** (source): IRC/platform clients are support transport, not multi-model room agent. Built-in IRC listener invokes per-connection callback and replies only there; no shared peer fan-out/member state.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e11](#evidence-e11).

### concurrent-work

**L — Concurrency** (source): Listener accepts multiple clients concurrently; manager broadcast/receive loops await serially. Active connection tasks outlive listener shutdown.

Evidence: [e5](#evidence-e5), [e12](#evidence-e12), [e15](#evidence-e15).

### steering-interrupt

**S — Steer/interrupt** (source): Disconnect/listener shutdown supplied, but no model cancellation; shutdown does not drain connection callbacks.

Evidence: [e10](#evidence-e10), [e12](#evidence-e12), [e15](#evidence-e15).

### turn-redefinition

**S — Turn program** (source): Caller chooses arbitrary async MessageHandler and pure group routing helpers; no predefined model turns to redefine.

Evidence: [e13](#evidence-e13), [e16](#evidence-e16).

### compaction

**— — Compaction** (inspection-limit): No model context compactor in inspected messaging/helper paths.

### context-repair

**— — Repair** (inspection-limit): No model history restoration operation in traced transport/helpers.

### original-audit

**L — Original audit** (source): Normalized transport messages with timestamps and optional edits; no full original/raw persistence in inspected IRC/core paths. Error swallowing and synthetic IDs limit study provenance.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e8](#evidence-e8), [e11](#evidence-e11).

### audit-query

**L — Audit query** (source): SearchQuery optional trait returns empty by default; neither unsupported signal nor audit archive. Remote search implementation not traced.

Evidence: [e4](#evidence-e4).

### hot-change

**L — Hot change** (source): Tagged config runtime selects backend at initialization, not watched reload; listeners added before run. No deferred affected-work boundary.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e14](#evidence-e14).

### rebuild-continuity

**— — Rebuild continuity** (inspection-limit): No harness rebuild/context transfer protocol in transport library.

### remote-services

**L — Remote** (source): Unified clients consume external platforms; IRC TLS intent currently remains plain TCP, unlike optional TLS-server feature.

Evidence: [e1](#evidence-e1), [e8](#evidence-e8), [e23](#evidence-e23), [e24](#evidence-e24).

### self-improvement

**— — Self-improve** (inspection-limit): No agent autoresearch/governed self-modification loop in transport library.

### complaints

**— — Complaints** (inspection-limit): No state-capturing agent complaint/bead/DB mechanism in inspected paths.

### authority

**L — Authority** (source): Transport credentials/network callback ownership; group policy helpers require caller enforcement. IRC listener accepts PRIVMSG before completed registration and has no traced auth/room authority.

Evidence: [e11](#evidence-e11), [e16](#evidence-e16), [e23](#evidence-e23).

### evaluation

**L — Evaluation** (source): Real localhost callback tests and Console-manager/pure group/stream assertions read only. Tests abort server and do not challenge shutdown child lifetime or TLS-client encryption.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22).

### time-order

**L — Time/order** (source): Stream uses Instant flush timing; normalized timestamps/IRC millisecond synthetic IDs do not establish durable seq/causal peer ordering or dedup.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8), [e17](#evidence-e17).

## Inspected test oracles

- [quarantine/chat-system/tests/irc_server.rs](../../../quarantine/chat-system/tests/irc_server.rs): Local wire→callback delivery Oracle: Real localhost socket tests exact text/both listener messages; fixed sleeps and abort teardown leave graceful child shutdown untested. Read, **not executed**.
- [quarantine/chat-system/tests/manager.rs](../../../quarantine/chat-system/tests/manager.rs): Manager forwarding Oracle: Console queue count and broadcast result count/Ok; weak delivery-content oracle and no remote backend. Read, **not executed**.
- [quarantine/chat-system/src/streaming.rs](../../../quarantine/chat-system/src/streaming.rs): Flush strategy Oracle: Pure action variants/chunk text and cleared length; no actual remote edit effect. Read, **not executed**.
- [quarantine/chat-system/src/group_chat.rs](../../../quarantine/chat-system/src/group_chat.rs): Group predicate helpers Oracle: Pure substring mention/prefix/allow-list decisions; no owned context store/isolation oracle. Read, **not executed**.

## Useful mechanisms

- Protocol-neutral callback and normalized message surface can support independently owned peer fabric.
- Group routing keys and stream strategies are small separable composition primitives.

## Material limits

- IRC client TLS flag uses plain TCP; readiness can succeed after welcome timeout/EOF.
- Built-in IRC listener is not a peer relay and shutdown omits active connection tasks.
- Optional APIs silently succeed/empty when unsupported; manager swallows receive errors.

## Arconaut design questions

- Should unsupported operations and transport receipt semantics be explicit sum types?
- What room router, membership/authentication, durable causal audit and child quiescence would an owned fabric need?

## Evidence

### Evidence e1

[quarantine/chat-system/README.md:1–17](../../../quarantine/chat-system/README.md#L1): Multi-protocol Rust chat library claim, not model engine.

### Evidence e2

[quarantine/chat-system/src/config.rs:251–266](../../../quarantine/chat-system/src/config.rs#L251): Runtime tagged config builds selected protocol backend, IRC TLS flag forwarded.

### Evidence e3

[quarantine/chat-system/src/config.rs:434–509](../../../quarantine/chat-system/src/config.rs#L434): GenericMessenger builds/initializes backend then forwards operations; uninitialized calls error.

### Evidence e4

[quarantine/chat-system/src/messenger.rs:57–184](../../../quarantine/chat-system/src/messenger.rs#L57): Unified lifecycle/send/receive and optional presence/reaction/profile/search/edit/delete/pin/membership operations; defaults silently no-op/empty.

### Evidence e5

[quarantine/chat-system/src/messenger.rs:209–255](../../../quarantine/chat-system/src/messenger.rs#L209): Manager awaits backends serially for initialization/send/receive; receives warn-and-swallow individual errors, broadcast returns per-backend results.

### Evidence e6

[quarantine/chat-system/src/message.rs:40–94](../../../quarantine/chat-system/src/message.rs#L40): Normalized message sender/channel/ID/time/thread/edit/media fields and SendOptions; not canonical causal event log.

### Evidence e7

[quarantine/chat-system/src/messengers/irc.rs:112–134](../../../quarantine/chat-system/src/messengers/irc.rs#L112): IRC send_raw returns Ok when no connection; per-reader/writer async mutex and timeout read.

### Evidence e8

[quarantine/chat-system/src/messengers/irc.rs:238–306](../../../quarantine/chat-system/src/messengers/irc.rs#L238): IRC TLS flag does not encrypt: stores Plain split TCP. Welcome wait timeout/EOF still proceeds joins and connected=true. Send ID derived recipient/content not server acknowledgment.

### Evidence e9

[quarantine/chat-system/src/messengers/irc.rs:309–346](../../../quarantine/chat-system/src/messengers/irc.rs#L309): IRC receive polls until 100ms idle and parses wire fields; receive depends on caller pumping PING too.

### Evidence e10

[quarantine/chat-system/src/messengers/irc.rs:429–434](../../../quarantine/chat-system/src/messengers/irc.rs#L429): Disconnect ignores QUIT failure then drops connection.

### Evidence e11

[quarantine/chat-system/src/servers/irc.rs:97–207](../../../quarantine/chat-system/src/servers/irc.rs#L97): Per-client nick/USER registration and callback/reply processing; no shared peer routing/channel membership state; handler error silently omitted and PRIVMSG not gated on registered.

### Evidence e12

[quarantine/chat-system/src/servers/irc.rs:220–274](../../../quarantine/chat-system/src/servers/irc.rs#L220): Listener spawns accept and per-connection tasks; shutdown only stops accept. Child connection tasks do not clone alive or receive shutdown.

### Evidence e13

[quarantine/chat-system/src/server.rs:23–49](../../../quarantine/chat-system/src/server.rs#L23): Generic Rust async MessageHandler receives Message and returns optional reply.

### Evidence e14

[quarantine/chat-system/src/server.rs:160–175](../../../quarantine/chat-system/src/server.rs#L160): Listeners added before run; no hot attach contract.

### Evidence e15

[quarantine/chat-system/src/server.rs:189–218](../../../quarantine/chat-system/src/server.rs#L189): Server starts listeners and waits alive channel closure; shutdown signals listeners but does not await children/request quiescence.

### Evidence e16

[quarantine/chat-system/src/group_chat.rs:106–176](../../../quarantine/chat-system/src/group_chat.rs#L106): Group eligibility, substring mention/prefix activation and isolation session-key helpers; exported support functions rather than automatic context owner.

### Evidence e17

[quarantine/chat-system/src/streaming.rs:112–205](../../../quarantine/chat-system/src/streaming.rs#L112): StreamBuffer accumulates text and yields send/edit actions; chunked flush clears pending, edit-in-place full accumulated projection; caller must perform transport effects.

### Evidence e18

[quarantine/chat-system/src/streaming.rs:288–328](../../../quarantine/chat-system/src/streaming.rs#L288): Pure streaming tests check action variants and chunk content/cleared length; no server edit acknowledgment.

### Evidence e19

[quarantine/chat-system/tests/irc_server.rs:41–76](../../../quarantine/chat-system/tests/irc_server.rs#L41): Real localhost listener/client source test asserts callback exact received text; sleeps and aborts server task, not graceful shutdown oracle.

### Evidence e20

[quarantine/chat-system/tests/irc_server.rs:110–123](../../../quarantine/chat-system/tests/irc_server.rs#L110): Two listener test sorts callback messages and asserts both arrived; no peer broadcast or child lifetime assertion.

### Evidence e21

[quarantine/chat-system/tests/manager.rs:108–134](../../../quarantine/chat-system/tests/manager.rs#L108): In-memory Console backend tests check receive count and broadcast result count/Ok, not content delivery or remote APIs.

### Evidence e22

[quarantine/chat-system/src/group_chat.rs:195–261](../../../quarantine/chat-system/src/group_chat.rs#L195): Pure mention/prefix/allow group predicate assertions, not model-context isolation persistence.

### Evidence e23

[quarantine/chat-system/src/messengers/slack.rs:12–80](../../../quarantine/chat-system/src/messengers/slack.rs#L12): Representative authenticated external backend stores token, client, per-channel in-memory cursor and channel watch configuration.

### Evidence e24

[quarantine/chat-system/Cargo.toml:27–60](../../../quarantine/chat-system/Cargo.toml#L27): Tokio Rust library; optional TLS server and platform dependencies distinct from plain IRC client flag.

