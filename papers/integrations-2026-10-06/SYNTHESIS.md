# Lean integrations, thematic feeds, n-Arconaut multiplayer

2026-10-06. Discussion candidate, using the operator's clarified meanings. Keep the
C++/Lua harness small; add common integrations as optional packages. HUD presents
project-relevant current events. Multiplayer is n independent Arconauts sharing
development/build over the network, with human-human, agent-agent and human-agent
communication. n=2 is the first concrete scenario, not a product limit.
Subsequent operator clarification selects P2P and end-to-end encryption: chat is
a complete minimal mode; deeper problem/file/build sharing and inter-Arco model
mail between live sessions are optional. The original shared-build exercise below
qualifies the richer mode, not the minimum bar for multiplayer.

## One extension mechanism, independently installed capabilities

A package can contribute tools, immediate commands, Lua workflows/event handlers,
feed subscriptions and native view descriptions. The core supplies generic execution,
audit/context/session identity, registration and generation lifecycle. Each service's
adapter, account setup, schemas, reconciliation and substantial dependencies live
with its optional package or independently managed service. Installed, enabled,
configured and running are different facts. Merely discovering metadata must not
initialize an SDK, launch a process, fetch data or advertise every tool to the model.

Examples: GitHub/Linear work tracking; Slack/Discord room bridges; Hugging Face
model/repository access; arXiv/vulnerability/financial feed sources. An optional MCP
client can give broad interoperability; it need not be the internal Lua extension
interface or define our room protocol. Model/operator can configure and author the
programs normally; changes activate after affected workflows by default. No new
per-operation approval process and no JS production runtime implied.

