import sys,subprocess,os,itertools,tempfile,json,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/bsbifunc_cover.json'))
F='BIFunc_AIStartCoverPointWalkToStored__FPPiPv'
A=fast.target(F)
base=open('c/base.cpp').read()
old=base[base.index('void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {'):]
old=old[:old.index('}\n')+2]
rets={'bool':'bool','int':'int','uchar':'unsigned char'}
bodies={'ne':'return **s != 0;','im':'return **s;','cast':'return (bool)**s;','cl':'bool const r = **s; return r;','nl':'bool r = **s; return r;','nl2':'bool r; r = **s; return r;'}
params={'v':'bool f','c':'const bool f','cr':'const bool& f','i':'int f'}
uses={'d':'GetAIObject(object)->StartCoverPointWalkToStored(f)','l':'CAIObject* ai = GetAIObject(object); return ai->StartCoverPointWalkToStored(f)','cp':'bool const g = f; return GetAIObject(object)->StartCoverPointWalkToStored(g)','cpn':'bool g = f; return GetAIObject(object)->StartCoverPointWalkToStored(g)'}
for (rk,rt),(bk,bd),(pk,pd),(uk,ud),wrap in itertools.product(rets.items(),bodies.items(),params.items(),uses.items(),['plain','local','clocal']):
  if bk in('cl','nl','nl2') and rk!='bool': continue
  ud2=ud if ud.startswith(('CAIObject','bool')) else 'return '+ud
  pre=f'inline {rt} Arg(int** s) {{ {bd} }}\ninline bool Walk(void* object, {pd}) {{ {ud2}; }}\n'
  if wrap=='plain': b='    bool result = Walk(object, Arg(stack));\n'
  elif wrap=='local': b='    bool flag = Arg(stack);\n    bool result = Walk(object, flag);\n'
  else: b='    bool const flag = Arg(stack);\n    bool result = Walk(object, flag);\n'
  body=b+'    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n'
  src=base.replace(old,pre+'void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {\n'+body+'}\n')
  dd=tempfile.mkdtemp(dir='/tmp/claude-1000'); obj,log=fast.compile_object(t,src,dd)
  if not obj: print('ERR',rk,bk,pk,uk,wrap); continue
  mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)[F]
  subprocess.run(['rm','-rf',dd])
  n=sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in difflib.SequenceMatcher(None,A,mine,autojunk=False).get_opcodes() if op!='equal')
  print(n,rk,bk,pk,uk,wrap,flush=True)
  if n<3: open(f'c/G2_{n}_{rk}{bk}{pk}{uk}{wrap}.cpp','w').write(src)
