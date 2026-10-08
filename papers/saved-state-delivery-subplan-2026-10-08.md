# Saved-state candidate delivery subplan

Owner: slot-0; private candidate branch. Total allowance: until epoch 1791429733,
including builds/provider waits. DEBUG=OFF, native C++/Lua, no installed-runtime change.

1. Add a versioned, checksummed, contiguous current-state sidecar owned by
   RetainedState, bound to the exact acknowledged journal cursor and header.
   Publish referenced bytes with file sync, replacement, directory sync; retain
   two generations. Missing/corrupt/stale data must fall back, never authorize effects.
2. Restore native context and effective program/session projections from that
   boundary and reduce only their suffix. Keep exact historical records/originals
   independently available; exclude rejected candidates and staged configuration
   from the saved current state.
3. Integrate the actual RetainedState launcher path, then implement physical and
   semantic suffix recovery with disk historical locators rather than RAM ordinals.
   RetainedEnvironment's multi-segment path stays full-replay fallback until its
   chain binding is covered.
4. Direct checks: generated histories, saved versus full current state + history,
   stale/corrupt/short publication, post-open append, audited deterministic request.
   Build candidate early with -j2. One findings recheck and fixes only; no third
   assurance layer. Commit/push candidate checkpoints and record actual results.

A projection accelerator without step 3 is partial, not native saved-state recovery.
No unit-complete marker unless every operator criterion is met. If the hard bound
arrives first, leave unsafe suffix bypass inactive and write exact continuation.
