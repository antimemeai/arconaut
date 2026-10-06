# Optional integrations and a thematic project HUD

2026-10-06, discussion research. No installation, authentication, external message, reference-code execution or product edit performed. Primary docs and two pinned source/data archives acquired; one existing plugin reference reread. [Manifest](MANIFEST.md) / [machine acquisition record](acquisition.json) give exact provenance and restoration.

## Proposed boundary

The ordinary install should contain the C++/Lua coding engine, its audit/context/session machinery, and generic extension/process interfaces. It should work with **zero connectors installed**. A disabled package should incur no process launch, network request, authentication prompt, SDK import, provider request, or advertised tool/context clutter. Package discovery reads bounded metadata, not executable initialization. Installed, enabled, configured and currently running are distinct states.

GitHub, Slack, Discord, Linear, Hugging Face, arXiv, vulnerability intelligence and financial sources belong in separately installed packages. Each carries its adapter code, schemas, tool descriptions, subscription definitions, optional UI contribution and actual external requirements. C++ binaries or Lua modules are suitable package artifacts; an adapter may connect to an externally managed service. This selects no dependencies and does not require JS/Python in production. A remote third-party service's implementation language need not become Arco's runtime dependency.

The small shared extension contract needs versioned operation descriptions, bounded input/output, cancellation/deadline semantics, namespacing, audit attribution, ordinary credential-reference resolution and activation at workflow boundaries. Service-specific OAuth, webhook signatures, API pagination, quota buckets, reconciliation and topic-ranking logic stay in packages or an external collector. A giant “universal integration SDK,” all-service account database, or mandatory message broker is not justified for the initial install.

Arco consumes shared collectors/databases; it does not become their fleet governor. A local optional collector can be an OS daemon when uninterrupted intake matters. RRC pauses Arco's own consumer/work, while that independent collector continues. An instance-local adapter instead pauses with Arco and reconciles afterward. These are explicit lifecycle choices, not an invisible change in ownership.

## What the HUD actually presents

The HUD is a view of **project-relevant external developments**: a vulnerability affecting a deployed component, a new paper relevant to current research, a tracked model/repository update, or a material company filing in a project watchlist. It is not the runtime status dashboard.

A project subscription profile identifies sources, queries/entities, relevance policy, cadence/freshness target, expected consumption routes and limits. Sharing a profile shares the topic configuration, not credentials, account identity or read position. Multiple projects can inherit a common C++/Lua/agent-research profile and locally add targets. Preserve profile revision in each triage decision; a changed query/ranker changes the selected stream.

A feed item needs the source/entity identity and source revision; canonical link; published/updated time if supplied; fetched time; original-byte reference; query/profile provenance; and current delivery/triage disposition. Distinguish unavailable fields from invented timestamps. A model-written synopsis and relevance rationale are derived, revisable items attached to originals. The operator and model can inspect the same item and recover source details without loading the whole feed into every provider request.

Display, room discussion, and autonomous work are separate consumption routes. An item may simply appear in the HUD. A relevance rule may propose it to the current project room with a source link and concrete reason. Station mode may schedule a configured workflow for matching items. Campaign mode may put it into the operator's queue or weave it into active work. Merely subscribing to a topic must not silently authorize a message to an external channel or an arbitrary new campaign.

Example: a relevant arXiv update appears as “context policy paper revised; new evaluation section.” A research station acquires and compares it with our current policy, then posts a local room note with sources. A KEV addition matching a deployed component creates a triage item: confirm our actual version/configuration, study upstream remediation, propose a meaningful test/change. A Hugging Face update on an experimental model enters the model-evaluation queue; it does not automatically download gigabytes or switch live providers.

## Representative official interfaces

| Interface | Intake/auth | Identity and reconnect limits | Package consequence |
| --- | --- | --- | --- |
| GitHub | Signed webhook; scoped app/token for authenticated REST | `X-GitHub-Delivery` repeats on redelivery; Events polling is bounded/delayed | Durable capture before webhook acknowledgement; reconcile using relevant object APIs |
| Slack | Signed HTTP events or app-token Socket Mode; method/workspace scopes | Event identity and Socket envelope acknowledgement identity differ | Ack intake promptly; thread/cursor/backfill belongs to adapter, not a generic websocket reconnect |
| Hugging Face | Hub webhook; optional configured secret header; token for restricted APIs | Retry uses same `Webhook-Id`; events can arrive out of order | Deduplicate deliveries, preserve source revisions and reconcile repo state |
| arXiv | Public Atom query/RSS, no interactive account needed | Stable paper identity with versions; offset pages are not durable event cursors | Overlap polls and deduplicate versions; cache and obey aggregate request limit |
| CISA KEV | Public full catalog JSON/CSV/schema, authoritative mirror source | Catalog revision/time and CVE ID; entries can change | Diff snapshots by CVE plus content revision, keep changed/retracted state |
| SEC EDGAR | Public JSON submissions/XBRL, no API key for those read APIs | Filings by CIK/accession; updates throughout day | Optional financial-topic source, not a price feed or recommendation engine |

