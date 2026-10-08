# Eight-tool fresh startup comparison — 2026-10-08

[Full SVG chart](../docs/measurements/agent-startup-total-2026-10-08.svg) · [PNG chart](../docs/measurements/agent-startup-total-2026-10-08.png) · [Numeric results](../docs/measurements/agent-startup-total-2026-10-08.json)

![Startup percentiles for eight tools](../docs/measurements/agent-startup-total-2026-10-08.png)

The operator requested bare Pi, Kimi Code and Oh My Pi after the five-tool comparison. 100 additional fresh starts per tool use the same unchanged PTY driver and endpoints. The combined chart has 800 selected launches across eight tools. Batch A is the earlier Blackbird/Codex/Claude/Hermes/OpenCode rotation; batch B is the added Pi/Kimi/Oh My Pi rotation. These were separate runs, not an eight-way simultaneous rotation. No inference prompts were submitted.

## Versions and selected models

| Tool | Version/build used | Runtime recorded | Selected model | Batch |
|---|---|---|---|:---:|
| Blackbird | Published Release/OFF wrap-up executable; native SHA-256 below | Native C++ build; profiler off | No provider request used | A |
| Claude Code | 2.1.163 | Installed executable | Opus 4.8 | A |
| Bare Pi | `@earendil-works/pi-coding-agent` 1.1.0 | Node 24.7.0 | OpenRouter `anthropic/claude-sonnet-4.6` | B |
| Kimi Code | 2.1.1 | Installed arm64 Mach-O executable | OpenRouter `anthropic/claude-sonnet-4.6` | B |
| Codex | 0.161.0 | Installed executable and fresh local server | GPT-6.1-Sol | A |
| Oh My Pi | `@oh-my-pi/pi-coding-agent` 18.8.4 | Bun 1.4.2 | OpenRouter `anthropic/claude-sonnet-4.6` | B |
| OpenCode | 1.15.13 | Installed executable | OpenRouter `anthropic/claude-sonnet-4.6` | A |
| Hermes classic | Source `a3ed4a173070e981332e4d879ff6cc8b9efd57ab`; reports `vunknown (2026.9.24)` | Python 3.14.3; frozen core `uv.lock` install | OpenRouter `anthropic/claude-sonnet-4.6` | A |

Blackbird's native executable SHA-256 is
`47282d5c978dbed9c4a7eb6e01a1aa0c30e810910b63a1f91e011ea882a88d99`.
Its launcher hash is recorded separately in the numeric metadata. The metadata
also records each tool's entry executable hash; Pi and Oh My Pi entries are script
files, so their installation lock/runtime hashes are recorded in the
[added-tool artifact](../docs/measurements/agent-startup-pi-kimi-omp-2026-10-08.json).
There is no invented Blackbird release number or Hermes package version.

## Exact test design

The unchanged owned [PTY driver](../scripts/benchmark-agent-startup) has SHA-256
`4d3225532bae81ebe2d0cf6723d96d4e8d7905c65e80c975107cb1e5f8e0b090`
in both batches. The driver, rather than the tested application, reads the parent
monotonic clock. This measures terminal output observed by the PTY consumer;
it does not time pixels painted by a running Ghostty window.

### Population and order

- **A:** Blackbird → Codex → Claude Code → Hermes → OpenCode, serial round robin,
  100 selected launches each. Start `2026-10-08T10:07:31Z`; selected population
  complete `2026-10-08T10:35:10Z`. Driver source commit
  `3f8ed57f49b313336f950e860e63339b7e9667bf`.
- **B:** Pi → Kimi → Oh My Pi, serial round robin, exactly 100 launches each.
  Start `2026-10-08T13:40:00Z`; finish `2026-10-08T13:50:26Z`.
  Driver source commit `7b876e0a2ceacf1922b3c270f23c02d46ea0e55f`.
- Each process starts and completes its shutdown/cleanup before the next launch.
  The combined table contains 800 selected launches; the eight tools were not
  interleaved together. Pilots and interrupted observations are separate.

### Preparation and terminal

