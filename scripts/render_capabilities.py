#!/usr/bin/env python3
"""Validate and render the authored capability corpus; never execute references."""
import argparse
import csv
import io
import json
import os
import re
from pathlib import Path
from urllib.parse import quote

MARKS = {'implemented':'I','documented':'D','limited':'L','mechanism':'S','unknown':'?','na':'—'}
LABELS = dict(zip(
    ['filesystem','processes','code_actions','persistent_kernel','standing_database',
     'workflow_programming','multi_model','live_collaboration','concurrent_work',
     'steering_interrupt','turn_redefinition','compaction','context_repair',
     'original_audit','audit_query','hot_change','rebuild_continuity','remote_services',
     'self_improvement','complaints','authority','evaluation','time_order'],
    ['Files','OS programs','Code actions','Kernel','Standing DB','Workflows',
     'Models','Peer chat','Concurrency','Steer/interrupt','Turn program','Compaction',
     'Repair','Original audit','Audit query','Hot change','Rebuild continuity','Remote',
     'Self-improve','Complaints','Authority','Evaluation','Time/order']))

class InvalidCorpus(ValueError):
    pass

def require(value, message):
    if not value:
        raise InvalidCorpus(message)

def read_json(path):
    try:
        return json.loads(path.read_text())
    except (OSError, ValueError) as exc:
        raise InvalidCorpus(f'{path}: {exc}') from exc

def object_value(value, where):
    require(isinstance(value, dict), f'{where}: expected object')
    return value

def list_value(value, where):
    require(isinstance(value, list), f'{where}: expected list')
    return value

def text_value(value, where, nullable=False):
    if nullable and value is None:
        return value
    require(isinstance(value, str) and bool(value.strip()), f'{where}: expected nonempty string')
    return value

def texts(value, where):
    list_value(value, where)
    for item in value:
        text_value(item, where)
    return value

def identifier(value, where):
    text_value(value, where)
    require(re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9_.-]*', value), f'{where}: invalid identifier')
    return value

def evidence_anchor(eid):
    # GitHub-flavored Markdown lowercases headings and removes punctuation such
    # as periods. Evidence IDs contain only ASCII letters, digits, _, - and .
    return 'evidence-'+eid.lower().replace('.','')

