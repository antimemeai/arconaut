# Fresh startup comparison — 2026-10-08

500/500 recorded launches reached the configured input frame and displayed
the same unsubmitted input marker. Each tool has 100 observations, matching the
fresh population in the earlier Blackbird trial. Blackbird was measured again in
the same serial rotation, rather than borrowing its earlier measurements.

The shared endpoints are the completed input frame and then a completed frame
containing `BB_BENCH_READY` in the input widget. The marker is never submitted.
The earlier Blackbird workflow-registry command is a different endpoint and is
not used for the cross-tool comparison. No inference prompts were submitted;
startup metadata/background network work is allowed. This is not an offline run.

## Method and installed configurations

Owned development driver: [scripts/benchmark-agent-startup](../scripts/benchmark-agent-startup).
Parent monotonic clock starts before process/launcher spawn. Serial round robin,
110×35 PTY, Ghostty identity, terminal queries answered, bytes drained. Each process
uses private APFS copies of authentication/onboarding/model-catalog seeds in an
empty, previously trusted external workspace. Preparation and cloning are excluded.
OS caches are ordinary and unflushed. User extensions are absent from these seeds;
these are isolated baseline sessions, not every operator customization. No heavy
or weird history-reopen comparison is attempted across incompatible formats.

- Blackbird: published Release/OFF wrap-up generation, native SHA256
  `47282d5c978dbed9c4a7eb6e01a1aa0c30e810910b63a1f91e011ea882a88d99`.
  Ordinary launcher, fresh empty session, profiler off, first completed `Enter send` frame.
- Codex: installed 0.161.0 with `--no-daemon`, so a local server starts for each process;
  the operator's existing server is not reused. Minimal config, existing auth/model
  catalog, GPT-6.1-Sol. Input-frame endpoint requires the configured model label
  as well as the input placeholder; its earlier loading splash is excluded.
  Installed help supplies the exact supported flags; [official developer commands](https://learn.chatgpt.com/docs/developer-commands?surface=cli)
  provide the accompanying CLI reference.
- Claude Code: installed 2.1.163, isolated config/onboarding cache and existing macOS
  authentication, no enabled plugins. Auto-update and nonessential traffic disabled.
  Header reports Opus 4.8. Complete prompt/shortcuts frame is the input endpoint.
- Hermes: installed for this measurement from retained upstream source
  `a3ed4a173070e981332e4d879ff6cc8b9efd57ab`; display reports
  `vunknown (2026.9.24)`. Python 3.14.3, frozen core `uv.lock` dependencies, standard
  classic CLI, normal bundled tool/skill discovery, OpenRouter `claude-sonnet-4.6`.
  Shared telemetry declined in the private profile. Prompt/status-line frame ending
  with cursor visibility is the input endpoint. Modern Hermes TUI is not measured.
- OpenCode: installed 1.15.13, minimal isolated XDG directories, existing auth and
  model catalog, no external plugins, auto-update disabled. Explicit OpenRouter
  `anthropic/claude-sonnet-4.6`; fresh database/schema initialization included.
  Complete home prompt/commands/version frame is the input endpoint.

Frame boundaries are synchronized-update end for Blackbird/Codex/Claude/OpenCode,
and cursor-show for the classic Hermes input widget. The bounded VT text observer
tracks cursor moves so input split over several writes is recognized in its screen
row, rather than concatenating unrelated footer text. Retained raw PTY bytes are
available for inspecting those observations; this observer does not render graphics.
Nearest-rank percentiles; every observation in the selected population is retained.

## Completed configured input frame from spawn (ms)

| Tool | p30 | p50 | p70 | p90 | max |
|---|---:|---:|---:|---:|---:|
| Blackbird | 98.931 | 102.231 | 105.771 | 115.428 | 210.033 |
| Codex | 1265.395 | 1369.478 | 1616.598 | 4562.046 | 7555.864 |
| Claude Code | 307.764 | 346.842 | 392.716 | 558.986 | 3847.130 |
| Hermes classic CLI | 4953.665 | 5227.299 | 6145.219 | 7665.620 | 9520.175 |
| OpenCode | 3399.774 | 3794.499 | 4224.717 | 4608.243 | 9277.945 |

## Unsubmitted input marker visible from spawn (ms)

| Tool | p30 | p50 | p70 | p90 | max |
|---|---:|---:|---:|---:|---:|
| Blackbird | 100.416 | 103.807 | 107.365 | 116.905 | 214.730 |
| Codex | 1316.076 | 1436.459 | 1659.862 | 4598.573 | 7600.149 |
| Claude Code | 337.621 | 385.113 | 429.971 | 638.103 | 3988.797 |
| Hermes classic CLI | 5007.837 | 5303.576 | 6222.462 | 7708.950 | 9621.021 |
| OpenCode | 3496.232 | 3896.906 | 4311.681 | 4798.681 | 9603.877 |

## Input marker response after typing (ms)

| Tool | p30 | p50 | p70 | p90 | max |
|---|---:|---:|---:|---:|---:|
| Blackbird | 0.624 | 0.721 | 0.902 | 1.297 | 2.551 |
| Codex | 39.930 | 42.942 | 49.587 | 64.988 | 176.607 |
| Claude Code | 30.317 | 36.317 | 37.511 | 65.960 | 228.083 |
| Hermes classic CLI | 47.896 | 65.252 | 72.765 | 83.062 | 139.473 |
| OpenCode | 76.205 | 86.619 | 100.551 | 120.164 | 324.579 |

## Shutdown and retained observations

Startup/input success is separate from shutdown. After recording the marker, the
driver sends local quit keystrokes, drains for up to three seconds, then terminates
its own private process group if needed. Shutdown time is excluded from the listed
endpoints.

| Tool | Recorded | Endpoint success | Forced shutdown | Nonzero exit |
|---|---:|---:|---:|---:|
| Blackbird | 100 | 100 | 0 | 0 |
| Codex | 100 | 100 | 100 | 100 |
| Claude Code | 100 | 100 | 0 | 0 |
| Hermes classic CLI | 100 | 100 | 0 | 0 |
| OpenCode | 100 | 100 | 0 | 0 |

The initial driver ceiling was 300 per tool. The population was reduced to 100
to match the prior fresh population and avoid an unnecessary longer run. External
stop occurred after that population completed. Any extra completed observations
(0) or interrupted tail launch (1) are recorded separately,
not silently removed or pooled into the selected percentiles.

An earlier method-development batch was stopped after a stream-text matcher missed
a marker that Codex had displayed over separate cursor updates. Those records,
including the observer timeout, remain in the public numeric artifact as excluded
preliminary observations, and all private raw terminal records remain on disk.
The corrected observer's serial pilot and the earlier shutdown pilots are also
preserved separately. No retries replace observations in the selected population.

Host load before the selected run: [3.552734375, 5.71044921875, 6.96240234375].
Host load at summary: (13.00439453125, 14.3623046875, 13.2333984375).
These are observed distributions for the listed configurations on this host;
they do not attribute differences to a runtime, establish inference/coding latency,
or describe every tool configuration. In particular, Codex includes its own local
server here, and Hermes is the classic CLI source installation named above.

Public [numeric results](../docs/measurements/agent-startup-comparison-2026-10-08.json)
contain every selected sample, summaries, tool/driver identities, preliminary
observations and any interrupted tail. Private noncredential seeds, logs and terminal transcripts
remain under ignored `context/agent-startup-comparison-2026-10-08/`. Retained
quarantine sources and the earlier Blackbird 300-start reports were not modified.
Temporary credential copies were removed after the run; original account files remain untouched.
