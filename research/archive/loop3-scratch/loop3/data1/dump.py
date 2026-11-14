import sys, json, struct, collections
from pathlib import Path
ROOT=Path('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,str(ROOT/'tools'))
from formats import Elf32
elf=Elf32((ROOT/'orig/GMFE69/Moh2RelGC.elf').read_bytes())
syms=list(elf.symbols())
secs=elf.sections
def secname(i):
    try: return secs[i]['name']
    except Exception: return '?'
fm=json.load(open(ROOT/'build/audit/file_map.json'))['files']
def file_of(sec,addr):
    for f in fm:
        for r in f['ranges']:
            if r['section']==sec and int(r['start'],16)<=addr<int(r['end'],16): return f['name']
    return '?'
relocs={}
for sec in secs:
    if sec['type']==4 and sec['info']:
        d=elf.contents(sec)
        for o in range(0,len(d),12):
            w,info,add=struct.unpack_from('>IIi',d,o)
            relocs[w]=(info&255,syms[info>>8]['name'],add)
names=[l.split('\t')[0] for l in open(sys.argv[1])]
byname=collections.defaultdict(list)
for s in syms:
    if s['name'] and 0<s['section']<0xff00: byname[s['name']].append(s)
verbose='-v' in sys.argv
for n in names:
    if n.startswith('__vt__'): continue
    L=byname.get(n)
    if not L: print(f'{n}\tNOT-IN-ORIGINAL'); continue
    for s in L:
        sn=secname(s['section'])
        print(f"{n}\t{sn}\t{s['address']:#x}\t{s['size']:#x}\t{file_of(sn,s['address'])}\ttype{s['type']} bind{s['binding']}")
        if verbose and sn not in ('.bss','.sbss','.text') and s['size']<=int(sys.argv[3] if len(sys.argv)>3 else 0x80):
            sec=secs[s['section']]; off=s['address']-sec['address']
            b=elf.contents(sec)[off:off+s['size']]
            print('    ',b.hex())
            for a in range(s['address'],s['address']+s['size'],4):
                if a in relocs: print('     reloc',hex(a),relocs[a])
