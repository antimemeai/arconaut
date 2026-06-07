# Phase 4 Conformance Specification

**Issue:** arconaut-nfv

---

## 1. Agent & Session Types

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.1 | `Agent` can be created with all modes | `agent::tests::create_all_modes` | Gold |
| 1.2 | `Session` has valid audit partition path | `session::tests::audit_partition` | Gold |
| 1.3 | `AgentRegistry` lists and retrieves agents | `agent::tests::registry_lookup` | Gold |

## 2. gRPC Inbox

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 2.1 | `SendMessage` round-trips | `inbox::tests::send_message` | Silver |
| 2.2 | `GetStatus` returns online state | `inbox::tests::get_status` | Silver |
| 2.3 | Stream receives pushed messages | `inbox::tests::stream_messages` | Silver |

## 3. IRC Server

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 3.1 | Server accepts TCP connection | `irc::tests::connect` | Silver |
| 3.2 | NICK/USER registration works | `irc::tests::register` | Silver |
| 3.3 | PRIVMSG broadcasts to channel | `irc::tests::broadcast` | Silver |
| 3.4 | JOIN creates/participates in channel | `irc::tests::join_channel` | Silver |

## 4. Brief Dispatch

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 4.1 | `Brief` renders per-agent prompt | `brief::tests::render_prompt` | Gold |
| 4.2 | Dispatcher assigns correct lanes | `brief::tests::lane_assignment` | Gold |
| 4.3 | Deliverables tracked | `brief::tests::deliverables` | Gold |

## 5. CLI Integration

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 5.1 | `--agent` flag parsed | `cli::tests::agent_flag` | Gold |
| 5.2 | `--session` flag parsed | `cli::tests::session_flag` | Gold |
| 5.3 | Agent state persisted and loaded | `cli::tests::persist_agent` | Silver |

---

## Oracle Tiers

- **Gold:** Deterministic, no external deps, run in CI
- **Silver:** Requires network I/O or process spawning

All Gold tests must pass. Silver tests run on-demand.
