BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
FN="BSGO_Basic* CreateRootHierObject("
NEW="new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox"
HB="    CVolBox* box;\n"
def H(helper, use): return [(FN, helper+"\n"+FN),(BOX,use)]
V={
 'H1':H("static inline CVolBox* NB() { CVolBox* v = %s; return v; }"%NEW,"    box = NB();"),
 'H2':H("static inline CVolBox* NB() { CVolBox* const v = %s; return v; }"%NEW,"    box = NB();"),
 'H3':H("static inline IVolume* NB() { return %s; }"%NEW,"    box = (CVolBox*)NB();"),
 'H6':H("static inline void NB(CVolBox** b) { *b = %s; }"%NEW,"    NB(&box);"),
 'N3':[(HB,"    register CVolBox* box;\n"),(BOX,"    box = %s;"%NEW)],
 'N5':[(BOX,"    box = (0, %s);"%NEW)],
 'N6':[(BOX,"    box = static_cast<CVolBox*>(%s);"%NEW)],
 'N7':[(BOX,"    if ((box = %s) != 0) {}"%NEW)],
 'N8':[(BOX,"    CVolBox* const nb = %s;\n    box = nb;"%NEW)],
 'N9':[(BOX,"    box = flag ? %s : %s;"%(NEW,NEW))],
}
