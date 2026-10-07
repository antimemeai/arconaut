Three findings:

- **P2 — Timestamp includes probe work.** In [startup_loading.cpp:64](/Users/patrickbeam/projects/blackbird/context/performance-armada/pool/slot-1/tooling/probes/startup_loading.cpp:64), the provider scans every committed fact and serializes input before taking `admitted`. Thus `admitted_ms` includes history-dependent probe work after provider entry. Take the timestamp immediately on entry.

- **P2 — Admission oracle lacks exact linkage.** That scan accepts any historical admission; it does not check the current request’s admission, open event, or terminal success. The inspected engine path does durably admit/open before calling the provider and records its returned result as success, but the probe would miss regressions in those relationships. Assert the current attempt’s request bytes and lifecycle.

- **P2 — Full-packet retention test checks only a prefix.** [context_test.cpp:327](/Users/patrickbeam/projects/blackbird/context/performance-armada/pool/slot-1/tests/context_test.cpp:327) compares only the first 65,536 history bytes. The newly added candidate alone is that size, so its complete contents cannot be covered. Total-length equality cannot detect same-length corruption beyond the page. Compare all pages against the original packet.

No syntax, UTF-8, duplicate-key, bound, root-only projection, or fixed16 overread defect found by inspection. Context restoration retains the original payload and publication checks. Escaped discarded strings still allocate a temporary decoded string.

One static review only; no edits, builds, execution, or speed certification.