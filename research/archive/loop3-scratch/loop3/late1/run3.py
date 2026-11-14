from v import *
boxes={
'A':"    void* mem = g_pVolumeAllocator.GetFreeElement(true);\n    box = (CVolBox*)mem;\n    new (mem) CVolBox;\n",
'B':"    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);\n    new (box) CVolBox;\n",
'C':"    void* mem = g_pVolumeAllocator.GetFreeElement(true);\n    new (mem) CVolBox;\n    box = (CVolBox*)mem;\n",
'D':"    void* mem = g_pVolumeAllocator.GetFreeElement(true);\n    box = (CVolBox*)new (mem) CVolBox;\n",
'E':"    void* mem;\n    box = (CVolBox*)(mem = g_pVolumeAllocator.GetFreeElement(true));\n    new (mem) CVolBox;\n",
'F':"    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);\n    if (box) new (box) CVolBox;\n",
}
for bn,b in boxes.items():
    s=base.replace(B_OLD,b)
    print(bn,score(s,'b'+bn))
