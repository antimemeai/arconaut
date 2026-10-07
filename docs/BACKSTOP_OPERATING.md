# G1 backstop operator guide

## Prepare and invoke

Use backstop for a bounded, independent response to a failed native workflow. It is not unattended crash recovery or general exec recovery.

Create `mission.json` containing a JSON object with a string `mission`:

```json
{"mission":"Improve docs/BACKSTOP_OPERATING.md using native file APIs; preserve sessions and unrelated dirty work."}
```

Run from the repository root:

```sh
./build/release/arco --backstop ./mission.json --once 'Inspect the existing guide and select a bounded native-file transformation.'
```

`--backstop` takes a FILE PATH, not inline JSON. It requires `--once 'prompt'`; the prompt must not start with `/`. An optional `--workflow FILE` may select a workflow.

## Authority and containment

- Native lifetime/session ownership is the custody boundary. Cooperative local quiescence concerns local callbacks and children, not remote success.
- There is no crash/reopen authority. Do not treat a reopened session or preserved audit as permission to resume effects.
- Arbitrary exec permanently makes containment unavailable; do not claim general exec recovery.
- A mission packet is author-declared, not verified inherited context. Preserve original unknowns and dirty work.
- Imported attempts carry no replay authority. Select an independent approach and lineage; do not replay uncertain operations.
- Predecessor session trees, including aliases, are protected from mutations and oracle placement.
- An explicit pause stays paused.

## Bounded selection and observation

Allow at most two assessment/pivot attempts within a two-minute total deadline, with one request per assessment. The selected action may use native file/context APIs, but no provider action, exec or restart.

Before mutation, read the relevant sources and destination; check tool errors and reconcile uncertain file effects by observation rather than replay. Declare a small exact-byte artifact oracle outside protected trees. It must be absent or different before the action.

After mutation, observe the artifact once and compare its exact bytes. A match establishes that artifact observation, not semantic certification, remote success or settlement of earlier unknowns. If blocked, keep the unsafe candidate inactive and choose independent useful work; do not reset the budget or add assurance layers.
