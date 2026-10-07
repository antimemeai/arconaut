Preserved root handoff for pushed candidate dce1481; source inactive, primary runtime unchanged.

# Durable-state r1 implementation note (bounded partial)

Allowance ends at 1791391681, including native builds, measurements and one <=90s independent review. Direct oracles: checkpoint/suffix vs full replay (including semantic rejection, context repair, issuer reservations and unresolved attempts); interruption at publication writes/sync/replace; corrupt, truncated, wrong-identity and stale checkpoint fallback/bounds. No new survey or repeated assurance layers.

## Scope chosen before edits

The complete target is a versioned current-state root containing session/program/context generations, live item order and exact original references, issuer reservation highwaters and unresolved effect/custody fences. A root must follow durable referenced data, retain a prior selectable generation, and recover committed transitions plus uncertain suffix without granting old attempts dispatch permission. Completing that requires replacing all-history consumers in session/context/coding/audit, not just parsing faster.

This bounded partial delivers **opt-in physical recovery hints**, producer and consumer together. Production defaults remain full scan. The new checkpoint names its exact journal header, committed sequence/end boundary and checked physical location references. It eliminates the first full payload scan; RetainedState still performs authoritative semantic replay and ContextStore still restores through history. It is not compact-current-state recovery, does not remove history-sized resident metadata/payloads, and must not be presented as the full target.

## Publication and selection rules

Format v1: canonical header bytes, committed cursor, count, fixed-size record locations/checksums, and whole checkpoint CRC32C. The final commit frame is also checked against a stored content checksum and cursor. Two named slots retain a prior hint; a new unique temporary is created, written and synchronized after synchronizing the journal, then replaces the older slot and synchronizes the directory. Only clean live state may publish. Failure is returned; no uncertain publication is blindly replayed. Orphan temporary files are ignored. Newest valid boundary wins; a stale root scans the remaining suffix. No usable root means existing full scan. Empty checkpoints are unnecessary.

Hints never promote semantic facts or admission. Reopen remains recovered-pending, full semantic replay validates each source/event using complete checked handles, reservations burn their durable range, uncertain suffix and custody reconciliation remain existing behavior. No automatic retry or external-effect replay is added.

Fault model: one writer under existing file lock, append-only journal, correctly implemented sync/rename filesystem behavior, accidental corruption detected within covered bytes by CRC32C. Not hostile archive replacement/authentication. A hint checksum checks its metadata; final-boundary check checks that frame; historical payload/frame checks occur when replay/read accesses them. Untouched old commit bytes are not independently rechecked by the hint path. Explicit default full scan checks every frame and batch and remains the reduction oracle. No mtime/inode shortcut or historical prefix hash pass is introduced.

## Source-grounded decisions

Pinned absolute paths are from references.json, read only, never executed:
- /Users/patrickbeam/projects/blackbird/quarantine/storage-lmdb-700e10f91a65/libraries/liblmdb/mdb.c:4868: alternate root publication; borrow lifetime would require stable owner-backed views, not transient decode buffers.
- /Users/patrickbeam/projects/blackbird/quarantine/storage-sqlite-5af1b822f5da/src/wal.c:1422 and :3740: suffix recovery and selected-offset reads are separate from historical replay.
- /Users/patrickbeam/projects/blackbird/quarantine/storage-aeron-ad4baf8dcd5a/aeron-cluster/src/main/java/io/aeron/cluster/RecordingLog.java:1694: bind snapshot selection to a replay boundary.
- /Users/patrickbeam/projects/blackbird/quarantine/storage-bitcask-d84c8d913713/src/bitcask_fileops.erl:405: checked hints fall back to data scanning. A resident all-history keydir is not the complete target.
- /Users/patrickbeam/projects/blackbird/quarantine/storage-tigerbeetle-c95d7a53a3d0/src/vsr/superblock.zig and src/vsr/grid.zig: bind locations to contents and make cache lifetime explicit. This partial adds no persistent payload cache; metadata lives for the journal owner's lifetime as required by existing APIs. Future compact-state integration must replace that history-sized interface.

## Measurement procedure

Own checkout, native Release BLACKBIRD_DEBUG=OFF, <=j2. One private 335MB synthetic fixture; truncate only its known-success probe suffix back to a recorded complete boundary before matched samples (no archive copies). Actual CodingEngine deterministic provider checks decision/invocation/admission/open linkage and known-success settlement. Warm samples are labeled warm; no cold claim without genuinely cold observation. Record wall/CPU/OS resource samples and native bytes-read/sync counts where supported. Large live context is a separate workload, not evidence from a 37-byte live prompt.

Final bounded result: PARTIAL, default inactive. Single authenticated Codex gpt-6.1-sol read-only review (session01a1173f-47b2-7192-a87e-f22681a68b32) found reservation loss, missing directory sync on equal-root shortcut, and abandoned temporary naming. Fixed by inserting into pre-reserved recovery storage, synchronizing equal-root directory, and distinct exclusive temporary attempt names. One affected checkpoint-test recheck passed, including explicit later publication after injected failures. Final build command timed out while compiling blackbird/main; checkpoint test and startup probe targets completed first. No retry of that build.

Native Release OFF warm matched fixture:335716494 bytes,642 facts,37 live bytes, identical reset suffix; three successful actual CodingEngine request samples each. Combined full scan median prompt167.259ms/readiness196.708ms/CPU165.433ms vs opt-in hint91.377ms/120.609ms/89.482ms. Actual reads671396924 vs335711578; readiness syncs9 both. RSS approx352.1-352.6MB; footprint351.2-351.7MB; virtual445.86GB is NOT mapped resident memory. Publication reads671396980 bytes once,30980-byte root,5syncs. Probe retains actual local request/effect linkage. Raw evidence build/durable-evidence. These are warm finite samples, not a census; no genuinely cold, varied-archive or large-live matched run completed, no allocation/copied-byte census. Still misses100ms request-readiness target: semantic historical payload decode dominates and retains rejected proposal bytes. NO compact durable current state, bounded semantic tail or paged history delivered. Full replay remains default and authoritative. Root acceleration is opt-in only under documented integrity assumptions. Not ready for default integration.
