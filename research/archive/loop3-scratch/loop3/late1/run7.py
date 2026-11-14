from v import *
NB="new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n"
h=H_OLD.replace("    CVolBox* box;\n","")
boxes={
'k1':"    CVolBox* const box = "+NB,
'k2':"    CVolBox* box = "+NB,
'k3':"    CVolBox* const box = (CVolBox*)"+NB,
'k4':"    void* const mem = g_pVolumeAllocator.GetFreeElement(true);\n    CVolBox* const box = (CVolBox*)mem;\n    if (box) new (mem) CVolBox;\n",
}
for bn,b in boxes.items():
    s=base.replace(H_OLD,h).replace(B_OLD,b)
    print(bn,score(s,bn)[:200])
