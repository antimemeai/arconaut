# Arconaut build and oracle assessment — 2026-09-29

## Judgment

The inherited workspace compiles on this Mac. Its tests contain useful small
behavioral checks, but they do not justify accepting the coding harness, provider
adapters, context policy, or phase-completion claims. Several named oracles never
exercise their advertised behavior; others explicitly preserve incomplete or
incorrect behavior. The most valuable next evidence is a small number of direct
tests across actual runtime boundaries, guided by a renewed behavioral contract.

This assessment reads the current Blackbird, AGENTS, README and journal, then the
manifests, build script, historical CI and tests. It assesses source inherited
through `3bf2056`. No product code, Git state or issue database was edited by this
reviewer. Temporary probes and logs are in ignored `context/oracle-assessment/`.
Reports and experiment priorities are research artifacts, not a replacement issue
tracker. No mutation testing, live provider requests, operator credential access,
editor startup or SSH execution was performed.

Source location updated 2026-09-30: source paths in this report are relative to
`quarantine/arconaut/`. The historical CI is now at `.github/workflows/ci.yml`
inside that reference tree. Executed commands describe the earlier assessment
layout; the fresh project root has no Cargo workspace. The saved probe's include
path now points into the quarantined source.

## Executed evidence and its limits

The host reports Rust/Cargo 1.89.0 and has `protoc` installed. The only repository
build script, `crates/arconaut-agent/build.rs:1`, invokes tonic protobuf codegen for
the inbox protocol. The manifest declares Rust 1.85 (`Cargo.toml:19`), which was
**not** tested. Cargo artifacts were directed to
`context/oracle-assessment/target`; builds used four jobs and a 180-second process
group limit. Missing registry dependencies were fetched with a 30-second network
timeout and one retry. Cargo.lock was respected with `--locked`.

| Command | Observed result | What it establishes |
| --- | --- | --- |
| `cargo metadata --locked --offline --format-version 1` | Exit 101: uncached `android_system_properties v0.1.5` | The initial local cache was insufficient for fully offline metadata. This is not a source defect. |
| `cargo test --locked -p arconaut-core -p arconaut-audit --lib` | Exit 0; 36 core and 8 audit tests passed; 36.8 seconds including dependencies | These selected tests execute successfully on this host. |
| `cargo check --locked --workspace --all-targets --all-features` | Exit 0; 104.5 seconds including dependencies | All workspace targets/features type-check on this host. It does not run the tests or product. |
| `cargo test --locked --offline -p arconaut-machine --lib providers::anthropic::tests::` | Exit 0; 13 passed; 40.1 seconds including compilation | The inspected Anthropic converter/unit tests pass, including the ineffective URL test. No HTTP requests occur in these selected tests. |
| `cargo test --locked --offline -p arconaut-machine --lib providers::openai_compat::tests::` | Exit 0; 13 passed; 0.3 seconds | The inspected compatible-provider converter/unit tests pass despite missing tool-call parsing. |
| Temporary `rustc --edition=2024` probe linked against the compiled core and including the unchanged compaction module | Exit 0, three observed counterexamples below | Actual runtime behavior contradicts stronger context claims that the passing core suite does not check. |

Logs: `metadata.stderr.log`, `core-audit-test.log`, `workspace-check.log`,
`core-probe.log`, `provider-anthropic-tests.log`, and
`provider-openai_compat-tests.log`, all under `context/oracle-assessment/`.
The probe is also preserved as
[`experiments/context_counterexamples.rs`](experiments/context_counterexamples.rs).
It is an assessment witness for the inherited defects, not a product test that
requires retaining those defects. In total, 70 selected inherited tests passed.

The probe printed:

```text
same_400_ascii_text user_tokens=100 tool_result_tokens=0
revert_after_clear history_len=0 token_count=100
inherited_preserves_recent_fixture compacted=false history_len=10 tokens=10
```

