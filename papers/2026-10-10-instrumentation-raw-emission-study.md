# Raw metric emission: older systems study and revised boundary

2026-10-10. Research/document correction only. Operator rejects OTel/Prometheus
as the architectural foundation and all transformations at or near the producer.
Capture raw metric events; get them out of Blackbird's main processes as fast as
physically possible, losslessly; preserve raw records on disk in a separate
recorder; perform transformations only in a tertiary process. No product code,
reference execution, benchmark, evaluator, library adoption or source test run.

## What was read and what transfers

**Saltzer and Gintell, The Instrumentation of Multics, 1969/1970.** Read the
hardware/software instrumentation sections and observations in the
[author's definitive text](https://www.mit.edu/~Saltzer/publications/instrumentation.html),
with acquired M0112 preprint. The GE-645 external data channel lets a PDP-8
observe memory without executing probe code on the measured CPU. This is a
strong example of moving measurement work away from the subject. It is a slow
snapshot facility, not a complete event stream. The module histograms and
moving averages are in-system transforms; the last-256-page ring overwrites
history. Those are counterexamples to the operator's required architecture,
not adopted mechanisms. Useful lesson: measurement interference is real, and
raw acquisition must be distinguished from downstream interpretation.

**Ritchie and Thompson, The UNIX Time-Sharing System, 1974/1978, and V7
pipe.c, 1979.** Read processes/pipes and filters in the original paper, and the
complete historical pipe implementation. A byte stream between separate
processes is a small interface. V7 writep waits when full, sends EPIPE/SIGPIPE
when the reader disappears and wakes readers after progress. It does not solve
disk durability or modern multiwriter record framing. Its inode-based buffer,
locking and PIPSIZ details are historical, not a proposal for Blackbird.
The transferable boundary is producer -> byte transport -> independent consumer.
[Paper](https://cm-bell-labs.github.io/who/dmr/cacm.pdf),
[historical source](https://www.tuhs.org/cgi-bin/utree.pl?file=V7/usr/sys/sys/pipe.c).

**Lampson, Hints for Computer System Design, 1983.** Read the interface, speed
and logging sections in the author's revised version. The relevant mechanisms
are narrow interfaces, separate resources, work in independent background
processes, sequential batches and append-only logging. Put this thinking ahead
of a metric SDK's instrument/aggregation/reader abstractions. Background work
still needs a precise handoff and overload behavior. Batch disk writes can
preserve original record bytes; batching is not permission to aggregate values.
[Author PDF](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/acrobat-17.pdf).

**Lamport, Specifying Concurrent Program Modules, 1983.** Read introduction
and section3.4, FIFO specification and ring-buffer implementation. A producer
must not finish PUT before publication; a consumer must not see a partial element.
Progress requires space/data, rather than an impossible guarantee that a finite
queue accepts infinitely many writes without a reader. The ring publishes its
tail after writing the element. These directly identify useful transport oracles.
The paper's atomic primitives, integer arithmetic and busy-wait implementation
must not be copied as modern C++ shared-memory code; memory ordering, bounded
sequence wrap and process lifetime need explicit qualification.
[Author PDF](https://lamport.azurewebsites.net/pubs/spec.pdf).

**Ousterhout et al., A Trace-Driven Analysis of the UNIX 4.2BSD File System,
1985.** Read tracing and missing-data sections of the
[1993-dated reprint](https://courses.cs.umbc.edu/graduate/691f/Spring00/Papers/ousterhout-sosp85.pdf)
through browser text; acquired the scanned original Berkeley report and checked
its title page. The mechanism separates trace acquisition from analyzer and
cache simulator programs. It deliberately omits individual reads/writes and
reconstructs access ranges from open/close/seek. That reduces volume but loses
exact access timing and order. Borrow the record-first/analyze-later separation;
do not import its event omission or estimates as lossless raw emission. The
reprint's header date is recorded, not misrepresented as an original1985 PDF.

**Hagmann, Reimplementing the Cedar File System Using Logging and Group
Commit, 1987.** Read design overview, log recovery and group commit sections.
Sequential logging and group commit amortize disk work. The paper explicitly
tolerates up to half a second of lost recent changes in its workstation use case,
and permits a client to force the log. This is a concrete warning that periodic
flush is not unconditional crash losslessness. Apply batch/append ideas to the
separate recorder only; do not import its crash-loss allowance or make Blackbird
wait for every disk flush without an agreed boundary.
[Paper](https://www.seltzer.com/margo/teaching/CS508.19/papers/hagmann87.pdf).

**Berkeley ktrace/kdump, 1988–1990 source.** Read header acquisition, syscall/return/I/O recording and write failure paths
in kern_ktrace.c, record structures in ktrace.h, and kdump's record-reading and
display loop.
ktrace records syscall inputs and returns separately; kdump reads persisted
headers/payloads and performs naming, formatting and relative-time subtraction.
This is the clearest concrete record/decode split in this study. However,
ktrgetheader allocates, ktrwrite locks a vnode and calls VOP_WRITE on the traced
path, and write errors disable tracing. ktrgenio also skips records on error.
These violate cheap producer work and complete failure capture. Header pointers,
padding and ABI dependence are not an owned portable record format. Keep the
separation, reject those mechanics. Exact paths, SCCS versions and hashes:
[source manifest](2026-10-10-instrumentation-old-unix-sources.json).

## Proposed research direction

The current [design sketch](../docs/INSTRUMENTATION.md) removes producer metric
registries, projected measurements, counters, sums, histogram updates and Lua
projection callbacks. All derived work moves behind raw disk preservation.
The old source census is useful for locating hooks; its aggregation advice is
superseded. No architecture is approved for implementation by this report.

Compare a simple raw-record OS stream with preallocated shared-memory handoff.
The first has syscall/copy cost; the second introduces publication/reclamation,
process lifetime and notification machinery. Measure those exact costs before
picking shared memory or calling anything lock-free. Neither mechanism alone
establishes disk persistence. A recorder must preserve records unchanged before
handing a preserved prefix to a transformer.

The producer should carry existing identity, the raw field value/type/unit and
necessary time observations. Separate raw timestamps become a duration later.
Transport sequence/length/publication state are mechanics, not derived metrics.
Do not route generic native objects, interpret payloads, or retain borrowed
pointers across process/thread handoff. Site/decoder metadata is prepared off the
emission path. Audit integration remains a concrete design question, not a new
permission for a duplicate authoritative metrics journal.

Concrete next design questions: record shape per source site; full-buffer handling
at safe owner boundaries; raw ownership across producer/recorder crashes; append
versus durable acknowledgement; orderly drain and abrupt death; partial writes,
recorder recovery and full disk; preservation of independently emitted failures
when ingest itself fails. No lossy policy or finite arbitrary queue size is
selected. The transformer cannot backpressure the producer through a synchronous
callback. It can lag the already preserved raw prefix.

Direct future checks must compare an independently known raw sequence with disk
records, including concurrent producers, bursts, a stalled recorder, short I/O,
process restart and failure boundaries. Performance checks must report copies,
allocation, synchronization, syscalls/wakeups and producer latency alongside drain
throughput and durable lag. No overhead number is inferred from reference code.

## Acquisition limits and restoration

All sources acquired/read2026-10-10. PDFs are ignored. Parsed text, one title-page
OCR and HTTP failure bodies are ignored context. TUHS HTML responses are retained
intact, with extracted C source kept separately; no full OS source archive,
reference compilation, nested Git metadata or executable was acquired.
Restoration commands and exact extracted-byte hashes are in the source manifest
and QUARANTINE.md. No local source is imported into Blackbird.

The UCSD download labeled cheriton79 contains the complete Thoth paper and only
the opening page of Reed/Kanodia's Eventcounts and Sequencers. It has been named
accordingly. Its abstract does not qualify a full-paper reading or queue design.
ACM full-paper access returned403. Berkeley's original report is image-only;
only its title was OCR'd, while the relevant reprint text was read via browser.
UMBC browser access worked but direct download returned403. Those limitations
are recorded rather than claiming the full scanned texts were studied.

| Ignored local PDF in papers/instrumentation-2026-10-10/ | Version/source | SHA-256 |
| --- | --- | --- |
| `ritchie-thompson-1978-unix.pdf` | [Ritchie/Thompson, 1974 article revised for BSTJ 1978](https://cm-bell-labs.github.io/who/dmr/cacm.pdf) | `44e66fa44a50d3d1b97c51423a6939f12a187748400afb4e0efe28d53cc47196` |
| `lampson-1983-hints-author.pdf` | [Lampson, 1983, slightly revised author version](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/acrobat-17.pdf) | `8a213977a1730adbc67a093a1363549d566b5f15555aecd566aa03b2f8d1eee5` |
| `lamport-1983-concurrent-modules.pdf` | [Lamport, TOPLAS 5(2), April 1983](https://lamport.azurewebsites.net/pubs/spec.pdf) | `914e1033e98e0bdd6421cc5f3590e922d4384e647df2e4a49f1f59a4230d120b` |
| `saltzer-gintell-1969-instrumentation.pdf` | [Saltzer/Gintell, 1969 preprint M0112](https://people.csail.mit.edu/saltzer/CTSS/Multics-Documents/M00s/M0112.pdf) | `4e8e5e86b976a9fa217368d6eeed74386b7b4f720149c45b98bdc23c5b5157d1` |
| `hagmann-1987-cedar-group-commit.pdf` | [Hagmann, SOSP 1987](https://www.seltzer.com/margo/teaching/CS508.19/papers/hagmann87.pdf) | `62e1bd37d9c04c313d561fc806ea0262505cece8d816d10d135657575a5bda37` |
| `ousterhout-1985-traces.pdf` | [Ousterhout et al., Berkeley CSD-85-230, scanned report](https://www2.eecs.berkeley.edu/Pubs/TechRpts/1985/Archive/CSD-85-230.pdf) | `68abaf0d2510a69a743a75bcc82e320402b1a1885d21c1b1b62e2a2b724d2d80` |
| `cheriton-1979-thoth-with-reed-opening.pdf` | [Cheriton et al., Thoth, plus only first Reed/Kanodia page](https://cseweb.ucsd.edu/classes/wi19/cse221-a/papers/cheriton79.pdf) | `e8031ad558b04afd7cf8e3ae3fcfe64f16d2f31fc7bd37c25427e650b4f23d15` |

Restore each PDF by downloading its linked URL to the stated local path with
`curl -fL URL -o PATH`, then verify the SHA-256. Keep source bytes intact and ignore
PDFs. A failed or changed upstream download is an acquisition failure, not a
reason to claim exact historical restoration. The earlier CMU Lampson mirror
was also acquired; the author-hosted PDF above is the study's citation.
