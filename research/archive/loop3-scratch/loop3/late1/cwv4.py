from cwv import *
T2="    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n"
SW="inline bool StartWalk(void* object, bool f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\n"
assert SW in base
V={
'k1':(SW,H+"    bool result = StartWalk(object, **stack);\n"+T2),
'k2':("inline bool StartWalk(void* object, bool f) { bool const c = f; return GetAIObject(object)->StartCoverPointWalkToStored(c); }\n",H+"    bool result = StartWalk(object, **stack);\n"+T2),
'k3':("inline bool StartWalk(void* object, bool const f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\n",H+"    bool result = StartWalk(object, **stack);\n"+T2),
'k4':("inline bool StartWalk(void* object, int f) { bool const c = f; return GetAIObject(object)->StartCoverPointWalkToStored(c); }\n",H+"    bool result = StartWalk(object, **stack);\n"+T2),
'k5':(SW,H+"    bool const flag = **stack;\n    bool result = StartWalk(object, (bool)flag);\n"+T2),
'k6':("inline bool StartWalk(void* object, bool const f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\n",H+"    bool const flag = **stack;\n    bool result = StartWalk(object, flag);\n"+T2),
'k7':("inline bool StartWalk(void* object, int f) { bool const c = f; return GetAIObject(object)->StartCoverPointWalkToStored(c); }\n",H+"    int const v = **stack;\n    bool result = StartWalk(object, v);\n"+T2),
}
for n,(sw,b) in V.items():
    open(f'c_{n}.cpp','w').write(base.replace(OLD,b).replace(SW,sw))
    print(n,subprocess.run(['python3','rm.py',f'c_{n}.cpp',F],capture_output=True,text=True).stdout.strip())
