exec(open('rd.py').read().split('for x in')[0])
import struct
syms=list(elf.symbols())
relocs={}
for sec in elf.sections:
    if sec['type']==4 and sec['info']:
        d=elf.contents(sec)
        for o in range(0,len(d),12):
            w,info,add=struct.unpack_from('>IIi',d,o); relocs[w]=(syms[info>>8]['name'],add)
def rows(vals, per, fmt):
    out=[]
    for i in range(0,len(vals),per): out.append('    '+', '.join(fmt(v) for v in vals[i:i+per])+',')
    return '\n'.join(out)
W='init/'
# printstr.cpp
b=rd(0x80193050,0x200); ents=[]
for i in range(0,0x200,8):
    p,f=struct.unpack('>II',b[i:i+8])
    if p==0 and f==0: continue
    s=rd(p,40); s=s[:s.index(0)].decode(); ents.append((s,f>>31&1,f>>30&1))
assert all((f&0x3fffffff)==0 for f in [struct.unpack('>I',b[i+4:i+8])[0] for i in range(0,0x200,8)])
lines='\n'.join(f'    {{"{s}", {e}, {g}}},' for s,e,g in ents)
open(W+'printstr.cpp','w').write(f'''// printstr.cpp: its initialised data (.data): PRINTchannellist, the print channels by
// number, {len(ents)} named and the rest of the 64 empty. Each has a name, an enabled bit
// (set for "all" and "alert") and a second bit set for every named channel; the layout
// is inferred from src/ea/printstr.cpp's and printstr_movechannel.cpp's views. A file static
// in the original, defined global here because the file's fragments reach it with extern.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/printstr.json

struct PrintChannelView {{ // inferred
    const char* name;
    unsigned char enabled : 1;
    unsigned char grouped : 1;
    unsigned char flags : 6;
}};

PrintChannelView PRINTchannellist[64] = {{ // file-local in the original
{lines}
}};
''')
# aemscompinfo
b=rd(0x801933e0,0x108); decl=[]; ent=[]
for i in range(0,0x108,8):
    p,c,d,e=struct.unpack('>IBBH',b[i:i+8])
    n=relocs.get(0x801933e0+i,(None,))[0]
    if p: assert n, hex(i)
    if n and n not in decl: decl.append(n)
    fn=n.split('__F')[0] if n else '0'
    ent.append(f'    {{{fn}, {c}, {d}, {e}}}, // {i//8}')
protos='\n'.join(f'void {n.split("__F")[0]}(AEMSCOMPDYNAMIC*, MODULE*);' for n in decl)
open(W+'unassigned_aems.inc','w').write(f'''{protos}

struct AEMSCOMPINFOVIEW {{ // inferred: src/ea/saems_system.c and saems_counter.c views
    void (*update)(AEMSCOMPDYNAMIC*, MODULE*);
    unsigned char definitionSize;
    unsigned char componentSize;
    unsigned short unknown06;
}};

AEMSCOMPINFOVIEW aemscompinfo[33] = {{
''' + '\n'.join(ent) + '\n};\n')
# srandom.c / sdecmut.c
fm=struct.unpack('>10i',rd(0x80193e28,0x28)); fs=struct.unpack('>10i',rd(0x801942d0,0x28))
tc=struct.unpack('>1296b',rd(0x80193918,0x510)); mc=rd(0x801942f8,0xd8)
def nest(vals,dims):
    if len(dims)==1: return '{'+', '.join(str(v) for v in vals)+'}'
    n=len(vals)//dims[0]; return '{'+', '.join(nest(vals[i*n:(i+1)*n],dims[1:]) for i in range(dims[0]))+'}'
def nest2(vals,dims,ind='    '):
    # outer two levels on separate lines
    n=len(vals)//dims[0]
    return '{\n'+''.join(ind+nest(vals[i*n:(i+1)*n],dims[1:])+',\n' for i in range(dims[0]))+'}'
open(W+'srandom.c','w').write(f'''// srandom.c (file per build/audit/file_map.json, as research/data/bss/srandom.cpp): its
// initialised data (.data): the mix effects' per-module filter sizes and tablecopyto, a
// [6][6][6][6] channel copy map (-1 = none; layout as src/ea/smixfx_modlap.c reads it).
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/srandom.json

signed char tablecopyto[6][6][6][6] = {nest2(list(tc),[6,6,6,6])};

int filtermodsizetable[10] = {{{', '.join(map(str,fm))}}};
''')
open(W+'sdecmut.c','w').write(f'''// sdecmut.c (file per build/audit/file_map.json): its initialised data (.data): the
// reverb filter sizes and mapchannel, a [6][6][6] channel map (layout as src/ea/sfxrevc.c
// reads it). Checked by research/tools/check_data.py.
// settings: config/GMFE69/srandom.json

int filtersizetable[10] = {{{', '.join(map(str,fs))}}};

unsigned char mapchannel[6][6][6] = {nest2(list(mc),[6,6,6])};
''')
# coda.cpp
xa=struct.unpack('>256f',rd(0x80194418,0x400)); xf=struct.unpack('>8f',rd(0x801943f8,0x20))
def ff(v):
    for p in range(1,12):
        s='%.*g'%(p,v)
        if struct.pack('>f',float(s))==struct.pack('>f',v):
            if 'e' not in s and '.' not in s: s+='.0'
            return s+'f'
fx=rd(0x80194818,0x18)
open(W+'coda.cpp','w').write(f'''// coda.cpp (file per build/audit/file_map.json): its initialised data (.data): the XA ADPCM
// decoder's filter coefficients and step table (namespace SND; read by research/drafts/
// decodexac.80168828.cpp) and fxdefault, the default effect settings (a byte view: 0x18
// bytes, layout not known). Checked by research/tools/check_data.py.
// settings: config/GMFE69/coda.json

namespace SND {{
float xafilterf[8] = {{{', '.join(ff(v) for v in xf)}}};

float xatablef[256] = {{
{rows(xa,8,ff)}
}};
}}

unsigned char fxdefault[0x18] = {{ // view: layout not known
{rows(list(fx),12,lambda v:'0x%02x'%v)}
}};
''')
# sndcents
sc=rd(0x801934f8,0x100)
open(W+'unassigned_sndcents.inc','w').write('unsigned char sndcents[256] = {\n'+rows(list(sc),16,str)+'\n};\n')
# FONTstandfile
fsb=rd(0x801910e0,0x1cf0)
assert not any(0x801910e0<=a<0x801910e0+0x1cf0 for a in relocs)
open(W+'unassigned_font.inc','w').write('char FONTstandfile[0x1cf0] = {\n'+rows(list(fsb),16,lambda v:'0x%02x'%v)+'\n};\n')
print(fx.hex(), len(ents))