Each launch receives a fresh run directory and APFS `clonefile` copies of its
prepared private profile seeds. Clone/preparation time is excluded. The working
directory is the same empty external workspace in both batches:
`/private/var/folders/nr/8bhjl__j67bbctrbn87yjt7w0000gn/T/blackbird-startup-comparison-q8hzbgku`.
Trust/onboarding and credentials are prepared before timing. Session history is
empty; tool-created database/schema initialization remains inside the timed launch.
Existing authentication and model/banner caches may be seeded as described below.
This is fresh application state with ordinary unflushed macOS caches, not a cold
OS-cache experiment. Background/startup networking is permitted.

The driver allocates a **110-column × 35-row PTY** and sets
`TERM=xterm-256color`, `TERM_PROGRAM=ghostty`, `LANG=en_US.UTF-8`.
It inherits the other parent environment values except the removals below and the
per-tool overrides in the adapter section. `HOME` remains the operator's home;
the isolated application-directory overrides do not isolate the entire OS account.
Each child starts in its own process group with the PTY connected to stdin/stdout/stderr.

The consumer drains up to 65,536 bytes per read, using 50 ms `select` waits.
It answers DSR (`CSI 6 n` → `CSI 1;1 R`), primary DA (`CSI c` → `CSI ?1;2 c`),
secondary DA (`CSI > c` → `CSI >0;0;0 c`), Kitty keyboard query
(`CSI ?u` → `CSI ?0u`), and OSC 10/11 color queries (white foreground, black
background). Split terminal queries are retained across reads. These replies are
part of the benchmark terminal environment, rather than queries to a real emulator.

### Timing endpoints and input check

1. The start timestamp is `time.monotonic_ns()` immediately before `Popen`.
2. After each PTY read, the driver timestamps the observation and checks the
   accumulated transcript through its latest configured frame boundary. It strips
   CSI/OSC sequences and requires **all** case-sensitive strings listed in the
   adapter table below. This is a tool-specific configured-input-frame predicate;
   strings need not all have arrived in the same read. Earlier loading screens
   that lack the required strings do not satisfy it.
3. `input_frame_ms` is that observation timestamp minus the spawn timestamp.
   The driver then checks that kernel terminal echo is off; an ordinary echoing
   shell or booting process cannot pass the typing check.
4. It timestamps the write and sends the exact bytes `BB_BENCH_READY`, **without
   Enter**. It does not send a model prompt or the earlier Blackbird registry query.
5. A small cursor-aware VT text observer must find the whole marker in a committed
   screen row, and the new transcript suffix must be nonempty after the driver's
   completed-text processing.
   `input_echo_ms` runs from spawn to that observation; `input_response_ms` runs
   from the marker write to that observation. The observer handles cursor movement
   and text/erasure, not full graphics rendering. Raw emitted PTY bytes remain the
   underlying record.

The frame boundary is synchronized-update end (`CSI ?2026 l`) for every tool
except Hermes classic, whose boundary is cursor-show (`CSI ?25 h`). The same
boundary commits the marker observer. A **30-second deadline**, starting before
spawn, covers both endpoints together; total terminal output is capped at 16 MiB.
Exit, timeout, still-enabled kernel echo, or missing marker is recorded as failure.

### Adapter commands, environment and readiness strings

`{run}` means that launch's private directory. Command basenames below denote
the exact installed paths retained in the combined artifact's batch metadata.
The driver passes those absolute entry paths to `Popen`; script entrypoints still
resolve their interpreter through their shebang and the inherited `PATH`.

| Tool | Exact arguments after executable | All required readiness strings |
|---|---|---|
| Blackbird | `--session {run}/session` | `Enter send` |
| Codex | `--no-daemon` | `Ask Codex to do anything`; `GPT-6.1-Sol` |
| Claude Code | None | `Claude`; `Code`; `shortcuts`; `❯` |
| Hermes | `chat` | `ctx --`; `❯` |
| OpenCode | `--model openrouter/anthropic/claude-sonnet-4.6` | `Ask anything`; `commands`; `1.15.13` |
| Pi | `--provider openrouter --model anthropic/claude-sonnet-4.6 --no-extensions --no-skills --no-mcp --no-prompt-templates --no-themes --no-context-files` | `v1.1.0`; `anthropic/claude-sonnet-4.6`; `ctrl+c/ctrl+d clear/exit` |
| Kimi | `--skills-dir {run}/session` | `Welcome to Kimi Code!`; `Version:`; `2.1.1`; `context: 0%`; `anthropic/claude-sonnet-4.6` |
| Oh My Pi | `--model openrouter/anthropic/claude-sonnet-4.6 --no-extensions --no-skills` | `v18.8.4`; `Sonnet 4.6`; `╰─`; `thinking effort` |

