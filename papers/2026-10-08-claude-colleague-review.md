# Claude selected-source colleague review — 2026-10-08

Operator explicitly authorized a new attempt with a 360-second deadline after
the earlier 90-second timeout. Same task and selected source; fresh request ID
`campaign-shared-colleague-360-1`. Completed through native `/colleague` in
337.331 seconds (338.011 seconds including harness startup/teardown). No hi
fallback was needed. The earlier request remains unknown, not retroactively
resolved. Claude Code2.1.163; requested sonnet; provider modelUsage reports
claude-sonnet-4-6 and claude-haiku-4-5-20251001, so actual_model is null rather
than inventing a single observed model. Sonnet reports21835 output tokens.

The text below is the original colleague answer. Its defect labels are reviewer
assertions, pending owning source checks; no fixes or confirmation are implied.
Request, audited session, raw results and timing: ignored
`context/campaign-colleague-360/`.

## Review: Common Colleague Integration

Sources reviewed: `coding-operation`, `coding-integration`, `native-colleague`.

---

### Finding 1 — Confirmed Defect: Claude `is_error=true` with absent `result` field misclassified as `unknown` instead of `failure`

In `call_colleague` (native-colleague), the Claude success-path code is:

```cpp
if (required_field(upstream, "is_error") != Json{false}) {
    set(reply, "status", Json{"failed"});
    set(reply, "remote_disposition", Json{"reported_failure"});
    set(reply, "error", required_field(upstream, "result")); // throws invalid_range if absent
}
```

`required_field` throws `ErrorCode::invalid_range` if `result` is not present. That exception is caught by the outer handler:

```cpp
} catch (const Error &e) {
    set(reply, "status", Json{"unknown"});
    set(reply, "remote_disposition", Json{"unknown"});
    set(reply, "error", Json{error_name(e.code)});
}
```

The reply returns successfully (no `boundary.error`) with `remote_disposition = "unknown"`. In `coding-operation`:

```cpp
disposition = remote == "unknown" ? AttemptDisposition::unknown : ...
```

A response the CLI explicitly flagged as `is_error=true` is therefore recorded as `AttemptDisposition::unknown` rather than `failure`. The remote operation is known to have run and reported an error; replaying it is unnecessary and possibly harmful.

**Fix:** Replace `required_field(upstream, "result")` with a `find`-plus-fallback so the known-failure path completes rather than throwing:
```cpp
const auto *err_result = upstream.find("result");
set(reply, "error", err_result ? *err_result : Json{"error_field_missing"});
```

---

### Finding 2 — Confirmed Defect: `std::bad_alloc` from post-transport processing recorded as `failure`, not `unknown`

`Boundary::dispatch` (coding-operation) converts `std::bad_alloc` → `ErrorCode::allocation`. The unknown-disposition gate for `colleague` is:

```cpp
if (boundary.error &&
    (...name == "colleague"...) &&
    (boundary.error->code == ErrorCode::capacity ||
     boundary.error->code == ErrorCode::interrupted ||
     boundary.error->code == ErrorCode::io ||
     boundary.error->code == ErrorCode::incomplete ||
     boundary.error->code == ErrorCode::external_unknown ||
     boundary.error->code == ErrorCode::provider_transport))
  disposition = AttemptDisposition::unknown;
```

`ErrorCode::allocation` is absent from this list. If allocation fails *after* the transport returns—for example inside `capture("upstream_result", encoded(result))` which is inside the try block and can involve large string copies—`std::bad_alloc` escapes `call_colleague` (it is not caught by `catch (const Error &)` or `catch (const std::bad_variant_access &)`), propagates to `Boundary::dispatch`, becomes `ErrorCode::allocation`, and the attempt is settled as `failure` even though the remote dispatch already occurred and the outcome is genuinely unknown.

For comparison, all `std::exception` subclasses other than `bad_alloc` map to `external_unknown`, which is on the unknown list. This is an inconsistency.

**Fix:** Add `ErrorCode::allocation` to the unknown-disposition code list for all operations that can produce unknown remote effects (`colleague`, `exec`, `provider`, etc.).

---

