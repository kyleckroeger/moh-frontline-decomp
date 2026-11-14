import sys,subprocess
src,dst=sys.argv[1:3]; s=open('gcw/'+src).read()
a=sys.argv[3:]
for i in range(0,len(a),2):
    if a[i] not in s: print('MISSING',a[i][:60]); sys.exit(1)
    s=s.replace(a[i],a[i+1],1)
open('gcw/'+dst,'w').write(s)
r=subprocess.run(['sh','gcw/s.sh',dst],capture_output=True,text=True)
print(dst,' '.join(r.stdout.split()[:1]+r.stdout.split()[-3:]), r.stderr[-300:])
