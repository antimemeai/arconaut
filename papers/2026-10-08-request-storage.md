# Redundant request storage: first delivery

Operator objective arconaut-eeu is less redundant request storage, not perpetual original-byte preservation. Native CodingEngine previously serialized the same provider request into decision continuation metadata, invocation input, admission input, and a provider.request source capture.

This delivery removes two of those full bodies:

- Decision metadata binds its planned invocation with input_binding=invocation-v1 and the invocation identity; it does not contain input JSON.
- provider.request remains a small log marker with attempt linkage, revision/generation and retry attribution. input_binding=attempt-invocation-v1 directs inspection to attempt -> invocation. It has no request source frame.
- Invocation and admission still each store the input under the existing native event codec. This duplication is explicit unfinished scope, not hidden behind a closure.

Provider recovery recognizes the new binding, requires invocation/decision/generation linkage, equality between invocation/admission input, exactly one matching planned invocation, valid object input, matching context/generation metadata, and no extra embedded input. Existing inline-input decisions keep their original equality check. Unsupported/malformed custody still refuses automatic recovery; recovery does not replay the provider request. No broader recovery support was added.

## Measurements and checks

A native fixture supplies 262,144 user-content bytes. Actual serialized provider input is276,533bytes. Persisted decision metadata is198bytes; provider.request source-body bytes are0. Invocation/admission still each hold276,533bytes. Thus two full-body copies were removed from this path, but this is not a measured physical-I/O or wall-time speedup claim. Context serialization, response storage and other records are outside this count.

request_storage runs successful completion and a forked provider that exits abruptly after durable admission/open but before accepting any response. On reopen the latter becomes terminal unknown; neither case dispatches again. Both recovered inputs remain inspectable through native invocation linkage. session_recovery covers new/legacy custody, unopened requests, capacity refusal, malformed bindings, wrong invocation identity/tag, embedded input with a new binding, invalid input shape and mixed non-provider work.

One recheck found the startup_loading probe still requiring decision-embedded input. It now verifies the explicit invocation binding while keeping its exact native invocation/admission checks. The probe builds; this unit did not rerun historical startup distributions.

Release build/release/blackbird built successfully. The one recheck passed session_recovery, request_storage and coding_stream_capture; the last is affected by the changed provider.request marker. No standalone capture test rerun or unrelated full suite. git diff --check passed.

The new on-disk fixture initially used committed_facts() and hit unsupported after compact recovery. It was corrected to fact_count()/fact() for descriptor-backed history. This supplies a plausible explanation for the previously recorded full coding-test retained-output failure, but the exact old failing call was not instrumented here and that test remains unclaimed.

## Remaining scope

arconaut-eeu stays in progress. Remove native invocation/admission disk duplication only through a bounded codec/recovery unit with measured ownership/lifetime and explicit supported-format policy. Do not infer a requirement for arbitrary historical byte reconstruction. Current changes retain those native inputs for existing execution/reconciliation behavior, not because original bytes are intrinsically privileged.

Unit allowance45minutes with at most25minutes hardening: initial implementation/direct checks, one recheck and its consumer fix. No third assurance layer. Capture-lifecycle measurements and the other four untouched tasks remain independent backlog.
