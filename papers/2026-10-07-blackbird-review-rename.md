# Blackbird rename adversarial review

2026-10-07. One review round, restricted to the new rename. Prior renderer review
was not reopened. Product files and operator sessions were not modified; no model
provider was called.

## Finding: compatibility build targets can succeed without their executable

Medium. `CMakeLists.txt:242–248` creates `arco` / `arco-candidate` custom targets
that depend only on the primary executable targets. Compatibility executable
copies are undeclared `POST_BUILD` side effects. If a compatibility file is absent
while the primary executable is current, building the compatibility target returns
success without restoring its executable. Existing scripts then fail at exec.

A faithful isolated CMake/Ninja project reproduced this: build target `arco`,
delete only its copied executable, build target `arco` again. Observed exit 0,
`ninja: no work to do.`, and compatibility executable still absent. The same rule
structure applies to `arco-candidate`. This is a missing-output dependency defect,
not a compiler or application failure.

Repair the dependency contract: an always-run compatibility target command, or a
declared output rule that rebuilds an absent compatibility executable. Recheck by
deleting only the alias output in an isolated build and requesting its target.

## Examined without additional findings

- C++ include and namespace migration, entry-point compile definition, changed
  production target links, test target selection and Linux compiler-profile use.
- Lua registration keeps `blackbird` and `arco` pointing at the same table. Legacy
  retained scripts retain their API entry point; new programs use `blackbird`.
- Default session selection uses an existing legacy default only when the new
  default is absent. The launcher and native executable use matching rules.
- Primary/compatibility launchers preserve argument boundaries and executable
  override precedence. Restart continues the selected session as before.
- Checkout compatibility symlink allows retained absolute workflow paths to keep
  resolving. No retained identity, record tag, journal magic, or head/proposal magic
  was changed in the examined diff.

## Retracted suspicion and limits

An initial suspicion that Linux `cmake -E copy_if_different` necessarily overwrites
the running executable inode and fails with `ETXTBSY` was directly tested and
retracted. An isolated Neuroses temporary copy of `/bin/sleep` was running while
the installed CMake replaced its path with `/bin/true`; CMake returned 0. No
product or operator process participated. Do not treat that suspicion as a defect.

This review did not run authenticated requests, migrate real sessions, review
sprite/profiler implementation, or repeat settled terminal rendering analysis.
The owner performs the implementation recheck; this report does not demand a
second review round or an assurance campaign.
