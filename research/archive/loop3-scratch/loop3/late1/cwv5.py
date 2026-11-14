from cwv4 import *
V={
'm1':(SW,H+"    bool flag = **stack;\n    bool const c = flag;\n    bool result = StartWalk(object, c);\n"+T2),
'm2':(SW,H+"    bool const c = **stack;\n    bool flag = c;\n    bool result = StartWalk(object, flag);\n"+T2),
'm3':("inline bool StartWalk(void* object, bool f) { bool const c = f; return GetAIObject(object)->StartCoverPointWalkToStored(c); }\n",H+"    bool flag = **stack;\n    bool result = StartWalk(object, flag);\n"+T2),
'm4':(SW,H+"    bool flag = **stack;\n    bool const result = StartWalk(object, flag);\n"+T2),
'm5':(SW,H+"    bool flag;\n    void* const c_object = object;\n    flag = **stack;\n    bool result = StartWalk(c_object, flag);\n"+T2),
'm6':("inline bool StartWalk(void* object, bool f) { void* const o = object; return GetAIObject(o)->StartCoverPointWalkToStored(f); }\n",H+"    bool flag = **stack;\n    bool result = StartWalk(object, flag);\n"+T2),
'm7':("inline bool StartWalk(void* const object, bool f) { return GetAIObject(object)->StartCoverPointWalkToStored(f); }\n",H+"    bool flag = **stack;\n    bool result = StartWalk(object, flag);\n"+T2),
}
for n,(sw,b) in V.items():
    open(f'c_{n}.cpp','w').write(base.replace(OLD,b).replace(SW,sw))
    print(n,subprocess.run(['python3','rm.py',f'c_{n}.cpp',F],capture_output=True,text=True).stdout.strip())
