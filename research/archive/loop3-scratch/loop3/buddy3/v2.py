W = """
inline void W1p(BuddyInfo* b) { GiveBuddyOrders(b); }
inline void W2p(BuddyInfo* b) { W1p(b); }
inline void W3p(BuddyInfo* b) { W2p(b); }
inline void W1r(BuddyInfo& b) { GiveBuddyOrders(&b); }
inline void W2r(BuddyInfo& b) { W1r(b); }
inline void W1i(dwi::vec_iter<BuddyInfo>& it) { GiveBuddyOrders(it.operator->()); }
inline void W2i(dwi::vec_iter<BuddyInfo>& it) { W1i(it); }
inline void W1v(dwi::vec_iter<BuddyInfo> it) { GiveBuddyOrders(it.operator->()); }
inline void W1b(BuddyInfo* b) { BuddyInfo* c = b; GiveBuddyOrders(c); }
"""
AT = ("inline void SetBuddyProgress(", W + "\ninline void SetBuddyProgress(")
CALL = "            GiveBuddyOrders(it.operator->());\n    }\n    DumpAttachedBuddies();"
def c(x): return (CALL, "            " + x + ";\n    }\n    DumpAttachedBuddies();")
VARS = {
 'w1p': [AT, c('W1p(it.operator->())')],
 'w2p': [AT, c('W2p(it.operator->())')],
 'w3p': [AT, c('W3p(it.operator->())')],
 'w1r': [AT, c('W1r(*it)')],
 'w2r': [AT, c('W2r(*it)')],
 'w1i': [AT, c('W1i(it)')],
 'w2i': [AT, c('W2i(it)')],
 'w1v': [AT, c('W1v(it)')],
 'w1b': [AT, c('W1b(it.operator->())')],
}
