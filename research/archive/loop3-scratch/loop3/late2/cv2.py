import sys,subprocess,os
base=open('c/base.cpp').read()
old=base[base.index('void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {'):]
old=old[:old.index('}\n')+2]
pre='''inline int TopI(int** s) { return **s != 0; }
inline bool StartWalkI(void* object, int f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }
'''
H='void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {\n'
R='    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n'
RS='    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **s = result;\n}\n'
V={
'a':H+'    bool flag;\n    int** const s = stack;\n    flag = **s;\n    bool result = StartWalk(object, flag);\n'+RS,
'b':H+'    int** const s = stack;\n    bool flag;\n    flag = **s;\n    bool result = StartWalk(object, flag);\n'+RS,
'c':H+'    bool flag;\n    flag = **stack;\n    bool result = StartWalk(object, flag);\n'+R,
'd':H+'    bool flag = **stack;\n    bool result = StartWalk(object, (bool)flag);\n'+R,
'e':H+'    bool result;\n    bool flag;\n    flag = **stack;\n    result = StartWalk(object, flag);\n'+R,
'f':H+'    bool flag;\n    bool result;\n    flag = **stack;\n    result = StartWalk(object, flag);\n'+R,
'g':H+'    bool flag = **stack;\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(flag);\n'+R,
'h':H+'    bool flag;\n    flag = **stack;\n    BSRetB(stack, StartWalk(object, flag));\n}\n',
'i':H+'    bool flag;\n    flag = **stack;\n    BSRet(stack, StartWalk(object, flag));\n}\n',
'j':H+'    bool const flag = **stack;\n    BSRetB(stack, StartWalk(object, flag));\n}\n',
'k':H+'    bool flag = **stack;\n    BSRetB(stack, StartWalk(object, flag));\n}\n',
'l':H+'    bool flag;\n    flag = (bool)**stack;\n    bool result = StartWalk(object, flag);\n'+R,
'm':H+'    int** const s = stack;\n    bool const flag = **s;\n    bool result = StartWalk(object, flag);\n'+RS,
'n':H+'    bool const flag = **stack;\n    int** const s = stack;\n    bool result = StartWalk(object, flag);\n'+RS,
'o':H+'    bool flag;\n    flag = **stack;\n    bool const f = flag;\n    bool result = StartWalk(object, f);\n'+R,
'p':H+'    bool flag = **stack;\n    bool const f = flag;\n    bool result = StartWalk(object, f);\n'+R,
}
only=sys.argv[1:] 
for k,v in V.items():
  if only and k not in only: continue
  s=base.replace(old,v).replace('inline bool BSArgB',pre+'inline bool BSArgB')
  open(f'c/w_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/w_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip()[:200])
