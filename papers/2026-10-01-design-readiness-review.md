# Design readiness: focused review

2026-10-01. Read the readiness assessment, native-reload/Lua-embedding/refit-custody
reports and acquisition catalogs, CLM source study and current baseline decisions.
Spot-read the acquired Lua control-transfer/continuation code, RCC++ module-load
path and LevelDB write/sync failure path. No acquired program or test was executed;
this review does not requalify archive hashes or any runtime.

**Judgment: the conclusion is honest at its stated scope.** There is sufficient
mechanism evidence to begin behavioral and foundation specification. This is not
implementation readiness, selection of a loader/binding/storage dependency, or
evidence that the eventual combined stack works on macOS arm64. The assessment
explicitly distinguishes those claims and retains meaningful consequential gaps.

CLM is correctly present in the first core, including model-authored transformations
and evolution of their governing programs. Its released-source losses, final-input
capture gap and unavailable swarm options are identified rather than inherited as
our intended behavior. Narrowing provider/UI breadth does not quietly defer CLM,
original capture, standing-state preservation or reviewed design.

## Resolved finding

The LevelDB acquisition catalog's `archive` is recorded as
`quarantine_proj/archives/arconaut-refit-custody-2026-10-01/leveldb-7ee830d02b62.zip`.
Unlike the other new catalogs, this resolves from the workspace rather than the
Arconaut project, without declaring a different root convention. A direct existence
check from Arconaut fails; `../quarantine_proj/archives/...` resolves to the archive
and agrees with the custody report's restoration location. Normalize that catalog
path or declare its root. This is a small reproducibility defect, not missing
mechanism evidence or a blocker to beginning design. The owning lane normalized
the path during review; rereading the catalog and checking its project-relative
location confirms this finding is resolved. No unresolved readiness finding remains.

## Consequential gaps remain visible

- Native reload: RCC++ destroys replaced objects despite keeping modules loaded;
  its loaded-module retention is unbounded and its fault machinery is not a C++/Lua
  recovery guarantee. Neither RCC++ nor cr supplies our old-work/reference contract.
  Generation isolation, activation and retirement therefore need owned design and
  actual-host qualification before implementation depends on a candidate.
- Lua: linkage and the exact unwind configuration, native continuation/root/finalizer
  custody, allocation failure and reload interaction remain unqualified. Matching
  source and tests establish concrete obligations and oracle inputs, not a working
  embedding configuration.
- Refit: no source implements the entire intended handoff. Parentage/wait duties
  are distinct from descriptors and effect authority. Arbitrary-tree pause and
  replacement of the physical custodian are unsupported general claims. Design
  must choose a supported owned scope and mechanism, rather than assert them.
- Audit: an append/blob publication protocol, acknowledged durability and recovery
  contract do not yet exist. The persistence literature does not qualify APFS;
  LevelDB salvage and fault-test predicates cannot certify complete originals.
- Provider: the initial provider's exact current protocol/transport still needs
  inspection once selected. The corpus supports foundation vocabulary and capture
  requirements, not a guessed provider-specific implementation contract.

These gaps are explicitly represented in the readiness assessment and grounding
reports. No additional consequential first-core claim was found hidden behind
corpus size, successful parsing, descriptor transfer, static annotations, or a
working-demo promise. Design may proceed while these choices are resolved in their
proper sequence; implementation remains contingent on reviewed contracts and a
bounded qualification/implementation plan.
