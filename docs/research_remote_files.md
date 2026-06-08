# Remote File Handling for arconaut: Architecture Research

**Date:** 2026-06-07

## 1. How Existing Tools Handle Remote Files

**VS Code Remote-SSH** installs a `vscode-server` on the remote host and streams JSON-RPC over SSH. Extensions, LSP, and file watchers run remotely; only UI rendering is local. This gives a native feel but requires managing a server binary per target.

**Emacs TRAMP** is *agentless*: it intercepts paths like `/ssh:user@host:/path` and satisfies I/O by issuing shell commands over a persistent SSH session. It needs no remote install, but it is chatty and sluggish over high-latency links. Remote LSP via TRAMP is notorious for path-mapping bugs and hangs.

**SSHFS / FUSE** mounts remote directories by translating POSIX calls into SFTP requests. Setup is trivial and encrypted, but every operation pays FUSE + SSH overhead. Over flaky links (e.g., Tailscale to a RunPod GPU pod), mounts can hang or drop. Our own ops notes document that `rsync` and `scp` fail with "Premature close" on such links, forcing per-file SSH pipe fallbacks.

**NFS** is fast and kernel-level, yet requires server configuration and Kerberos for safe WAN use—unsuitable for ad-hoc agent targets.

**Devin** and **OpenHands** run the agent inside a container or sandbox. OpenHands exposes a `RemoteWorkspace` abstraction where `file_read`, `file_write`, and `execute_command` are explicit RPC calls rather than mounted filesystem operations.

## 2. Remote LSP State of the Art

Language servers must run where source code lives because they need random project access. VS Code and Zed run LSP binaries on the remote host; Neovim plugins like `remote-ssh.nvim` bridge LSP messages across SSH. The consensus: **mounting alone is insufficient**—language intelligence must be colocated with the code.

## 3. Recommended Architecture for arconaut

Arconaut is an agent runtime, not an IDE. Megan thinks in tools (`read_file`, `search`, `execute`), not kernel mounts. An explicit RPC boundary fits her model.

### Phase 1: MVP — SSH Native Tools (Now)
Implement four remote tools over a single persistent SSH connection with `ControlMaster`:
- `remote_read_file(path, offset, limit)`
- `remote_write_file` / `remote_patch`
- `remote_search(pattern, glob)` — runs `rg` remotely
- `remote_execute(command, cwd, timeout)`

This requires **zero installation** on the remote host and degrades gracefully on flaky links.

### Phase 2: Lightweight Remote Agent Server (Later)
When latency demands it, install an optional static Rust binary (`arconaut-remote`) on the target. It speaks JSON-RPC over stdin/stdout via SSH, providing batched ops, remote file-system event forwarding, and LSP proxying. This mirrors the Hermes MCP `remote-filesystem` architecture.

### Phase 3: Full Remote Development (Future)
Multi-hop SSH, delta-sync workspace caching, and container sandbox integration.

## 4. Security Implications

- **SSH keys:** Stay in the local sandbox (read-only). Never forward keys to remote helpers.
- **Privileges:** Default to non-root. Require an explicit flag for elevated commands.
- **Write protection:** Deny-list sensitive paths (`/etc`, `~/.ssh`) and confirm destructive ops.
- **Connection hygiene:** Use `ControlMaster` with short timeouts. Prefer key auth.
- **LSP exposure:** Bind language servers to loopback only on the remote side.

## 5. Why Not SSHFS?

Agents perform many small random-access operations. SSHFS amplifies latency by turning each into an SFTP round-trip. A dropped connection leaves mounts in a broken state, whereas an explicit SSH session can reconnect cleanly. RPC tools return structured errors (`PermissionDenied`, `NotFound`); FUSE returns opaque `EIO` or hangs, making failures hard for Megan to reason about.

## 6. Summary

Arconaut should treat remote hosts as first-class targets via **RPC-style tools over SSH**, not local mounts. Begin with the zero-install exec model (Phase 1), add an optional remote binary for performance and LSP bridging (Phase 2), and only then consider deeper IDE-style features (Phase 3). This keeps Megan fast, safe, and comprehensible.
