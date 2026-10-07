# Conversation renderer implementation

2026-10-07. Implement CHAT_PRESENTATION_PROPOSAL with explicit typed conversation
events, cached styled blocks, viewport-sized reusable cells, damage output and
clock-driven motion. Preserve composer, command behavior, audit retention and
plain mode. Reference: rendering-internals-study (Notcurses/libvterm/FTXUI) and
terminal-ux-study (agent message/Markdown/tool patterns).

Direct oracles: typed role/diagnostic separation; cached-block reflow counts;
incremental/full terminal-cell agreement including Unicode, resize and stale areas;
same-frame zero output; animation-only unchanged conversation work; actual PTY
keyboard/cancel/recovery. Measure bytes and render latency on fixed local fixtures.
One adversarial tribunal round after completed implementation, covering native
memory/ABI, terminal recovery and presentation/performance. Fix valid findings
and run targeted rechecks; no second tribunal. Blackbird rename remains pending
visual acceptance; this implementation retains existing executable/session paths.

First implementation unit complete: papers/2026-10-07-chat-review-disposition.md
records the sole tribunal, remedies, measurements and limitations. Final renderer
recheck4/4 in11.34s; prior full release42/42; scoped ASan passes. New renderer
clang-tidy clean. Whole-repository rigor remains blocked by recorded older lint
debt (arconaut-lh1). Full proposal acceptance and rename are still pending.
