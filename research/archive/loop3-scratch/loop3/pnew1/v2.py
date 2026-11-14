BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
HB="    CVolBox* box;\n"
HW="    CVolBox* world;\n"
W="    world = (CVolBox*)AllocateBoxVolume();"
NB="    CVolBox* const box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;"
V={
 'K1':[(HB,""),(BOX,NB)],
 'K2':[(HB,""),(HW,""),(BOX,NB),(W,"    CVolBox* world = (CVolBox*)AllocateBoxVolume();")],
 'K3':[(HB,""),(HW,""),(BOX,NB),(W,"    CVolBox* const world = (CVolBox*)AllocateBoxVolume();")],
 'K4':[(HB,""),(HW,""),(BOX,NB),(W,"    CVolBox* world;\n    world = (CVolBox*)AllocateBoxVolume();")],
 'K5':[(HB,""),(BOX,"    CVolBox* box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;")],
}
