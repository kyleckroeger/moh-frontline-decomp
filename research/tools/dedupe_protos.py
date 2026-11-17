#!/usr/bin/env python3
"""dedupe_protos.py FILE HEADER...: delete FILE's own one-line or multi-line prototypes of functions
the given SDK headers declare (used when moving a file onto the SDK headers, research/notes/sdk-views.md).
Check every removal with one_unit.py: a prototype with a different return type changes nothing in the
bytes, but one the file defines itself must stay."""
import re,sys
f=sys.argv[1]; names=set()
for h in sys.argv[2:]:
    for m in re.finditer(r'\b([A-Za-z_]\w*)\s*\([^;{]*\)\s*;', open(h).read()):
        names.add(m.group(1))
s=open(f).read()
out=[];i=0;removed=[]
lines=s.split('\n'); k=0
while k<len(lines):
    l=lines[k]
    m=re.match(r'^(?:extern\s+)?[A-Za-z_][\w\s\*]*?\b([A-Za-z_]\w*)\s*\(',l)
    if m and m.group(1) in names and m.group(1) not in ('void','int','long','char','short','unsigned','float','double','signed','bool','const') and not l.rstrip().endswith('{') and not l.startswith(' '):
        j=k
        while not lines[j].rstrip().endswith(';'):
            if lines[j].rstrip().endswith('{'): break
            j+=1
        if lines[j].rstrip().endswith(';'):
            removed.append(m.group(1)); k=j+1; continue
    out.append(l); k+=1
open(f,'w').write('\n'.join(out))
print('removed',len(removed),sorted(set(removed)))
