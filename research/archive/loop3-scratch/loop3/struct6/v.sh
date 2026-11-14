# v.sh base name old new ... (files in tlt/), full-file replace
b=$1; n=$2; shift 2
python3 - "$b" "$n" "$@" <<'E' || exit
import sys
b,n=sys.argv[1:3]; a=sys.argv[3:]
s=open('tlt/'+b).read()
for i in range(0,len(a),2):
    assert a[i] in s, a[i]
    s=s.replace(a[i],a[i+1],1)
open('tlt/'+n+'.cpp','w').write(s)
E
sh t.sh $n.cpp $V | tr '\n' ' '; echo
