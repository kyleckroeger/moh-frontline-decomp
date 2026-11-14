W = """
inline void W1p(BuddyInfo* b) { GiveBuddyOrders(b); }
inline void W2p(BuddyInfo* b) { W1p(b); }
inline void D1() { DumpAttachedBuddies(); }
inline void D2() { D1(); }
inline void DV(dwi::vector<BuddyInfo>& v) { DumpBuddyVector(v); }
"""
AT = ("void BIFunc_SetBuddyPathGroup(", W + "\nvoid BIFunc_SetBuddyPathGroup(")
CALL = "            GiveBuddyOrders(it.operator->());\n    }\n    DumpAttachedBuddies();"
def c(x, d): return (CALL, "            " + x + ";\n    }\n    " + d + ";")
VARS = {}
for gn, g in [('g1', 'GiveBuddyOrders(it.operator->())'), ('g2', 'W2p(it.operator->())'), ('g3', 'W1p(it.operator->())')]:
    for dn, d in [('d0', 'DumpAttachedBuddies()'), ('dv', 'DumpBuddyVector(g_attached_buddies)'), ('d1', 'D1()'), ('d2', 'D2()'), ('dvv', 'DV(g_attached_buddies)')]:
        VARS[gn + dn] = [AT, c(g, d)]
