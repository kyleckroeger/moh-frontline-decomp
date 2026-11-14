exec(open('rd.py').read().split('for x in')[0])
import struct,sys
syms=list(elf.symbols())
relocs={}
for sec in elf.sections:
    if sec['type']==4 and sec['info']:
        d=elf.contents(sec)
        for o in range(0,len(d),12):
            w,info,add=struct.unpack_from('>IIi',d,o); relocs[w]=(syms[info>>8]['name'],add,syms[info>>8])
by={s['name']:s for s in syms if s['name'] and 0<s['section']<0xff00}
def cstr(a):
    b=rd(a,512); return b[:b.index(0)].decode('latin1')
def info(root):
    s=by[root]; b=rd(s['address'],s['size']); out=[]
    for i in range(0,s['size'],4):
        a=s['address']+i
        if a in relocs: out.append(('ptr',relocs[a][0],relocs[a][1]))
        else: out.append(('int',struct.unpack('>i',b[i:i+4])[0]))
    return s,out
for root in sys.argv[1:]:
    s,out=info(root); print(root,hex(s['address']),s['size'],out[:8])

def esc(s): return s.replace('\\','\\\\').replace('"','\\"')
def strarr(name):
    s=by[name]; b=rd(s['address'],s['size']); vals=[]
    for i in range(0,s['size'],4):
        assert s['address']+i in relocs
        vals.append('"%s"'%esc(cstr(struct.unpack('>I',b[i:i+4])[0])))
    return s,vals
def zeros(name):
    s=by[name]; assert not any(rd(s['address'],s['size'])); return s
def libinfo(root, out, title):
    s,ents=info(root); recs=[]
    for i in range(0,len(ents),3):
        n=ents[i][1]; sz=ents[i+1][1]; iv=ents[i+2][1]; recs.append((n,sz,iv))
    body=[]
    for n,sz,iv in recs:
        ss,vals=strarr(sz); zs=zeros(iv)
        m=len(vals); assert zs['size']==4*m and (m==n or n==0 and m==1)
        body.append('const char* %s[%d] = {\n%s\n};\nlong %s[%d] = {0};\n' % (sz,m,'\n'.join('    %s,'%v for v in vals),iv,m))
        if n==0: body[-1]='// An empty library: the exporter still wrote one (empty) name.\n'+body[-1]
    table='LibraryInfoView %s[%d] = {\n%s\n};\n' % (root,len(recs),'\n'.join('    {%d, %s, %s},'%r for r in recs))
    return body, table
def screen(root, arr):
    s,ents=info(root); n=ents[0][1]; assert ents[1][1]==arr
    ss,vals=strarr(arr); assert len(vals)==n
    return 'const char* %s[%d] = {\n%s\n};\n\nScreenInfoView %s = {%d, %s};\n'%(arr,n,'\n'.join('    %s,'%v for v in vals),root,n,arr)
LIB_VIEW = """struct LibraryInfoView { // inferred: count, then names and objects (src/game/TibText.cpp's view)
    int count;
    const char** names;
    long* objects;
};
"""
SCR_VIEW = """struct ScreenInfoView { // inferred: the number of screens and their .SCR file names
    int count;
    const char** names;
};
"""
