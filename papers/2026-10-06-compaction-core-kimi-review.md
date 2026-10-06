Review complete. I read docs/MANAGED_COMPACTION_SUBPLAN.md, include/arconaut/context.hpp, src/context.cpp, the manage/inspect/workflow sections of src/coding.cpp, the /compact//inspect commands in src/main.cpp, the schemas in src/tools.cpp, and the managed cases in tests/context_test.cpp. Findings below, ordered by consequence. Generic CLM edit/append/restore surfaces were treated as deliberately unrestricted and are only cited where they interact with managed safety.

---

## Finding 1 (critical): the `context_manage` tool is systematically rejected `stale-base` in real engine flow

**Location:** src/context.cpp:324 (`if (string_field(proposal, "base") != head_) return reject_managed(proposal, "stale-base");`) interacting with src/coding.cpp:731–733 (provider response output appended to context before any tool executes) and src/coding.cpp:783–784 (`context_manage` dispatch).

**Trigger:** any model-authored `context_manage` call. The model composes `proposal.base` from a `context_view`/`context_stats` observation made at or before the previous provider request. When the response containing the `context_manage` function_call arrives, `CodingEngine` appends the entire response output to context (`context_.append(field(response, "output").array(), ...)`, coding.cpp:733) *before* the Lua workflow invokes the tool. That append advances `head_` to a fresh random revision the model cannot predict. When `manage` runs, `proposal.base != head_` → `stale-base` rejection, every time.

**Consequence:** the primary B1 model surface (`context_manage` tool) is dead in live use; only operator `/compact` (idle, head known via `/context`) and carefully scripted `arco.manage` (Lua re-reads `arco.context()` post-append) can ever stage. The subplan's core scenario — "a model request made as a tool must stage, not publish while its call lacks a result" — is unreachable. Note the rejection is *audited as stale-base*, so the failure is silent-looking in tests: the existing test (context_test.cpp:154–155) stages with `restored.head()` and no intervening append, which no real tool call can do.

**Fix:** the staged path should not CAS against live `head_`. Validate the proposal against the recorded workflow snapshot (e.g., require `base` to equal head at `begin_workflow`, or accept any ancestor of current head and record the submitted base), since settlement already re-validates via `pending.expected` and the basis-prefix check (context.cpp:380, 405). Keep exact CAS only for the idle/operator immediate-publish path.

**Red case (spec-derived):** engine turn → model calls `context_manage{base: B}` where B was the view base before the request → expected `staged:true`; actual `accepted:false, reason:"stale-base"`.

---

## Finding 2 (critical): staged `select` can never settle while the proposing tool call is open — i.e., always, in-engine

**Location:** src/context.cpp:288–292 (`whole_groups` open-tail rule), 399 (`mode == "select" && chosen`), 404–407 (suffix re-append), 432 (`protocol_complete`).

