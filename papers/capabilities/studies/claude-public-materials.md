# Claude Code: three different evidence boundaries

Source study, 2026-10-01. This unit covers all three acquired Claude Code references,
separately from [Claude Science](../dossiers/claude-science.md). Nothing here runs a
downloaded executable or establishes the current closed product's internal behavior.

| Reference | What can be inspected | Resulting scope |
| --- | --- | --- |
| [Official public source](../dossiers/claude-code-official-public.md) | Plugin/mod examples and their hook programs | Actual extension behavior, plus documented host contracts; not the whole agent |
| [Official distribution](../dossiers/claude-code-official-distribution.md) | Pinned 2.1.286 native packages, metadata, installer and official setup material | Distribution/update/authentication contracts; opaque native engine |
| [Unofficial source artifact](../dossiers/claude-code-unofficial.md) | Modified, incomplete TypeScript/Bun implementation | Its own exposed source behavior, with provenance and missing modules explicit |

## Official public extension machinery

The agents-md mod registers context middleware and tool-read handling. It discovers
instruction files, tracks inheritance and selected file reads, and changes the context
actually supplied through the extension interface. It is a useful example of making
context construction programmable rather than a single fixed prompt concatenation.
[Registration and context construction](../../../quarantine/claude-code-official-public/mods/agents-md/hooks/register.ts#L40).

Hookify reloads rule material for hook execution; its pretool program catches failures
and continues rather than making rule failure fatal. That is an actual activation/failure
choice, not proof that the closed host can replace its whole turn program.
[Pretool hook](../../../quarantine/claude-code-official-public/plugins/hookify/hooks/pretooluse.py#L35).
Ralph's stop hook can continue the same task until a model-produced completion promise
or iteration limit. A promise asserted by the candidate model is not an independent
quality oracle for autodroit.
[Stop continuation](../../../quarantine/claude-code-official-public/plugins/ralph-wiggum/hooks/stop-hook.sh#L50).

The inspected mod test exercises actual registration/context outputs through SDK
fixtures. It was read, not executed. Its narrow oracle establishes expected instruction
handling in those fixtures, not native engine correctness or complete original audit.
[Test](../../../quarantine/claude-code-official-public/mods/agents-md/tests/register.test.ts#L17).

## Official distribution

The retained installer selects platform material and checks its digest before handing
installation to the downloaded product. The official setup contract says a background
update takes effect on the next start. That is distribution integrity and next-start
activation, not an outpost that preserves an active model conversation while recompiling
and transferring the harness.
[Installer](../../../quarantine/claude-code-official-distribution/metadata/install.sh#L148),
[update contract](../../../quarantine/claude-code-official-distribution/metadata/setup.md#L195).

No filesystem/process/kernel/audit/collaboration implementation is inferred from the
opaque binary or borrowed from the unofficial tree. Documented product operations are
marked D; unavailable internals remain explicit. The native product's implementation
language is not derived from its npm launcher's requirements.

## Unofficial implementation

This artifact has actual loop, tool batching, teammate and transcript code. Its query
loop handles queued inputs at several distinct boundaries; concurrency-safe tool batches
apply context modifiers in input order. Provider/model/tool changes affect a subsequent
request, not the request already sent.
[Query](../../../quarantine/claude-code-unofficial/src/query.ts#L293),
[dispatch](../../../quarantine/claude-code-unofficial/src/services/tools/toolOrchestration.ts#L19),
[queued input](../../../quarantine/claude-code-unofficial/src/query.ts#L1547).

In-process teammate execution distinguishes work interruption from ending a teammate's
lifecycle, and an idle participant polls for new work. These are stronger collaboration
ingredients than merely choosing multiple providers. However, the locked JSON mailbox
swallows read/write failures while the send operation can report success. The source
therefore supports a false acknowledgement concern; it has not been reproduced by
executing this reference.
[Teammate lifecycle](../../../quarantine/claude-code-unofficial/src/utils/swarm/inProcessRunner.ts#L1317),
[mailbox](../../../quarantine/claude-code-unofficial/src/utils/teammateMailbox.ts#L84),
[send](../../../quarantine/claude-code-unofficial/src/tools/SendMessageTool/SendMessageTool.ts#L149).

Compaction handles retained recent material and contextual attachments, and writes
boundary/previous-identity metadata. The transcript nevertheless omits selected progress
and partial activity; asynchronous writes and optional prompt dumps are not the original
audit requested for Arconaut.
[Compaction](../../../quarantine/claude-code-unofficial/src/services/compact/compact.ts#L440),
[lineage](../../../quarantine/claude-code-unofficial/src/services/compact/compact.ts#L596),
[storage filter](../../../quarantine/claude-code-unofficial/src/utils/sessionStorage.ts#L121).

Workflow/REPL/context-inspection registry references do not supply their missing or gated
implementation. The issue command is a disabled stub. Neither receives implemented
capability credit from its name.
[Gates](../../../quarantine/claude-code-unofficial/src/tools.ts#L104),
[issue stub](../../../quarantine/claude-code-unofficial/src/commands/issue/index.js#L1).

## Arconaut questions

Study context middleware and peer lifecycle separation as ideas. Specify the actual
model authority and activation boundary rather than inheriting a hook interface's
limitations. For peer delivery, inject a mailbox write failure and require the result
to distinguish admission, durable commit and consumption. For autodroit, a completion
promise must remain different from a behavioral/performance result. None of these
references is selected as a dependency or authoritative source for the closed engine.
