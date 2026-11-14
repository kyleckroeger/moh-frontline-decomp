import sys, itertools
s = open('U/ub.cpp').read()
base = s[s.index('void AddBuddy_ZZbizyxyzg2('):]; base = base[:base.index('\n}\n') + 3]
V = {}
B = lambda **kw: base
asg = ["            g_attached_buddies[i] = info;\n", "            g_attached_buddies[i].state.active = true;\n", "            g_attached_buddies[i].state.path = g_buddy_path_base[i].base;\n"]
for gname, g in [('g1', 'GiveBuddyOrders'), ('g2', 'G2'), ('g3', 'G3')]:
    b = base.replace('G2(', g + '(')
    V[f'base{gname}'] = b
    V[f'hoisti{gname}'] = b.replace('    for (int i = 0;', '    int i;\n    for (i = 0;')
    V[f'slot{gname}'] = b.replace('            if (giveOrders)\n                ' + g + '(&g_attached_buddies[i]);', '            if (giveOrders) {\n                BuddyInfo* slot = &g_attached_buddies[i];\n                ' + g + '(slot);\n            }')
    V[f'slotA{gname}'] = b.replace('            g_attached_buddies[i] = info;\n', '            BuddyInfo* slot = &g_attached_buddies[i];\n            g_attached_buddies[i] = info;\n').replace(g + '(&g_attached_buddies[i])', g + '(slot)')
    V[f'slotB{gname}'] = b.replace('            UB_bi', '            BuddyInfo* slot = &g_attached_buddies[i];\n            UB_bi').replace(g + '(&g_attached_buddies[i])', g + '(slot)')
    V[f'data{gname}'] = b.replace(g + '(&g_attached_buddies[i])', g + '(g_attached_buddies.m_data + i)')
    V[f'it{gname}'] = b.replace(g + '(&g_attached_buddies[i])', g + '(&*(g_attached_buddies.begin() + i))')
    for p in itertools.permutations(range(3)):
        V[f'ord{"".join(map(str,p))}{gname}'] = b.replace(''.join(asg), ''.join(asg[k] for k in p))
out = []
for k, v in V.items(): out.append(v.replace('void AddBuddy_ZZbizyxyzg2(', f'void AddBuddy_ZZq{k}('))
open('U/ab2.cpp', 'w').write(s + '\n' + '\n'.join(out))
