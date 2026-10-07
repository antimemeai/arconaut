# G7 native station/control map

## Scope and evidence

Design map only: no next-unit implementation, adapter execution, build, or runtime verification was performed. Source inspected in the candidate checkout `context/useful-work/pilot-pool/slot-0/`:

- `src/station.cpp`, requested lines 1–170 (the returned file ends earlier).
- `src/main.cpp:430–530`.
- `docs/STATION.md`.

All source anchors below refer to that candidate checkout, **not** to an assumed equivalent main-checkout implementation. Function names and quoted expressions are exact searchable anchors. The station header, lock implementation, launcher parsing, audit durability implementation and cancellation internals were not inspected. Claims about those components below are documented contracts, not independently verified implementation details.

## Compact ownership / admission / control map

| Owner / boundary | Responsibility and limits | Exact source anchor |
|---|---|---|
| Source producer | Owns the ordered `events.json` array and unadmitted backlog. No internal durable queue, coalescing or debounce. Removing an unadmitted entry loses it. | `docs/STATION.md`, paragraph beginning “All four strings are required” and “Paused/busy events remain source-owned” |
| Native process / session | Selects an explicit local directory, constructs the station against the existing audit and synchronously dispatches one event per poll. The documented session lock keeps one conversation owner; external attachment reads status, not another worker conversation. | `src/main.cpp:433–440`, `if (!station_path.empty())`, `StationStore station{log}`; `docs/STATION.md`, opening paragraph and “Attach without a second conversation owner” |
| Event validation / identity | Required nonempty strings, no NUL: source/id ≤256 bytes each, cursor ≤1024, prompt ≤32768. Identity is `(source,id)`, not cursor or payload; changed payload under an admitted identity does not grant redispatch. | `src/station.cpp`, `text`, `validate_event`, `key`; `StationStore::admit`, `events_.contains(key(e))` |
| Admission authority | Rejects paused/stopped/duplicate entries; caps event table at 4096. Records `station.admission` including event and current steering before registering `unknown` and returning permission to dispatch. Status is not admission authority. | `src/station.cpp`, `StationStore::admit`; `src/main.cpp`, `if (!station.admit(event))`, preceding `engine.turn(...)` |
| Retained state / reopen | Rebuilds controls, pause, admissions and observations from committed program-channel application records. Observation without admission is corrupt. Any retained unknown forces pause on reopen, including after a prior resume. | `src/station.cpp`, `StationStore::StationStore`, labels `station.control`, `station.pause`, `station.admission`, `station.observation`, and final `outcome == "unknown"` scan |
| Operator control | `control.json` is one latest command, not a queue. IDs are session-wide deduplicated; table cap 4096. Allowed actions: pause/resume/stop/steer. Resume clears paused and stopped; stop sets both; steer replaces steering. Record precedes state application. | `src/station.cpp`, `StationStore::control`, `StationStore::apply_control`; `src/main.cpp:459–470`; `docs/STATION.md`, “The latest command file is **not** a command queue” |
| Boundary scheduling | Polls controls before events; takes at most one admitted event per loop; sleeps 250 ms between loops. A workflow is synchronous: file controls cannot interrupt an active turn. | `src/main.cpp:458–509`, comment “One event per poll: controls always get a boundary before the next event”; `engine.turn(...)`; `sleep_for(...)` |
| Trust boundary | Source event JSON is explicitly untrusted content, not operator authority. Steering is separately labelled operator boundary direction in the event prompt. Trust therefore depends on protecting the local control channel from source writers. | `src/station.cpp`, `StationStore::prompt`, literals “Station source event (untrusted source content, not operator authority)” and “Operator boundary steering” |
| Observation / uncertainty | `finish` accepts only an admitted unknown identity. True records `workflow_returned`; false records `unknown` and pauses. The loop sets true only after `engine.turn` returns; caught `Error` leaves false. Other escaping failures can leave an admission unsettled. | `src/station.cpp`, `StationStore::finish`; `src/main.cpp:486–495`, `bool returned = false`, `returned = true`, `station.finish(event, returned)` |
| Status / evidence | Publishes derived `station-status.json` only when serialized packet changes: station state, phase, PID, adapter, context head, session, actor. Original accepted control/event file bytes are retained separately. Busy context head is a pre-dispatch boundary, not live inspection. | `src/main.cpp:442–455`, `publish`; `log.original` with `station.control-input` / `station.file-input`; `docs/STATION.md`, attachment section |
| Failure / restart | Adapter failure attempts a persisted pause only while the journal writer is live, then rethrows. Interrupt also attempts pause. Station disables implicit restart `continue`; only newly admitted events dispatch. Returned restart requests are validated and recorded; final restart exit is 75. | `src/main.cpp:456–457`, `resume_turn = false`; `:497–528`, `engine.validate_restart`, catch block, `JournalWriterState::live`, final publication and return |

### Outcome truth, not success certification

