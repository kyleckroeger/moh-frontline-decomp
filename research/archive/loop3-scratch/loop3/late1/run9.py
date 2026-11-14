from v import *
M="    void* mem = g_pVolumeAllocator.GetFreeElement(true);\n"
boxes={
't1':M+"    box = mem ? new (mem) CVolBox : 0;\n",
't2':M+"    box = (CVolBox*)(mem ? new (mem) CVolBox : 0);\n",
't3':M+"    if (mem) { box = (CVolBox*)mem; new (mem) CVolBox; } else box = 0;\n",
't4':M+"    if ((box = (CVolBox*)mem) != 0) { new (mem) CVolBox; }\n",
't5':M+"    box = (CVolBox*)mem;\n    if (!box) goto skip;\n    new (mem) CVolBox;\nskip:\n",
}
for bn,b in boxes.items():
    s=base.replace(B_OLD,b)
    print(bn,score(s,bn)[:200])
