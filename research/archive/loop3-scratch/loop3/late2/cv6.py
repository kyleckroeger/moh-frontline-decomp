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
'a':H+'    register bool flag = **stack;\n    bool result = StartWalk(object, flag);\n'+R,
'b':H+'    bool flag(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'c':H+'    const bool flag(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'd':H+'    int& arg = **stack;\n    bool const flag = arg;\n    bool result = StartWalk(object, flag);\n'+R,
'e':H+'    int* const top = *stack;\n    bool flag = *top;\n    bool result = StartWalk(object, flag);\n'+R,
'f':H+'    int* top = *stack;\n    bool const flag = *top;\n    bool result = StartWalk(object, flag);\n'+R,
'g':H+'    bool const flag = **stack;\n    bool f2 = flag;\n    bool result = StartWalk(object, f2);\n'+R,
'h':H+'    bool const flag = *stack[0];\n    bool result = StartWalk(object, flag);\n'+R,
'i':H+'    int const arg = **stack;\n    bool flag = arg;\n    bool result = StartWalk(object, flag);\n'+R,
'j':H+'    int arg = **stack;\n    bool const flag = arg;\n    bool result = StartWalk(object, flag);\n'+R,
'k':H+'    bool flag = **stack;\n    CAIObject* ai = GetAIObject(object);\n    bool result = ai->StartCoverPointWalkToStored(flag);\n'+R,
'l':H+'    bool const flag = **stack;\n    CAIObject* const ai = GetAIObject(object);\n    bool const result = ai->StartCoverPointWalkToStored(flag);\n'+R,
'm':H+'    bool flag;\n    if (**stack) flag = true; else flag = false;\n    bool result = StartWalk(object, flag);\n'+R,
'n':H+'    bool const flag = **stack;\n    int** const s = stack;\n    bool result = StartWalk(object, flag);\n    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **s = result;\n}\n',
'o':H+'    bool flag = **stack;\n    int** const s = stack;\n    bool result = StartWalk(object, flag);\n    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **s = result;\n}\n',
}
only=sys.argv[1:] 
for k,v in V.items():
  if only and k not in only: continue
  s=base.replace(old,pre+v)
  open(f'c/w_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/w_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip().replace(chr(10),' ')[:300])
