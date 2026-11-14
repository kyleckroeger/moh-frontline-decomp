from gv import *
import itertools
OLD="    CBuddySoldier* soldier = ((BSOwnerView*)caller)->GetSoldier();\n    dwi::vec_iter<BuddyInfo> it;\n"
assert OLD in base
parts={'S':"    CBuddySoldier* soldier;\n",'P':"    int** const c_stack = stack;\n",'C':"    void* const c_caller = caller;\n",'I':"    dwi::vec_iter<BuddyInfo> it;\n"}
res=[]
for k in range(1,5):
 for perm in itertools.permutations('SPCI',k):
  if 'S' not in perm or 'I' not in perm: continue
  h=''.join(parts[p] for p in perm)
  cal='c_caller' if 'C' in perm else 'caller'
  h+=f"    soldier = ((BSOwnerView*){cal})->GetSoldier();\n"
  s=base.replace(OLD,h)
  if 'P' in perm:
    gi=s.index('void BIFunc_GetInitialBuddyOrders'); ge=s.index('\n}\n',gi)
    s=s[:gi]+s[gi:ge].replace("    *stack -=","    *c_stack -=")+s[ge:]
  r=score(s,'g2'); res.append((int(r.split()[0]),''.join(perm),r))
for r in sorted(res)[:8]: print(r)
