from v import *
b2=base.replace("    CVolBox() {}\n","    CVolBox() {}\n    static void* operator new(unsigned long) { return g_pVolumeAllocator.GetFreeElement(true); }\n    static void* operator new(unsigned long, void* p) { return p; }\n")
boxes={'cls':"    box = new CVolBox;\n"}
wbs={'wcls':"    CVolBox* worldBox = new CVolBox;\n",'wpl':None}
WB="    CVolBox* worldBox = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n"
heads={'o4':H_OLD,
 'r1':"    TriggerCoreView* core;\n    int const c_param = param;\n    TriggerObject_struct* const c_trigger = trigger;\n    int y;\n    int x;\n    int z;\n    CVolBox* box;\n    core = c_trigger->core;\n",
 'r4':"    TriggerCoreView* core;\n    int const c_param = param;\n    TriggerObject_struct* const c_trigger = trigger;\n    int z;\n    int y;\n    int x;\n    CVolBox* box;\n    core = c_trigger->core;\n",
}
for hn,h in heads.items():
  for wn,w in wbs.items():
    s=b2.replace(H_OLD,h)
    if hn!='o4': s=s.replace("    TriggerCoreView* core = c_trigger->core;\n","")
    s=s.replace(B_OLD,boxes['cls'])
    if w: s=s.replace(WB,w)
    print(hn,wn,score(s,hn+wn))
