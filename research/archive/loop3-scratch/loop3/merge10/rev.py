"""rev.py in out first_marker: reverse the top-level definitions from first_marker to the end.
A chunk is a run of lines ending with a column-0 '}' line; a leading comment/blank stays with its chunk.
Inline helper chunks (marked by 'inline ' at line start) stay attached BEFORE the next chunk."""
import sys
a=open(sys.argv[1]).read(); m=sys.argv[3]
i=a.index(m); head, body=a[:i], a[i:]
chunks=[];cur=[]
for line in body.splitlines(True):
    cur.append(line)
    if line.startswith('}') or (line[:1] not in ' \t\n/' and line.rstrip().endswith('}') and '{' in line):
        chunks.append(''.join(cur)); cur=[]
tail=''.join(cur)
groups=[];pend=''
for c in chunks:
    s=c.lstrip('\n')
    if s.startswith('inline ') or s.startswith('static inline') :
        pend+=c; continue
    groups.append(pend+c); pend=''
groups=[g.strip('\n')+'\n' for g in groups]
open(sys.argv[2],'w').write(head+'\n'.join(reversed(groups))+tail+(pend))