The first result follows from counting only top-level `ContentPart::Text`
(`crates/arconaut-core/src/context.rs:104`), while tool output is nested in a
`ToolResult` (`crates/arconaut-core/src/message.rs:78`). A tool-heavy coding turn
can therefore grow substantially without its context counter reflecting that
growth. This does not require arguing about the accuracy of a tokenizer heuristic.

The second follows from `clear()` retaining checkpoints (`context.rs:56`) and
`revert_to()` truncating current history while restoring the earlier token count
(`context.rs:88`). After clear, the missing messages are never restored. The
existing `context_clear_resets` deliberately asserts checkpoint preservation
(`context.rs:124`) but does not compose clear with revert. The restart must decide
whether a checkpoint survives destructive edits; either way, the current result is
internally inconsistent.

The third reproduces the exact setup of `compaction_preserves_recent`
(`crates/arconaut-agent/src/compaction.rs:123`): ten four-character messages
produce 10 tokens against a 50-token trigger. The test discards the compaction
boolean and checks an element already present in the unchanged history.

## Where the tests do real work

Keep useful behaviors and fixtures as candidates, with their claims bounded:

- Registry dispatch checks exact output and unknown-tool errors
  (`crates/arconaut-core/src/tool_registry.rs:112`, `:134`). Message serialization
  checks a concrete roundtrip (`crates/arconaut-core/src/message.rs:113`). These
  detect local dispatch/serialization mistakes, not validity of whole provider
  transcripts.
- Filesystem tool tests compare resulting bytes for writes and edits, and reject
  missing/ambiguous edits (`crates/arconaut-machine/src/tools.rs:630`, `:666`,
  `:684`, `:720`). These were inspected, not executed in this assessment. Failure
  cases should eventually also check that original bytes remain unchanged.
- Audit tests inspect the file after reopening and appending, and parse a logged
  event (`crates/arconaut-audit/src/logger.rs:102`, `:124`). They passed. They do
  not establish that every runtime outcome emits an event, that write failure is
  reported to the caller, or that data survives a crash.
- Old Soul tests use deterministic responses to check completion, max steps, and
  association of a tool result with its call ID
  (`crates/arconaut-agent/src/soul.rs:394`, `:407`, `:440`). These are useful local
  assertions. Its mock ignores the incoming request and produces a default
  successful response when exhausted (`soul.rs:324`), so malformed transcripts
  and unexpected extra calls can escape.
- OAuth wiremock tests exercise actual HTTP method/path handling and response
  parsing with a loopback server (`crates/arconaut-machine/src/auth/oauth.rs:486`).
  Their matchers do not constrain grant bodies or device headers. Their value is
  narrower than the protocol conformance claims in the documents.

## Highest-consequence oracle failures

### Provider conversions are tested below the broken boundary

Anthropic constructs `format!("{}messages", base_url.trim_end_matches('/'))`
(`crates/arconaut-machine/src/providers/anthropic.rs:95`), producing
`.../v1messages`. Its `url_construction_no_double_slash` test checks only string
trimming and comments that the complete URL will contain the missing slash
(`anthropic.rs:488`). A test named for URL construction therefore cannot find this
URL defect. The HTTP method is callable through the public provider trait; the
comment that it is hard to test is not an architectural necessity.

The shared OpenAI-compatible adapter has the same concatenation defect
(`crates/arconaut-machine/src/providers/openai_compat.rs:123`). It leaves response
tool-call parsing explicitly unimplemented (`:183`), while the response fixture
contains text only (`:481`). The converter test positively requires a tool result
to become plain user text (`:398`); the converter drops tool call identity and
error structure (`:271`). Such expectations cannot be carried forward merely
because they pass. The relevant artifact is an independently specified complete
tool-use exchange, including the second outgoing request.

The historical Phase 5.5 document already names real Anthropic request-body and
tool-use response tests (`docs/phase5_5-conformance.md:41`). Those named tests are
absent from the source. These are planned assertions, not execution evidence.

