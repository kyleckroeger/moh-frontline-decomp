import subprocess,os
os.environ['T']='/home/kyle/workspace/moh-frontline/config/GMFE69/bsbifunc_cover.json'
F='BIFunc_AIStartCoverPointWalkToStored__FPPiPv'
base=open('cw.cpp').read()
OLD=base[base.index('void BIFunc_AIStartCoverPointWalkToStored'):]
OLD=OLD[:OLD.index('\n}\n')+3]
H="void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {\n"
V={
'r1':H+"    bool flag;\n    int** const c_stack = stack;\n    flag = **c_stack;\n    bool result = StartWalk(object, flag);\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
'r2':H+"    bool flag;\n    int** const c_stack = stack;\n    void* const c_object = object;\n    flag = **c_stack;\n    bool result = StartWalk(c_object, flag);\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
'r3':H+"    bool flag;\n    bool result;\n    int** const c_stack = stack;\n    flag = **c_stack;\n    result = StartWalk(object, flag);\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
'r4':H+"    bool flag;\n    int** const c_stack = stack;\n    flag = **stack;\n    bool result = StartWalk(object, flag);\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
'r5':H+"    int** const c_stack = stack;\n    bool flag;\n    flag = **c_stack;\n    bool result = StartWalk(object, flag);\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
}
for n,b in V.items():
    open(f'c_{n}.cpp','w').write(base.replace(OLD,b))
    print(n,subprocess.run(['python3','rm.py',f'c_{n}.cpp',F],capture_output=True,text=True).stdout.strip())
