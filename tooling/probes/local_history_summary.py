#!/usr/bin/env python3
"""Development-only summarizer of native fixture samples; no telemetry collection."""
import json, math, pathlib, sys
base = pathlib.Path(sys.argv[1])
lines = ["| History appends | Context entries | Mode | Action | Clock | n | p50 µs | p95 µs | p99 µs | max µs |", "|---:|---:|:---|:---|:---|---:|---:|---:|---:|---:|"]
summary = []
for history in (0,128,512,2048):
    for mode in ('off','on'):
        rows = [json.loads(x) for x in (base/f'growing-h{history}-{mode}.samples.jsonl').read_text().splitlines()]
        for action in ('append','prepare_subset'):
            for clock in ('wall_ns','cpu_ns'):
                values = sorted(r[clock] for r in rows if r['action']==action)
                q = [values[math.ceil(len(values)*p)-1]/1000 for p in (.5,.95,.99,1)]
                summary.append(dict(history=history, mode=mode, action=action, clock=clock, n=len(values), p50_us=q[0],p95_us=q[1],p99_us=q[2],max_us=q[3]))
                lines.append(f'| {history} | {64+history} | {mode} | {action} | {clock} | {len(values)} | '+ ' | '.join(f'{v:.2f}' for v in q)+' |')
print('\n'.join(lines))
print('\nPrimitive enabled-minus-disabled ns/span (2000 spans; single aggregate per run, no percentile claim):')
for history in (0,128,512,2048):
    primitive = {}
    for mode in ('off','on'):
        rows = [json.loads(x) for x in (base/f'growing-h{history}-{mode}.samples.jsonl').read_text().splitlines()]
        primitive[mode] = next(r for r in rows if r['action']=='primitive_2000')
    print(f"- history {history}: wall {(primitive['on']['wall_ns']-primitive['off']['wall_ns'])/2000:.1f}, CPU {(primitive['on']['cpu_ns']-primitive['off']['cpu_ns'])/2000:.1f}")
(base/'aggregate.json').write_text(json.dumps(summary,indent=2)+'\n')
