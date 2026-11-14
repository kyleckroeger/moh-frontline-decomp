from cwv4 import *
V={
'q1':(SW,H+"    bool const flag = **stack;\n    bool f2 = flag;\n    bool result = StartWalk(object, f2);\n"+T2),
'q2':(SW,H+"    bool flag = **stack;\n    bool result = StartWalk(object, flag);\n    int** const s = stack;\n"+T2.replace('*stack','*s')),
'q3':(SW,H+"    register bool flag = **stack;\n    bool result = StartWalk(object, flag);\n"+T2),
'q4':(SW,H+"    bool result;\n    bool flag = **stack;\n    result = StartWalk(object, flag);\n"+T2),
'q5':(SW,H+"    bool flag;\n    bool result;\n    flag = **stack;\n    result = StartWalk(object, flag);\n"+T2),
'q6':(SW,H+"    bool result;\n    bool flag;\n    flag = **stack;\n    result = StartWalk(object, flag);\n"+T2),
'q7':(SW,H+"    bool const flag = (**stack != 0);\n    bool result = StartWalk(object, flag);\n"+T2),
}
for n,(sw,b) in V.items():
    open(f'c_{n}.cpp','w').write(base.replace(OLD,b).replace(SW,sw))
    print(n,subprocess.run(['python3','rm.py',f'c_{n}.cpp',F],capture_output=True,text=True).stdout.strip())
