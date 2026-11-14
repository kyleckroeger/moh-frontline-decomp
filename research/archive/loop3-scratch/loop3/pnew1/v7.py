BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
W="    world = (CVolBox*)AllocateBoxVolume();"
NEW="new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox"
HB="    CVolBox* box;\n"; HW="    CVolBox* world;\n"
BF={'bcast':[(BOX,"    box = (CVolBox*)%s;"%NEW)], 'bdir':[(BOX,"    box = %s;"%NEW)], 'babv':[(BOX,"    box = (CVolBox*)AllocateBoxVolume();")],
    'bconst':[(HB,""),(BOX,"    CVolBox* const box = %s;"%NEW)], 'bsplit':[(BOX,"    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);\n    new (box) CVolBox;")]}
WF={'wabv':[], 'wdir':[(W,"    world = %s;"%NEW)], 'wcast':[(W,"    world = (CVolBox*)%s;"%NEW)], 'wconst':[(HW,""),(W,"    CVolBox* const world = %s;"%NEW)],
    'wsplit':[(W,"    world = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);\n    new (world) CVolBox;")], 'winit':[(HW,""),(W,"    CVolBox* world = %s;"%NEW)]}
V={}
for b,br in BF.items():
    for w,wr in WF.items(): V[b+'_'+w]=br+wr
