from v import *
boxes={'plain':"    box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n",
       'cast':"    box = (CVolBox*)new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n"}
heads={
 'r1':"    TriggerCoreView* core;\n    int const c_param = param;\n    TriggerObject_struct* const c_trigger = trigger;\n    int y;\n    int x;\n    int z;\n    CVolBox* box;\n    core = c_trigger->core;\n",
 'r2':"    TriggerCoreView* core;\n    CVolBox* box;\n    int const c_param = param;\n    TriggerObject_struct* const c_trigger = trigger;\n    int y;\n    int x;\n    int z;\n    core = c_trigger->core;\n",
 'r3':"    CVolBox* box;\n    TriggerCoreView* core;\n    int const c_param = param;\n    TriggerObject_struct* const c_trigger = trigger;\n    int y;\n    int x;\n    int z;\n    core = c_trigger->core;\n",
 'r4':"    TriggerCoreView* core;\n    int const c_param = param;\n    TriggerObject_struct* const c_trigger = trigger;\n    int z;\n    int y;\n    int x;\n    CVolBox* box;\n    core = c_trigger->core;\n",
 'r5':"    TriggerCoreView* core;\n    TriggerObject_struct* const c_trigger = trigger;\n    int const c_param = param;\n    int y;\n    int x;\n    int z;\n    CVolBox* box;\n    core = c_trigger->core;\n",
}
for hn,h in heads.items():
  for bn,b in boxes.items():
    s=base.replace(H_OLD,h).replace("    TriggerCoreView* core = c_trigger->core;\n","").replace(B_OLD,b)
    print(hn,bn,score(s,hn+bn))
