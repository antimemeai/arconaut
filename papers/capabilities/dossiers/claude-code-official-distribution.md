# claude-code-official-distribution

Pinned 2.1.286 packaged official product, inspectable installer and official setup contract; no native engine behavior traced.

Role: official opaque executable distribution and installer metadata. Runtime: shell (installer), JavaScript (npm wrapper metadata).

Pinned source: [https://code.claude.com/docs/en/setup](https://code.claude.com/docs/en/setup); revision/version `2.1.286`.

Native platform executables retained as data; npm wrapper dispatches platform package per metadata; engine language not inferred.

Local executable consumes provider services; engine/resource boundary unavailable.

Inspection: Read npm metadata, shell installer checksum/dispatch and setup/install/update/authentication contracts.

Limits of this study: Native macOS/Linux executable and npm archive unexecuted; no core source, runtime tests or disassembly claim.

## Actions

### claude / native install

Surface: documented operator CLI.

Input: project directory, installer channel/version

Result: interactive coding session / launcher

Lifecycle: installer downloads then delegates install to closed binary; update activates at next start

Authority: operator-installed executable; ordinary tool authorization unavailable

Evidence: [installer](#evidence-installer), [setup](#evidence-setup).

### claude update / doctor / release channel

Surface: documented operator CLI.

Input: selected channel or installation diagnostics

Result: installed version / read-only diagnostics

Lifecycle: background replacement files then restart boundary

Authority: operator configuration

Evidence: [update](#evidence-update).

## Capabilities

### filesystem

**D — Files** (documentation): Package advertises codebase understanding/editing; engine implementation not available.

Evidence: [package](#evidence-package).

### processes

**D — OS programs** (documentation): Advertised terminal commands and setup Bash/PowerShell tools; cancellation, PTY and detached work unavailable.

Evidence: [package](#evidence-package), [setup](#evidence-setup).

### code-actions

**? — Code actions** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### persistent-kernel

**? — Kernel** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### standing-database

**? — Standing DB** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### workflow-programming

**D — Workflows** (documentation): Package advertises handling workflows, without an inspected programmable scheduling contract.

Evidence: [package](#evidence-package).

### multi-model

**D — Models** (documentation): Authentication documentation lists direct Anthropic and third-party provider channels; no model-addressed peers inferred.

Evidence: [auth](#evidence-auth).

### live-collaboration

**? — Peer chat** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### concurrent-work

**? — Concurrency** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### steering-interrupt

**? — Steer/interrupt** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### turn-redefinition

**? — Turn program** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### compaction

**? — Compaction** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### context-repair

**? — Repair** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### original-audit

**? — Original audit** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### audit-query

**? — Audit query** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### hot-change

**L — Hot change** (documentation): Executable update installs in background but documented activation is next start; not live replacement.

Evidence: [update](#evidence-update).

### rebuild-continuity

**L — Rebuild continuity** (documentation): Documented executable update/restart contains no outpost, context transfer or unresolved-work continuity contract.

Evidence: [update](#evidence-update).

### remote-services

**D — Remote** (documentation): Consumes direct/third-party provider APIs; remote outpost chat/execution semantics unavailable.

Evidence: [auth](#evidence-auth).

### self-improvement

**? — Self-improve** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### complaints

**? — Complaints** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### authority

**? — Authority** (inspection scope): Setup describes provider authentication, not standing tool authority, skip-permission scope or model control of programs.

### evaluation

**? — Evaluation** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

### time-order

**? — Time/order** (inspection scope): Not established by inspected installer/package metadata and setup contract; native engine is opaque.

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Versioned executable files plus custom launcher retention offer a useful replacement primitive.
- Official platform/package boundaries are explicit without guessing engine language.

## Material limits

- Installer integrity checks do not establish coding-agent semantics.
- Restart activation is documented; context/work handoff unavailable in this material.

## Arconaut design questions

- How should an outpost retain identity/context and observe build/update before activating a replacement?
- Treat executable artifact replacement and conversational continuity as distinct contracts.

## Evidence

### Evidence package

[quarantine/claude-code-official-distribution/metadata/npm-package.json:1–38](../../../quarantine/claude-code-official-distribution/metadata/npm-package.json#L1): Package points claude at native executable, lists platform dependencies and advertises editing/terminal/workflow functions; Node requirement is wrapper metadata.

### Evidence installer

[quarantine/claude-code-official-distribution/metadata/install.sh:148–229](../../../quarantine/claude-code-official-distribution/metadata/install.sh#L148): Installer fetches manifest, verifies downloaded SHA256 and invokes binary install; no refit/context transfer mechanism in shell path.

### Evidence setup

[quarantine/claude-code-official-distribution/metadata/setup.md:96–135](../../../quarantine/claude-code-official-distribution/metadata/setup.md#L96): Official documented interactive session and Windows Bash/PowerShell choice.

### Evidence auth

[quarantine/claude-code-official-distribution/metadata/setup.md:189–193](../../../quarantine/claude-code-official-distribution/metadata/setup.md#L189): Documented direct account/API-key and third-party provider authentication.

### Evidence update

[quarantine/claude-code-official-distribution/metadata/setup.md:195–218](../../../quarantine/claude-code-official-distribution/metadata/setup.md#L195): Background updates take effect next process start, custom launchers persist with installed versions; package-manager updates may require restart.

