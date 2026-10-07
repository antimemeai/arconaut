# Debug machinery is a build choice

Operator requests one debug bool, default false. BLACKBIRD_DEBUG=OFF is the normal
build. It excludes local timing implementation/TLS/buffers and profiling fixture
targets. Inactive timing call sites compile as empty inline operations; correctness
checks, required audit and normal operation timing remain ordinary functionality.
This switch is independent of compiler optimization level.

Explicit debug, sanitizer/fuzz and profile presets choose ON. Release explicitly
chooses OFF, so reconfiguring an old release cache cannot preserve enabled hooks.
Profiler symbols/frame pointers and sanitizer modes require the debug switch;
inconsistent choices fail configuration rather than sneak into normal builds.
Even ON does not start recording: BLACKBIRD_LOCAL_TIMING names a fresh output file
to enable the native sink. OS profiler collectors remain explicit development tools.

Implementation plan: conditional source/targets and public numeric compile define;
header supplies empty inline timing facade when OFF, full API when ON; compile
worker sink binding only ON. Direct checks: both configurations build; normal
compile graph/symbols omit implementation/fixtures, runtime env cannot create
telemetry in normal binary; debug clock/allocation/output oracle still passes.
Run existing affected Beads/process/terminal/context/coding/retained checks once
after integration. One focused code review and findings-driven recheck only.

Delivery: both OFF release and ON optimized-profile builds pass. OFF compile
graph omits timing/profiling/fault/fuzz sources and runtime symbols; even setting
BLACKBIRD_LOCAL_TIMING creates no file. Fresh CMake defaults OFF; profiling/OFF
configuration is rejected. ON timing oracle passes and explicit recording works.
One focused review caught two old unconditional fault/fuzz targets; both gated.
Beads/tools/coding/context/retained integration checks pass; terminal recheck
passes after preserving existing command ordering with Beads appended. Native
readonly ready/show smoke passed against installed bd0.58; no test issue writes.
