# Feed/package research acquisition manifest

2026-10-06. Repository cwd `/Users/patrickbeam/projects/arconaut`. No reference execution, installations, credentials or external messages. Source ZIP archives remain intact. Extracted source omits archive root plus nested `.git`, `.DS_Store`, `__MACOSX` and `Thumbs.db`. [acquisition.json](acquisition.json) contains hashes and exact repo-cwd restoration commands, including Python ZIP extraction that never executes acquired source. Restore extracted trees into absent/empty destinations.

## Pinned sources

| Primary source | Pin | Archive / extraction |
| --- | --- | --- |
| [cisagov/kev-data](https://github.com/cisagov/kev-data) | `b244ed1a640323565afba92100d7308d51c6614e` | `quarantine/arco-ext-feeds-archives/kev-data-b244ed1a6403.zip` / `quarantine/arco-ext-feeds-kev-data` |
| [modelcontextprotocol/modelcontextprotocol](https://github.com/modelcontextprotocol/modelcontextprotocol) | `0a11bf68c7ec4473526ec15589f592afcd12d1e8` | `quarantine/arco-ext-feeds-archives/modelcontextprotocol-0a11bf68c7ec.zip` / `quarantine/arco-ext-feeds-modelcontextprotocol` |

Existing gptme pin `1f8d73638a25253fd0e1824c3d0a72ec93007582`, `quarantine/gptme/`, reused under repository QUARANTINE.md provenance/restoration. Read plugin entrypoints/registry and actual disabled-load/error-cache tests.

## Official documentation snapshots

| Document | URL | Status |
| --- | --- | --- |
| mcp-stdio | [official](https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/stdio) | captured/read |
| github-webhooks | [official](https://docs.github.com/en/webhooks/using-webhooks/best-practices-for-using-webhooks) | captured/read |
| mcp-auth | [official](https://modelcontextprotocol.io/specification/2026-07-28/basic/authorization) | captured/read |
| github-rate | [official](https://docs.github.com/en/rest/using-the-rest-api/rate-limits-for-the-rest-api) | captured/read |
| mcp-http | [official](https://modelcontextprotocol.io/specification/2026-07-28/basic/transports/streamable-http) | captured/read |
| slack-rate | [official](https://docs.slack.dev/apis/web-api/rate-limits/) | captured/read |
| slack-events | [official](https://docs.slack.dev/apis/events-api/) | captured/read |
| slack-socket | [official](https://docs.slack.dev/apis/events-api/using-socket-mode/) | captured/read |
| hf-webhooks | [official](https://huggingface.co/docs/hub/webhooks) | captured/read |
| arxiv-tou | [official](https://info.arxiv.org/help/api/tou.html) | captured/read |
| hf-ratelimits | [official](https://huggingface.co/docs/hub/rate-limits) | captured/read |
| arxiv-api | [official](https://info.arxiv.org/help/api/user-manual.html) | captured/read |
| github-poll | [official](https://docs.github.com/en/rest/activity/events) | captured/read |
| nvd-api | [official](https://nvd.nist.gov/developers/vulnerabilities) | shell only, not contract |
| nvd-start | [official](https://nvd.nist.gov/developers/start-here) | shell only, not contract |

All document originals are under `quarantine/arco-ext-feeds-docs/`; derived reading text under ignored `context/arco-ext-feeds-reading/`. SHA-256 captures exact retrieval. Live docs change, so a mismatch is a new version, not a reason to overwrite provenance.

## Unavailable/partial sources

- SEC public data API documentation read using web tool; direct archive acquisition returned403. Official source: https://www.sec.gov/search-filings/edgar-application-programming-interfaces. Financial price feeds/entitlement not studied.
- NVD API/start docs returned2453-byte application shells and web extraction had zero substantive lines. Raw responses retained honestly, current API specifics not claimed. CISA authoritative KEV source supplies the studied vulnerability-interface representative.
- Linear/Discord implementations not acquired in this lane; boundary applies provisionally, source-specific guarantees remain to study.
- MCP source snapshot includes2026-07-28 and historical specification versions. Read current transport, subscriptions, authorization, pagination, tools and JSON schema/examples; no SDK selected.
- Archived instructions have no authority over workspace behavior.