### Component checks assume a scheduling environment the application lacks

The terminal bridge test spawns an independent receiver before awaiting the tool
(`crates/arconaut-cli/src/terminal_bridge.rs:75`). The production receiver and
awaited `soul.run_turn()` instead occupy branches of one select loop
(`crates/arconaut-cli/src/main.rs:640`, `:653`). A tool call waits for the reply
that this blocked loop must send. The unit test validates the bridge under a
different scheduling arrangement, leaving the application deadlock untested.

Dedup/intervention tests repeatedly call `insert()` to manufacture the consecutive
count (`crates/arconaut-agent/src/intervention.rs:160`, `:181`). In the production
Soul, a cache hit returns before `insert()` (`crates/arconaut-agent/src/soul.rs:282`).
Repeating the same call does not follow the trajectory used to test the hard stop.
The cache also treats every same-name/same-argument tool call as reusable within a
turn, including reads after writes and retries after failures. A local test
explicitly requires duplicate effects to be suppressed (`soul.rs:485`) without
establishing when suppressing an effect is semantically valid.

### Compaction tests reward deletion, not preservation of useful state

The compactor replaces older content with only a count of messages/tokens
(`crates/arconaut-agent/src/compaction.rs:53`). Its reduction test asserts fewer
tokens, a system role, and a `[SUMMARY]` prefix (`:98`), all compatible with losing
every important fact. The recent-window test never triggers, as executed above.
No inspected oracle checks preservation of constraints or pairing of tool calls
with results when the cut falls between them. A new compaction contract is needed
before a stronger preservation test can be specified honestly.

### New-loop completeness is absent and incompleteness is green

The reactor's advertised complete-turn test is ignored
(`crates/arconaut-reactor/tests/logic_tests.rs:79`); even its future assertions omit
the exact final message and tool execution count described in its comments. The
implementation returns an unimplemented error (`crates/arconaut-reactor/src/logic.rs:71`).
Pilot's `continue_turn` test asserts that the unimplemented path returns an error
while describing itself as a red test (`crates/arconaut-pilot/tests/soul_tests.rs:138`).
It therefore passes precisely because continuation is absent.

The pilot purity oracle checks only stop reason and total token usage across two
calls (`soul_tests.rs:112`). It checks neither message equality nor request
preservation, and never reads the mock's call count despite claiming two calls
confirm statelessness. This does not establish the stated purity contract.

### Time and environmental behavior are often replaced with easier assertions

- `interval_returns_none_when_very_idle` constructs a fresh active tracker and
  asserts `Some`, exercising no idle path
  (`crates/arconaut-machine/src/auth/refresh.rs:233`). The test for
  `tombstone_blocks_then_expires` explicitly removes the tombstone instead of
  allowing expiry (`crates/arconaut-machine/src/auth/tombstone.rs:61`).
- `device_auth_request_shape` checks only two nonempty configuration strings,
  although the conformance table promises a correct POST body
  (`crates/arconaut-machine/src/auth/oauth.rs:454`; `docs/oauth-conformance.md:32`).
- Lock tests acquire and release an uncontended lock; no cross-process exclusion
  test exists (`crates/arconaut-machine/src/auth/lock.rs:97`). A comment assigning
  contention to integration/manual verification is not evidence that it occurred.
- Remote tests return successfully when localhost SSH is unavailable
  (`crates/arconaut-reactor/tests/remote_tools_tests.rs:16`); AST tests do the same
  when a parser is absent (`crates/arconaut-reactor/tests/nvim_tools_tests.rs:79`).
  Cargo will count these as passing, not skipped. The remote search test makes no
  assertion about output (`remote_tools_tests.rs:122`).
- `nvim_spawn_succeeds_within_timeout` names a 200-ms limit but contains no elapsed
  assertion or timeout (`crates/arconaut-reactor/tests/nvim_tests.rs:4`).
  `version_flag_is_recognized` accepts any exit status as long as one TTY error
  string is absent (`crates/arconaut-cli/tests/cli.rs:36`).

