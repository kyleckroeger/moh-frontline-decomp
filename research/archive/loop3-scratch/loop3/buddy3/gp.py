# gp.py base out: GBO GPR decl perms (p,o,i hoisted to top of the !moving block) x DBV GPR perms (s,o,i at DBV top)
import sys, os, itertools
s0 = open(sys.argv[1]).read(); out = sys.argv[2]; os.makedirs(out, exist_ok=True)
G_old = ["            int path = buddy->progress + 1;\n", "            BSObject* object = buddy->soldier->GetScriptObject();\n", "            int index = 0;\n"]
G_new = ["            path = buddy->progress + 1;\n", "            object = buddy->soldier->GetScriptObject();\n", "            index = 0;\n"]
GD = {'p': '            int path;\n', 'o': '            BSObject* object;\n', 'i': '            int index;\n'}
a = s0.index('inline void GiveBuddyOrders(BuddyInfo* const buddy)'); b = s0.index('\n}\n', a)
g = s0[a:b]
for o, n in zip(G_old, G_new): assert g.count(o) == 1; g = g.replace(o, n)
g = g.replace('        if (!buddy->state.moving) {\n', '        if (!buddy->state.moving) {\n@G@', 1)
a2 = s0.index('inline void DumpBuddyVector('); b2 = s0.index('\n}\n', a2)
d = s0[a2:b2]
D_old = ["    CBuddySoldier* soldier;\n", "            BSObject* object = soldier->GetScriptObject();\n", "            int index = 0;\n"]
D_new = ["", "            object = soldier->GetScriptObject();\n", "            index = 0;\n"]
DD = {'s': '    CBuddySoldier* soldier;\n', 'o': '    BSObject* object;\n', 'i': '    int index;\n'}
for o, n in zip(D_old, D_new): assert d.count(o) == 1; d = d.replace(o, n)
d = d.replace('    DebugMsg("BUDDY_DEBUG: Dumping attached list...\\n");\n', '    DebugMsg("BUDDY_DEBUG: Dumping attached list...\\n");\n@D@', 1)
for gp in itertools.permutations('poi'):
    for dp in itertools.permutations('soi'):
        s = s0[:a] + g.replace('@G@', ''.join(GD[c] for c in gp)) + s0[b:a2] + d.replace('@D@', ''.join(DD[c] for c in dp)) + s0[b2:]
        open(f'{out}/{"".join(gp)}-{"".join(dp)}.cpp', 'w').write(s)
