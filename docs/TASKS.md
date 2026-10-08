# Tasks and subtasks

Blackbird keeps a durable session task list beside the conversation, at the far
right below the sprite. Tasks can have subtasks; subtasks cannot have children.
The pane has independent navigation. On narrow terminals a summary replaces it;
`/tasks expand` shows the task view while retaining the input. Task edits are
shown as soon as they are accepted, including during a running Lua workflow.

## Operator commands

| Command | Action |
| --- | --- |
| `/tasks` | Show a readable current page. |
| `/tasks help` | List editing and pane commands. |
| `/tasks add TITLE` | Add a task. |
| `/tasks sub ID TITLE` | Add a subtask under a top-level task. |
| `/tasks active ID`, `/tasks queued ID` | Start or return work to the queue. |
| `/tasks done ID`, `/tasks dropped ID` | Explicitly finish or drop an item. |
| `/tasks block ID REASON` | Record what is blocking it. |
| `/tasks rename ID TITLE` | Rename without changing identity. |
| `/tasks owner ID NAME`, `/tasks note ID TEXT` | Set owner or detail. |
| `/tasks title TITLE`, `/tasks bead BEAD_ID` | Name the list or link a bead. |
| `/tasks archive ID` | Remove a task and its subtasks from the working view; retained edits remain in the journal. |
| `/tasks up`, `/tasks down` | Move the task viewport. |
| `/tasks fold ID` | Collapse or expand a task's subtasks. |
| `/tasks show`, `/tasks hide`, `/tasks expand` | Control the pane or narrow expanded view. |
| `/tasks read JSON`, `/tasks apply JSON` | Use the structured read/edit API. |

Pane navigation works while a turn is running. Operator mutations use the existing
command queue during a busy turn; they execute at the next idle boundary. They
do not silently alter a provider request already in flight. Input focus remains
in the composer. Viewport position anchors to the first visible task across edits.

The summary counts top-level tasks. A parent shows its own `done/total` subtask
rollup. Finishing its last subtask does not automatically finish the parent.
Multiple tasks can be active. Blocker text and owner are separate from status.
Colors have glyph/text counterparts. Closed rows remain visible until folded,
scrolled past or explicitly archived; unfinished work has no automatic expiry.
The diagnostics TTL of 30 days does not expire tasks.

## Model and Lua use

`tasks_read` accepts an optional `query`: `id`, `status`, `owner`, `offset`,
`limit` (1–64, default32), and `revision`. Status `open` selects queued, active
and blocked items. Paging returns `next`; pass the returned revision on later
pages so intervening edits return a conflict rather than mixed data. `compact`
omits notes/timestamps; `collapsed` contains parent IDs to hide their children.
Subtask results include their parent's title.

`tasks_edit` takes `op_id` and `ops` (1–64), with `base` required for structural
operations. Operations:

- `add`: title, optional parent, status, owner, note, blocker, after sibling ID.
- `set`: id and read item version; title/status/owner/note/blocker fields to change.
- `move`: id, read item version, parent (empty=root), optional after sibling ID.
- `archive`: id and read item version; includes children.
- `list`: title and/or bead link.

`add`, `move`, `archive` and `list` check the current list base. `set` checks
the affected item version, allowing independently read rows to be changed without
conflicting on unrelated edits. An existing row may be edited once per batch.
Invalid batches publish nothing. Conflicts return current affected rows and the
list revision; reread, reconcile the edit, and use a new operation key.

An operation key identifies a batch. Identical retries return the original result
for the last16 edits. Older retries with stale guards conflict; they are not
silently reapplied. Keep keys unique. The working list is bounded to2048 items,
titles512 bytes, notes2048 bytes, owners128 bytes, blockers512 bytes and edit
requests65536 bytes. Oversized values are rejected, not silently truncated.
Mutation results return changed rows, added IDs, removed IDs, counts and revision.
They do not echo the whole list.

Lua uses the same tools:

```lua
local list = blackbird.tasks.read()
local added = blackbird.tasks.edit({
  op_id = "initial-investigation", base = list.revision,
  ops = { { op = "add", title = "Investigate the failing request" } },
})
local row = added.changed[1]
blackbird.tasks.edit({
  op_id = "begin-investigation",
  ops = { { op = "set", id = row.id, version = row.version, status = "active" } },
})
blackbird.call("exec", { task_id = row.id, argv = { "git", "status", "--short" } })
```

Reuse returned revisions/versions directly: the existing Lua JSON bridge preserves
JSON numbers as exact-number objects, not ordinary Lua numbers. Runtime task
binding is optional and explicit. Bound commands show observed running/finished/
failed/unknown activity; command success never marks a task done. Activity is a
live process view, not restored as a running spinner after reopening. Original
operation inputs/results retain the binding and attempt information in the audit.

Each provider request receives a compact current open-task page (up to16 rows)
and counts when the list has been used, including explicit empty state. This
projection is part of the recorded final instructions and survives context
compaction through rereading native task state. More detail is available on demand.
Task state uses the session's retained journal and saved-current-state reduction;
there is no separate authoritative todo file. One native sequential owner accepts
edits. Cross-session synchronization, peer transport, automatic dependency
scheduling and branch/fork task policies are not implemented by this feature.
