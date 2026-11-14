from cwv import *
V={
'i1':H+"    int flag = **stack;\n    bool result = StartWalk(object, flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n",
'i2':H+"    int* const top = *stack;\n    bool flag = *top;\n    bool result = StartWalk(object, flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n",
'i3':H+"    bool const flag = **stack;\n    bool result = StartWalk(object, flag);\n    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **stack = result;\n}\n",
'i4':H+"    bool flag = **stack;\n    int** const c_stack = stack;\n    bool result = StartWalk(object, flag);\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
'i5':H+"    bool flag;\n    flag = **stack;\n    int** const c_stack = stack;\n    bool result = StartWalk(object, flag);\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
'i6':H+"    bool flag = **stack;\n    bool result = StartWalk(object, flag);\n    int** const c_stack = stack;\n    *c_stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **c_stack = result;\n}\n",
}
for n,b in V.items():
    open(f'c_{n}.cpp','w').write(base.replace(OLD,b))
    print(n,subprocess.run(['python3','rm.py',f'c_{n}.cpp',F],capture_output=True,text=True).stdout.strip())
