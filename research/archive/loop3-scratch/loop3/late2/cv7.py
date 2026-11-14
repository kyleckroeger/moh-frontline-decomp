import sys,subprocess,os
base=open('c/base.cpp').read()
old=base[base.index('void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {'):]
old=old[:old.index('}\n')+2]
pre='''struct BArg { BArg() {} BArg(int x) : v(x) {} operator bool() const { return v; } bool v; };\nstruct BArg2 { BArg2(int** s) : v(**s) {} bool v; };\nstruct BArgI { BArgI(int x) : v(x) {} operator bool() const { return v; } int v; };\nstruct SP { SP(int** s) : s(s) {} bool Top() const { return **s; } void Ret(bool r) { *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount; **s = r; } int** s; };\ninline int TopI(int** s) { return **s != 0; }
inline bool StartWalkR(void* object, bool& f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkCR(void* object, const bool& f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkC(void* object, const bool f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline void BSRetR(int**& s, bool v) { *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount; **s = v; }\ninline void BSRetP(int*** s, bool v) { **s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount; ***s = v; }\ninline bool StartWalk2(void* object, bool f) { return StartWalk(object, f); }\ninline bool StartWalkS(int** s, void* object) { bool const f = **s; return StartWalk(object, f); }\ninline bool StartWalkS2(int** s, void* object) { bool f = **s; return GetAIObject(object)->StartCoverPointWalkToStored(f); }\ninline bool StartWalkI(void* object, int f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }
'''
H='void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {\n'
R='    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n'
RS='    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **s = result;\n}\n'
V={
'a':H+'    BArg flag(**stack);\n    bool result = StartWalk(object, flag.v);\n'+R,
'b':H+'    BArg flag;\n    flag.v = **stack;\n    bool result = StartWalk(object, flag.v);\n'+R,
'c':H+'    BArg const flag(**stack);\n    bool result = StartWalk(object, flag.v);\n'+R,
'd':H+'    BArg2 flag(stack);\n    bool result = StartWalk(object, flag.v);\n'+R,
'e':H+'    bool result = StartWalk(object, BArg(**stack).v);\n'+R,
'f':H+'    BArg flag(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'g':H+'    BArgI flag(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'h':H+'    BArgI flag(**stack);\n    bool result = StartWalk(object, flag.v);\n'+R,
'i':H+'    SP sp(stack);\n    bool const flag = sp.Top();\n    bool result = StartWalk(object, flag);\n    sp.Ret(result);\n}\n',
'j':H+'    SP sp(stack);\n    bool flag = sp.Top();\n    bool result = StartWalk(object, flag);\n    sp.Ret(result);\n}\n',
'k':H+'    SP sp(stack);\n    bool result = StartWalk(object, sp.Top());\n    sp.Ret(result);\n}\n',
}
only=sys.argv[1:] 
for k,v in V.items():
  if only and k not in only: continue
  s=base.replace(old,pre+v)
  open(f'c/w_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/w_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip().replace(chr(10),' ')[:300])
