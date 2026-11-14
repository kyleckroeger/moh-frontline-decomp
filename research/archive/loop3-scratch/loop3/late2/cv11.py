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
'v0':H+'    int const x = **stack;\n    bool result = StartWalk(object, x);\n'+R,
'v1':H+'    int const x = **stack;\n    bool result = StartWalk(object, (bool)x);\n'+R,
'v2':H+'    int const x = **stack;\n    bool result = StartWalkC(object, x);\n'+R,
'v3':H+'    int const x = **stack;\n    bool result = StartWalkCR(object, x);\n'+R,
'v4':H+'    int const x = **stack;\n    bool result = StartWalkCR(object, (bool)x);\n'+R,
'v5':H+'    int const x = **stack;\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(x);\n'+R,
'v6':H+'    int x = **stack;\n    bool result = StartWalk(object, x);\n'+R,
'v7':H+'    int x = **stack;\n    bool result = StartWalk(object, (bool)x);\n'+R,
'v8':H+'    int x = **stack;\n    bool result = StartWalkC(object, x);\n'+R,
'v9':H+'    int x = **stack;\n    bool result = StartWalkCR(object, x);\n'+R,
'v10':H+'    int x = **stack;\n    bool result = StartWalkCR(object, (bool)x);\n'+R,
'v11':H+'    int x = **stack;\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(x);\n'+R,
'v12':H+'    int* const x = *stack;\n    bool result = StartWalk(object, *x);\n'+R,
'v13':H+'    int* const x = *stack;\n    bool result = StartWalk(object, (bool)*x);\n'+R,
'v14':H+'    int* const x = *stack;\n    bool result = StartWalkC(object, *x);\n'+R,
'v15':H+'    int* const x = *stack;\n    bool result = StartWalkCR(object, *x);\n'+R,
'v16':H+'    int* const x = *stack;\n    bool result = StartWalkCR(object, (bool)*x);\n'+R,
'v17':H+'    int* const x = *stack;\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(*x);\n'+R,
'v18':H+'    int const& x = **stack;\n    bool result = StartWalk(object, x);\n'+R,
'v19':H+'    int const& x = **stack;\n    bool result = StartWalk(object, (bool)x);\n'+R,
'v20':H+'    int const& x = **stack;\n    bool result = StartWalkC(object, x);\n'+R,
'v21':H+'    int const& x = **stack;\n    bool result = StartWalkCR(object, x);\n'+R,
'v22':H+'    int const& x = **stack;\n    bool result = StartWalkCR(object, (bool)x);\n'+R,
'v23':H+'    int const& x = **stack;\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(x);\n'+R,
}
only=sys.argv[1:] 
for k,v in V.items():
  if only and k not in only: continue
  s=base.replace(old,pre+v)
  open(f'c/w_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/w_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip().replace(chr(10),' ')[:300])
