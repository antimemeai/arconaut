# Native request reference

CodingEngine operations now own one immutable request byte buffer and share it
between the invocation and admission in memory. Invocation schema 1 continues
to store the input. Admission schema 2 stores attempt, invocation and decision
identities only; with no dependencies its payload is 56 bytes, replacing the
previous 60-byte envelope plus request bytes.

RetainedState resolves references when admitting/replaying events and when
reading archived facts, attempts or pinned history. Empty marker-only duplicate
submissions resolve before comparison and write nothing. A supplied mismatching
input is rejected. Historical history readers capture archive/storage lifetime,
not the RetainedState pointer. The execution boundary still receives the resolved
input and the existing recovery/effect fences still govern dispatch.

Compatibility: schema 1 remains readable and is still emitted for all unmarked
admissions, including consumers with genuinely distinct admission input. Schema 2
is supported only for reference admissions; unknown schemas and schema 2 on other
kinds are refused. Older executables cannot read a session after its first schema 2
admission. No legacy rewrite or original-body reconstruction requirement was added.

The native request fixture covers a completed request and a provider process
exiting after durable admission/open. Its 262,144-byte user input yields a
276,533-byte serialized invocation input, 198-byte decision metadata and 56-byte
admission. Reopen resolves the same input, completion/unknown settlement follows
existing policy, and dispatch does not replay either request. The fixture now
checks the actual persisted admission frame too, rather than only re-encoding it.
This is a storage/ownership change, not a wall-time speedup measurement.

Direct checks cover schema 1/2, exact schema 2 size and identities, short/trailing
wire rejection, unknown schema refusal, shared input storage, mismatched reference
input, zero-write marker duplicates, and recovery/no replay. Initial native
request_storage and retained_events runs passed. Other shared-tree runs overlapped
ContextStore/Snapshot ABI edits; integrated results belong to the root journal.
