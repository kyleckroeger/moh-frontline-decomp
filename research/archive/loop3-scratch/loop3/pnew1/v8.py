BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
CALL="g_pVolumeAllocator.GetFreeElement(true)"
V={
 'M3':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    new (box) CVolBox;"%CALL)],
 'M4':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    box = new (box) CVolBox;"%CALL)],
 'M5':[(BOX,"    box = (CVolBox*)%s;\n    box = new (box) CVolBox;"%CALL)],
 'M6':[(BOX,"    void* mem = %s;\n    box = new (mem) CVolBox;"%CALL)],
 'M7':[(BOX,"    box = (CVolBox*)%s;\n    new ((void*)box) CVolBox;"%CALL)],
}
