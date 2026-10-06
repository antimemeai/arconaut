# Focused correction to the final implementation notes

## Note on in-proposal duplicates — withdrawn

You are correct, and the existing source supports your reading on both points:

1. **The clause requires one semantic fact, not physical suppression.** Current root `apply()` returns exact-existing success without pushing a second Snapshot fact (src/retained_state.cpp:183–186), while `append_impl` still pushes the draft for every successfully applied event (src/retained_state.cpp:471–481). So the accepted root behavior already is: repeated equal identities → one semantic fact, all submitted physical frames retained in order. The revised clause ("first matching record/evidence in ordered results, original positions intact") specifies *result mapping*, not write suppression — the first occurrence's record/evidence is returned for every equal input position, in input order.
2. **Suppression would be a real defect, not a refinement.** Predicted provenance for an uncertain capture is descriptor-derived as `first_sequence + source_count + event_ordinal` over the retained physical positions. Dropping a duplicate frame would shift the ordinal-to-sequence mapping for every later event in the packet, corrupting the predicted references of an uncertain proposal — exactly the provenance the refinement pins as Uncertain-typed but positionally exact. So the pinned rule "physical duplicate frames retained, first semantic record/evidence in ordered results" is the only consistent reading, and my earlier "requires suppressing the physical write" remark is withdrawn. There is no contradiction: one fact in the Snapshot, N frames on disk, N ordered result positions all naming the first record.

## Note on create-time root validation — withdrawn, with scope confirmation

Confirmed at src/journal.cpp:36–40: `validate_header` refuses `issuer_namespace == 0` with `invalid_identity`, and `EnvironmentHead::create`/`open` already route the caller-supplied root header through `encode_journal_header` (which validates) before any file I/O (src/environment_head.cpp:189–191, 231–233), alongside the existing `root.predecessor` refusal (src/environment_head.cpp:186, 228). So the codec was never permissive on this point, create was never actually exposed to the zero-namespace-then-unopenable-environment trap, and my "one-sentence gap" finding is withdrawn. Scope clarification accepted: pairwise-distinctness remains an open/chain-level check (lines 145–147) and candidate-level check (line 238–239), since per-header `validate_header` can only see one header; nonzero-ness is already enforced at creation by the existing codec. Nothing further is needed.

## Head phase factoring

The named internal `prepare`/`publish_initial` core phases, accessible to the environment owner and to direct phase tests without exposing ordinary admission or model binding, are consistent with the resolved F1 structure: `prepare` = allocate + exclusive authority create/lock + head-absence check + root declaration write/sync (no selector); `publish_initial` = selector write/sync + healthy. This composes exactly with the existing public `create` body (src/environment_head.cpp:183–224) split at the selector write, and matches the refinement's text at lines 126–138. No new surface or contradiction.

## Restated status

The four material findings from the first review remain resolved; the oracle tuples stand as previously verified; both of my residual implementation notes are withdrawn as shown above. Within the scoped clauses, the refinement is ready to govern direct red cases and implementation. As before, this is not code review and not acceptance of the owner or whole U1 — the plan itself still requires the integrated/failure/capture histories, native process-death/head-cut qualification, and the separately tracked emergency controls before any such acceptance.
