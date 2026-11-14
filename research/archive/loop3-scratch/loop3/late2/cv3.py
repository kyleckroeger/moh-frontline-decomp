import sys,subprocess,os
base=open('c/base.cpp').read()
old=base[base.index('void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {'):]
old=old[:old.index('}\n')+2]
pre='''inline int TopI(int** s) { return **s != 0; }
inline bool StartWalkR(void* object, bool& f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkCR(void* object, const bool& f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkC(void* object, const bool f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline void BSRetR(int**& s, bool v) { *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount; **s = v; }\ninline void BSRetP(int*** s, bool v) { **s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount; ***s = v; }\ninline bool StartWalkI(void* object, int f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }
'''
H='void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {\n'
R='    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n'
RS='    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **s = result;\n}\n'
V={
'a':H+'    bool flag = **stack;\n    int** s = stack;\n    bool result = StartWalk(object, flag);\n'+RS,
'b':H+'    bool result;\n    {\n        bool flag = **stack;\n        result = StartWalk(object, flag);\n    }\n'+R,
'c':H+'    int** s;\n    bool flag = **stack;\n    s = stack;\n    bool result = StartWalk(object, flag);\n'+RS,
'd':H+'    bool flag = **stack;\n    bool result = StartWalk(object, flag);\n    BSRetR(stack, result);\n}\n',
'e':H+'    bool flag = **stack;\n    bool result = StartWalk(object, flag);\n    BSRetP(&stack, result);\n}\n',
'f':H+'    bool flag = **stack;\n    bool result = StartWalkR(object, flag);\n'+R,
'g':H+'    bool flag = **stack;\n    bool result = StartWalkCR(object, flag);\n'+R,
'h':H+'    bool const flag = **stack;\n    bool result = StartWalkCR(object, flag);\n'+R,
'i':H+'    bool const flag = **stack;\n    bool result = StartWalkR(object, (bool&)flag);\n'+R,
'j':H+'    int const v = **stack;\n    bool result = StartWalkCR(object, v);\n'+R,
'k':H+'    bool result = StartWalkCR(object, **stack);\n'+R,
'l':H+'    bool result = StartWalkCR(object, BSArgB(stack));\n'+R,
'm':H+'    bool const flag = **stack;\n    bool result = StartWalkC(object, flag);\n'+R,
}
only=sys.argv[1:] 
for k,v in V.items():
  if only and k not in only: continue
  s=base.replace(old,v).replace('inline bool BSArgB',pre+'inline bool BSArgB')
  open(f'c/w_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/w_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip()[:200])
