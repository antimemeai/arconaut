# A5 — read-only local session discovery

Grounding: main's private session directory/audit path, exclusive native journal
owner, SessionStore audited settings and stable session identities. Do not open a
RetainedState merely to list: recovery/admission would contend with live owners.

Write a small derived session-info.json configuration/identity snapshot from the
already-owning process at startup and after local commands/turns. Audit remains
authority; metadata failure warns, does not roll back retained settings or abort
work. No effect output is copied. Discovery scans direct children of the selected
root (and the root itself if a session), detects audit or summary, reads only the
bounded snapshot and stats audit mtime for last activity. It never acquires a
session lock, repairs an audit, creates directories or refreshes snapshots.

CLI --list-sessions [ROOT] defaults ~/.local/state/arconaut; /sessions lists the
current session parent. Sort stable absolute paths; resume uses explicit --session
path, never transient list index. Return structured entries with configuration,
last_activity_unix_seconds, metadata status and shell-quoted ./scripts/arco resume
command. Damaged/missing metadata entries remain visible with unavailable config,
not silently omitted or repaired. Symlink children excluded. Legacy sessions have
no summary until ordinarily opened; report this limitation, don't replay audits.

Direct actual-process oracle: live locked session listing succeeds, chosen config
and explicit path selection stable, damaged sibling tolerated, paths with quotes
shell-quoted, listing missing root does not create it, and audit/metadata byte
snapshots are unchanged by listing. Native snapshot tests validate size/schema.
Build release and affected checks, restart/verify CLI list. Review with A6 and
batch Mac diagnostic/sanitizer/Neuroses qualification.
