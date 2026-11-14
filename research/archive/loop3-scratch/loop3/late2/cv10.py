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
'a':H+'    bool const flag = (bool)(**stack != 0);\n    bool result = StartWalk(object, flag);\n'+R,
'A':H+'    bool flag = (bool)(**stack != 0);\n    bool result = StartWalk(object, flag);\n'+R,
'b':H+'    bool const flag = bool(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'B':H+'    bool flag = bool(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'c':H+'    bool const flag = static_cast<bool>(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'C':H+'    bool flag = static_cast<bool>(**stack);\n    bool result = StartWalk(object, flag);\n'+R,
'd':H+'    bool const flag = **stack != false;\n    bool result = StartWalk(object, flag);\n'+R,
'D':H+'    bool flag = **stack != false;\n    bool result = StartWalk(object, flag);\n'+R,
'e':H+'    bool const flag = (**stack != 0) != 0;\n    bool result = StartWalk(object, flag);\n'+R,
'E':H+'    bool flag = (**stack != 0) != 0;\n    bool result = StartWalk(object, flag);\n'+R,
'f':H+'    bool const flag = (bool)(int)(bool)**stack;\n    bool result = StartWalk(object, flag);\n'+R,
'F':H+'    bool flag = (bool)(int)(bool)**stack;\n    bool result = StartWalk(object, flag);\n'+R,
'g':H+'    bool const flag = **stack && true;\n    bool result = StartWalk(object, flag);\n'+R,
'G':H+'    bool flag = **stack && true;\n    bool result = StartWalk(object, flag);\n'+R,
'h':H+'    bool const flag = true && **stack;\n    bool result = StartWalk(object, flag);\n'+R,
'H':H+'    bool flag = true && **stack;\n    bool result = StartWalk(object, flag);\n'+R,
'i':H+'    bool const flag = **stack != 0 ? true : false;\n    bool result = StartWalk(object, flag);\n'+R,
'I':H+'    bool flag = **stack != 0 ? true : false;\n    bool result = StartWalk(object, flag);\n'+R,
'j':H+'    bool const flag = (bool)!!**stack;\n    bool result = StartWalk(object, flag);\n'+R,
'J':H+'    bool flag = (bool)!!**stack;\n    bool result = StartWalk(object, flag);\n'+R,
'k':H+'    bool const flag = !(bool)!**stack;\n    bool result = StartWalk(object, flag);\n'+R,
'K':H+'    bool flag = !(bool)!**stack;\n    bool result = StartWalk(object, flag);\n'+R,
'l':H+'    bool const flag = **stack > 0 || **stack < 0;\n    bool result = StartWalk(object, flag);\n'+R,
'L':H+'    bool flag = **stack > 0 || **stack < 0;\n    bool result = StartWalk(object, flag);\n'+R,
'm':H+'    bool const flag = (unsigned)**stack != 0u;\n    bool result = StartWalk(object, flag);\n'+R,
'M':H+'    bool flag = (unsigned)**stack != 0u;\n    bool result = StartWalk(object, flag);\n'+R,
'n':H+'    bool const flag = *(*stack) != 0;\n    bool result = StartWalk(object, flag);\n'+R,
'N':H+'    bool flag = *(*stack) != 0;\n    bool result = StartWalk(object, flag);\n'+R,
'o':H+'    bool const flag = (*stack)[0] != 0;\n    bool result = StartWalk(object, flag);\n'+R,
'O':H+'    bool flag = (*stack)[0] != 0;\n    bool result = StartWalk(object, flag);\n'+R,
}
only=sys.argv[1:] 
for k,v in V.items():
  if only and k not in only: continue
  s=base.replace(old,pre+v)
  open(f'c/w_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/w_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip().replace(chr(10),' ')[:300])
