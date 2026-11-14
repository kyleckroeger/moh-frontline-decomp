s = open('U/ub.cpp').read()
b0 = s[s.index('void AddBuddy_ZZbizyxyzg1('):]; b0 = b0[:b0.index('\n}\n') + 3]
out = []
HEAD = 'void AddBuddy_ZZbizyxyzg1(BuddyInfo info, bool giveOrders) {\n'
for g in ['GiveBuddyOrders', 'G2', 'G3']:
    for ek, e in [('add', 'g_attached_buddies.m_data + i'), ('ref', '&g_attached_buddies[i]')]:
        for dk, decls in [('ts', ['    BuddyInfo* slot;\n']), ('tsi', ['    BuddyInfo* slot;\n', '    int i;\n']), ('tis', ['    int i;\n', '    BuddyInfo* slot;\n'])]:
            v = b0.replace(HEAD, HEAD + ''.join(decls))
            if 'int i' in ''.join(decls): v = v.replace('for (int i = 0;', 'for (i = 0;')
            v1 = v.replace('            if (giveOrders)\n                GiveBuddyOrders(&g_attached_buddies[i]);', f'            if (giveOrders) {{\n                slot = {e};\n                {g}(slot);\n            }}')
            v2 = v.replace('            if (giveOrders)\n                GiveBuddyOrders(&g_attached_buddies[i]);', f'            slot = {e};\n            if (giveOrders)\n                {g}(slot);')
            for tag, vv in [('in', v1), ('out', v2)]:
                out.append(vv.replace('void AddBuddy_ZZbizyxyzg1(', f'void AddBuddy_ZZv{g}{ek}{dk}{tag}('))
open('U/ab5.cpp', 'w').write(s + '\n' + '\n'.join(out))