[Claude mods](https://code.claude.com/docs/en/plugins/mods/overview) supply a concrete
reference: engine events, tools/commands and UI independently of model turns.
Their public sources also show why lifetime and outcome distinctions matter:
sample replay records intended edits before the result, while built-in diff tests
reject failed/refused edits and do no Git work at startup. Public types lag current
runtime docs, and the closed host is not source-audited. Root read declarations,
replay source and relevant diff-test locations as well as the lane study.

Start with Lua modules plus native view descriptions and optional program/service
clients. Native shared libraries can follow a demonstrated need; their ABI/unload
complexity is not a prerequisite for a Slack adapter or feed pane. No external
library, UI framework, service or protocol implementation adopted here.

## HUD: the world relevant to this project set

A subscription profile selects sources, entities/topics, relevance policy and
freshness/cadence. Share profiles across project sets without sharing credentials
or read positions. The underlying item retains source ID/revision, link, original
reference and publication/update/fetch times actually supplied or measured.
Summaries/relevance explanations are derived and repairable. Show stale, missing
and revised data honestly.

Three configured uses of one item: display it; discuss it in a room/model context;
admit work through a station trigger. These can coexist. Subscription and routing
policy can authorize automatic actions; inference is not needed for every fetch or
paint. Campaigns keep ordinary operator steering. Shared collectors/DBs are services
Arco consumes, not a new control plane governed by an individual Arconaut.

Concrete scenario: a revised context-management paper appears in the agent-research
HUD; a configured research workflow studies changed claims/reference code and posts
findings to the project room. A CVE item is matched to actual dependency/deployment
inventory before a proposed fix. A Hugging Face notification can update a watchlist
without downloading model weights or switching the live provider.

A first useful account-free discussion candidate is arXiv topic polling plus CISA
KEV snapshot differences. KEV is known-exploitation coverage, not every CVE. General
CVE enrichment, streaming market data, Linear and Discord need their own interface
qualification before implementation. Financial filings were studied as one feed
example; that does not select a price-data provider.

The [feed study](feeds/report.md) reads actual official delivery contracts. Source
notification, durable capture, displayed item and acted-on work have separate
identities/states. GitHub Events polling is bounded and delayed; HF retries can be
out of order; Slack event IDs differ from acknowledgement IDs. The
[July MCP transport](https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http)
removes legacy GET/session assumptions and unsupported resumable-stream claims.
Each package must reconcile its source; a generic connection cannot promise every
source event was received. Root checked the current primary transport page.

## Multiplayer: shared development, separate minds and processes

A human and its Arconaut can join the same project room as another human/Arconaut.
Humans address humans or models; models address models or humans. A lead role can be
chosen as workflow policy, not imposed as a permanent spawning hierarchy. Room
history and shared build facts are common; each local context, programs, provider
credentials and active executable generation remain independent. Importing a peer's
context or finding is an explicit, attributed operation, not automatic context fusion.

[Claude teams](https://code.claude.com/docs/en/agent-teams) are a useful comparator,
not this product definition. Their current documented team is lead-centered and
interactive; separate [cross-session messaging](https://code.claude.com/docs/en/cross-session-messaging)
reaches one's own sessions. The 2.1.178 changelog removed TeamCreate/TeamDelete in
favor of an implicit team and named Agent spawning, not the feature itself. Neither
is evidence of n humans sharing code/build across independently deployed Arconauts.

Three independent shared facts need designs: room communication; source/workspace
publication; build/artifact outcomes. A message claiming a build passed does not
substitute for the actual source/settings/artifact and build-result identity. A
claim/lease coordinates work but cannot prevent arbitrary filesystem writes or
magically merge conflicting code. A message can be accepted yet not included in
the next model request. Shared source changes do not silently refit another Arco.

The first thought experiment:

1. A on the laptop and B on another machine join one project/build with separate
   local contexts/providers and identified human/model participants.
2. The humans discuss scope; both models read the attributed room constraints.
   A proposes a code change; B independently reviews/builds the exact source state.
3. Test/build results carry source and environment/artifact identities, not merely
   a green status. Different host targets may produce different binaries.
4. Simultaneous edits to one area produce an explicit conflict or agreed publication
   transition. A shared editor convergence mechanism is not a semantic merge oracle.
5. B disconnects while A continues. B reconnects, recovers missed room/build facts,
   and sees gaps/unknown effects honestly. Duplicate delivery does not redo a write.
6. A compacts/refits/restarts; B and independently managed services continue. A
   resumes its own context/identity and catches up without resetting the room.

The same identities, addressing and lifecycle rules extend to n; no pair-specific
protocol or hardcoded provider graph. Direct acceptance should observe all three
communication classes, actual request inclusion and correctly finished shared work,
conflicting publication, reconnect, and one refit while another participant proceeds.
A room message test alone is weaker than the shared-build claim.

The teams lane studies ICE, Commonly and Agent Room as contrasting implementations:
collaborative source/Git publication; human/agent rooms with separate native driver
sessions; durable ordered messaging/listen state. Their mechanisms must not be
collapsed into one alleged multiplayer stack. ICE’s concurrent publication test
allows the final whole snapshot to replace another; Commonly’s delivery nonce does
not establish model consumption; Agent Room’s durable replay does not supply shared
source/build machinery. These are inspected test/source contracts, not executed
qualifications. Neither their libraries nor a
room service or shared-filesystem implementation is selected by this research.
A centralized plaintext room is not the selected product direction; P2P/E2EE
constrains any future rendezvous/relay design.

## Fit with the agreed course

Managed compaction -> useful-work evolution pilot -> giga remains the sequence.
These requirements shape the generic tool/module boundary and later room/feed work;
they do not turn every social integration or n-way collaborative editor into a
prerequisite for the pilot. The audit explorer/rageshake can itself become an early
extension view and evidence source. The next design decisions are the package
contract/state lifetimes, first HUD subscription, and shared-source/build publication
model for n=2. Discuss adoption only where a concrete dependency/service is needed.

Sources and limits: [mods](mods/report.md), [feeds](feeds/report.md), [teams](teams/report.md)
and their acquisition manifests. Reference code/tests were read, not executed;
source study does not qualify the runtime. Primary docs captured through browser
excerpts are identified separately from intact downloads. No installs, account-auth,
external messages, third-party dependency or product implementation through this study.
