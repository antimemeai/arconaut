# Participants and operating modes

2026-10-06 discussion sketch following the operator's requirements. Cross-provider
spawning and configurable limits are requirements; API shapes, trigger mechanics
and attachment semantics below are proposals to specify and test before coding.
Current compaction work continues unchanged.

## A participant can choose any configured colleague

Participant model/provider, programmable workflow, context lineage, tools, working
environment and operating profile are independent selections. OpenAI can request
Kimi; Kimi can request Claude; Claude can request MiMo; any can request another
participant using its own provider. No hardcoded pairing matrix or privileged
model acts as sole dispatcher. The initial preferred roster is Kimi, MiMo, ChatGPT,
Claude and Grok; future adapters can extend it without changing the agent topology.

The requesting model invokes an Arco operation. Arco creates the child identity,
selected context and workflow, then makes calls through the chosen actual adapter.
This is orchestration by Arco, not a demand that an upstream vendor expose a
cross-vendor child-agent API. Native provider requests and external CLI colleagues
can share lifecycle semantics while exposing their actual differing capabilities.
Errors name missing configuration, unsupported capability or upstream refusal.

Spawning creates lineage and a default lifecycle relationship. It does not force
all communication through the parent. Peers can exchange addressed messages or
join rooms; each retains its own request/context/program attribution. Parent exit,
child detachment, cancellation, waiting and remote uncertainty need explicit
contracts, never an assumption that a completed parent means all descendants
stopped. Models can request colleagues during normal operation, not only autodroit.

## Limits are inspectable configuration

Support operator profiles for concurrent participants, nesting, request/usage/time
budgets, tools, workspace access, context-sharing selection and continuation or
detachment. Where meaningful allow an explicit unbounded policy; actual provider
quotas and machine resources still apply. Usage must not masquerade as measured
currency spend when price/account data is unavailable.

Resolve each participant's effective profile and show its provenance. Model
configuration/modification agency operates within the operator's chosen envelope;
there is no per-spawn approval dialogue. Routine change activation follows current
turn/affected-workflow conclusion, with explicit interrupt-and-apply-now. Specify
exhaustion as queue/pause/refusal/cancellation per policy; never silently drop work
or promise cancellation of an unobserved remote effect.

An upstream model's context/tool/reasoning/stream capabilities and provider account
limits are reported truthfully. The harness should avoid adding arbitrary topology
or model-choice ceilings. It cannot make an upstream model obey instructions or
supply an unavailable inference capability.

## Station mode

Station mode makes Arco a background participant whose workflow admits work from
configured feeds/triggers. Examples: room/mail arrival, issue changes, repository
or build events, schedules, or an external service event. Idle listening does not
require continuously running inference. The trigger adapter supplies source and
event identity, payload and observed cursor; the workflow chooses ignore, gather,
queue, start, notify, or explicit interruption.

Retain trigger input and resulting admission. Reconnect distinguishes consumed,
pending and unknown events, using the source's real replay/dedup semantics. A
repeated event ID is not a license to repeat an external effect; distinct events
can legitimately concern the same artifact. Coalescing and debounce are explicit
program policy. Busy-participant triggers queue by default; overload disposition
and cursor advancement must be observable. Existing request inputs do not change
when a new event arrives.

Station mode consumes external feeds/computation. It does not become the feeds'
control plane or stop shared services during its own refit. UI absence does not
mean absent operator control: attach, inspect, steer, pause admission and recover
pending work. Programmatic/rageshake observations remain available in the background.
A background job detached from today's terminal is not by itself station mode;
actual trigger admission is the distinguishing mechanism.

## Campaign mode

Campaign mode is the operator-in-seat session: conversation, steering, queued
prompts, interrupts, plans and visible progress. It can launch autonomous workflows
and colleague work while remaining an interactive working session. It is not
restricted to synchronous request/answer exchanges.

Proposed attachment semantics: operator presence and scheduling profile are separate.
Attach to a station to inspect/steer without restarting it; explicitly select campaign
scheduling when taking the seat. Define whether feeds remain admitted, queued or
paused during that transition. Leaving the terminal can continue already-admitted
work; starting feed-driven admission remains a deliberate station-profile choice.
No duplicate conversation owner appears merely because another UI connects.

Autodroit is another independent dimension: either mode can run manual experiments
or a model-governed research program. A participant need not rebuild/recreate its
identity, context or known work merely to change operating mode.

## Productive scenario and direct checks

A station receives a build-failure event, preserves source evidence and dispatches
Kimi to diagnose it. Kimi requests a separate ChatGPT or Claude colleague for a
narrow disagreement. The station carries out a real repair with the chosen tool/
workspace profile. Operator attaches mid-work, changes priority in campaign mode,
then leaves with an explicit station scheduling choice. The same context/identity
and work remain inspectable.

Check all configured caller/callee combinations at the actual adapter seam with
fake providers, then selected live combinations through existing configured auth.
Use externally counted dispatches for repeated/uncertain feed events, busy-target
queues, cursor/reconnect gaps and limit exhaustion. Check effective policy per
participant, independent context, actual inclusion of steering and change timing.
Attach/disconnect must not duplicate work or invent settlement. Exact supported
live combinations depend on the adapters/account setup, not marketing labels.

Grounding: CORE_DESIGN's participant/lifecycle invariants, BEHAVIOR's live room and
turn-redefinition scenarios, capability studies on Dagger/AgentPool/Oh My Pi,
coordination-services and NTM's uncertain delivery. The acquired product named
Station is a source comparator; our station mode has the operator's meaning and
does not adopt that ecosystem's roles, cooldowns or governance.

## Multiplayer clarification

Operator2026-10-06: n Arconauts sharing development/build over network, with all
agent-agent, human-human and human-agent communication. n=2 grounds the first
scenario. Multiple running Arco runtimes are first-class participants; shared
project/build and room facts do not imply shared/magically merged local contexts.
The spawning hierarchy does not define the communication graph. A UI attaching
to one participant is not by itself multiplayer. Network/workspace/build coordination
and reconnect/refit semantics remain design work. The operator subsequently
selected P2P with end-to-end encryption as the multiplayer baseline; see below.

HUD is thematic project-set current events (CVE, arXiv, finance, etc.), separate
from participant runtime status. Optional integration packages provide sources
and capabilities when installed/configured; no every-install integration bundle.
Read papers/integrations-2026-10-06/SYNTHESIS.md and its source studies.

### P2P, encrypted conversation and optional deeper collaboration

Operator clarification2026-10-06: multiplayer is peer-to-peer and end-to-end
encrypted. Its simplest useful form is harness-integrated IRC-style conversation
among humans and models. This describes interaction, not adoption of plain IRC
transport, a centralized plaintext server, or an IRC implementation dependency.
Rendezvous/relay details, peer identities, key lifecycle and cryptographic
implementation remain design work; any relay must not receive plaintext content.

Optional deeper collaboration (operator calls this “delve”) shares a problem,
file, source proposal or build evidence. Chat alone is a valid multiplayer mode;
shared files/builds are additional capabilities, not prerequisites for joining.
Shared artifacts need explicit scope, versions and provenance, with independent
local contexts and existing default turn/workflow-boundary admission.

Optional inter-Arco model mail addresses a model in another live session without
requiring participation in a continuously active room. Sending, receiving, model
context inclusion and acting remain distinct observable stages. Mail transport
must not itself merge histories or imply immediate interruption. Offline durable
mail is not promised by the live-session requirement. P2P/E2EE requirements apply
to shared message/artifact content; exact metadata exposure remains to be designed.
