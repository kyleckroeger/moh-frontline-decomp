# mkh.py out_dir [perm...]: GBO with hoisted decls in given orders + UBC alias contexts
import sys, itertools, os
s = open('f_xyz.cpp').read()
old = """            int path = buddy->progress + 1;
            BSObjectTriggerEvent(buddy->soldier->GetScriptObject(), 211, (void*)(buddy->state.path + path), g_pBSObject, false);
            buddy->state.moving = 1;
            BSObject* object = buddy->soldier->GetScriptObject();
            float x = 0.0f;
            float y;
            float z;
            y = x;
            z = x;
            int index = 0;
"""
new = """@DECLS@            path = buddy->progress + 1;
            BSObjectTriggerEvent(buddy->soldier->GetScriptObject(), 211, (void*)(buddy->state.path + path), g_pBSObject, false);
            buddy->state.moving = 1;
            object = buddy->soldier->GetScriptObject();
            x = 0.0f;
            y = x;
            z = x;
            index = 0;
"""
assert s.count(old) == 1; s = s.replace(old, new)
ubc0 = s[s.index('void BIFunc_UpdateBuddyCheckpoints('):]; ubc0 = ubc0[:ubc0.index('\n}\n') + 3]
W = """inline void W1p(BuddyInfo* b) { GiveBuddyOrders(b); }
inline void W2p(BuddyInfo* b) { W1p(b); }
"""
alias = ubc0.replace('BIFunc_UpdateBuddyCheckpoints(', 'BIFunc_UpdateBuddyCheckpoints_ZZa(').replace('GiveBuddyOrders(it.operator->())', 'W2p(it.operator->())')
s = s.replace(ubc0, W + ubc0 + '\n' + alias)
T = {'p': 'int path;', 'o': 'BSObject* object;', 'x': 'float x;', 'y': 'float y;', 'z': 'float z;', 'i': 'int index;'}
out = sys.argv[1]; os.makedirs(out, exist_ok=True)
perms = sys.argv[2:] or [''.join(p) for p in itertools.permutations('poxyzi')]
for p in perms:
    open(f'{out}/{p}.cpp', 'w').write(s.replace('@DECLS@', ''.join(f'            {T[c]}\n' for c in p)))
