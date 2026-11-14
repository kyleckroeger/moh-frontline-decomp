BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
V={
 'W1':[(BOX,"    box = (CVolBox*)AllocateBoxVolume();")],
}
