# Jev for development and review

This is development tooling, separate from the production C++/Lua harness. The
operator requested Jev before core implementation. We reuse the workspace's
owned, standard-library client rather than adopting an SDK or adding another
authentication implementation. Read [workspace guidance](../../JEV.md) and the
[TypeSafe skill](../../../.codex/skills/typesafe-ai/SKILL.md).

## Written setup sub-plan

Prepare explicit, bounded review cases from named current source/specification
sections. Retain the exact section bytes, complete source hashes, model version
and typed questions. Send one batch through the existing client; retain its raw
probabilities, actual model and usage in an ignored working directory. Jev's
answers are review leads: a reasoning colleague investigates them against actual
code, independent expected outcomes and execution evidence.

Section selection ignores ATX-like headings inside backtick or tilde fences,
including when finding the end of a section. A missing or duplicate actual
heading fails preparation. The pinned requested model must equal the returned
model: mismatch retains `response.json` unchanged, records the actual model and
`model_mismatch_failed` disposition, and exits unsuccessfully. Retaining a
response is not accepting its attribution.

The wrapper must not read credentials, install libraries, search the checkout
automatically, evaluate arbitrary instructions from a response, alter code, or
turn probability into approval. Its exit status concerns preparing/transporting
the request, never whether the change is correct. Preparing a request is offline;
live use is explicit. Each run has a new directory; replay is reading its saved
response, never an implicit cache hit. Bound input size and client elapsed time.

Qualification: prepare the supplied case without API access; examine the exact
request; reject credential-path, missing-section and oversized cases; then make
one authorized, bounded live call and inspect all semantic outputs. These are
direct checks of this small setup, not a calibration study or tests of tests.

## Commands

```sh
python3 scripts/jev-check.py tooling/jev-cases/review-foundation.json
# The same preparation plus an actual request through the workspace client:
python3 scripts/jev-check.py tooling/jev-cases/review-foundation.json --live
```

Outputs live under `context/jev/<new-run>/`: `request.json`, `response.json` when
available, and `run.json` with source/client identity and transport disposition.
Read `request.json` before sending a new case; selected contents go to TypeSafe.
The script prints a concise transport summary and the unchanged answers. A live
call uses the prepared snapshot even if source files change during the request.
Reassess freshness before applying a judgment to a changed checkout.

Case JSON contains a pinned `model`, named `sources` with repository-relative
`path` and exact Markdown `section` heading, structured `state`, and a nonempty
`questions` map. Question instructions reference `case.<field>` and
`sources.<id>.text` explicitly; IDs themselves are not inference instructions.
For code review, select a complete bounded function or implementation unit in a
Markdown review packet, alongside its real specification and test proposal.
Do not omit decisive cleanup/error code to make an answer more convenient.

Useful questions compare a concrete test's expected result with its claim,
classify a cited specification claim as supported/contradicted/not stated, or
rank candidate fault schedules for one invariant. Jev cannot generate tests or
review commentary; Kimi/Codex supply candidates and inspect its judgments.
Do not ask it to prove memory safety, perform exact arithmetic, count events,
authorize effects, or replace a behavioral oracle. No hard confidence threshold
or automatic exclusion is configured. Keep contradictory evidence available.

The initial model is `jev-1.13.0`, confirmed against the
[current model documentation](https://docs.typesafe.ai/models) on 2026-10-01.
Questions and response fields follow the [HTTP API](https://docs.typesafe.ai/api).
The [citation-check pattern](https://docs.typesafe.ai/cookbooks/citation_check)
separates deterministic source presence from semantic support; we borrow that
division without its SDK, cache, or example approval threshold. Its
[documented failure modes](https://docs.typesafe.ai/model-jaggedness/jev-1.13)
favor short, directly named state and code-owned exact rules.

This integration does not add Jev to the production runtime, provider set, or
formal acceptance oracles. The shared client handles credentials privately,
rejects authenticated redirects, and limits retries to documented overload/rate
responses. We do not read the workspace `.env` ourselves.