def load_corpus(root, study, draft=False):
    registry = object_value(read_json(study / 'registry.json'), 'registry')
    text_value(registry['scope'], 'registry.scope')
    statuses = object_value(registry['statuses'], 'registry.statuses')
    require(set(statuses) == set(MARKS), 'registry status vocabulary mismatch')
    for name, description in statuses.items():
        text_value(description, f'registry.statuses/{name}')
    references = list_value(registry['references'], 'registry.references')
    for ref in references:
        object_value(ref, 'registry reference')
        identifier(ref['id'], 'reference.id')
        for field in ('group','destination'):
            text_value(ref[field], f'{ref["id"]}/{field}')
        for field in ('source','revision','version'):
            text_value(ref[field], f'{ref["id"]}/{field}', nullable=True)
        require(not Path(ref['destination']).is_absolute(), 'reference destination must be relative')
        require((root/ref['destination']).resolve().is_relative_to(root.resolve()), 'reference destination outside root')
    ids = [r['id'] for r in references]
    require(len(ids) == len(set(ids)), 'duplicate registry reference')
    list_value(registry['capabilities'], 'registry.capabilities')
    for axis in registry['capabilities']:
        object_value(axis, 'registry axis')
        identifier(axis['id'], 'axis.id')
        text_value(axis['meaning'], f'axis/{axis["id"]}/meaning')
    axes = [a['id'] for a in registry['capabilities']]
    require(len(axes) == len(set(axes)), 'duplicate registry axis')
    object_value(registry['groups'], 'registry.groups')
    for name, members in registry['groups'].items():
        text_value(name, 'group name')
        texts(members, f'group/{name}')
    grouped = [i for group in registry['groups'].values() for i in group]
    require(sorted(grouped) == sorted(ids), 'groups must partition registered references')
    membership = {rid:name for name,members in registry['groups'].items() for rid in members}
    require(all(ref['group'] == membership[ref['id']] for ref in references), 'reference group disagrees with group partition')
    reference_by_id = {r['id']:r for r in references}
    rows, lengths = {}, {}
    for path in sorted((study/'rows').glob('*.json')):
        row = object_value(read_json(path), str(path))
        rid = row.get('id')
        identifier(rid, f'{path}/id')
        require(rid in reference_by_id, f'{path}: unregistered id {rid}')
        require(path.stem == rid, f'{path}: filename/id mismatch')
        require(rid not in rows, f'{rid}: duplicate row')
        for field in ['kind','summary','runtime','inspection','actions','capabilities','evidence','tests','strengths','limits','arconaut_questions']:
            require(row.get(field) is not None, f'{rid}: missing {field}')
        for field in ('kind','summary'):
            text_value(row[field], f'{rid}/{field}')
        for field in ('runtime','inspection','capabilities'):
            object_value(row[field], f'{rid}/{field}')
        for field in ('actions','evidence','tests'):
            list_value(row[field], f'{rid}/{field}')
        for field in ('strengths','limits','arconaut_questions'):
            texts(row[field], f'{rid}/{field}')
        texts(row['runtime']['languages'], f'{rid}/runtime.languages')
        for field in ('execution','service_boundary'):
            text_value(row['runtime'][field], f'{rid}/runtime.{field}')
        for field in ('scope','limits'):
            text_value(row['inspection'][field], f'{rid}/inspection.{field}')
        texts(row['inspection']['files'], f'{rid}/inspection.files')
        require(set(row['capabilities']) == set(axes), f'{rid}: axes missing/extra {set(axes)^set(row["capabilities"])}')
        evidence = {}
        anchors = set()
        def source_path(rel, range_end=None):
            require(isinstance(rel,str), f'{rid}: source path must be string')
            require(not Path(rel).is_absolute(), f'{rid}: source path must be relative')
            p = root / rel
            expected = root / reference_by_id[rid]['destination']
            require(p.resolve().is_relative_to(expected.resolve()), f'{rid}: source outside reference {rel}')
            require(p.is_file(), f'{rid}: source missing {rel}')
            if range_end is not None:
                if p not in lengths:
                    lengths[p] = len(p.read_bytes().splitlines())
                require(range_end <= lengths[p], f'{rid}: range beyond EOF {rel}:{range_end} (actual {lengths[p]})')
        for e in row['evidence']:
            object_value(e, f'{rid}/evidence')
            identifier(e['id'], f'{rid}/evidence.id')
            text_value(e['claim'], f'{rid}/evidence.claim')
            require(e['id'] not in evidence, f'{rid}: duplicate evidence {e["id"]}')
            anchor=evidence_anchor(e['id'])
            require(anchor not in anchors, f'{rid}: evidence heading collision {e["id"]}')
            anchors.add(anchor)
            require(type(e['start']) is int and type(e['end']) is int and 1 <= e['start'] <= e['end'], f'{rid}: invalid range {e}')
            require(e['claim'], f'{rid}: empty evidence claim')
            source_path(e['path'],e['end'])
            require(e['path'] in row['inspection']['files'], f'{rid}: evidence not in inspected files {e["path"]}')
            evidence[e['id']] = e
        require(evidence, f'{rid}: no source evidence')
        for rel in row['inspection']['files']:
            source_path(rel)
        for axis,cell in row['capabilities'].items():
            object_value(cell, f'{rid}/{axis}')
            for field in ('status','detail','basis'):
                text_value(cell[field], f'{rid}/{axis}/{field}')
            texts(cell['evidence'], f'{rid}/{axis}/evidence')
            require(cell['status'] in MARKS, f'{rid}/{axis}: unknown status')
            require(cell['detail'] and cell['basis'], f'{rid}/{axis}: unexplained cell')
            if cell['status'] in ('implemented','documented','limited','mechanism'):
                require(cell['evidence'], f'{rid}/{axis}: unsupported positive/limited cell')
            require(set(cell['evidence']) <= set(evidence), f'{rid}/{axis}: dangling evidence')
        require(row['actions'], f'{rid}: no action/artifact surface described')
        for action in row['actions']:
            object_value(action, f'{rid}/action')
            for field in ('name','surface','input','output','lifecycle','authority'):
                text_value(action.get(field), f'{rid}/action/{field}')
            texts(action.get('evidence'), f'{rid}/action/evidence')
            for field in ('name','surface','input','output','lifecycle','authority','evidence'):
                require(action.get(field), f'{rid}: incomplete action {action.get("name")}/{field}')
            require(set(action['evidence']) <= set(evidence), f'{rid}: action dangling evidence')
        for test in row['tests']:
            object_value(test, f'{rid}/test')
            for field in ('path','focus','oracle'):
                text_value(test[field], f'{rid}/test/{field}')
            source_path(test['path'])
            require(test['focus'] and test['oracle'] and test['executed'] is False, f'{rid}: tests must be scoped, read-only records')
            require(test['path'] in row['inspection']['files'], f'{rid}: test not in inspected files')
        rows[rid] = row
    missing = sorted(set(ids)-set(rows))
    require(draft or not missing, 'missing references: '+', '.join(missing))
    ordered = [rid for group in registry['groups'].values() for rid in group if rid in rows]
    return registry, rows, ordered, missing

