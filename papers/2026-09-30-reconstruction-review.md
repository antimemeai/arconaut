# Reconstruction authority and documentation review

2026-09-30. Reviewed current AGENTS, Blackbird, README, FOUNDATION, RESTART,
QUARANTINE, the journal, report entry points, and Rhizome's current scaffold.
Scope: authority, reconstruction scope, and usable source guidance. This does
not repeat the archive comparison, rerun the old product, or approve a product
design. No legacy source, Git state, or beads data was changed.

No material authority or scope defect found. Current entry points consistently
place the inherited code, phases, instructions, issues, and automation in
quarantine without current authority. `docs/FOUNDATION.md:13-31` treats the
operator's remembered time work as an inquiry, preserves uncertain attribution,
and explicitly leaves clock, storage, crate, and execution choices open. Its
research → reviewed design → reviewed plan → coding sequence follows Blackbird.
The broader harness studies in `docs/RESTART.md:120-122` are subordinate research
leads; its candidate list selects no legacy module. The Rhizome pattern has not
silently imported Rhizome's product assumptions or publishing instructions.

Two small documentation corrections are warranted:

1. **Make the architecture report's path convention complete.** Its new notice
   (`papers/2026-09-29-architecture-assessment.md:11-14`) says all source paths
   are relative to `quarantine/arconaut/`, but many existing citations use
   `arconaut-machine/src/...` or `arconaut-agent/src/...` without the `crates/`
   component (`:59`, `:75`, `:99`). Other citations are contextual abbreviations,
   such as `providers/openai_compat.rs`. The source remains locatable, but literal
   application of the new guidance produces nonexistent paths. Normalize
   crate-qualified citations or explain that `arconaut-*/...` abbreviates
   `crates/arconaut-*/...` and shorter paths inherit the containing module.

2. **Name the original ZIP where comparison remains incomplete.**
   `docs/RESTART.md:24` says “The source ZIP records the same master but was not
   exhaustively compared.” QUARANTINE now distinguishes the original 211,400-entry
   ZIP from the new restorable 182-file source ZIP, whose retained files were
   compared directly (`QUARANTINE.md:16-34`, `:43`). Say “original large ZIP” in
   RESTART so the old qualification cannot be mistaken for a limit on the new
   restoration artifact. No further file comparison is needed for this wording.

The oracle report correctly marks its Cargo commands as historical and says the
fresh root has no Cargo workspace. The saved probe points into quarantine.
Research-report links now identify the quarantined June documents, while the
current foundation and assessment remain separate entry points. Historical
journal entries can retain their earlier locations because the dated migration
entry explains the move; they should not be rewritten as if that earlier layout
never existed.

These are navigation/clarity corrections, not reasons to reopen the completed
migration or choose an implementation. Parent synthesis owns their disposition.

## Finding dispositions

Both findings accepted and resolved on 2026-09-30. The architecture report now
explains crate and module abbreviations within the quarantine source tree.
RESTART now identifies the original large ZIP as the archive not exhaustively
compared, distinguishing it from the directly verified 182-file restoration ZIP.
No additional source checks or implementation changes were needed.
