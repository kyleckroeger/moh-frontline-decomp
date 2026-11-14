from v import *
M="    void* mem = g_pVolumeAllocator.GetFreeElement(true);\n"
boxes={
'a':M+"    box = (CVolBox*)mem;\n    if (box) new (box) CVolBox;\n",
'b':M+"    box = (CVolBox*)mem;\n    if (mem) new (mem) CVolBox;\n",
'c':M+"    box = (CVolBox*)mem;\n    if (box != 0) new (mem) CVolBox;\n",
'd':M+"    if ((box = (CVolBox*)mem) != 0) new (mem) CVolBox;\n",
'e':M+"    if (box = (CVolBox*)mem) new (mem) CVolBox;\n",
'f':M+"    box = (CVolBox*)mem;\n    if (box) box->CVolBox::CVolBox();\n",
'g':M+"    box = static_cast<CVolBox*>(mem);\n    if (box) new (mem) CVolBox();\n",
}
for bn,b in boxes.items():
    s=base.replace(B_OLD,b)
    print(bn,score(s,'c'+bn)[:200])
