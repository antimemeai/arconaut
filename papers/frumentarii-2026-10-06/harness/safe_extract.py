#!/usr/bin/env python3
"""Study-corpus extraction only; never imports or runs acquired source."""
import argparse, pathlib, tarfile, json
parser=argparse.ArgumentParser();parser.add_argument('archive');parser.add_argument('target');args=parser.parse_args()
target=pathlib.Path(args.target);target.mkdir(parents=True,exist_ok=True);omitted=[]
with tarfile.open(args.archive) as archive:
 for item in archive.getmembers():
  parts=pathlib.PurePosixPath(item.name).parts[1:]
  if not parts:continue
  if any(x in ('.git','__MACOSX','.DS_Store','__pycache__') for x in parts):
   omitted.append({'path':item.name,'reason':'nested metadata or detritus'});continue
  item.name=str(pathlib.PurePosixPath(*parts))
  try:archive.extract(item,target,filter='data')
  except tarfile.FilterError as e:omitted.append({'path':item.name,'reason':str(e)})
print(json.dumps({'omitted':omitted},indent=2))
