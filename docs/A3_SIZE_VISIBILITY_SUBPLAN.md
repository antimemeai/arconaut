# A3 — context/request byte visibility

Keep /context's full view compatible; add /stats and arco.stats() plus a
context_stats tool for focused inspection without dumping content. ContextStore
reports current revision, entry count, compact serialized input-array bytes,
view bytes (IDs/wrapper included), and per-entry serialized item bytes/ID. Counts
are UTF-8 JSON bytes, not tokens or resident-memory estimates. No tokenizer or
limit guesses. Original audit is unchanged.

CodingEngine records the exact compact JSON request byte size after all Lua
options are applied and before provider dispatch. stats reports last_request_bytes
and a numeric usage allowlist from the last successful response in this process;
null means none, never a fabricated zero. Show request size before transport and
usage only when returned. Retain authoritative provider data through existing
audit, but do not expose arbitrary usage extension strings/credentials via summary.
Use provider returned input/output/total token counts and cached/reasoning details
only when nonnegative integral numeric lexemes. They are actual usage, not estimates.

Direct tests: independent literal JSON byte counts including escaped/non-ASCII
items, empty context, edit/restore updates; scripted provider checks exact request
byte count after override, actual usage vs absent usage and extension exclusion.
Release build and affected context/coding tests, RRC, verify live stats. A3/A4
batch review/sanitizer/portable checkpoint follows A4.
