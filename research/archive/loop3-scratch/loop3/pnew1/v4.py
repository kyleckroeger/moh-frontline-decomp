BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
FN="BSGO_Basic* CreateRootHierObject("
CALL="g_pVolumeAllocator.GetFreeElement(true)"
def H(helper, use): return [(FN, helper+"\n"+FN),(BOX,use)]
V={
 'O1':H("inline void* operator new(unsigned long, void* p, CVolBox*& out) { out = (CVolBox*)p; return p; }","    new (%s, box) CVolBox;"%CALL),
 'O2':H("inline void* operator new(unsigned long, void* p, CVolBox** out) { *out = (CVolBox*)p; return p; }","    new (%s, &box) CVolBox;"%CALL),
 'O3':H("inline void* operator new(unsigned long, void* p, CVolBox*& out) { void* r = p; out = (CVolBox*)p; return r; }","    new (%s, box) CVolBox;"%CALL),
 'O4':H("inline void* operator new(unsigned long, void* p, CVolBox*& out) { return out = (CVolBox*)p; }","    new (%s, box) CVolBox;"%CALL),
 'M1':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    new (mem) CVolBox;"%CALL)],
 'M2':[(BOX,"    void* mem;\n    box = (CVolBox*)(mem = %s);\n    new (mem) CVolBox;"%CALL)],
}
