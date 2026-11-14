from v import *
PL="    box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n"
H_o4=H_OLD
vars={
 'I0': (H_o4.replace("    CVolBox* box;\n","    CVolBox* box = 0;\n"), PL, None),
 'H': (H_o4, "    NewBox(box);\n", "inline void NewBox(CVolBox*& b) { b = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox; }\n"),
 'H2': (H_o4, "    NewBox(&box);\n", "inline void NewBox(CVolBox** b) { *b = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox; }\n"),
 'J': (H_o4, "    box = (CVolBox*)(IVolume*)new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n", None),
 'K': (H_o4.replace("    CVolBox* box;\n","    IVolume* ibox;\n"), "    ibox = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n#define box ((CVolBox*)ibox)\n", None),
}
for n,(h,b,pre) in vars.items():
    s=base.replace(H_OLD,h).replace(B_OLD,b)
    if pre: s=s.replace("BSGO_Destructable* CreateDestructable(",pre+"BSGO_Destructable* CreateDestructable(")
    print(n,score(s,n))
