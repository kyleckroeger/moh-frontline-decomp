BOX="""    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;"""
CALL="g_pVolumeAllocator.GetFreeElement(true)"
V={
 'Q1':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    if (mem) new (mem) CVolBox;"%CALL)],
 'Q2':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    if (box) new (box) CVolBox;"%CALL)],
 'Q3':[(BOX,"    void* mem = %s;\n    if ((box = (CVolBox*)mem) != 0) new (mem) CVolBox;"%CALL)],
 'Q4':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    if (!box) goto skip;\n    new (mem) CVolBox;\nskip:"%CALL)],
 'Q5':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    if (box) { new (mem) CVolBox; }"%CALL)],
 'Q6':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    box && new (mem) CVolBox;"%CALL)],
 'Q7':[(BOX,"    void* mem = %s;\n    box = (CVolBox*)mem;\n    if (box) ::new (mem) CVolBox;"%CALL)],
}
