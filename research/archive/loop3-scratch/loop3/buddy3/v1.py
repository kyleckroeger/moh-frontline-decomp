ITOV = """inline void GiveBuddyOrders(dwi::vec_iter<BuddyInfo>& it) {
    GiveBuddyOrders(it.operator->());
}
"""
ITOV_DEF = "\n" + ITOV
ADDIT = ("inline void SetBuddyProgress(", ITOV + "\ninline void SetBuddyProgress(")
RMIT = (ITOV_DEF, "\n")
UBC_IT = ("""        if (it->state.active)
            GiveBuddyOrders(it.operator->());""", """        if (it->state.active)
            GiveBuddyOrders(it);""")
SBP_PTR = ("""inline void SetBuddyProgress(dwi::vec_iter<BuddyInfo>& it, int checkpoint) {
    it->progress = checkpoint;
    it->state.moving = 0;
    GiveBuddyOrders(it.operator->());
}""", """inline void SetBuddyProgress(BuddyInfo* it, int checkpoint) {
    it->progress = checkpoint;
    it->state.moving = 0;
    GiveBuddyOrders(it);
}""")
UBP_PTR = ("SetBuddyProgress(it, checkpoint);", "SetBuddyProgress(it.operator->(), checkpoint);")
UBP_DIRECT = ("SetBuddyProgress(it, checkpoint);", "it->progress = checkpoint;\n                it->state.moving = 0;\n                GiveBuddyOrders(it.operator->());")
GI_PTR = ("""            GiveBuddyOrders(it);
            break;""", """            GiveBuddyOrders(it.operator->());
            break;""")
VARS = {
 'ubc_it': [RMIT, ADDIT, UBC_IT],
 'ubp_ptr': [SBP_PTR, UBP_PTR],
 'ubp_direct': [UBP_DIRECT],
 'gi_ptr': [GI_PTR],
 'all': [RMIT, ADDIT, UBC_IT, UBP_DIRECT, GI_PTR],
}
