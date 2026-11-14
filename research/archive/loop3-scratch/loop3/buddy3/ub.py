# ub.py base out : AddBuddy aliases with the UsingBuddy block in an inline helper (param forms x decl forms x GBO depth)
import sys, os, itertools, blk
s = open(sys.argv[1]).read(); out = sys.argv[2]; os.makedirs(out, exist_ok=True)
i = s.index('void AddBuddy(BuddyInfo info'); j = s.index('\n}\n', i) + 3; ab = s[i:j]
blkstart = ab.index('            BSObject* object = info.soldier->GetScriptObject();')
blkend = ab.index('            g_attached_buddies[i] = info;')
body = ab[blkstart:blkend]
HP = {  # helper signature, call, body renames
 'si': ('CBuddySoldier* soldier, int i', 'info.soldier, i'),
 'is': ('int i, CBuddySoldier* soldier', 'i, info.soldier'),
 'bi': ('BuddyInfo& info, int i', 'info, i'),
 'ib': ('int i, BuddyInfo& info', 'i, info'),
 'ci': ('const BuddyInfo& info, int i', 'info, i'),
 'pi': ('BuddyInfo* info, int i', '&info, i'),
}
al = []; helpers = ''
for hk, (sig, call) in HP.items():
    hb = body
    if hk in ('si', 'is'): hb = hb.replace('info.soldier', 'soldier')
    if hk == 'pi': hb = hb.replace('info.soldier', 'info->soldier')
    hb = '\n'.join(l[8:] if l.startswith('        ') else l for l in hb.split('\n'))
    for f in blk.FORMS:
        nm = f'UB_{hk}_{blk.name(f)}'
        helpers += f'inline void {nm}({sig}) {{\n' + blk.setblk(hb, 0, f) + '}\n'
        for gk, g in [('g1', 'GiveBuddyOrders(&g_attached_buddies[i])'), ('g2', 'G2(&g_attached_buddies[i])'), ('g3', 'G3(&g_attached_buddies[i])')]:
            v = ab[:blkstart] + f'            {nm}({call});\n' + ab[blkend:]
            v = v.replace('GiveBuddyOrders(&g_attached_buddies[i])', g)
            al.append(v.replace('void AddBuddy(BuddyInfo info', f'void AddBuddy_ZZ{hk}{blk.name(f).replace("_","")}{gk}(BuddyInfo info'))
k = s.index('void BIFunc_SetBuddyPathGroup(')
s = s[:k] + helpers + s[k:] + '\n' + '\n'.join(al)
open(f'{out}/ub.cpp', 'w').write(s)
