from cwv4 import *
V={
'n1':(SW,H+"    int const flag = **stack != 0;\n    bool result = StartWalk(object, flag);\n"+T2),
'n2':(SW,H+"    unsigned char const flag = **stack != 0;\n    bool result = StartWalk(object, flag);\n"+T2),
'n3':(SW,H+"    bool const flag = **stack;\n    bool result = StartWalk(object, flag);\n    int** s = stack;\n    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;\n    **s = result;\n}\n"),
'n4':(SW,H+"    unsigned long const flag = **stack != 0;\n    bool result = StartWalk(object, flag);\n"+T2),
'n5':(SW,H+"    bool const flag = **stack ? true : false;\n    bool result = StartWalk(object, flag);\n"+T2),
'n6':(SW,H+"    bool const flag = !!**stack;\n    bool result = StartWalk(object, flag);\n"+T2),
}
for n,(sw,b) in V.items():
    open(f'c_{n}.cpp','w').write(base.replace(OLD,b).replace(SW,sw))
    print(n,subprocess.run(['python3','rm.py',f'c_{n}.cpp',F],capture_output=True,text=True).stdout.strip())