### Finding 3 — Suspicious Asymmetry: `submit` vs `submit_settlement` for no-error unknown disposition

```cpp
auto recorded =
    boundary.error ? root.submit_settlement(terminal) : root.submit(terminal);
```

When `call_colleague` returns with no exception but `remote_disposition = "unknown"` (e.g., when capture of the upstream result fails inside the try block, triggering the catch path), the result has `AttemptDisposition::unknown` and `!boundary.error`. `root.submit` is called. Error cases always reach `root.submit_settlement`. Whether `root.submit` handles an unknown-disposition terminal event with the same durability and replay semantics as `root.submit_settlement` cannot be determined from the provided source, but the asymmetry is structurally suspicious—it also affects `beads`. **Requires verification against the journal writer implementation.**

---

### Finding 4 — Capture/Audit Concern: Admission-capture size is not bounded relative to `prepared`

`prepare_colleague` enforces:
```cpp
if (encoded(request).size() > 65536)
    throw Error{ErrorCode::capacity};
```

However, `capture("admission", encoded(prepared))` records a `prepared` object that contains:
- `request` (≤ 65536 bytes)
- `prompt` (a re-encoding of `selected_context` + routing fields, similar size)
- `instructions` (~ 500 bytes)
- `upstream.input` (OpenAI) or `exec.argv` (Claude), each repeating the full prompt

The logged admission record is roughly 3–4× the size of the input request. If the log writer's capacity threshold is close to 65536 bytes per entry, a valid request can trigger a capacity stop at admission. A capacity error at `capture("admission", ...)` correctly prevents dispatch (per the comment), so this is safe rather than incorrect—but the user-visible limit does not match the actual behavior. Worth documenting or adjusting the check to bound `encoded(prepared)` instead.

---

### Finding 5 — Provider Result Classification: OpenAI refusal with partial text output

`openai_text` skips refusal parts and returns any non-refusal `output_text` content. `openai_refusal` then checks for any refusal part. If both are present:

1. `output` is set to the non-refusal text
2. `status = "refused"` is set
3. The final `if (!reply.find("status"))` block is skipped; `text` and `usage` are never added to the reply

The partial text is silently discarded rather than attached to the reply for inspection. This is likely intentional (a refusal should not be presented as a successful answer), but it means callers receiving `status = "refused"` have no access to any partial content. **Not a defect, but confirm this matches the intended contract.**

---

### Finding 6 — Cancellation: No check between admission commit and transport call

After `capture("admission", encoded(prepared))` commits the attempt to the log, there is no explicit cancellation check before `transport(prepared, capture)` is called. The two native transports (`openai_http` via `config.cancelled`, `LocalTools` via `tools.cancelled`) detect cancellation during their blocking I/O. A cancellation arriving in the narrow window between admission commit and transport-entry triggers a subsequent interrupted error from the transport, which is then caught and classified as `remote_disposition = "unknown"`—the correct conservative result. **Functionally correct; not a defect.**

---

### Finding 7 — No Accidental Context Export Identified

`prepare_colleague` constructs the prompt exclusively from the explicit fields of the caller-supplied `request` (`request_id`, `from`, `to`, `task`, `selected_context`). It does not access any `CodingEngine` internal state, session history, or ambient system context. The `instructions` string is hardcoded. Context text passes through only what the caller explicitly places in `selected_context`. No ambient host state is inadvertently forwarded to the upstream model or logged beyond what the caller supplied.

---

### Prioritised Action List

| Priority | Finding | Action |
|---|---|---|
| High | `is_error=true` + absent `result` → unknown disposition | Replace `required_field` with `find`+fallback on the error-result field |
| High | `bad_alloc` post-transport → `failure` disposition for `colleague` | Add `ErrorCode::allocation` to the unknown-disposition code list |
| Medium | `submit` vs `submit_settlement` for no-error unknown | Verify journal writer treats `submit` with unknown disposition identically to `submit_settlement` |
| Low | Admission size check bounds request, not prepared object | Document or tighten limit to cover `encoded(prepared)` |
| Info | OpenAI refusal discards partial text | Confirm intent with spec owners |
