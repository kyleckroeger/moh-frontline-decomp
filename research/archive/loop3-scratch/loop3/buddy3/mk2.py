# UBC aliases: GBO depth a (1..5) x dump depth b (1..5)
import blk
s = open('f_xyz.cpp').read()
ubc0 = s[s.index('void BIFunc_UpdateBuddyCheckpoints('):]; ubc0 = ubc0[:ubc0.index('\n}\n') + 3]
W = """inline void G2(BuddyInfo* b) { GiveBuddyOrders(b); }
inline void G3(BuddyInfo* b) { G2(b); }
inline void G4(BuddyInfo* b) { G3(b); }
inline void G5(BuddyInfo* b) { G4(b); }
inline void D2() { DumpBuddyVector(g_attached_buddies); }
inline void D3() { DumpAttachedBuddies(); }
inline void D4() { D3(); }
inline void D5() { D4(); }
"""
G = {1: 'GiveBuddyOrders', 2: 'G2', 3: 'G3', 4: 'G4', 5: 'G5'}
D = {1: None, 2: 'D2', 3: 'D3', 4: 'D4', 5: 'D5'}
al = ''
for a in range(1, 6):
    for b in range(1, 6):
        u = ubc0.replace('BIFunc_UpdateBuddyCheckpoints(', f'BIFunc_UpdateBuddyCheckpoints_ZZg{a}d{b}(').replace('GiveBuddyOrders(it.operator->())', G[a] + '(it.operator->())')
        if b == 1:
            u = u.replace('    DumpAttachedBuddies();\n', """    DebugMsg("BUDDY_DEBUG: Dumping attached list...\\n");
    CBuddySoldier* soldier;
    for (int i = 0; i < g_attached_buddies.m_size; i++) {
        if (g_attached_buddies[i].state.active == true) {
            soldier = g_attached_buddies[i].soldier;
            BSObject* object = soldier->GetScriptObject();
            float z;
            float y;
            float x = 0.0f;
            y = x;
            z = x;
            int index = 0;
            if (object->m_node) {
                BSPlacementView* placement = object->m_node->placement;
                if (placement) {
                    x = placement->x;
                    y = placement->y;
                    z = placement->z;
                }
                if (placement)
                    index = placement->pathIndex;
            }
            BSStateView* state = object->m_class->GetState();
            DebugMsg("BUDDY_DEBUG:   Attached=%x[%3d @(%4d,%4d,%4d) %4d] In Slot %d\\n", soldier, index, (int)x, (int)y,
                     (int)z, state->state, i);
        }
    }
""")
        else:
            u = u.replace('    DumpAttachedBuddies();\n', f'    {D[b]}();\n')
        al += '\n' + u
s = s.replace(ubc0, W + ubc0 + al)
import sys, os
os.makedirs('m2', exist_ok=True)
for k, f in enumerate(blk.FORMS):
    open(f'm2/{blk.name(f)}.cpp', 'w').write(blk.setblk(s, 1, f))
