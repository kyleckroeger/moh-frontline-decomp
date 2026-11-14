import sys,subprocess,itertools,os
os.environ['T']='/home/kyle/workspace/moh-frontline/config/GMFE69/buddy_head.json'
F='BIFunc_GetInitialBuddyOrders__FPPiPv'
base=open('gi.cpp').read()
i=base.index('inline void DumpReadyBuddies() {'); j=base.index('\n}\n',i)+3
DR=base[i:j]
def score(src,tag):
    open(f'g_{tag}.cpp','w').write(src)
    return subprocess.run(['python3','rm.py',f'g_{tag}.cpp',F],capture_output=True,text=True).stdout.strip()
def put(dr): return base[:i]+dr+base[j:]
