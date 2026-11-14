from gv import *
GI=base[base.index('void BIFunc_GetInitialBuddyOrders'):]
GI=GI[:GI.index('\n}\n')+3]
N="    dwi::list_iter<BuddyInfo> node(0);\n"; S="    CBuddySoldier* soldier;\n"
assert N+S in DR
V={}
# node by reference from GI
dr1=DR.replace("inline void DumpReadyBuddies() {\n"+N,"inline void DumpReadyBuddies(dwi::list_iter<BuddyInfo>& node) {\n")
dr2=DR.replace("inline void DumpReadyBuddies() {\n"+N+S,"inline void DumpReadyBuddies(dwi::list_iter<BuddyInfo>& node, CBuddySoldier*& soldier) {\n")
dr3=DR.replace("inline void DumpReadyBuddies() {\n"+N+S,"inline void DumpReadyBuddies(CBuddySoldier*& soldier) {\n"+N)
top="void BIFunc_GetInitialBuddyOrders(int** stack, void* caller) {\n"
for nm,dr,decl,call in [('n_first',dr1,N,"node"),('ns',dr2,N+"    CBuddySoldier* rs;\n","node, rs"),('sn',dr2,"    CBuddySoldier* rs;\n"+N,"node, rs"),('s',dr3,"    CBuddySoldier* rs;\n","rs")]:
    for pos in ('top','bottom'):
        g=GI.replace("    DumpReadyBuddies();\n",f"    DumpReadyBuddies({call});\n")
        if pos=='top': g=g.replace(top,top+decl)
        else:
            k=g.index("    dwi::vec_iter<BuddyInfo> it;\n")+len("    dwi::vec_iter<BuddyInfo> it;\n"); g=g[:k]+decl+g[k:]
        ov={'n_first':'inline void DumpReadyBuddies() { dwi::list_iter<BuddyInfo> node(0); DumpReadyBuddies(node); }\n','ns':'inline void DumpReadyBuddies() { dwi::list_iter<BuddyInfo> node(0); CBuddySoldier* s; DumpReadyBuddies(node, s); }\n','sn':'inline void DumpReadyBuddies() { dwi::list_iter<BuddyInfo> node(0); CBuddySoldier* s; DumpReadyBuddies(node, s); }\n','s':'inline void DumpReadyBuddies() { CBuddySoldier* s; DumpReadyBuddies(s); }\n'}[nm]
        s=base[:i]+dr+ov+base[j:]
        s=s.replace(GI,g)
        print(nm,pos,score(s,'g4'+nm+pos)[:120])
