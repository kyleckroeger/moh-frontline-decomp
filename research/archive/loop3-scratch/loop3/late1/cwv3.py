from cwv import *
T2="    bool result = StartWalk(object, flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n"
V={
'j1':H+"    bool const flag = BSArgB(stack);\n"+T2,
'j2':H+"    int const v = **stack;\n    bool const flag = v;\n"+T2,
'j3':H+"    bool const flag = (bool)**stack;\n"+T2,
'j4':H+"    bool const flag = **stack != 0;\n"+T2,
'j5':H+"    bool b = **stack;\n    bool const flag = b;\n"+T2,
'j6':H+"    const bool flag = **stack;\n    bool const result = StartWalk(object, flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n",
'j7':H+"    bool const flag = **stack;\n    bool result;\n    result = StartWalk(object, flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n",
}
for n,b in V.items():
    open(f'c_{n}.cpp','w').write(base.replace(OLD,b))
    print(n,subprocess.run(['python3','rm.py',f'c_{n}.cpp',F],capture_output=True,text=True).stdout.strip())