def md(value):
    return str(value).replace('|','\\|').replace('\n','<br>')

def local_link(label, path, root, directory, line=None):
    label = label.replace('\\', '\\\\').replace('[', '\\[').replace(']', '\\]')
    destination = quote(os.path.relpath(root/path, directory), safe='/')
    if line is not None:
        destination += f'#L{line}'
    return f'[{label}]({destination})'

def render(study, registry, rows, ordered, missing, *, root=None):
    root = root or study.parent.parent
    dossier_directory = study/'dossiers'
    outputs = {}
    refs={r['id']:r for r in registry['references']}
    axes=[a['id'] for a in registry['capabilities']]
    labels={a:LABELS.get(a,a) for a in axes}
    state='INCOMPLETE DRAFT' if missing else 'Complete registered corpus'
    intro=f'{state}: {len(rows)}/{len(refs)} references; {len(axes)} axes. I = implementation traced, D = documented, L = material limit, S = supporting mechanism, ? = not established, — = outside role. **No runtime conformance implied by I.**\n\n'
    if missing:
        intro+='Pending: '+', '.join(missing)+'\n\n'
    matrix=['# Action and capabilities matrix\n\n',intro,
        '[Method](README.md) · [Registry](registry.json) · [Action inventory](actions.md) · [CSV](matrix.csv) · [JSON](matrix.json)\n\n',
        'Cells link to scope, limitations and exact evidence. Compare within roles: a supporting runtime and a complete agent do not have interchangeable obligations.\n\n']
    for group, group_ids in registry['groups'].items():
        matrix.append(f'## {group}\n\n')
        for start in range(0,len(axes),6):
            chunk=axes[start:start+6]
            matrix.append('| Reference | '+' | '.join(labels[a] for a in chunk)+' |\n')
            matrix.append('| --- | '+' | '.join('---' for _ in chunk)+' |\n')
            for rid in group_ids:
                if rid not in rows:continue
                row=rows[rid]
                matrix.append(f'| [{md(rid)}](dossiers/{rid}.md) | '+' | '.join(f'[{MARKS[row["capabilities"][a]["status"]]}](dossiers/{rid}.md#{a.replace("_","-")})' for a in chunk)+' |\n')
            matrix.append('\n')
    matrix.append('## Axis definitions\n\n')
    for axis in registry['capabilities']:
        matrix.append(f'- **{labels[axis["id"]]}** (`{axis["id"]}`): {axis["meaning"]}\n')
    outputs[study/'matrix.md']=''.join(matrix)
    outputs[study/'matrix.json']=json.dumps({'scope':registry['scope'],'complete':not missing,'missing':missing,'capabilities':registry['capabilities'],'statuses':registry['statuses'],'references':[dict(reference=refs[rid],**rows[rid]) for rid in ordered]},indent=2)+'\n'
    def csv_view(name, fields, records):
        stream=io.StringIO(newline='');writer=csv.DictWriter(stream,fieldnames=fields);writer.writeheader();writer.writerows(records)
        outputs[study/name]=stream.getvalue()
    records=[]
    for rid in ordered:
        row=rows[rid]; rec={'id':rid,'group':refs[rid]['group'],'kind':row['kind'],'runtime':'; '.join(row['runtime']['languages']),'revision':refs[rid]['revision'] or refs[rid]['version'] or 'unversioned','summary':row['summary']}
        for a in axes:
            cell=row['capabilities'][a]
            for field in ('status','detail','basis','evidence'):rec[f'{a}_{field}']='; '.join(cell[field]) if isinstance(cell[field],list) else cell[field]
        records.append(rec)
    csv_view('matrix.csv',['id','group','kind','runtime','revision','summary']+[f'{a}_{f}' for a in axes for f in ('status','detail','basis','evidence')],records)
    actions=[]
    inventory=['# Native action inventory\n\n',intro,'Every operation family listed here has its input, result/handle, lifecycle, authority and direct evidence in the linked dossier. Generated/plugin catalogs are described through actual discovery/dispatch, not claimed to be manually enumerated in full.\n\n', '| Reference | Role | Exposed operation families |\n| --- | --- | --- |\n']
    for rid in ordered:
        row=rows[rid];ref=refs[rid];lookup={e['id']:e for e in row['evidence']}
        inventory.append(f'| [{md(rid)}](dossiers/{rid}.md#actions) | {md(row["kind"])} | {md("; ".join(a["name"] for a in row["actions"]))} |\n')
        source=f'[{ref["source"]}]({ref["source"]})' if ref['source'] else 'Origin URL unrecorded; local material identified by the registry catalog'
        dossier=[f'# {rid}\n\n',f'{row["summary"]}\n\n',f'Role: {row["kind"]}. Runtime: {", ".join(row["runtime"]["languages"])}.\n\n',f'Pinned source: {source}; revision/version `{ref["revision"] or ref["version"] or "unversioned"}`.\n\n',f'{row["runtime"]["execution"]}\n\n{row["runtime"]["service_boundary"]}\n\n',f'Inspection: {row["inspection"]["scope"]}\n\nLimits of this study: {row["inspection"]["limits"]}\n\n', '## Actions\n\n']
        for a in row['actions']:
            actions.append(dict(id=rid,group=ref['group'],**{k:'; '.join(v) if isinstance(v,list) else v for k,v in a.items()}))
            dossier.extend([f'### {a["name"]}\n\n',f'Surface: {a["surface"]}.\n\n',f'Input: {a["input"]}\n\nResult: {a["output"]}\n\nLifecycle: {a["lifecycle"]}\n\nAuthority: {a["authority"]}\n\n', 'Evidence: '+', '.join(f'[{eid}](#{evidence_anchor(eid)})' for eid in a['evidence'])+'.\n\n'])
        dossier.append('## Capabilities\n\n')
        for a in axes:
            cell=row['capabilities'][a]
            dossier.append(f'### {a.replace("_","-")}\n\n**{MARKS[cell["status"]]} — {labels[a]}** ({cell["basis"]}): {cell["detail"]}\n\n')
            if cell['evidence']:dossier.append('Evidence: '+', '.join(f'[{eid}](#{evidence_anchor(eid)})' for eid in cell['evidence'])+'.\n\n')
        dossier.append('## Inspected test oracles\n\n')
        if not row['tests']:dossier.append('No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.\n\n')
        for t in row['tests']:dossier.append(f'- {local_link(t["path"],t["path"],root,dossier_directory)}: {t["focus"]} Oracle: {t["oracle"]} Read, **not executed**.\n')
        for field,title in [('strengths','Useful mechanisms'),('limits','Material limits'),('arconaut_questions','Arconaut design questions')]:
            dossier.append(f'\n## {title}\n\n');dossier.extend(f'- {item}\n' for item in row[field])
        dossier.append('\n## Evidence\n\n')
        for e in row['evidence']:
            link=local_link(f'{e["path"]}:{e["start"]}–{e["end"]}',e['path'],root,dossier_directory,e['start'])
            dossier.append(f'### Evidence {e["id"]}\n\n{link}: {e["claim"]}\n\n')
        outputs[study/f'dossiers/{rid}.md']=''.join(dossier)
    outputs[study/'actions.md']=''.join(inventory)
    csv_view('actions.csv',['id','group','name','surface','input','output','lifecycle','authority','evidence'],actions)
    # Construct every view before publication: deterministic data errors must not
    # leave a mixture of new and old outputs. Filesystem failures are not a
    # transactional multi-file commit; this is an ordinary research renderer.
    owned_ids=set(refs)
    previous=study/'matrix.json'
    if previous.exists():
        try:
            previous_ids=[row['id'] for row in read_json(previous)['references']]
            owned_ids.update(rid for rid in previous_ids if isinstance(rid,str) and re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9_.-]*',rid))
        except (InvalidCorpus,KeyError,TypeError):
            pass
    dossier_directory.mkdir(parents=True,exist_ok=True)
    for path,content in outputs.items():
        path.write_text(content)
    for rid in owned_ids-set(ordered):
        (dossier_directory/f'{rid}.md').unlink(missing_ok=True)
    return len(actions),sum(len(rows[r]['evidence']) for r in ordered)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1])
    parser.add_argument('--study',type=Path,default=Path('papers/capabilities'))
    parser.add_argument('--draft',action='store_true')
    parser.add_argument('--check-only',action='store_true')
    args=parser.parse_args();root=args.root.resolve();study=root/args.study
    try:
        registry,rows,ordered,missing=load_corpus(root,study,args.draft)
        if args.check_only:print(f'Valid authored corpus: {len(rows)}/{len(registry["references"])}; missing {len(missing)}')
        else:
            actions,evidence=render(study,registry,rows,ordered,missing,root=root)
            print(f'Rendered {len(rows)}/{len(registry["references"])} references, {actions} operation families, {evidence} evidence ranges; complete={not missing}')
    except (InvalidCorpus,KeyError,TypeError) as exc:
        parser.exit(1,f'Invalid corpus: {exc}\n')

if __name__=='__main__':main()