| Tool | Environment overrides |
|---|---|
| Blackbird | None beyond the shared terminal environment |
| Codex | `CODEX_HOME={run}/codex` |
| Claude Code | `CLAUDE_CONFIG_DIR={run}/claude`; `DISABLE_AUTOUPDATER=1`; `CLAUDE_CODE_DISABLE_NONESSENTIAL_TRAFFIC=1` |
| Hermes | `HERMES_HOME={run}/hermes` |
| OpenCode | `XDG_CONFIG_HOME={run}/opencode/config`; `XDG_DATA_HOME={run}/opencode/data`; `XDG_CACHE_HOME={run}/opencode/cache`; `XDG_STATE_HOME={run}/opencode/state`; `OPENCODE_DISABLE_AUTOUPDATE=1` |
| Pi | `PI_CODING_AGENT_DIR={run}/pi` |
| Kimi | `KIMI_CODE_HOME={run}/kimi`; `KIMI_DISABLE_TELEMETRY=1`; `KIMI_CODE_NO_AUTO_UPDATE=1` |
| Oh My Pi | `PI_CODING_AGENT_DIR={run}/omp` |

Both batches remove `CLAUDECODE`, `CLAUDE_CODE_ENTRYPOINT`, `CODEX_THREAD_ID`,
`BLACKBIRD_EXECUTABLE` and `ARCO_EXECUTABLE` from the inherited environment.
Batch B additionally removes `CLAUDE_CONFIG_DIR`, `PI_OFFLINE`, `PI_PROFILE`,
`OMP_PROFILE`, `PI_CODING_AGENT_DIR` and `KIMI_CODE_HOME` before applying overrides.

### Shutdown and aggregation

After recording the marker, local quit bytes are sent as follows. Hexadecimal
`11` is Ctrl-Q, `15` is Ctrl-U, `03` is Ctrl-C and `04` is Ctrl-D.

| Tool | Quit writes |
|---|---|
| Blackbird | One write: `11` |
| Codex | Three separate writes: `03`, `03`, `03`, at least 150 ms apart |
| Claude Code / OpenCode | One write: `150303` |
| Hermes | One write: `1504` |
| Pi / Kimi / Oh My Pi | Three separate writes: `15`, `03`, `03`, at least 150 ms apart |

The consumer drains output for up to three seconds. Remaining children are marked
as forced shutdowns; cleanup sends SIGTERM to the private process group, waits up
to two seconds, then sends SIGKILL if needed and waits up to two seconds. Shutdown
is outside the three startup/input endpoints. The transcript is saved and the
private per-launch profile is removed, including on failure.

For each tool and endpoint, sort successful endpoint values and take element
`ceil(p × n) − 1` (zero-based) for p30/p50/p70/p90. With 100 successful records,
these are the 30th, 50th, 70th and 90th observations. No interpolation, trimming
or replacement retries are used. All 800 selected records succeeded; shutdown
outcomes are reported separately below. Batch A's initial 300/tool ceiling was
stopped after the selected 100/tool population; its one interrupted tail and
earlier observer-development records remain separately identified in the artifacts.

The driver invocation is `scripts/benchmark-agent-startup SPEC_JSON OUTPUT --count N`.
For A, N was initially 300 and the selected population was externally stopped at
100/tool; for B, N was 100. Repeating the final population directly uses N=100.
Credentials and private onboarding/cache contents are not published and temporary
credential copies were removed. Reproduction requires preparing equivalent private
seeds; the public metadata records commands, versions, identities and endpoint
predicates, rather than promising identical private cache contents.

## Profile configurations