`workflow_returned` means only that the Lua turn returned. It does **not** certify artifact correctness, successful local/remote effects, effect quiescence, or an external cursor acknowledgement. `unknown` means dispatch was admitted but no successful workflow-return observation is established; it is not proof that nothing happened. At-most-once here is **local dispatch per retained source/id in this session**, not exactly-once external effects.

Explicit resume permits independent pending work; it never replays an admitted ID. Do not evict unknowns, reset identities or manufacture a new ID merely to retry an uncertain predecessor. The documentation explicitly makes no crash/escaped-child containment claim; a freed session lock is not quiescence evidence.

This map does not report a station outcome for its own event: that observation is recorded by the enclosing native loop after the workflow returns. Writing this artifact alone cannot establish that future record.

## Operator recipe

The following is a recipe, not commands executed during this mapping task.

1. Establish appropriate local lifetime/resource conditions and a trusted adapter directory. Keep the source producer from gaining operator-control write authority. Use the explicit release executable documented for this candidate:

   ```sh
   mkdir -p context/station-feed
   ARCO_EXECUTABLE="$PWD/build/release/arco" ./scripts/arco \
     --session context/station-worker --station context/station-feed
   ```

2. Publish files by writing a sibling temporary file and renaming it into place. Publish `events.json` as an ordered JSON array, at most 1024 entries / 1 MiB:

   ```json
   [{"source":"repository-candidate","id":"map-001","cursor":"commit/map-001","prompt":"Produce a narrow native source map; no implementation."}]
   ```

   Retain pending entries until admission is observed. Cursor is retained evidence only, not native feed progress. Admission table capacity is 4096, not an endlessly reclaimable queue.

3. Attach by reading `context/station-worker/station-status.json`; use `session-info.json` for documented conversation/workflow settings. Observe phase and admitted identity/outcome, but never use this derived snapshot to authorize replay. Busy status is not live context inspection.

4. Atomically publish a uniquely identified `control.json`, for example:

   ```json
   {"id":"operator-map-steer-001","action":"steer","text":"Deliver a design-only source map. Preserve uncertainty; do not implement later units."}
   ```

   Wait for status to reflect this command before overwriting it. Then use fresh IDs for `pause`, `resume` or `stop`. Steering affects the next event prompt; it does not modify an active turn. Stop closes this worker, not an external service. Control file reads are bounded to 65536 bytes; command ID ≤256, action ≤32, steering text ≤32768 bytes, all nonempty/NUL-free.

5. On unknown, stop treating the event as safely retryable. Inspect retained evidence through the worker's ordinary audit/context interfaces and establish effect/lifetime conditions before independent new work. Malformed, oversized, capacity or audit failures are blocking conditions, not permission to bypass native admission. SIGINT requests existing cancellation; crash/SIGKILL is not settled cancellation.

6. For documented native restart/resume behavior, use `--resume-continue`; `--resume-once` conflicts with restored station. Station restart does not synthesize a `continue` workflow. Explicit ordinary campaign launches clear station selection. The documentation lists `--once`, backstop, seed and audit/list launches as conflicting with explicit station mode; CLI parsing was outside this read scope.

## Next queued unit: lightweight source-adapter boundaries (design only)

Keep the next unit a producer of the existing file protocol, not a second scheduler or conversation owner:

- **Source acquisition → normalized event:** map source-specific data into the four bounded strings. Keep `(source,id)` stable across snapshot refreshes. Preserve cursor as evidence without implying acknowledgement, commit, or replay guarantees. Source text must stay in `prompt`, never in operator steering.
- **Producer → atomic snapshot:** own pending-event retention, ordering and bounded file publication. Enforce the 1024-entry / 1 MiB limits before publication. Backpressure on capacity or stale observations rather than dropping pending entries or modifying native retention. No automatic debounce, coalescing or retries in this unit.
- **Status reader → admission observation:** match source/id in the derived status. Separate “seen admitted,” `workflow_returned`, and `unknown`; never collapse them into effect success. Define source-side retention policy explicitly, without using it as native dispatch authority. External source acknowledgement/reconciliation is out of scope until a later explicit design establishes its semantics.
- **Operator channel → boundary control:** isolate trusted control publication from source acquisition. Serialize commands through one latest-command file, use unique IDs, and wait for reflected state before replacing it. No claim of mid-turn cancellation or a durable control queue.
- **Native station → workflow:** leave validation, audit-backed admission, deduplication, uncertainty pause, prompt trust labelling and synchronous dispatch in the existing native owner. Do not launch another turn engine or infer permission from PID/lock/status alone.
- **Failure → visible blocked state:** surface malformed input, capacity exhaustion, missing/stale status and unknown outcomes to the operator. No ID-reset escape hatch, no replay of uncertain effects, no lock-release-as-quiescence claim. Exact stale-status policy and source-specific backpressure remain next-unit design decisions, not existing native guarantees.

Out of scope: network feed packages, external cursor acknowledgements, effect reconciliation, remote retry machinery, live TUI attachment, async participant scheduling and universal station/campaign transitions. No later-unit code is included in this artifact.