**Trigger:** stage a `select` proposal during a workflow. At stage time `entries_` ends with the currently-executing `context_manage` function_call with no result yet (the normal in-engine state per Finding 1's append ordering). `whole_groups` permits the open tail batch only if *none* of its IDs are selected (context.cpp:289–291). But `select` keeps only chosen IDs, so the unselected open call is dropped from `next` — while settlement then appends the suffix (context.cpp:407) containing that call's `function_call_output`. Result: a dangling output; `protocol_complete(next)` fails → `reject_managed(proposal, "invalid-tool-protocol")` at `finish_workflow`. Selecting the open call instead is rejected at stage time as `partial-tool-group`. Every in-engine staged `select` therefore fails one way or the other.

**Consequence:** `select` mode — one of the four required modes — is unusable through the tool/Lua mid-workflow surface; it only works for idle operator `/compact`. The failure surfaces only at settlement, after the whole turn's work, as an audited rejection of a valid proposal. The test suite stages only `archive` (context_test.cpp:155, 163, 167), so this is uncovered.

**Fix:** treat the open tail batch as pinned/untouchable for staging: either exclude it from the transformation domain entirely (snapshot basis = entries up to the open batch start, suffix = open batch + results) or require/select-forbid it consistently and force-retain it in `next` for `select`.

**Red case:** staged `select` keeping everything except one old completed entry while the proposing call is open → expected publish with the current exchange retained (subplan: "Publication … preserves all appended suffix entries (including the proposing tool and its results)"); actual `invalid-tool-protocol` at settlement.

---

## Finding 3 (medium): `arco.append` (generic Lua surface) advances the pending settlement expectation

**Location:** src/context.cpp:182–183 (`if (pending_ && pending_->expected == head_) pending_->expected = revision;` inside `ContextStore::append`), reached from Lua via src/coding.cpp:351–353 (`append_op`, origin `"lua.synthetic"`).

**Trigger:** with a staged proposal pending, Lua runs `arco.append(items)`. Per the subplan, "only engine/context appends may advance the pending expected revision; generic edits invalidate it." `arco.append` is part of the deliberately unrestricted generic surface, yet it silently advances `pending_->expected`, and its items are then preserved verbatim in the settlement suffix (context.cpp:407) and folded into the managed publication. Additionally, if the Lua-synthetic append contains an unpaired `function_call`, settlement publish fails `invalid-tool-protocol` (context.cpp:432) and a structurally valid staged proposal is destroyed by an unrelated generic-side action — with no invalidation signal at the time of the append.

**Consequence:** managed safety depends on a distinction (engine append vs. generic edit) that the implementation does not enforce — the generic surface can both smuggle content into a managed publication and torpedo settlement without the proposer's knowledge. Either classify `arco.append` as invalidating (like `edit`) or explicitly whitelist engine origins (`"provider.branch:*"`, `"interrupt.linkage"`, `"operator"`) for expectation advancement.

---

## Finding 4 (low): interrupted-turn ordering leaves the cancellation audit before linkage repair, but `finish_workflow(false)` is skipped when the journal is not live

**Location:** src/coding.cpp:874–903.

For `ErrorCode::interrupted` with a live journal the order is correct: `finish_workflow(false)` (audited `workflow-cancelled`), then synthetic `interrupt.linkage` outputs appended with no pending stage. However, in both catch handlers, when `root.state() != live` the store keeps `workflow_ == true` and `pending_` set in-memory; if the process survives (non-fatal error path where the journal is merely blocked), the next `turn()` throws `ErrorCode::conflict` at `begin_workflow` (context.cpp:319) and the retained stage can later settle against a head the operator no longer expects. Reopen is safe (staged packets are `accepted:false` and skipped by the constructor, matching "retained stages are evidence, not executable obligations"), so this is an in-process-only edge. Fix: always clear workflow/pending state in the catch handlers; gate only the *audited* rejection on journal liveness.

---

## Finding 5 (low): `inspect(kind:"index")` omits explicit chronology

**Location:** src/context.cpp:454–462. The subplan requires index records to "include chronology and ancestry." Records carry only `id`, `item_bytes`, and optional `ancestry`; chronology is implicit in array order, and there is no capture sequence, source revision, or packet reference tying an original to the packet that captured it. For repair auditing ("exact immutable originals/ancestry") this is weaker than specified. Bounded paging itself (offset/limit/hex, 4096/65536 caps, context.cpp:470–483) is correct, including UTF-8-safe hex slicing.

---

## Test coverage gaps (oracle weaknesses, spec-derived red cases not present)

1. **No engine-level test of `context_manage` as a tool.** tests/context_test.cpp calls `manage` directly with a fresh `head()`; it cannot observe Findings 1–2. Add a red case driving the real append-response-then-execute ordering (coding.cpp:733 → 783).
2. **No staged `select` case** (only `archive`/`restore` are staged; context_test.cpp:154–170).
3. **No crash/reopen case with a staged-but-unsettled proposal** verifying the orphan stage is retained in `history` yet never activated — the replay equality check (context_test.cpp:175–178) only covers settled state.
4. **No case of `arco.append`/generic append interleaved with a pending stage** (Finding 3); the only invalidation tested is a generic `edit` (context_test.cpp:168–169).
5. **No restore case where the insertion successor was itself later archived/summarized** (the `successors` walk, context.cpp:422–429, inserts at end then); valid, but order-exactness of batch repair across a summarize→restore cycle with an edited *and* reordered group is only partially asserted (context_test.cpp:148–152 reorders but does not edit bytes).
6. **Bounded-inspection paging round-trip** (multi-slice reassembly of a large original, unknown-entry `invalid_identity`, `limit:0`/`>65536` rejection) is untested; only one 7-byte slice is checked (context_test.cpp:171–173).

## What I verified as correct

- Stale CAS, duplicate/unknown/absent IDs, mandatory retention (all system/developer + latest user), and complete-group validation on the idle path (context.cpp:322–363); replay rejects tampered revisions (`revision`/`observed`/`base` checks, context.cpp:100–110).
- Settlement is genuinely append-only: basis-prefix equality (context.cpp:405) plus expected-revision tracking means generic edits invalidate and appended suffixes (proposing call + results) are preserved — for `archive`/`summarize`/`restore`.
- Failed/interrupted workflows cancel with an audited `workflow-cancelled` rejection; overlap staging rejects; crash/reopen does not activate orphan stages (constructor skips `accepted:false`).
- Restore erases edited presentations and merges exact originals in capture order before the first present later-captured successor, repairing reordered groups (context.cpp:409–431); originals/ancestry bytes are never mutated (summary is a fresh `revision.0` entry with `ancestry`, context.cpp:394–397).
- Structural validation (`protocol_complete`, `whole_groups`) does not claim semantic summary accuracy; the tool schema (tools.cpp:253) states this, matching the subplan.
- Generic `edit`/`restore`/`originals` remain unrestricted as specified; the legacy append-at-end `ContextStore::restore` (context.cpp:223–240) is the known-insufficient generic surface the plan explicitly retains, not a managed-path defect.

**Limitations:** I did not execute anything (read-only review); Findings 1–2 are derived from the append-before-execute ordering in coding.cpp — if some un-reviewed caller path executes tools before appending the response, their trigger changes, but lines 731–733 and 783 show the opposite. Provider/tool-name details and the Lua workflow file itself were outside the requested scope.