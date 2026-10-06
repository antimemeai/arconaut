Read all three files in full. Findings limited to A1–A6 consistency against USING_ARCO.md and RRC_SUBPLAN.md; no code inspected, no claims about current implementation.

## Findings

**F1 — A4 × RRC: queued-submission restoration is undefined and can double-execute (the dangerous one).**
- AUTODEV_QUEUE.md:34 requires queued submissions "represented distinctly so restoration cannot execute them twice," but neither source doc defines their fate across restart:
  - USING_ARCO.md:31–33 — typed-during-turn prompts and slash commands *queue* and apply after the current turn.
  - USING_ARCO.md:100–111 and RRC_SUBPLAN.md:8–19 — restart is deferred until the turn finishes, then the new process injects `continue` and consumes the intent *before* requesting a provider.
- Gap: a prompt queued at the moment a model-scheduled restart fires is neither drained (turn ended) nor covered by the intent dedup (dedup only covers the injected `continue`, RRC_SUBPLAN.md:17–18). On resume, a restored queue entry could be submitted ahead of or alongside the injected `continue` — i.e., an unintended execution, or silently dropped (operator loses work with no trace).
- Fix: extend A4's completion condition with two explicit rules: (a) queued prompts persist as drafts only and are **never auto-submitted** after resume; (b) their ordering relative to the injected `continue` is specified (recommend: always restore *behind* it) and tested in the PTY reopen oracle A4 already requires.

**F2 — A2: retrieval path for omitted output is unspecified; risks violating the queue's own no-second-subsystem rule.**
- AUTODEV_QUEUE.md:32 demands "a way to retrieve omitted output without rerunning the command," and line 38–39 forbids a second durability subsystem and mandates the existing audit/context path. USING_ARCO.md:80–82 confirms process chunks are already retained as originals.
- These are compatible *only if* retrieval reads back through existing audit originals. As written, "a way to retrieve" doesn't say that, and an implementer could add a side store to satisfy it, contradicting line 38.
- Fix: one clause in A2 — "omitted-output retrieval reads the retained audited chunks/originals; no new output store." Cheap, kills the contradiction.

**F3 — A4 storage bounds vs existing presentation limits (minor).**
- USING_ARCO.md:44–46 fixes drafts at 1 MiB and history at 128 entries and states these limits don't trim audit. A4 says only "bounded storage." Fine in spirit, but A4 should state it reuses those same bounds for persistence so nobody "fixes" the bound by growing it or by writing drafts into audited context (line 40 already says drafts are UI state — keep it).

## Checked and found consistent (no action)

- **Full audit vs bounded output**: no contradiction as long as A2's budget is model-facing presentation only; A2's "original stdout/stderr chunks remain audited" matches USING_ARCO.md:80–82. Only the F2 ambiguity needs pinning.
- **Quiet RRC**: queue's execution model (AUTODEV_QUEUE.md:89–93 — build, `/restart`, verify from new process) matches RRC_SUBPLAN.md:8–19 and USING_ARCO.md:100–113 (defer to turn end, exit 75, single dedup'd `continue`, no prompt replay, no auto-retry after consumption). Nothing in A1–A6 demands replacement mid-operation. A6's "composer usable during work" is compatible with deferral.
- **A5** "must not contend for or mutate live session journals" aligns with the one-process-per-session rule (USING_ARCO.md:13–14).
- Checkpoint cadence (AUTODEV_QUEUE.md:95–101) is boundary reviews, not a new plan gate — consistent with the operator's intent.

## Limitations

Docs-only review of the three named files; I did not verify whether any claimed current behavior (e.g., "terminal drafts are currently process-local") is true in code, per the parallel-implementation constraint.
