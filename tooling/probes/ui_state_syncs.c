/* macOS-only measurement shim: count UI-state fsync calls, not request bytes.
 * Build: cc -dynamiclib -O2 tooling/probes/ui_state_syncs.c -o /tmp/ui-syncs.dylib
 * The launcher probe opts in with --sync-probe; production does not load it.
 */
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if !defined(__APPLE__)
#error "This measurement shim uses macOS F_GETPATH and dyld interposition"
#endif
static int counted_fsync(int fd) {
  char path[PATH_MAX];
  const char *log = getenv("BLACKBIRD_UI_SYNC_LOG");
  if (log && fcntl(fd, F_GETPATH, path) == 0 && strstr(path, "/ui-state.json.arco-")) {
    int out = open(log, O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC, 0600);
    if (out >= 0) { (void)write(out, "ui-state-fsync\n", 15); (void)close(out); }
  }
  return fsync(fd);
}
__attribute__((used)) static struct {
  const void *replacement;
  const void *original;
} interpose __attribute__((section("__DATA,__interpose"))) = {
  (const void *)counted_fsync, (const void *)fsync
};
