import sys, itertools
s = open('U/ub.cpp').read()
ARGS = {'ref': '&g_attached_buddies[i]', 'add': 'g_attached_buddies.m_data + i', 'idx': '&g_attached_buddies.m_data[i]', 'radd': 'i + g_attached_buddies.m_data',
        'cast': '(BuddyInfo*)g_attached_buddies.m_data + i', 'bytes': '(BuddyInfo*)((char*)g_attached_buddies.m_data + i * sizeof(BuddyInfo))',
        'op': '&g_attached_buddies.operator[](i)'}
out = []
for hk in ['si', 'is', 'bi', 'ib', 'ci', 'pi']:
    for f in ['zyx_yz', 'zyx_zy']:
        b0 = s[s.index(f'void AddBuddy_ZZ{hk}{f.replace("_","")}g1('):]; b0 = b0[:b0.index('\n}\n') + 3]
        for g in ['GiveBuddyOrders', 'G2', 'G3']:
            for ak, a in ARGS.items():
                v = b0.replace('GiveBuddyOrders(&g_attached_buddies[i])', f'{g}({a})')
                out.append(v.replace(f'void AddBuddy_ZZ{hk}{f.replace("_","")}g1(', f'void AddBuddy_ZZr{hk}{f.replace("_","")}{g}{ak}('))
open('U/ab3.cpp', 'w').write(s + '\n' + '\n'.join(out))
