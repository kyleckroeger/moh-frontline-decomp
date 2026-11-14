import sys
# rep.py base out old new  (old/new python string literals via files?) -> uses exec of pairs from argv
b,o=sys.argv[1:3]; s=open(b).read()
for i in range(3,len(sys.argv),2):
    old=sys.argv[i].encode().decode('unicode_escape'); new=sys.argv[i+1].encode().decode('unicode_escape')
    assert old in s,(old)
    s=s.replace(old,new)
open(o,'w').write(s)
