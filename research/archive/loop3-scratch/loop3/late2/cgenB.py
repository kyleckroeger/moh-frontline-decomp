import sys,subprocess,os,itertools,tempfile,json
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
import difflib
t=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/bsbifunc_cover.json'))
F='BIFunc_AIStartCoverPointWalkToStored__FPPiPv'
A=fast.target(F)
base=open('c/base.cpp').read()
old=base[base.index('void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {'):]
old=old[:old.index('}\n')+2]
pre='''inline bool StartWalkC(void* object, const bool f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }
inline bool StartWalkCR(void* object, const bool& f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }
inline bool StartWalkDirect(void* object, bool f) { CAIObject* ai = GetAIObject(object); return ai->StartCoverPointWalkToStored(f); }
'''
types=['bool','unsigned char','int']
cvs=['c','n','late']
inits=['**stack','(bool)**stack','**stack != 0','!!**stack','**stack ? true : false','BSArgB(stack)','BSTopBool(stack)','(bool)BSTopBool(stack)','!(**stack == 0)','(**stack != 0) ? 1 : 0']
args=['flag','(bool)flag','flag != 0']
calls=['StartWalk(object, %s)','StartWalkCR(object, %s)','GetAIObject(object)->StartCoverPointWalkToStored(%s)','StartWalkDirect(object, %s)']
rets=['R','B']
part=int(sys.argv[1]); nparts=int(sys.argv[2])
best=99
for i,(ty,cv,ini,ar,ca,re) in enumerate(itertools.product(types,cvs,inits,args,calls,rets)):
  if i%nparts!=part: continue
  if cv=='c': d=f'    {ty} const flag = {ini};\n'
  elif cv=='n': d=f'    {ty} flag = {ini};\n'
  else: d=f'    {ty} flag;\n    flag = {ini};\n'
  c=ca%ar
  if re=='R': body=d+f'    bool result = {c};\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n'
  else: body=d+f'    BSRetB(stack, {c});\n'
  src=base.replace(old,pre+'void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {\n'+body+'}\n')
  dd=tempfile.mkdtemp(dir='/tmp/claude-1000'); obj,log=fast.compile_object(t,src,dd)
  if not obj: continue
  mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)[F]
  subprocess.run(['rm','-rf',dd])
  n=sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in difflib.SequenceMatcher(None,A,mine,autojunk=False).get_opcodes() if op!='equal')
  print(n,i,repr(body),flush=True)
  if n==0: open(f'c/MATCH_{i}.cpp','w').write(src)
