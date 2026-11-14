import itertools
base=open('ium0.cpp').read()
old='raw = Both(pad->buttons, input >> 8, input & 0xff);'
oldB='static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & (1 << x)) != 0) & ((b & (1 << y)) != 0); }'
H='input >> 8'; L='input & 0xff'
# param kinds: 's' shift amount, 'm' mask
def expr(kind,v): return f'(1 << {v})' if kind=='s' else v
n=0
for kx,ky in itertools.product('sm',repeat=2):
  for first in ['x','y']:
    for args in [(H,L),(L,H)]:
      for ret in ['xy','yx']:
        ax = f'(1 << ({args[0]}))' if kx=='m' else args[0]
        ay = f'(1 << ({args[1]}))' if ky=='m' else args[1]
        ex = f'(b & (1 << x))' if kx=='s' else '(b & x)'
        ey = f'(b & (1 << y))' if ky=='s' else '(b & y)'
        body = f'return ({ex} != 0) & ({ey} != 0);' if ret=='xy' else f'return ({ey} != 0) & ({ex} != 0);'
        B=f'static inline int Both(unsigned short b, unsigned int x, unsigned int y) {{ {body} }}'
        t=base.replace(oldB,B).replace(old,f'raw = Both(pad->buttons, {ax}, {ay});')
        n+=1; open(f'iw_{kx}{ky}_{"hl" if args[0]==H else "lh"}_{ret}_{first}.cpp','w').write(t)
print(n)
