import re
def parse_dis(text):
    funcs={}; cur=None
    for line in text.splitlines():
        m=re.match(r'^([0-9a-f]+) <(.+)>:$',line)
        if m: cur=(m.group(2),int(m.group(1),16)); funcs[cur]=[]; continue
        m=re.match(r'^\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)$',line)
        if m and cur: funcs[cur].append(m.group(2).strip())
    return funcs
def loose(ins):
    out=[]
    for i in ins:
        i=re.sub(r'\s+',' ',i)
        i=re.sub(r' [0-9a-f]+ <.*>$',' T',i)
        op=i.split(' ')[0]
        if op in ('bl','b') : i=op
        i=re.sub(r'-?\d+\((r2|r13)\)','X(sda)',i)
        if op in ('lis',): i=re.sub(r',.*',',X',i)
        if op in ('addi','li','ori') and re.search(r',(r2|r13|r\d+),-?\d+$',i) is None and op=='li': pass
        if op=='addi' : i=re.sub(r',-?\d+$',',X',i)
        i=re.sub(r'-?\d+\((r\d+)\)',lambda m: m.group(0),i)
        out.append(i)
    return tuple(out)
def vloose(ins):
    out=[]
    for i in ins:
        i=re.sub(r'\s+',' ',i)
        i=re.sub(r' [0-9a-f]+ <.*>$','',i)
        parts=i.split(' ',1)
        if len(parts)==2:
            ops=[]
            for o in parts[1].split(','):
                if re.fullmatch(r'-?(0x)?[0-9a-f]+',o) and not re.fullmatch(r'[rf]\d+',o): o='N'
                o=re.sub(r'^-?(0x)?[0-9a-f]*\(','N(',o) if '(' in o else o
                ops.append(o)
            i=parts[0]+' '+','.join(ops)
        out.append(i)
    return tuple(out)