- Blackbird: published Release/OFF launcher and fresh empty session, profiler off.
- Codex: minimal config with existing authentication and model catalog; no user
  hooks, MCP servers or plugins. `--no-daemon` starts a separate local server for
  every trial instead of reusing the operator's server. The readiness predicate
  includes the configured model label, excluding its earlier loading splash.
- Claude Code: copied onboarding/config cache and existing macOS authentication;
  no enabled plugins. Automatic updates and nonessential traffic disabled.
- Hermes: isolated retained-source installation using frozen core dependencies,
  classic CLI and normal bundled tool/skill discovery. Telemetry declined in its
  private profile. The newer Hermes TUI is not part of this measurement.
- OpenCode: minimal isolated XDG config, copied authentication and model catalog,
  no external plugins, fresh database/schema initialization, auto-update disabled.

- Bare Pi 1.1.0: isolated npm installation of `@earendil-works/pi-coding-agent@1.1.0` with `--ignore-scripts`; Node 24.7.0. Standard fullscreen TUI and built-in tools, with extensions, skills, MCP, prompt templates, external themes and context-file discovery disabled. Normal startup networking remains enabled; no offline flag. Private auth/settings seed and fresh session state per launch. Input endpoint requires version, model and composer help.
- Kimi Code 2.1.1: installed arm64 Mach-O executable, current CLI command family. Private `KIMI_CODE_HOME`, minimal OpenAI-compatible OpenRouter provider configuration, previously accepted folder trust and existing banner/client caches; explicit empty skills directory, update preflight and telemetry disabled. Expired operator OAuth credentials were not used in repeated launches. Input endpoint requires the configured welcome/model/version and context line. Kimi lazily creates a session on the first submitted message, so typing the unsubmitted marker does not create a model turn.
- Oh My Pi 18.8.4: isolated Bun package installation of `@oh-my-pi/pi-coding-agent@18.8.4`; Bun 1.4.2. External extensions and skills disabled, normal built-in machinery retained. Private config/model credentials, onboarding version 2 prepared beforehand, update checks disabled. Fresh database/catalog initialization is included. Default `startup.showSplash: false`; normal welcome logo animation is retained and runs alongside the composer. Endpoint requires version, Sonnet model label, composer and thinking-effort hint; it does not wait for decorative animation to stop.

All three use OpenRouter `anthropic/claude-sonnet-4.6` with existing API authentication copied privately. Setup/trust/auth preparation and APFS seed cloning are excluded. The workspace and 110×35 Ghostty-identified PTY match batch A. OS caches are ordinary and unflushed. The same parent monotonic clock starts before launcher spawn; the first configured synchronized-update frame is followed by a completed screen row containing `BB_BENCH_READY`, typed without Enter. Earlier tool-specific status commands are not used.

The [earlier report](2026-10-08-agent-startup-comparison.md) contains batch A configuration details. Codex starts its own local server with `--no-daemon` and its endpoint excludes the earlier loading splash; Hermes is the classic Python CLI. These configurations and the separate batches limit what can be inferred from the chart.

## Configured input screen from spawn (ms)

| Tool | Batch | p30 | p50 | p70 | p90 | max |
|---|:---:|---:|---:|---:|---:|---:|
| Blackbird | A | 98.931 | 102.231 | 105.771 | 115.428 | 210.033 |
| Claude Code | A | 307.764 | 346.842 | 392.716 | 558.986 | 3847.130 |
| Bare Pi | B | 486.891 | 531.217 | 616.275 | 1055.236 | 2486.377 |
| Kimi Code | B | 819.818 | 908.239 | 1121.223 | 1850.711 | 4168.018 |
| Codex | A | 1265.395 | 1369.478 | 1616.598 | 4562.046 | 7555.864 |
| Oh My Pi | B | 1656.041 | 1802.545 | 2115.854 | 3352.869 | 5793.897 |
| OpenCode | A | 3399.774 | 3794.499 | 4224.717 | 4608.243 | 9277.945 |
| Hermes classic | A | 4953.665 | 5227.299 | 6145.219 | 7665.620 | 9520.175 |

## Unsubmitted marker visible from spawn (ms)

