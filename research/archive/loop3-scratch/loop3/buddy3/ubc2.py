s = open('A/xyz_yz-zyx_yz-v-xyz_yz.cpp').read()
b0 = s[s.index('void BIFunc_UpdateBuddyCheckpoints('):]; b0 = b0[:b0.index('\n}\n') + 3]
W = """inline void G2r(BuddyInfo& b) { GiveBuddyOrders(&b); }
inline void G3r(BuddyInfo& b) { G2r(b); }
inline void G4(BuddyInfo* b) { G3(b); }
inline void GI2(dwi::vec_iter<BuddyInfo>& it) { GiveBuddyOrders(it); }
inline void GI3(dwi::vec_iter<BuddyInfo>& it) { GI2(it); }
inline void DA3() { DA2(); }
"""
CALL = '            GiveBuddyOrders(it.operator->());'
G = {'p1': 'GiveBuddyOrders(it.operator->())', 'p2': 'G2(it.operator->())', 'p3': 'G3(it.operator->())', 'p4': 'G4(it.operator->())',
     'i2': 'GiveBuddyOrders(it)', 'i3': 'GI2(it)', 'i4': 'GI3(it)', 'r2': 'G2r(*it)', 'r3': 'G3r(*it)'}
D = {'a0': 'DumpAttachedBuddies()', 'a1': 'DA1()', 'a2': 'DA2()', 'a3': 'DA3()'}
out = []
for gk, g in G.items():
    for dk, d in D.items():
        v = b0.replace(CALL, '            ' + g + ';').replace('    DumpAttachedBuddies();', '    ' + d + ';')
        out.append(v.replace('void BIFunc_UpdateBuddyCheckpoints(', f'void BIFunc_UpdateBuddyCheckpoints_ZZu{gk}{dk}('))
        # index loop
        v = b0.replace('    dwi::vec_iter<BuddyInfo> it;\n    for (it = g_attached_buddies.begin(); it != g_attached_buddies.end(); ++it) {\n        if (it->state.active)\n' + CALL,
                       '    for (int i = 0; i < g_attached_buddies.m_size; i++) {\n        if (g_attached_buddies[i].state.active)\n            ' + g.replace('it.operator->()', '&g_attached_buddies[i]').replace('*it', 'g_attached_buddies[i]').replace('(it)', '(it)') + ';').replace('    DumpAttachedBuddies();', '    ' + d + ';')
        if '(it)' not in v.split('BIFunc_UpdateBuddyCheckpoints(')[1][:600] or True:
            out.append(v.replace('void BIFunc_UpdateBuddyCheckpoints(', f'void BIFunc_UpdateBuddyCheckpoints_ZZx{gk}{dk}('))
out = [o for o in out if not ('_ZZxi' in o)]
k = s.index('void BIFunc_SetBuddyPathGroup(')
s = s[:k] + W + s[k:]
open('A/ubc2.cpp', 'w').write(s + '\n' + '\n'.join(out))
