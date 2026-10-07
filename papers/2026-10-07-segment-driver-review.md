# Successful-segment continuation driver review

2026-10-07. One code check bounded to 90 seconds of
`scripts/autodev-segments`; no implementation edits, additional test campaign,
or review of unrelated controller code. Root supplied the finite native-parent
process-budget contract and reported direct fake oracles for six successful
segments, completion marker, expired deadline and retained pause.

No actionable finding within this scope.

- `"$@" --prompt "$prompt"` preserves each command argument and the prompt path
  as separate argv entries; run paths containing spaces or shell metacharacters
  are not evaluated as shell code. Absolute run-directory and numeric deadline
  inputs are checked before launch.
- The first segment uses `mission.txt`; subsequent successful segments use
  `continue.txt`. Success alone does not imply unit completion and there is no
  arbitrary segment-count ceiling that can prematurely stop unfinished work.
- Explicit `unit-complete.json` presence is the completion convention. Retained
  pause and absolute deadline stop unfinished work with distinct nonzero exits.
  These are checked at segment boundaries; the native parent, not this helper,
  enforces the in-flight process-group budget. No deadline reset is introduced.
- A nonzero child status is preserved and terminates the loop immediately,
  even if a marker appeared during that failed segment. Failure/uncertainty is
  not converted into a successful boundary or blind mutation replay. Provider
  retry remains the existing supervisor's responsibility.

Scope limits: this helper does not validate completion-file contents, forcibly
interrupt an active segment upon pause, reconcile unknown effects, or replace
native child custody. Those are not claimed by its supplied contract. The marker
convention and finite parent budget are required when invoking it; running it
standalone would not bound the duration of an individual child.
