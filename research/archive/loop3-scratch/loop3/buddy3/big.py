# big.py outdir dbv_form drb_form dastruct(v|d) [gbo_form] : one file with all per-function alias variants
import sys, os, itertools, blk
out, dbvf, drbf, das = sys.argv[1:5]
gbof = sys.argv[5] if len(sys.argv) > 5 else 'xyz_yz'
F = lambda n: tuple(n.split('_'))
s = open('f_xyz.cpp').read()
def hoist(s, k, spec, anchor):
    # spec 'H<perm>[_<asg>]' : floats declared at the inline's top (after anchor line), assigned in place
    perm = spec[1:4]; asg = spec[5:7] if len(spec) > 4 else 'yz'
    m = blk.blocks(s)[k]; ind = m.group('ind')
    s = s[:m.start()] + f'{ind}x = 0.0f;\n' + ''.join(f'{ind}{v} = x;\n' for v in asg) + s[m.end():]
    i = s.index(anchor) + len(anchor)
    return s[:i] + ''.join(f'    float {v};\n' for v in perm) + s[i:]
GA = 'inline void GiveBuddyOrders(BuddyInfo* const buddy) {\n'
DA_ = 'inline void DumpBuddyVector(dwi::vector<BuddyInfo>& buddies) {\n'
if dbvf.startswith('H'): s = hoist(s, 1, dbvf, DA_)
else: s = blk.setblk(s, 1, F(dbvf))
if gbof.startswith('H'): s = hoist(s, 0, gbof, GA)
else: s = blk.setblk(s, 0, F(gbof))
# DRB float order / assignment order
a = s.index('inline void DumpReadyBuddies()'); b = s.index('void BIFunc_SetBuddyPathGroup(')
d = s[a:b]
p, asg = F(drbf)
d = d.replace('    float x;\n    float y;\n    float z;\n', ''.join(f'    float {v};\n' for v in p))
d = d.replace('        y = x;\n        z = x;\n', ''.join(f'        {v} = x;\n' for v in asg))
s = s[:a] + d + s[b:]
ITOV = "inline void GiveBuddyOrders(dwi::vec_iter<BuddyInfo>& it) {\n    GiveBuddyOrders(it.operator->());\n}\n"
s = s.replace('\n' + ITOV, '\n')
if das == 'd':
    a = s.index('inline void DumpBuddyVector('); e = 'inline void DumpAttachedBuddies() {\n    DumpBuddyVector(g_attached_buddies);\n}\n'; b = s.index(e)
    body = s[a:b].replace('inline void DumpBuddyVector(dwi::vector<BuddyInfo>& buddies)', 'inline void DumpAttachedBuddies()').replace('buddies.', 'g_attached_buddies.').replace('buddies[', 'g_attached_buddies[')
    s = s[:a] + body + s[b + len(e):]
W = ITOV + """inline void G2(BuddyInfo* b) { GiveBuddyOrders(b); }
inline void G3(BuddyInfo* b) { G2(b); }
inline void DA1() { DumpAttachedBuddies(); }
inline void DA2() { DA1(); }
inline void DR1() { DumpReadyBuddies(); }
inline void SBP2(dwi::vec_iter<BuddyInfo>& it, int checkpoint) { SetBuddyProgress(it, checkpoint); }
inline void SBPp(BuddyInfo* it, int checkpoint) {
    it->progress = checkpoint;
    it->state.moving = 0;
    GiveBuddyOrders(it);
}
"""
s = s.replace('void BIFunc_SetBuddyPathGroup(', W + '\nvoid BIFunc_SetBuddyPathGroup(')
def fn(name):
    i = s.index(name); j = s.index('\n}\n', i) + 3; return s[i:j]
def own(text, f):
    m = blk.blocks(text); assert len(m) == 1, len(m)
    return blk.setblk(text, 0, f)
al = []
# UBC
u = fn('void BIFunc_UpdateBuddyCheckpoints(')
for gi, g in enumerate(['GiveBuddyOrders(it.operator->())', 'GiveBuddyOrders(it)', 'G2(it.operator->())', 'G3(it.operator->())']):
    for di, dd in enumerate(['DumpAttachedBuddies()', 'DA1()', 'DA2()']):
        al.append(u.replace('BIFunc_UpdateBuddyCheckpoints(', f'BIFunc_UpdateBuddyCheckpoints_ZZg{gi}a{di}(').replace('GiveBuddyOrders(it.operator->())', g).replace('    DumpAttachedBuddies();', '    ' + dd + ';'))
# UBP
u = fn('void BIFunc_UpdateBuddyProgress(')
top = '    float z;\n    float y;\n    float x;\n'
assert u.count(top) == 1
for tp in itertools.permutations('xyz'):
    for asg in (('y', 'z'), ('z', 'y')):
        for si, sb in enumerate(['SetBuddyProgress(it, checkpoint)', 'SBP2(it, checkpoint)', 'SBPp(it.operator->(), checkpoint)',
                                 'it->progress = checkpoint;\n                it->state.moving = 0;\n                GiveBuddyOrders(it.operator->())']):
            for ri, rr in enumerate(['DumpReadyBuddies()', 'DR1()']):
                v = u.replace(top, ''.join(f'    float {c};\n' for c in tp)).replace('SetBuddyProgress(it, checkpoint)', sb).replace('    DumpReadyBuddies();', '    ' + rr + ';')
                v = v.replace('            y = x;\n            z = x;\n', ''.join(f'            {c} = x;\n' for c in asg))
                v = v.replace('                y = x;\n                z = x;\n', ''.join(f'                {c} = x;\n' for c in asg))
                al.append(v.replace('BIFunc_UpdateBuddyProgress(', f'BIFunc_UpdateBuddyProgress_ZZ{"".join(tp)}{"".join(asg)}s{si}r{ri}('))
# Died / GI / BA / AddBuddy
for name, calls in [('BIFunc_BuddyDied', [None]), ('BIFunc_GetInitialBuddyOrders', ['GiveBuddyOrders(it)', 'GiveBuddyOrders(it.operator->())', 'G2(it.operator->())', 'G3(it.operator->())']),
                    ('BIFunc_AddBuddy', [None]), ('AddBuddy', ['GiveBuddyOrders(&g_attached_buddies[i])', 'G2(&g_attached_buddies[i])', 'G3(&g_attached_buddies[i])'])]:
    sig = 'void ' + name + '('
    u = fn(sig + ('BuddyInfo info' if name == 'AddBuddy' else ''))
    for f in blk.FORMS:
        for ci, c in enumerate(calls):
            for di, dd in enumerate(['DumpAttachedBuddies()', 'DA1()'] if name != 'AddBuddy' else [None]):
                for ri, rr in enumerate(['DumpReadyBuddies()', 'DR1()'] if name != 'AddBuddy' else [None]):
                    v = own(u, f)
                    if c: v = v.replace(calls[0], c)
                    if dd: v = v.replace('DumpAttachedBuddies();', dd + ';')
                    if rr: v = v.replace('DumpReadyBuddies();', rr + ';')
                    al.append(v.replace(sig, f'void {name}_ZZ{blk.name(f).replace("_","")}c{ci}a{di}r{ri}('))
s = s + '\n' + '\n'.join(al)
os.makedirs(out, exist_ok=True)
open(f'{out}/{dbvf}-{drbf}-{das}-{gbof}.cpp', 'w').write(s)
