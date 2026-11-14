import sys,json,difflib,subprocess,os
sys.path.insert(0,'scratch/game');import fast
t=json.load(open(sys.argv[1]));src=open(sys.argv[2]).read();f=sys.argv[3]
w=os.path.abspath(sys.argv[1]+'.d');os.makedirs(w,exist_ok=True)
o,log=fast.compile_object(t,src,w)
if not o: print(log);sys.exit()
a=[x[0]+(' R' if x[1] else '') for x in fast.target(f)]
b=[x[0]+(' R' if x[1] else '') for x in fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout)[f]]
print(len(a),len(b))
for l in difflib.unified_diff(a,b,n=0,lineterm=''):print(l)
