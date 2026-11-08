"""genh.py: write the shared ShellMenu block header from the original's bytes (strings and tables only)."""
import sys,struct
from blockgen import *
def f(w):
    x=struct.unpack('>f',w.to_bytes(4,'big'))[0]
    return ('%g'%x)+('.0f' if float(x).is_integer() else 'f')
def vert(ws):
    return '{'+', '.join(f(w) for w in ws[:5])+', 0x%08X}'%ws[5]
NAMES={'@2136':('SMBCreditTimes','s_creditTimes'),'@2146':('SMBQuad','s_creditQuad'),'@2182':('SMBBriefingCounts','s_briefingCounts'),
       '@2183':('SMBBriefingTimes','s_briefingTimes'),'@2203':('SMBQuad','s_briefingQuad'),'@2586':('SMBQuad','s_unusedQuad')}
out=['''// The layout of ShellMenu.cpp's constant block (...rodata.0, 0x801783c8).
//
// The functions of this file reach their strings and tables as block base +
// immediate offset, so their code depends on where every item of the block
// sits, including items whose users the original linker removed. This header
// recreates that layout. Its strings and tables are copied from the original
// block. ShellMenuBlockStrings is a layout device, not recovered source: it has
// no logic, and each unit discards it (discarded_functions), so it never reaches
// the image. See docs/Dolphin.md, pool_references.
#ifndef SHELLMENU_BLOCK_H
#define SHELLMENU_BLOCK_H

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};
struct SMBQuad {
    _spritepolyvert v[4];
};
struct SMBCreditTimes {
    int v[20];
};
struct SMBBriefingCounts {
    int v[6];
};
struct SMBBriefingTimes {
    int v[6][11];
};

''',PRIME_SRC.replace('\n}\n','\n}\n\n')]
strs=[]
for n,o,raw in ITEMS:
    if n=='@725':
        out.append('static const char s_debugIni[] = %s;\n'%lit(raw)); continue
    if n in TABLES:
        t,v=NAMES[n]; w=words(raw)
        if t=='SMBQuad':
            b=',\n'.join('    '+vert(w[i:i+6]) for i in range(0,24,6))
            out.append('static const SMBQuad %s = {{\n%s\n}};\n'%(v,b))
        elif t=='SMBBriefingTimes':
            b=',\n'.join('    {'+', '.join(str(x) for x in w[i:i+11])+'}' for i in range(0,66,11))
            out.append('static const SMBBriefingTimes %s = {{\n%s\n}};\n'%(v,b))
        else:
            out.append('static const %s %s = {{%s}};\n'%(t,v,', '.join(str(x) for x in w)))
    else: strs.append(lit(raw))
out.append('\n#pragma exceptions off\nvoid ShellMenuBlockItem(const void*);\nvoid ShellMenuBlockStrings() {\n')
for s in strs: out.append('    ShellMenuBlockItem(%s);\n'%s)
out.append('}\n#pragma exceptions reset\n\n#endif\n')
open(sys.argv[1],'w').write(''.join(out))
