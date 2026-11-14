BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
FN="BSGO_Basic* CreateRootHierObject("
CLS="""    CVolBox() {}
    virtual ~CVolBox();"""
V={
 'A_newbox':[(FN,"inline CVolBox* NewBox() { return new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox; }\n"+FN),(BOX,"    box = NewBox();")],
 'B_clsop':[(CLS,CLS+"\n    void* operator new(unsigned long, void* p) { return p; }"),(BOX,"    box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;")],
 'C_nb':[(BOX,"    {\n    CVolBox* nb = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n    box = nb;\n    }")],
 'D_blk':[(BOX,"    {\n    box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n    }")],
 'H_asg':[(BOX,"    new (box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true)) CVolBox;")],
 'H2_asg':[(BOX,"    new ((void*)(box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true))) CVolBox;")],
 'F_ref':[(BOX,"    CVolBox*& rb = box;\n    rb = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;")],
}
