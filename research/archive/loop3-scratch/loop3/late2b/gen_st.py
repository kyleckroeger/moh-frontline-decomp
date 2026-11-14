import itertools
src=open('st0.c').read()
head,rest=src.split('    int result;\n    int voice;\n    int key;\n',1)
n=0
for gs in [None,'char* gs = sndgs;','char* const gs = sndgs;']:
    decls=['int result;','int voice;','int key;']+([gs] if gs else [])
    for p in itertools.permutations(decls):
        body=rest
        if gs:
            body=body.replace('*(char**)(sndgs + 468)','*(char**)(gs + 468)')
            p=[d if d!=gs else d.split(' = ')[0]+';' if 'const' not in d else d for d in p]
            if 'const' not in gs: body=body.replace('    result = -7;\n','    result = -7;\n    gs = sndgs;\n',1)
        open('st/v%d.c'%n,'w').write(head+''.join('    %s\n'%d for d in p)+body); n+=1
print(n)
