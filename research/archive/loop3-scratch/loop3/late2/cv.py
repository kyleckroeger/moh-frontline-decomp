import sys,subprocess,os
base=open('c/base.cpp').read()
old='''    bool const flag = **stack;
    bool result = StartWalk(object, flag);'''
assert old in base
pre='''inline int TopI(int** s) { return **s != 0; }
inline bool TopB(int** s) { return **s != 0; }
inline bool TopB2(int** s) { return **s; }
inline unsigned char TopU(int** s) { return **s != 0; }
inline bool StartWalkI(void* object, int f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }
'''
V={
'a':'    bool const flag = TopI(stack);\n    bool result = StartWalk(object, flag);',
'b':'    int const flag = TopI(stack);\n    bool result = StartWalk(object, flag);',
'c':'    int const flag = BSTopBool(stack);\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(flag);',
'd':'    bool const flag = BSTopBool(stack);\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(flag);',
'e':'    bool const flag = (bool)**stack;\n    bool result = StartWalk(object, flag);',
'f':'    bool const flag = TopB(stack);\n    bool result = StartWalk(object, flag);',
'g':'    bool const flag = TopB2(stack);\n    bool result = StartWalk(object, flag);',
'h':'    int const flag = **stack != 0;\n    bool result = StartWalk(object, flag);',
'i':'    int const flag = **stack != 0;\n    bool result = StartWalkI(object, flag);',
'j':'    bool const flag = TopU(stack);\n    bool result = StartWalk(object, flag);',
'k':'    const int v = **stack;\n    bool const flag = v;\n    bool result = StartWalk(object, flag);',
'l':'    bool const flag = **stack;\n    bool const f2 = flag;\n    bool result = StartWalk(object, f2);',
'm':'    bool flag;\n    flag = **stack;\n    bool result = StartWalk(object, (bool)flag);',
'n':'    int const flag = TopI(stack);\n    bool result = GetAIObject(object)->StartCoverPointWalkToStored(flag);',
'o':'    bool const flag = **stack != 0;\n    bool result = StartWalkI(object, flag);',
'p':'    bool const flag = TopI(stack) != 0;\n    bool result = StartWalk(object, flag);',
'q':'    bool const flag = (bool)TopI(stack);\n    bool result = StartWalk(object, flag);',
}
for k,v in V.items():
  s=base.replace(old,v).replace('inline bool BSArgB',pre+'inline bool BSArgB')
  open(f'c/v_{k}.cpp','w').write(s)
  r=subprocess.run(['python3','rm.py',f'c/v_{k}.cpp','BIFunc_AIStartCoverPointWalkToStored__FPPiPv'],capture_output=True,text=True)
  print(k,(r.stdout+r.stderr).strip()[:200])