| Tool | Batch | p30 | p50 | p70 | p90 | max |
|---|:---:|---:|---:|---:|---:|---:|
| Blackbird | A | 100.416 | 103.807 | 107.365 | 116.905 | 214.730 |
| Claude Code | A | 337.621 | 385.113 | 429.971 | 638.103 | 3988.797 |
| Bare Pi | B | 565.222 | 611.802 | 696.914 | 1183.677 | 2630.456 |
| Kimi Code | B | 829.349 | 919.737 | 1137.450 | 1862.698 | 4207.706 |
| Codex | A | 1316.076 | 1436.459 | 1659.862 | 4598.573 | 7600.149 |
| Oh My Pi | B | 1712.762 | 1863.729 | 2162.608 | 3578.689 | 6528.768 |
| OpenCode | A | 3496.232 | 3896.906 | 4311.681 | 4798.681 | 9603.877 |
| Hermes classic | A | 5007.837 | 5303.576 | 6222.462 | 7708.950 | 9621.021 |

## Marker response after typing (ms)

| Tool | Batch | p30 | p50 | p70 | p90 | max |
|---|:---:|---:|---:|---:|---:|---:|
| Blackbird | A | 0.624 | 0.721 | 0.902 | 1.297 | 2.551 |
| Claude Code | A | 30.317 | 36.317 | 37.511 | 65.960 | 228.083 |
| Bare Pi | B | 76.702 | 79.484 | 87.317 | 120.845 | 501.812 |
| Kimi Code | B | 10.087 | 12.410 | 16.066 | 22.743 | 40.743 |
| Codex | A | 39.930 | 42.942 | 49.587 | 64.988 | 176.607 |
| Oh My Pi | B | 45.553 | 54.080 | 66.947 | 124.563 | 1652.129 |
| OpenCode | A | 76.205 | 86.619 | 100.551 | 120.164 | 324.579 |
| Hermes classic | A | 47.896 | 65.252 | 72.765 | 83.062 | 139.473 |

## Endpoint and shutdown observations

| Tool | Recorded | Endpoint success | Forced shutdown | Nonzero exit |
|---|---:|---:|---:|---:|
| Blackbird | 100 | 100 | 0 | 0 |
| Claude Code | 100 | 100 | 0 | 0 |
| Bare Pi | 100 | 100 | 0 | 0 |
| Kimi Code | 100 | 100 | 0 | 0 |
| Codex | 100 | 100 | 100 | 100 |
| Oh My Pi | 100 | 100 | 6 | 6 |
| OpenCode | 100 | 100 | 0 | 0 |
| Hermes classic | 100 | 100 | 0 | 0 |

Shutdown uses local quit keys and a three-second drain before terminating the private process group if needed. Its duration is excluded from startup/input percentiles. Endpoint success does not imply a graceful exit. All selected records, including failures and outliers if present, remain in the numeric artifacts; percentiles use successful endpoint records. No retries replace selected observations.

One Pi/Kimi endpoint pilot and one Oh My Pi endpoint pilot are preserved separately. Earlier untimed probes reached the trust/setup screens; those were configuration preparation, not timed startup samples. Private transcripts and isolated installations remain in ignored `context/agent-startup-pi-kimi-omp-2026-10-08/`. Temporary credential copies were removed afterward. Original account files and retained quarantine sources were not changed.

The combined artifact identifies each source batch and its metadata/driver hash. Both use nearest-rank p30/p50/p70/p90; maxima are retained in the tables and samples. The chart uses a logarithmic time axis and displays the rounded numeric percentiles alongside the points. These are observed startup/input distributions for the specified configurations, without causal runtime, inference or coding-speed claims.

Configuration sources: [Pi upstream](https://github.com/earendil-works/pi), [Kimi environment variables](https://www.kimi.com/code/docs/en/kimi-code-cli/configuration/env-vars), [Kimi providers](https://www.kimi.com/code/docs/en/kimi-code-cli/configuration/providers), [Oh My Pi upstream](https://github.com/can1357/oh-my-pi). Installed help, package manifests and retained source informed the actual adapters; executable and install-lock identities are recorded in the added numeric artifact.
