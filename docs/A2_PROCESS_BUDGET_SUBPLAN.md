# A2 — bounded process presentation, retained retrieval

LocalTools validates optional output_max_bytes before dispatch; it bounds raw
prefix bytes, not JSON overhead, and returns output_bytes/omitted_bytes when set.
UTF-8 cut mid-codepoint uses existing hex representation, never invalid JSON.
No budget retains legacy result shape. Exit status and timeout/error behavior stay
unchanged. Original observer process.output chunks are never clipped.

CodingEngine attaches output_ref (the admitted attempt ID) to exec results,
including failure results when returned. read_process_output accepts that reference and
optional byte_start/byte_end (A1 indexing). It reconstructs only matching audited
process.output source dependencies, with original order, and slices presentation.
Unknown references fail; an admitted exec with empty output returns empty. This
uses RetainedState::source, not LocalTools' ephemeral captured vector, side files,
or a rerun. Retrieval can survive ordinary session reopen. Timeout and interrupted calls still
throw to preserve existing cancellation semantics; retained attempt IDs are
available in the audit for explicit later retrieval.

Tests: bounded nonzero exit, zero budget, invalid budget preventing effects,
UTF-8/binary boundary, existing full raw capture; native engine retrieval after
reopen and timeout partial output. Review A1/A2 source/fault classes together;
run affected debug/release/ASan, diagnostics and portable Neuroses checks.
