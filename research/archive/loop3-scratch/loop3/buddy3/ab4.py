s = open('U/ub.cpp').read()
b0 = s[s.index('void AddBuddy_ZZbizyxyzg1('):]; b0 = b0[:b0.index('\n}\n') + 3]
W = """inline void G2r(BuddyInfo& b) { GiveBuddyOrders(&b); }
inline void G3r(BuddyInfo& b) { G2r(b); }
inline void G2i(int i) { GiveBuddyOrders(g_attached_buddies.m_data + i); }
inline void G3i(int i) { G2i(i); }
inline void G2v(dwi::vector<BuddyInfo>& v, int i) { GiveBuddyOrders(v.m_data + i); }
inline void G2vr(dwi::vector<BuddyInfo>& v, int i) { GiveBuddyOrders(&v[i]); }
"""
out = []
C = {'g2r': 'G2r(g_attached_buddies[i])', 'g3r': 'G3r(g_attached_buddies[i])', 'g2i': 'G2i(i)', 'g3i': 'G3i(i)', 'g2v': 'G2v(g_attached_buddies, i)', 'g2vr': 'G2vr(g_attached_buddies, i)',
     'g2rd': 'G2r(g_attached_buddies.m_data[i])', 'g2rd2': 'G2r(*(g_attached_buddies.m_data + i))'}
for k, c in C.items():
    out.append(b0.replace('GiveBuddyOrders(&g_attached_buddies[i])', c).replace('void AddBuddy_ZZbizyxyzg1(', f'void AddBuddy_ZZw{k}('))
# slot local at positions with add form
SL = '            BuddyInfo* slot = g_attached_buddies.m_data + i;\n'
for pos, anchor in [('top', '            UB_bi_zyx_yz(info, i);\n'), ('mid', '            g_attached_buddies[i] = info;\n')]:
    for useall in (0, 1):
        for g in ['GiveBuddyOrders', 'G2', 'G3']:
            v = b0.replace(anchor, SL + anchor).replace('GiveBuddyOrders(&g_attached_buddies[i])', g + '(slot)')
            if useall:
                v = v.replace('            g_attached_buddies[i] = info;\n            g_attached_buddies[i].state.active = true;\n            g_attached_buddies[i].state.path',
                              '            *slot = info;\n            slot->state.active = true;\n            slot->state.path')
            out.append(v.replace('void AddBuddy_ZZbizyxyzg1(', f'void AddBuddy_ZZw{pos}{useall}{g}('))
k = s.index('void BIFunc_SetBuddyPathGroup(')
s = s[:k] + W + s[k:]
open('U/ab4.cpp', 'w').write(s + '\n' + '\n'.join(out))