## Build and suite reproducibility

Historical CI runs stable Rust on Ubuntu with warnings denied and check, format,
clippy, tests, documentation and dependency policy jobs
(`.github/workflows/ci.yml:9`). These are reasonable independent mechanical
checks, not a conformance argument. This review did not recreate Linux CI or run
format/clippy/docs/dependency-advisory checks; no implementation changed. The
current all-target/all-feature check passed without warnings in its log, but was
not run with the historical `RUSTFLAGS=-Dwarnings` setting.

CI never explicitly installs the protobuf compiler used by the build script,
Neovim, tree-sitter parsers or an SSH fixture. Availability on a particular runner
was not verified. Tests also embed global `/tmp/arconaut_*` paths
(`crates/arconaut-reactor/tests/nvim_tools_tests.rs:51`,
`crates/arconaut-reactor/tests/remote_tools_tests.rs:88`). Blindly running the full
suite could use the operator's SSH identity or touch ambient device identity
state: `DeviceInfo::generate()` calls `get_device_id()`, which reads or writes the
platform configuration directory (`crates/arconaut-machine/src/auth/device.rs:20`,
`:60`). Even OAuth construction tests can reach this helper. This is why the
execution above selects inspected tests instead of claiming a whole-suite pass.

An inventory finds 247 literal `#[test]`/`#[tokio::test]` attributes, including
property macro cases and the ignored test. That is a source inventory, not a
passing count or quality measure. Property tests occur in the token estimator
(`crates/arconaut-core/src/context.rs:195`): nonpanic, empty input, an upper bound,
and monotonicity under appended text. These properties do not cover nested tool
content or checkpoint state transitions. No executable formal model, fuzz target,
deterministic simulation harness or mutation result was found in the active
source. Eval and corpus are single-comment placeholders
(`crates/arconaut-eval/src/lib.rs:1`, `crates/arconaut-corpus/src/lib.rs:1`).

## Experiments that would change the retention decision

Prioritize by the behavior at risk, after agreeing what the restart should do:

1. **A complete local provider/tool exchange.** Drive the real adapter against a
   strict loopback server. Match path, headers, full request body, call IDs, error
   flags and tool-result correlation. Return text and tool calls, then inspect
   the next request. Fault classes: malformed transport, lossy conversion,
   capability claims unsupported by actual behavior. This replaces converter
   self-confirmation; it need not certify another test.
2. **One deterministic turn through the actual task arrangement.** Use a provider
   script that rejects unexpected requests and fails when exhausted. Exercise
   terminal dispatch, cancellation, tool failure, max steps and provider error.
   Check bounded completion and actual effects. Fault classes: deadlock, lost
   cancellation, skipped work, missing outcomes. The model should distinguish
   requested calls from executed effects and permit a read/write/read history.
3. **A small reference model for context state.** Generate append, insert,
   checkpoint, clear, revert and compaction traces; assert accepted histories and
   coherent accounting against the model. Include nested tool outputs and cuts
   around call/result pairs. Separately evaluate what information a compacted
   representation must retain; shrinking is not that oracle.
4. **Controlled clock/process experiments for authentication and tools.** Model
   active/idle/sleep-wake time, token expiry and rejection cooldown without wall
   sleeps. Use isolated child processes for lock contention, process cleanup,
   file replacement failure and editor EOF/crash. Use explicitly created fixtures
   and distinguish unavailable prerequisites from a passing behavior check.
5. **Fleet mutation against retained contracts.** Once direct oracles exist,
   mutate the retained conceptual units on the fleet. Assess surviving mutants
   by the fault they reveal, not as a quota. Do not spend the fleet characterizing
   placeholder tests or use a second layer of tests to validate a first layer.

These experiments do not imply retaining the old architecture. They distinguish
useful mechanisms from scaffolding and policy mistakes, and let the refreshed
design decide which mechanisms are worth strengthening.