### GitHub

Read official webhook best practices and Events REST docs. Webhooks expect a 2xx within ten seconds; original and redelivered payload share delivery ID. The Events endpoint honors ETag/304 and `X-Poll-Interval`, but only retains up to 300 events from 30 days and documented delay can reach six hours. It cannot supply a complete immediate event log. [Webhooks](https://docs.github.com/en/webhooks/using-webhooks/best-practices-for-using-webhooks), [Events](https://docs.github.com/en/rest/activity/events).

Use delivery deduplication for intake, but correlate actual object revisions separately. A lost webhook gap calls for relevant issue/PR/commit state reconciliation, not faith in the general Events timeline. Quotas are source/account/installation dependent; observe actual headers and secondary throttling rather than hardcode one account-wide rate from a sample.

### Slack

Read Events API, Socket Mode and rate-limit docs. HTTP delivery expects acknowledgement within three seconds and retries failed delivery; `event_id` identifies the event. Socket payloads carry an `envelope_id` for acknowledgement, and connections periodically refresh. Payloads may arrive over any of several active connections. HTTP 429 responses carry `Retry-After`, with limits tied to method/workspace. [Events](https://docs.slack.dev/apis/events-api/), [Socket Mode](https://docs.slack.dev/apis/events-api/using-socket-mode/), [limits](https://docs.slack.dev/apis/web-api/rate-limits/).

Socket Mode avoids needing an inbound public endpoint but still requires installation/scopes/token management. Reconnecting a websocket is not proof no messages were lost. Backfill availability and rate limits must be qualified for the particular conversation API/account; this lane did not authenticate or reproduce those guarantees. No routine approval per inbound event is proposed.

### Hugging Face

Read official webhook delivery/retry section and Hub rate limits. `X-Webhook-Secret` transmits the configured secret; it is not the GitHub HMAC scheme. `Webhook-Id` is stable across retries and ordering is not guaranteed. Hub limits separate APIs, resolvers and pages, expose rate headers, and depend on actual plan/account. [Webhooks](https://huggingface.co/docs/hub/webhooks), [limits](https://huggingface.co/docs/hub/rate-limits).

Prefer the header over secret-bearing URLs. A source notification should expose a repo revision and link; fetching model metadata and downloading model weights are distinct operations with separate resource consequences. Do not use arrival order to replace newer metadata with older state.

### arXiv and vulnerability topics

Read arXiv API manual and terms: Atom exposes entry identity/published/updated metadata; search sorting and pagination can be selected. Legacy APIs, including RSS, share an aggregate limit across machines under one's control: one request per three seconds and a single connection. [API](https://info.arxiv.org/help/api/user-manual.html), [terms](https://info.arxiv.org/help/api/tou.html).

A project set should coalesce identical upstream queries rather than every Arconaut polling independently. Poll with overlap, retain versions, and label freshness. New/revised publication is not evidence its claims are sound; a research workflow still reads literature and implementation.

Acquired the authoritative `cisagov/kev-data` snapshot, read README, JSON catalog and schema. It provides catalog version/release time, CVE IDs, remediation fields and known/unknown ransomware-use indication, and tracks updates in repository history. [Source](https://github.com/cisagov/kev-data).

KEV is evidence of known exploitation, not the complete CVE universe and not proof our deployment is vulnerable. Match against actual components/versions/configuration and retain uncertainty. NVD developer pages returned an empty application shell, so their current API paging/rate contract was **not** acquired or qualified. Add NVD/general-CVE enrichment after reading a usable primary source; do not fill that gap from remembered numbers or a third-party wrapper.

### Financial topics

Read SEC's public submissions/XBRL API documentation through the web tool: public data endpoints need no authentication, submissions are updated throughout the day, and entity history includes additional files for older filings. [Official API](https://www.sec.gov/search-filings/edgar-application-programming-interfaces).

Direct snapshot acquisition returned 403; this source is read but not locally archived. A filing watchlist illustrates thematic financial intelligence without claiming streaming price data. Market-price sources require a separately researched package, entitlement and freshness policy.

## Two architecture references actually studied

**MCP's official protocol, pinned source:** read `docs/specification/2026-07-28/basic/transports/{stdio,streamable-http}.mdx`, subscriptions, pagination and authorization; inspected `schema/2026-07-28/schema.json` definitions/examples. Current Streamable HTTP uses POST, request-scoped responses and `subscriptions/listen`; the July revision removes legacy GET/session behavior and does not support `Last-Event-ID` resumability. Subscription acknowledgement reports the supported subset of requested notifications. Pagination cursors refer to lists, not durable event replay. [Current transport](https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http).

This is an attractive optional interoperability package. It is not a required internal plugin ABI and supplies no universal feed durability. Its HTTP authorization is a substantial scoped OAuth contract, while stdio obtains credentials from its launch environment. Older servers need explicit version compatibility. Model-generated sampling/input requests remain governed by Arco's configured participation rules. MCP tool annotations are advisory; “idempotent” is not evidence an uncertain upstream effect is safe to repeat.

**Existing gptme source**, pin `1f8d73638a25253fd0e1824c3d0a72ec93007582`: reread `gptme/plugins/entrypoints.py`, `registry.py`, and `tests/test_unified_plugins.py`. Disabled entry points are filtered before loading; the test asserts their loader was never called. Failed discovery is retained for diagnostics, including cached failures. Registries merge discovery paths and avoid double loading editable-install duplicates. [Source](https://github.com/gptme/gptme/tree/1f8d73638a25253fd0e1824c3d0a72ec93007582).

Transfer its *boundary*: metadata discovery before executable initialization, explicit activation, failure visible without breaking the coding engine. Do not transfer Python import/runtime machinery. Arco's native/Lua packages can advertise metadata in a small file; a connector process starts only when enabled and needed. Hot replacement should publish a new package capability revision at the configured workflow boundary, preserving attribution of in-flight results to the earlier version.

## Feed → triage → room/work semantics

1. **Capture:** bound raw bytes/metadata, verify source delivery as applicable, retain originals and source delivery identity. Commit ingress before a push acknowledgement. If capture capacity is exhausted, reject/retry or declare an explicit gap; no silent discard.
2. **Reconcile/normalize:** source package separates delivery duplicate from object revision. Retrying a webhook adds delivery evidence, not a second work item. A changed CVE/paper/repo is a new source revision attached to the same entity. Persist source-native cursor/ETag only after covered records are retained.
3. **Triage:** shared project profiles first filter exact tracked entities/categories, then optionally model-rank relevance. Keep the profile/ranker revision, cited source fields, rationale and uncertainty. Rejected originals remain recoverable; synopsis repair does not mutate source evidence.
4. **Route:** write a local HUD item, room proposal, or configured station job. Give each logical job a stable identity separate from delivery and execution attempts. Coalesce only under an explicit rule; “latest state per repository” is different from “every comment must be handled.”
5. **Work/settle:** record actual attempt result. A crash after dispatch but before confirmation leaves unknown effect. Reconcile upstream state when possible; make a new attempt only with evidence/idempotency semantics. Reattaching after RRC resumes backlog consumption, not blind side-effect replay.

Readable at-least-once intake with deduplication and explicit gaps is more honest than universal “exactly once.” A source delivery ID, a content revision, a triage decision and an outbound message each denote different facts. A durable ingress acknowledgement does not prove the model acted, and a model's summary does not prove an external write succeeded.

## Independent direct oracles for eventual implementation

- **Empty install/disabled package:** a fixture package initializer would write a marker or attempt a connection. Start ordinary Arco and assert neither effect occurs and no connector tools appear. Test enabled lazy activation separately; do not accept a mock registry count as proof no executable initialization happened.
- **Ingress crash/duplicate:** fixture delivers the same event twice with the same source ID, then a real revision with changed bytes. Kill/reopen before and after ingress commit/ack. Expected retained revisions and logical jobs come from the supplied fixture, including required gap/unknown state; not from the implementation's own counters.
- **Out-of-order delivery:** supply repo/CVE revisions in reverse order; final current view must match source revision semantics while all originals remain inspectable. Source without comparable ordering stays explicitly uncertain and reconciles.
- **Cursor/poll bounds:** script known pages, 304 responses, expired cursor, 429/Retry-After and mutation between pages. The oracle checks actual request sequence and exact retained identities; cursor advances only over committed covered data. Virtual-clock timing can verify quota behavior without sleeping for provider limits.
- **Unknown outbound effect:** fake upstream records a write then drops the connection. Reopen the client and assert no second write is blindly issued; known reconciliation or unresolved outcome is visible. This is independent of the inbound dedup test.
- **Thematic relevance:** predeclare known project inventory/topics and a small labeled source set with clear matches, irrelevant high-score items and ambiguous matches. Measure false dismissal/false escalation separately; ranking quality is not established by a crash-free UI. Exact query filtering and model relevance require different evidence.
- **MCP version/notification contract:** independently authored 2025 and 2026 fixtures distinguish GET/session versus POST/listen behavior, acknowledge a reduced filter and disconnect during a call. Unsupported notification and replay capability must not be invented.

## First useful slice

Discuss one optional `feeds` package with arXiv topic polling plus CISA KEV snapshot diff; both can be studied without account setup. It writes retained source items and local project triage/HUD entries through generic Arco facilities. A local research workflow acquires one relevant paper and compares it with our existing design; a vulnerability workflow checks actual inventory and writes a grounded assessment. Neither requires a broker, all-service connector bundle, hosted webhook receiver or model call for every fetched item. GitHub/HF watchlists and Slack room bridging follow when needed, with source-specific reconciliation and actual account qualification.

The UI's visual design and room behavior are other lanes. This report defines feed semantics and optional-package boundaries only; it does not select a UI/library or claim implementation readiness.
