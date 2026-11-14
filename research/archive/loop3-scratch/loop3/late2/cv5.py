import sys,subprocess,os
base=open('c/base.cpp').read()
old=base[base.index('void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {'):]
old=old[:old.index('}\n')+2]
pre='''inline int TopI(int** s) { return **s != 0; }
inline bool StartWalkR(void* object, bool& f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkCR(void* object, const bool& f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkC(void* object, const bool f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline void BSRetR(int**& s, bool v) { *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount; **s = v; }\ninline void BSRetP(int*** s, bool v) { **s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount; ***s = v; }\ninline bool StartWalk2(void* object, bool f) { return StartWalk(object, f); }\ninline bool StartWalkS(int** s, void* object) { bool const f = **s; return StartWalk(object, f); }\ninline bool StartWalkS2(int** s, void* object) { bool f = **s; return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkI(void* object, int f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }
'''
H='void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {\n'
R='    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n'
RS='    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **s = result;\n}\n'
V={
'a':H+'    bool flag = **stack;\n    flag = StartWalk(object, flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = flag;\n}\n',
'b':H+'    bool flag = **stack;\n    flag = GetAIObject(object)->StartCoverPointWalkToStored(flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = flag;\n}\n',
'c':H+'    bool flag = **stack != 0;\n    flag = GetAIObject(object)->StartCoverPointWalkToStored(flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = flag;\n}\n',
'd':H+'    int flag = **stack != 0;\n    flag = GetAIObject(object)->StartCoverPointWalkToStored(flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = flag;\n}\n',
'e':H+'    bool flag = **stack;\n    flag = StartWalk(object, flag);\n    BSRetB(stack, flag);\n}\n',
}
only=sys.argv[1:] 
for k,v in V.items():
  if only and k not in only: continue
  s=base.replace(old,pre+v)
  open(f'c/w_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/w_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip().replace(chr(10),' ')[:300])
