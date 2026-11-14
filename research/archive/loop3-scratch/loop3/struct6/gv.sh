# gv.sh base name old new ...
b=$1; n=$2; shift 2
python3 - "$b" "$n" "$@" <<'E' || exit
import sys
b,n=sys.argv[1:3]; a=sys.argv[3:]
s=open('gcw/'+b).read()
for i in range(0,len(a),2):
    assert a[i] in s, a[i]
    s=s.replace(a[i],a[i+1],1)
open('gcw/'+n+'.cpp','w').write(s)
E
echo -n "$n: "; sh g.sh $n.cpp $V
