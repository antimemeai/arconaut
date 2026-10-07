# phux as Arconaut's optional terminal substrate

Operator selected https://github.com/no-phux/phux, by phall, for general Arco
multiplexing, explicitly broader than colleague invocation. Studied immutable
0fd4e511621f50c70c857ef0300b90fcd34a295f, current workspace0.52.0.
Source/archive/restoration recorded in QUARANTINE.md. No dependency installed or
quarantined build/integration executed. Findings are source inspection, not a
runtime qualification or independent reproduction of upstream performance results.

## Recommendation and fit

Use phux as an optional independently running terminal service. Arco remains the
coding harness consuming it. This directly matches the operator's shared-service
boundary: many Arcos can consume one server, its lifetime is independent of a refit,
and the ordinary computing environment remains available to humans and models.

Keep Arco's C++ TUI as the program running in a phux Terminal. phux supplies splits,
detach/reattach, terminal history and remote attachment. The same pane can be observed
through CLI snapshots/watch without a second harness process holding its session.
Arcoboard remains another external program; a fleet arrangement can show operator
Arco, autonomous Arcos, Arcoboard and build/REPL shells in separate panes.
No libghostty/Rust dependency needs to enter Arco to consume the external CLI.

Initial automation seam should use explicit terminal/resource IDs and the structured
CLI, through ordinary audited tools/Lua. Avoid attaching a headless reader to resize
or steal operator focus. Long-running external shells/REPLs can remain in their own
terminals, usable by multiple Arcos subject to deliberate input arbitration.

## Actual mechanisms read

- README and docs/architecture/data-model.md: Terminal and AgentSession resources;
  grouping/layout/focus are client conventions. Server PTYs and clients separate.
- docs/consumers/agents.md: bounded snapshots, input paste versus submit semantics,
  watch/wait, retained process exits and event cursors. Snapshot/wait do not attach or
  resize. Event streams have loss/reconnect caveats; current fleet wait is local,
  satellite targets must be waited on at their own server.
- docs/consumers/harness.md: emit lifecycle directly; stream outranks detector state.
  Identity-only metadata fallback; capability probe RESOURCE_KINDS before relying on
  AgentSession. Separate open creates a separate stream; no producer deduplication.
- crates/phux-server/src/resource/agent_session/{mod,ring}.rs: append validation,
  sequence/time stamping, bounded64-message mailboxes, byte-limited record FIFO with
  explicit dropped count, sealed graceful-upgrade cut, ended-session refusal.
  This is a live projection with bounded retention, not Arco's immutable audit.
- docs/consumers/claude.md: existing identity/attention plugin and newer shim lifecycle
  events differ by release. Avoid claiming every installed shim emits native streams.
- docs/architecture/transport.md and docs/remote-access.md: shared framed transport,
  local Unix socket, SSH enrollment followed by QUIC/direct attachment, certificate
  and service configuration. Detach preserves live jobs; server crash/reboot does not.
- docs/spec/L1.md and agents.md: input lease, operation IDs and keyed creates/signals
  address a different question from inference completion. Preserve unknown delivery;
  server incarnation changes require re-observation. A paste acknowledgment is not a
  completed coding action or a semantic conversation message acknowledgment.

## Arco integration shape

1. Launch unmodified scripts/arco in a phux pane. Its existing RRC script keeps the
   conversation persistent independently of terminal detach. Give other Arcos unique
   sessions/panes. Do not restart or kill the shared phux server during Arco refit.
2. Add a small optional lifecycle adapter keyed by PHUX_TERMINAL_ID/PHUX_SOCKET.
   Emit prompt, tool_start/tool_end, stop/interruption, restart and session_end using
   the appropriate existing event vocabulary. Include compact audit/session/turn
   references, not duplicate provider streams, full context or tool outputs.
   Keep emission bounded; phux outage must not turn into a provider/tool retry.
3. Arcoboard may consume explicit lifecycle streams/resources to show fleet state.
   Its accepted-source/disposition/task fields still come from the work controller;
   screen idleness and a phux done event cannot certify a unit or settle remote work.
4. Expose snapshot/paste/send/wait to the model's Lua orchestration. Coordinate shared
   input using explicit target/lease semantics. Preserve paste versus Enter intent;
   these are ordinary operator/model tools, not routine approval gates.
5. Explore direct Neuroses terminal access over existing SSH/Tailscale after local
   use works. Remote setup changes per-user service/auth state; choose explicitly.

This can supply substantial early multiplayer experience: shared views and terminal
control among humans and agents. Arco-to-Arco addressed messages, selected context,
file ownership and campaign coordination still need their semantic contract. phux's
remote transport is not by itself qualification of the whole planned E2EE peer
protocol; study its exact trust endpoint rather than inventing cryptographic claims.

## Small next experiment

Propose an optional external install and local private test server; discussion here
is not an installation instruction. Exercise one Arco TUI pane plus Arcoboard and a
shell: detach/reattach preserves the live Arco session; model snapshot preserves human
geometry; paste plus explicit submit arrives once; a normal Arco RRC keeps the pane
and conversation. Then test explicit lifecycle events and one dropped/reconnected
reader, retaining loss indicators. Bound this to practical integration checks, no
new platform/certification campaign. Measure actual idle memory/redraw and retained
history costs at that boundary; upstream reported benchmarks are not our results.

Upstream performance.md records two heavily loaded-host runs with ranking reversals,
separate server/client RSS and unmeasured GUI boundaries. It supports designing our
measurement, not advertising universal speed. Initial external process use avoids
turning each Arco installation into a terminal toolkit distribution.
