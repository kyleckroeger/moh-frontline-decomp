# blk.py: helpers for the x/y/z blocks
import re, itertools
PAT = re.compile(r'(?P<ind>[ \t]*)float [xyz](?: = 0\.0f)?;\n(?:[ \t]*float [xyz](?: = 0\.0f)?;\n){2}(?:[ \t]*[yz] = x;\n){2}')
def blocks(s): return list(PAT.finditer(s))
FORMS = []
for p in itertools.permutations('xyz'):
    for a in (('y', 'z'), ('z', 'y')):
        FORMS.append((''.join(p), ''.join(a)))
def form(ind, f):
    p, a = f
    out = ''.join(f'{ind}float {v}{" = 0.0f" if v == "x" else ""};\n' for v in p)
    return out + ''.join(f'{ind}{v} = x;\n' for v in a)
def setblk(s, k, f):
    m = blocks(s)[k]
    return s[:m.start()] + form(m.group('ind'), f) + s[m.end():]
def name(f): return f[0] + '_' + f[1]
