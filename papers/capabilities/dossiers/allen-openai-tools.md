# allen-openai-tools

literature PDF/text summaries, embeddings and a keyed local publication cache

Role: experimental literature-processing library/scripts. Runtime: Python.

Pinned source: [https://github.com/AllenInstitute/openai_tools](https://github.com/AllenInstitute/openai_tools); revision/version `161dbb40044110da30fe73f2e1404dc6268805d3`.

Synchronous client wrappers run asyncio chunk worker pool; separate serial embedding calls and diskcache object/value cache.

Consumes OpenAI chat/embedding and literature metadata APIs; owns local cached paper data, not a host agent/kernel/service control plane.

Inspection: LocalDatabase CRUD/class restore, LongText cache/summarization and OpenaiLongParser chunk/dispatch/retry/abort/save paths; selected literal database/chunk and live-API cache tests

Limits of this study: No acquired module was imported because source has import-time network setup; no test/model/API calls ran. No autonomous coding-agent loop is supplied.

## Actions

### LocalDatabase.save_to_database/load_from_database/reset_key/get_list_keys

Surface: Programmatic keyed cache API.

Input: Caller keys/values and optional database path

Result: Stored value, key list or deletion

Lifecycle: Named diskcache path persists; default temp lifetime documented; destructor closes connection

Authority: Calling Python client; no model SQL dispatcher or external governor

Evidence: [e2](#evidence-e2).

### save_class_to_database / load_class_from_database

Surface: Programmatic object snapshot API.

Input: Key and Python object __dict__

Result: Restored attributes into supplied object

Lifecycle: Mutable keyed snapshots overwrite; guard checks outer key instead of dict_key for intended database-pointer skip

Authority: Library caller can restore all saved fields; no original audit guarantees

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### LongText.summarize_longtext_into_chunks(...)

Surface: Programmatic literature transformation.

Input: Text/chunk size and desired final chunk count, optional output path/concurrency

Result: Model-derived cached summary and optional text file

Lifecycle: Repeated reduction/cleanup calls until target, no reduction-round budget; cached summary reused irrespective of later target

Authority: Library caller; paid provider uses fixed authored prompt

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### OpenaiLongParser.multi_call_chatGPT / async_call_chatGPT

Surface: Programmatic concurrent provider primitive.

Input: Prompt list/temperature/penalties; timeout/retries on async API

Result: Texts ordered by input slot or exception

Lifecycle: Bounded workers and per-attempt timeout; common abort cancels workers without await; no remote job handle/quiescence guarantee

Authority: Caller uses externally configured OpenAI authority; fixed chat model

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### process_chunks_through_prompt(prompt, save_path=...)

Surface: Programmatic chunk map.

Input: Chunked text and prepended prompt

Result: Per-chunk texts; optional assembled input/result text files

Lifecycle: Concurrent map then saves outputs; numbered files can overwrite; no complete attempts/raw-provider log

Authority: Python caller file/provider permissions

Evidence: [e5](#evidence-e5), [e9](#evidence-e9).

### call_embeddingGPT / process_chunks_through_embedding

Surface: Programmatic embedding API.

Input: Text chunks

Result: Ada vectors and corresponding chunks

Lifecycle: Serial provider requests; paper fields cached after calculation

Authority: Calling client/provider credential

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e12](#evidence-e12).

## Capabilities

### filesystem

**S — Files** (source): Explicit publication-cache and optional chunk/summary files; no general model file tool.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e9](#evidence-e9).

### processes

**— — OS programs** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### code-actions

**— — Code actions** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### persistent-kernel

**— — Kernel** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### standing-database

**S — Standing DB** (source): Program-accessible diskcache key/value publication/object cache, keyed get/list/reset; not exposed standing model SQL or original Science DB.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### workflow-programming

**S — Workflows** (source): Programmable library composes chunk map, repeated summarization, embedding/cache; no agent turn/workflow engine.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e10](#evidence-e10).

### multi-model

**— — Models** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### live-collaboration

**— — Peer chat** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### concurrent-work

**L — Concurrency** (source): Worker pool preserves input-result slot order; cancellation and failure paths are fragile and no durable job identities/reconnect.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### steering-interrupt

**L — Steer/interrupt** (source): Internal failure abort cancels pool tasks; no user steering API and no awaited cancellation/remote quiescence proof.

Evidence: [e7](#evidence-e7).

### turn-redefinition

**— — Turn program** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### compaction

**— — Compaction** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### context-repair

**— — Repair** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### original-audit

**L — Original audit** (source): Optional assembled prompt/result text files and object snapshots; raw provider responses, retries, failures and context transformations are not a complete original audit.

Evidence: [e2](#evidence-e2), [e9](#evidence-e9).

### audit-query

**S — Audit query** (source): Cache key/value retrieval and list give publication/snapshot access, not activity query.

Evidence: [e2](#evidence-e2).

### hot-change

**— — Hot change** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### rebuild-continuity

**— — Rebuild continuity** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### remote-services

**S — Remote** (source): Fixed OpenAI chat/Ada embedding and Crossref consume external APIs; no service governor/reconnect kernel machinery.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8), [e12](#evidence-e12).

### self-improvement

**— — Self-improve** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### complaints

**— — Complaints** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### authority

**— — Authority** (inspection): Outside this experimental literature-processing library/scripts role: the reference supplies literature PDF/text summaries, embeddings and a keyed local publication cache rather than an agent execution engine.

### evaluation

**L — Evaluation** (source): Literal CRUD/chunk oracles; generation tests require live credentials and expect exact model prose. Cache tests compare stored field to same generated output, not scientific validity or no-new-call behavior.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### time-order

**L — Time/order** (source): perf_counter timings and increasing per-attempt timeout; no total operation budget; result order is input slots, update timestamp is wall time.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e11](#evidence-e11).

## Inspected test oracles

- [quarantine/allen-openai-tools/tests/test_database_parser.py](../../../quarantine/allen-openai-tools/tests/test_database_parser.py): Cache CRUD/class-field restoration Oracle: Literal saved longtext, overwritten target attribute, deletion and key-list assertions; no durable restart or pointer-identity oracle inspected. Read, **not executed**.
- [quarantine/allen-openai-tools/tests/test_openai_parsers.py](../../../quarantine/allen-openai-tools/tests/test_openai_parsers.py): Chunk boundaries and live model calls Oracle: Expected literal sentence groups and exact generated text; imports require credentials. No offline failure/timeout/cancellation fault oracle in this file. Read, **not executed**.
- [quarantine/allen-openai-tools/tests/test_unique_paper_database.py](../../../quarantine/allen-openai-tools/tests/test_unique_paper_database.py): Model result caching Oracle: Same-run generated field equals stored field; does not assert provider call count/parameter invalidation or scientific correctness. Read, **not executed**.

## Useful mechanisms

- Straightforward keyed persistent publication cache and programmatic retrieval.
- Chunk/CRUD expected literals attack concrete parsing/storage faults.

## Material limits

- Cache identity omits final summary target and model/prompt version; restoring class uses wrong variable in intended database-pointer skip.
- Failure handling falls through into response processing and repeated queue acknowledgments; canceled workers are not awaited.
- Summary reduction lacks progress/round bound; text/cache snapshots are not original audit.

## Arconaut design questions

- Which result cache keys must include program/model/parameter versions so a durable cached result cannot masquerade as current computation?
- How should mapped-provider tasks retain original attempts/failures and prove quiescence before refit?

## Evidence

### Evidence e1

[quarantine/allen-openai-tools/README.md:4–8](../../../quarantine/allen-openai-tools/README.md#L4): Experimental publication API scripts, not agent product.

### Evidence e2

[quarantine/allen-openai-tools/src/papers_extractor/database_parser.py:33–123](../../../quarantine/allen-openai-tools/src/papers_extractor/database_parser.py#L33): Diskcache lifecycle, dictionary/class snapshots, keys/load/reset; restore guard compares outer key rather than field key.

### Evidence e3

[quarantine/allen-openai-tools/src/papers_extractor/long_text.py:16–62](../../../quarantine/allen-openai-tools/src/papers_extractor/long_text.py#L16): LongText cache key is text hash plus chunk-size hash, or caller key; restores object fields and saves snapshots.

### Evidence e4

[quarantine/allen-openai-tools/src/papers_extractor/long_text.py:141–212](../../../quarantine/allen-openai-tools/src/papers_extractor/long_text.py#L141): Repeated model summarization until chunk target, optional cleanup/save, cached summary bypasses changed target; no maximum reduction rounds.

### Evidence e5

[quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py:113–151](../../../quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py#L113): Sentence-boundary tokenization/chunks, detokenized source copies; literature transformation, not managed agent compaction.

### Evidence e6

[quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py:153–205](../../../quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py#L153): Async queued workers with fixed gpt-3.5 model, per-attempt increasing wait_for timeout, retry/abort on failure.

### Evidence e7

[quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py:206–288](../../../quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py#L206): Failure branches fall through to response processing; success ordering by input slots; shared abort, worker.cancel without await, polling private semaphore value.

### Evidence e8

[quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py:290–340](../../../quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py#L290): Sync asyncio.run wrapper and fixed Ada embedding endpoint.

### Evidence e9

[quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py:358–394](../../../quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py#L358): Optional input_chunk/output_chunk text files preserve assembled prompts/text results, not raw provider IO/attempt history.

### Evidence e10

[quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py:396–422](../../../quarantine/allen-openai-tools/src/papers_extractor/openai_parsers.py#L396): Embedding chunks are submitted serially and return vectors/chunks.

### Evidence e11

[quarantine/allen-openai-tools/src/papers_extractor/unique_paper.py:191–211](../../../quarantine/allen-openai-tools/src/papers_extractor/unique_paper.py#L191): Paper fields, wall update timestamp and cache restoration.

### Evidence e12

[quarantine/allen-openai-tools/src/papers_extractor/unique_paper.py:472–506](../../../quarantine/allen-openai-tools/src/papers_extractor/unique_paper.py#L472): Field embedding reuse/cache and Crossref metadata request.

### Evidence e13

[quarantine/allen-openai-tools/tests/test_database_parser.py:12–65](../../../quarantine/allen-openai-tools/tests/test_database_parser.py#L12): Literal saved data, class-field restore, reset and list-key oracles.

### Evidence e14

[quarantine/allen-openai-tools/tests/test_openai_parsers.py:9–127](../../../quarantine/allen-openai-tools/tests/test_openai_parsers.py#L9): Module demands API credential; chunk tests use expected literals, live generation expects fixed strings; no failure/cancellation oracle here.

### Evidence e15

[quarantine/allen-openai-tools/tests/test_unique_paper_database.py:63–97](../../../quarantine/allen-openai-tools/tests/test_unique_paper_database.py#L63): Live-model cache tests compare persisted fields with same run output, not scientific truth or backend call suppression.

