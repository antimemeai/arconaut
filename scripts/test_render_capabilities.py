"""Direct artifact oracles for inventory completeness, attribution and source bounds."""
import csv
import importlib.util
import json
import re
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
from urllib.parse import unquote

spec=importlib.util.spec_from_file_location('renderer',Path(__file__).with_name('render_capabilities.py'))
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)

class CorpusTest(unittest.TestCase):
 def setUp(self):
  self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup)
  self.root=Path(self.temp.name);self.study=self.root/'papers/capabilities'
  (self.study/'rows').mkdir(parents=True)
  for rid in ('alpha','beta'):
   (self.root/f'quarantine/{rid}').mkdir(parents=True)
   (self.root/f'quarantine/{rid}/source.txt').write_text('operation\nresult\n')
  self.registry={'scope':'literal two-reference fixture','statuses':m.MARKS,'capabilities':[{'id':'filesystem','meaning':'files'}], 'groups':{'family':['alpha','beta']},'references':[{'id':rid,'group':'family','destination':f'quarantine/{rid}','source':'https://example.invalid/'+rid,'revision':'pinned','version':None} for rid in ('alpha','beta')]}
  self.write_registry()
  for rid in ('alpha','beta'):
   self.write_row(rid)
 def write_registry(self):
  (self.study/'registry.json').write_text(json.dumps(self.registry))
 def write_row(self,rid,change=None):
  row={'id':rid,'kind':'fixture','summary':rid+' source trace','runtime':{'languages':[],'execution':'closed fixture','service_boundary':'consumer'},'inspection':{'files':[f'quarantine/{rid}/source.txt'],'scope':'two lines','limits':'not executed'},'actions':[{'name':'read','surface':'API','input':'path','output':'bytes','lifecycle':'settled','authority':'caller','evidence':['read']}],'capabilities':{'filesystem':{'status':'implemented','basis':'source','detail':'fixture read returns bytes','evidence':['read']}},'evidence':[{'id':'read','path':f'quarantine/{rid}/source.txt','start':1,'end':2,'claim':'read returns bytes'}],'tests':[],'strengths':['explicit bytes'],'limits':['fixture'],'arconaut_questions':['ownership']}
  if change:change(row)
  (self.study/f'rows/{rid}.json').write_text(json.dumps(row))
 def test_literal_output_retains_identity_claim_and_action(self):
  registry,rows,ordered,missing=m.load_corpus(self.root,self.study)
  self.assertEqual(ordered,['alpha','beta']);self.assertEqual(missing,[])
  self.assertEqual(m.render(self.study,registry,rows,ordered,missing),(2,2))
  with (self.study/'matrix.csv').open() as f:matrix=list(csv.DictReader(f))
  self.assertEqual([(r['id'],r['filesystem_status'],r['filesystem_detail']) for r in matrix],[('alpha','implemented','fixture read returns bytes'),('beta','implemented','fixture read returns bytes')])
  self.assertIn('(dossiers/beta.md#filesystem)',(self.study/'matrix.md').read_text())
  self.assertIn('quarantine/beta/source.txt#L1',(self.study/'dossiers/beta.md').read_text())
  self.assertIn('**I — Files**',(self.study/'dossiers/beta.md').read_text())
  with (self.study/'actions.csv').open() as f:actions=list(csv.DictReader(f))
  self.assertEqual([(a['id'],a['name'],a['output']) for a in actions],[('alpha','read','bytes'),('beta','read','bytes')])
 def test_missing_reference_rejected_and_draft_disclosed(self):
  (self.study/'rows/beta.json').unlink()
  with self.assertRaisesRegex(m.InvalidCorpus,'missing references: beta'):m.load_corpus(self.root,self.study)
  registry,rows,ordered,missing=m.load_corpus(self.root,self.study,True)
  m.render(self.study,registry,rows,ordered,missing)
  self.assertIn('INCOMPLETE DRAFT',(self.study/'matrix.md').read_text())
  self.assertEqual(json.loads((self.study/'matrix.json').read_text())['missing'],['beta'])
 def test_cross_reference_evidence_rejected(self):
  self.write_row('alpha',lambda r:r['evidence'][0].update(path='quarantine/beta/source.txt'))
  with self.assertRaisesRegex(m.InvalidCorpus,'source outside reference'):m.load_corpus(self.root,self.study)
 def test_impossible_source_range_rejected(self):
  self.write_row('alpha',lambda r:r['evidence'][0].update(end=3))
  with self.assertRaisesRegex(m.InvalidCorpus,'range beyond EOF'):m.load_corpus(self.root,self.study)
 def test_dangling_or_missing_evidence_rejected(self):
  for evidence in ([],['imaginary']):
   with self.subTest(evidence=evidence):
    self.write_row('alpha',lambda r:r['capabilities']['filesystem'].update(evidence=evidence))
    with self.assertRaises(m.InvalidCorpus):m.load_corpus(self.root,self.study)
 def test_missing_axis_rejected(self):
  self.write_row('alpha',lambda r:r['capabilities'].clear())
  with self.assertRaisesRegex(m.InvalidCorpus,'axes missing/extra'):m.load_corpus(self.root,self.study)
 def snapshot(self):
  return {p.relative_to(self.study):p.read_bytes() for p in self.study.rglob('*') if p.is_file() and p.name not in ('registry.json','alpha.json','beta.json')}
 def test_malformed_values_rejected_before_publication(self):
  m.render(self.study,*m.load_corpus(self.root,self.study))
  previous=self.snapshot()
  changes=[lambda r:r['runtime'].update(languages=[17]),lambda r:r.update(summary={}),lambda r:r.update(actions={}),lambda r:r['actions'][0].update(name=['read']),lambda r:r.update(strengths=[False]),lambda r:r['capabilities']['filesystem'].update(evidence='read')]
  for change in changes:
   with self.subTest(change=change):
    self.write_row('alpha',change)
    with self.assertRaises(m.InvalidCorpus):m.load_corpus(self.root,self.study)
    self.assertEqual(self.snapshot(),previous)
 def test_reordered_axes_retain_literal_labels(self):
  self.registry['capabilities']=[{'id':axis,'meaning':axis} for axis in reversed(list(m.LABELS))]
  self.write_registry()
  for rid in ('alpha','beta'):
   self.write_row(rid,lambda r:r.update(capabilities={axis:{'status':'implemented','basis':'fixture','detail':'fixed claim','evidence':['read']} for axis in m.LABELS}))
  m.render(self.study,*m.load_corpus(self.root,self.study))
  dossier=(self.study/'dossiers/alpha.md').read_text()
  self.assertIn('### filesystem\n\n**I — Files**',dossier)
  self.assertIn('### time-order\n\n**I — Time/order**',dossier)
  self.assertIn('**Files** (`filesystem`)',(self.study/'matrix.md').read_text())
 def test_inconsistent_group_rejected(self):
  self.registry['references'][0]['group']='different-family';self.write_registry()
  with self.assertRaisesRegex(m.InvalidCorpus,'group disagrees'):m.load_corpus(self.root,self.study)
 def test_custom_study_depth_and_reserved_source_characters(self):
  destination=self.root/'deep/reports/study';destination.parent.mkdir(parents=True)
  self.study.rename(destination);self.study=destination
  rel='quarantine/alpha/source notes[1].txt'
  (self.root/'quarantine/alpha/source.txt').rename(self.root/rel)
  def change(row):
   row['inspection']['files']=[rel];row['evidence'][0]['path']=rel
   row['tests']=[{'path':rel,'focus':'literal fixture','oracle':'two lines','executed':False}]
  self.write_row('alpha',change)
  m.render(self.study,*m.load_corpus(self.root,self.study),root=self.root)
  dossier=(self.study/'dossiers/alpha.md').read_text()
  encoded='source%20notes%5B1%5D.txt'
  self.assertEqual(dossier.count(encoded),2)
  self.assertIn('source notes\\[1\\].txt',dossier)
  for destination in re.findall(r'\]\(([^)]+)\)',dossier):
   if encoded in destination:
    path=unquote(destination.split('#')[0])
    self.assertEqual((self.study/'dossiers'/path).resolve(),(self.root/rel).resolve())
 def test_draft_removes_only_owned_stale_dossiers(self):
  m.render(self.study,*m.load_corpus(self.root,self.study))
  (self.study/'dossiers/notes.md').write_text('human notes')
  (self.study/'rows/beta.json').unlink()
  m.render(self.study,*m.load_corpus(self.root,self.study,True))
  self.assertFalse((self.study/'dossiers/beta.md').exists())
  self.assertEqual((self.study/'dossiers/notes.md').read_text(),'human notes')
 def test_construction_failure_preserves_previous_generation(self):
  m.render(self.study,*m.load_corpus(self.root,self.study));previous=self.snapshot()
  self.write_row('alpha',lambda r:r.update(summary='new generation'))
  corpus=m.load_corpus(self.root,self.study)
  with patch.object(m.csv,'DictWriter',side_effect=RuntimeError('construction failed')):
   with self.assertRaisesRegex(RuntimeError,'construction failed'):m.render(self.study,*corpus)
  self.assertEqual(self.snapshot(),previous)
 def test_evidence_heading_collisions_rejected(self):
  for other in ('READ','re.ad'):
   with self.subTest(other=other):
    self.write_row('alpha',lambda r:r['evidence'].append(dict(r['evidence'][0],id=other)))
    with self.assertRaisesRegex(m.InvalidCorpus,'evidence heading collision'):m.load_corpus(self.root,self.study)
 def test_punctuated_evidence_citation_matches_literal_heading(self):
  def change(row):
   row['evidence'][0]['id']='Read.result'
   row['actions'][0]['evidence']=['Read.result']
   row['capabilities']['filesystem']['evidence']=['Read.result']
  self.write_row('alpha',change)
  m.render(self.study,*m.load_corpus(self.root,self.study))
  dossier=(self.study/'dossiers/alpha.md').read_text()
  self.assertIn('### Evidence Read.result\n',dossier)
  self.assertEqual(dossier.count('[Read.result](#evidence-readresult)'),2)

if __name__=='__main__':unittest.main()
