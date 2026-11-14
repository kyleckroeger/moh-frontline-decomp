BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
FN="BSGO_Basic* CreateRootHierObject("
CALL="g_pVolumeAllocator.GetFreeElement(true)"
NEW="new (%s) CVolBox"%CALL
HB="    CVolBox* box;\n"
def H(helper, use): return [(FN, helper+"\n"+FN),(BOX,use)]
V={
 'Z1':[(HB,"    CVolBox* box = 0;\n"),(BOX,"    box = %s;"%NEW)],
 'Z2':[(BOX,"    box = 0;\n    box = %s;"%NEW)],
 'Z3':[("    core = trigger->core;\n","    core = trigger->core;\n    box = 0;\n"),(BOX,"    box = %s;"%NEW)],
 'R1':H("inline void* operator new(unsigned long, CVolBox& r) { return &r; }","    void* mem = %s;\n    box = (CVolBox*)mem;\n    if (box) new (*(CVolBox*)mem) CVolBox;"%CALL),
 'R2':H("inline void* operator new(unsigned long, CVolBox& r) { return &r; }","    box = (CVolBox*)%s;\n    new (*box) CVolBox;"%CALL),
 'R3':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    if (box) new (box) CVolBox;"%CALL)],
}
