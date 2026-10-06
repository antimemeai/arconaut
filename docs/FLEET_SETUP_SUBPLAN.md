# Fleet tooling setup

Status after operator steering on 2026-10-01: Runpod provisioning and mutation are
deferred from the current path to working Arconaut. No Runpod pod was created.
The operator accepts responsibility for node termination; the automatic lifecycle
machinery proposed below is retired discussion, not required work. Neuroses access
and minimal Linux development-tool installation succeeded and remain useful for
general Linux checks. Do not resurrect cloud plumbing as a prerequisite to U0–U9.

2026-10-01. Operator selects Neuroses over Tailscale for general Linux checks and
Runpod's existing credit account for mutation. This finishes the previously open
resource selection for the rigor setup; production units retain their own oracles.

Neuroses is reachable through the existing SSH alias and key. Ubuntu 24.04 x86_64
has GCC but no Clang/CMake/Ninja. Install only the necessary development packages
without upgrading unrelated software, then qualify a deliberately declared Linux
compiler/profile. Jobs use isolated temporary workspaces and bounded build/test
parallelism; existing services and their data are not project fixtures. Do not
attribute Mac custody, SDK or pause behavior to Linux results.

Runpod CLI 2.3.0 and existing private credential configuration authenticate. The
account has no pods and about $73 credit; no new funding or billing configuration
is requested. Use disposable CPU-only pods, no GPU or persistent/network volume.
Before a job, record account/pod status without secrets; create under a unique
Arconaut job name and set provider-side automatic termination within 45 minutes.
Require returned rate <= $0.25/hour before transferring work; stop/delete immediately
on unexpected shape/rate. Always delete the owned pod after downloading results,
including failures, and inspect authoritative absence. Unknown creation/cleanup
must reconcile by the exact unique job name, never blindly create another pod.

The owned launcher uses the existing CLI rather than reading the API key or adding
an SDK. CPU pod SSH must be provided by the image/start command; REST CPU creation
does not guarantee managed SSH. Inject only the operator's existing public key,
retain private authentication locally, use bounded bootstrap/readiness, and confine
all job source/results to the pod. A local invocation may coordinate compilation
but may never compile or execute mutants on this laptop. No shared server becomes
the mutation worker merely because it runs Linux.

Qualify actual lifecycle first: create CPU worker, observe SSH readiness and
Linux/tool identity, run a known-result native capability fixture there, fetch its
result, terminate, verify absence and record elapsed/rate. A later real mutation
campaign edits selected production predicates under TESTING_PLAN, compiles/runs
each mutant on Runpod and distinguishes killed/survived/build-failed/timed-out
outcomes. Baseline must pass remotely before interpretation; test failures from
unrelated bootstrap errors are not killed mutants. Do not substitute the lifecycle
fixture for characterized product oracles.

Meaningful red cases: missing configuration fails before remote mutation; conflicting
job identity/rate rejects; cleanup remains explicit after a failed remote test.
General Linux tests run independently on Neuroses. Independent Kimi review examines
launcher job custody, billing cleanup, source scope and actual evidence before the
setup is called complete. Journals/beads retain resource choices and achieved scope.
