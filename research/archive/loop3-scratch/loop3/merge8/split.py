"""split.py FILE: print JSON {decls: text, funcs: [(name, text)]} splitting top-level function definitions
(lines starting at column 0, ending with '{' and containing '(', through the column-0 '}') from the rest."""
import sys,json,re
def split(path):
    lines=open(path).read().splitlines(True)
    decls=[];funcs=[];i=0
    while i<len(lines):
        l=lines[i]
        if l[:1] not in ' \t/#\n}' and '(' in l and not l.rstrip().endswith(';') and not l.startswith(('struct','class','union','enum','extern "C" {','namespace','typedef')):
            # gather signature lines until a line ending with '{'
            j=i
            while not lines[j].rstrip().endswith('{'): j+=1
            k=j+1
            while not lines[k].startswith('}'): k+=1
            # attach preceding comment lines
            c=[]
            while decls and decls[-1].startswith(('//','/*',' *')) : c.insert(0,decls.pop())
            funcs.append(''.join(c+lines[i:k+1])); i=k+1
        else:
            decls.append(l); i+=1
    return ''.join(decls),funcs
if __name__=='__main__':
    d,f=split(sys.argv[1]); print(d); print('----'); [print(x.splitlines()[0]) for x in f]
