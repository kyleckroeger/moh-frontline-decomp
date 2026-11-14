from v import *
P="inline void* operator new(unsigned long, void* p) {"
for tag,rep in [('thr',"inline void* operator new(unsigned long, void* p) throw() {")]:
  for bn,b in {'o4':B_OLD,'A':"    void* mem = g_pVolumeAllocator.GetFreeElement(true);\n    box = (CVolBox*)mem;\n    new (mem) CVolBox;\n",
     'pl':"    box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n"}.items():
    s=base.replace(P,rep).replace(B_OLD,b)
    print(tag,bn,score(s,tag+bn))
