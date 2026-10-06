# Native live provider retries: .14.2 source checkpoint

Subplan: [provider/capacity resilience](../docs/PROVIDER_RESILIENCE_SUBPLAN.md).
This fresh session is not continuation settlement of the exhausted old journal.
Pilot remains completed, B1/.14.1 closed; no campaign or recertification rerun.

## Changed behavior

CodingEngine::request keeps its frozen request/revision through bounded native
attempts without restarting Lua. Every attempt is independently admitted before
provider dispatch and has its own receipt/terminal observation; raw request and
partial chunks remain immutable. `provider.request` metadata contains retry_group,
ordinal, attempt, generation and revision. Unknown transport attempts remain
unknown even after a later attempt succeeds. Partial previews are not context
entries or tool dispatch instructions. Preview state is reset on each attempt.

Curl exit codes no longer alias errno: appended provider_transport error category,
with retained event encoding/decoding bounds updated without renumbering old codes.
HTTP408/429/500/502/503/504 and curl5/6/7/16/18/28/52/55/56/92/95/96 are retryable.
HTTP auth/client/config errors, TLS/config curl errors, explicit cancellation,
malformed/semantic/incomplete responses and local audit/storage failures stop.
Auth RPC deadlines remain local io errors, not transport retries. HTTP Child
alone labels its deadline provider_transport28. Failed recording closes admission
or propagates; no retry dispatch on an unavailable journal.

Default five total attempts, exponential1s/2s/4s/8s waits, capped16s. Request-local
Lua configuration, not provider wire parameters:

```lua
arco.request({retry_policy={max_attempts=8, base_ms=1000, cap_ms=16000}})
```

Attempts1..100; delays0..60000ms; invalid values/keys rejected before dispatch.
max_attempts=1 disables retry. No additional provider/model/library adoption.
Status and conversation display show failure category/detail, next ordinal/limit,
and delay, warning that partial output was not accepted. Steady-clock waiting
checks cancellation every25ms and again before the next attempt. Waiting overhead
can exceed a tiny configured delay by up to one polling interval. Neither jitter
nor Retry-After parsing ships here. Policy is per request, not persisted session
settings. This is bounded outage recovery, not an infinite unattended supervisor.
Provider usage exposes only actually supplied accepted-response usage; partial
attempt billing is unavailable, never inferred zero.

## Direct checks and findings

- Scripted midstream text plus partial function-call event fails then recovers:
  exact frozen requests, two distinct attempt IDs/shared group/ordinals, raw failed
  chunks retained, no partial context insertion, exactly one accepted tool dispatch.
- Repeated outage exhausts three attempts;401/auth, TLS60, explicit cancellation,
  corrupt/capacity and untyped io92/ioETIMEDOUT stop at one. Nonzero backoff2ms then
  capped3ms observed via actual wait status; invalid policy and cancellation before
  dispatch produce zero provider calls. Wait cancellation stops before attempt2
  within1s despite a configured5s wait.
- Native fake curl emits partial bytes and exits92: typed error and exact partial
  capture; HTTP1s deadline after partial output: typed28 and retained partial.
- First transport receipt test exposed enum upper-bound rejection; updated all
  retained-event error bounds. Setup required private directory permissions and
  linked function_call_output before later requests; corrected tests, not relaxed
  production protocol.
- Independent ChatGPT read-only source review found generic ioETIMEDOUT also
  catches authentication RPC deadlines: valid, fixed at HTTP deadline source;
  direct untyped timeout stop regression added. No other consequential static
  findings claimed. Private capture context/resilience/chatgpt-review.txt;
  reported85,731 review tokens (not a claim of low orchestration cost).
  Kimi run-0fknp8r7 timed out90s without review; Claude returned auth401 without
  review. These are unavailable reviews, not approvals.

Final affected Mac release/debug4/4 coding/openai/auth/retained_events; ASan+UBSan
initial4/4 then final changed coding/auth2/2. Linux Clang18.1.3/libstdc++13 initial
all three profiles4/4; final auth deadline/code/test changes2/2 in each profile.
Private checks under context/resilience, initial Linux run-gwymmi9u. Initial
Linux wrapper exceeded120s tool deadline during ASan: inspected retained output
and stopped remote users, resumed only remaining ASan build/checks, not blind
whole-run replay. No full-suite/B1/campaign claim. Release/arco built. Actual native
quiet replacement and ordinary activation observation remain next, not implied
by compilation or source commit.

## Remaining scope

Close .14.2 after actual quiet RRC/activation evidence. .14.3 needs measured real
history growth/replay cost, visible approach to capacity, reserved clean handoff
headroom, immutable old journals and explicit successor lineage. Old audit has
not been truncated, moved or enlarged; successful final settlement remains
unestablished. Parent active. Do not raise its cap to disguise exhaustion.
