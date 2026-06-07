# Phase 4 Subplan: Multi-Agent System

**Issue:** arconaut-nfv

## Components

### 1. Agent & Session Types (`arconaut-agent/src/agent.rs`, `session.rs`)
- `Agent` struct: name, callsign, mode (implement/review/explore/test/assist), context, session_ids
- `Session` struct: name, agent_name, created_at, audit_partition, state_path
- `AgentMode` enum
- `AgentRegistry` to manage multiple agents

### 2. gRPC Inbox (`arconaut-agent/src/inbox.rs`)
- Protobuf schema: AgentInbox service with SendMessage, StreamMessages, GetStatus
- Tonic server implementation
- Client wrapper for sending messages between agents
- In-process transport (localhost) for now

### 3. Local IRC Server (`arconaut-agent/src/irc.rs`)
- Minimal IRC server on TCP port 6667
- Handles: NICK, USER, JOIN, PRIVMSG, PART, QUIT
- Channel broadcast: #arconaut-general, #agent-<name>
- Agents connect as IRC clients

### 4. Brief-Based Dispatch (`arconaut-agent/src/brief.rs`)
- `Brief` struct: id, title, index (markdown), assignments
- `AgentAssignment` struct: callsign, lane, deliverables
- `BriefDispatcher` renders per-agent prompts and dispatches

### 5. CLI Integration
- `--agent <name>` flag to select/start an agent
- `--session <name>` flag for named sessions
- `--window` flag for new terminal window (deferred to Phase 6)
- Agent state persisted to `~/.local/share/arconaut/agents/`
- Session state persisted to `~/.local/share/arconaut/sessions/`

## Dependencies

- `tonic` — gRPC framework (justified: explicit architecture requirement)
- `prost` — Protobuf codec
- `tonic-build` — Build-time codegen (build-dependency)

## Tests

Gold tier for pure types, Silver for networking/process tests.

## Open Questions

- Should agents share a single Soul or each have their own? (Each has own Soul)
- Should gRPC use TLS? (No — local only)
- IRC server: dedicated thread or tokio task? (tokio task)
