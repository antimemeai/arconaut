# phux support

[phux](https://github.com/no-phux/phux), by phall, is Arconaut's officially
supported external terminal multiplexer. It runs as a separate service; Arco
runs its ordinary C++ TUI inside a pane. No terminal library or phux runtime is
bundled into Arco's core. Ordinary launch still works without phux.

## Start and return

Install the official binaries if needed:

```sh
curl -fsSL https://phux.sh/install | sh
phux --version
```

Tested locally with official macOS arm64 **phux0.52.0**; installer verified the
release archive checksum and installed phux/phux-mcp in ~/.local/bin.

From your project working directory:

```sh
/Users/patrickbeam/projects/arconaut/scripts/arco-phux arco --session context/my-arco
```

The first argument names the phux session. Remaining arguments go to scripts/arco
literally; current working directory remains the tool working directory.
The launcher creates a new named session, then attaches. Existing name refuses;
return to that session with `phux attach arco`. It inherits PHUX_SOCKET/PHUX_PROFILE
if you deliberately choose another local server.

- `Ctrl-A`, then `%`: split side by side.
- `Ctrl-A`, then `d`: detach, leaving the agent running.
- `phux attach arco`: reattach to the same running pane.
- `/restart NOTE`: Arco's existing RRC; keeps the pane and retained conversation.
- `/quit` or `/exit`: quit Arco. Detach is how to leave it running.

Each concurrent Arco needs its own --session directory. Sharing a phux server
never makes sharing a writable Arco audit safe. Model/provider requests and tools
still belong to their harnesses. Individual Arco restarts do not restart phux.

## Human and model terminal tools

```sh
phux ls --json
phux snapshot --json @1
phux paste @1 'text to insert'
phux send-keys @1 Enter
phux watch --json --timeout 30 @1
```

Select the actual target ID from the current listing rather than assuming @1.
Snapshot/watch observe without attaching or resizing. Paste inserts; Enter submits.
Ordinary audited exec/Lua workflows can call these external tools. For another
program beside a chosen pane, use `phux spawn --target @ID --cwd PATH -- PROGRAM`;
Arcoboard remains a separate program, e.g. `arco-board --fleet`.

Screen text is observation. It is not an inference result, an audit, or proof an
effect completed. `wait --output-only` requires OSC-133 shell integration; Arco's
TUI doesn't provide shell marks, so use a distinct observed output or explicit
lifecycle integration rather than matching a word echoed from the submitted prompt.

## Verified boundary and follow-up

Private local test profile/socket, fresh retained Arco session: actual model turn
returned PHUX_READY; independent snapshots kept52x22 geometry. Detach and reattach
kept wrapper PID8463. Actual `/restart NOTE` replaced native8467 with9391, kept
wrapper8463, same terminal and byte-identical session-info, retained reply visible.
Two ordinary provider turns only; no source modification by model. Test server
stopped explicitly on its private socket, not the operator/default server.

First naked `phux new -- COMMAND` cold launch seeded a default shell instead of
requested Arco. The launcher therefore uses the documented atomic `new --json`
create path, then attach; actual cold launch above worked. Initial `/restart`
without NOTE was correctly rejected; the check used documented `/restart NOTE`.
A separate optional `phux run` shell probe timed out10s; its command-completion
behavior is unresolved, not claimed qualified by the TUI test. Capture paths
context/phux-support. No third assurance campaign.

Native Arco lifecycle emission, a phux-specific fleet view, remote enrollment and
multiplayer protocol integration are subsequent work. They are not implemented
by this launch support. phux's bounded live record history complements Arco's
complete retained audit. Source/fit discussion: papers/2026-10-07-phux-multiplexing.md.
