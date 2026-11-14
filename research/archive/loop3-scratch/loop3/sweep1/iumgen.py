base=open('ium0.cpp').read()
old='raw = Both(pad->buttons, input >> 8, input & 0xff);'
oldB='static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & (1 << x)) != 0) & ((b & (1 << y)) != 0); }'
assert old in base and oldB in base
bodies={
 'shl1':'int mx = 1; mx <<= x; int my = 1; my <<= y; return ((b & my) != 0) & ((b & mx) != 0);',
 'shl2':'int my = 1; my <<= y; int mx = 1; mx <<= x; return ((b & my) != 0) & ((b & mx) != 0);',
 'shl3':'int mx = 1; mx <<= x; int my = 1; my <<= y; return ((b & mx) != 0) & ((b & my) != 0);',
 'bool1':'bool by = (bool)(b & (1 << y)); bool bx = (bool)(b & (1 << x)); return by & bx;',
 'bool2':'bool bx = (bool)(b & (1 << x)); bool by = (bool)(b & (1 << y)); return by & bx;',
 'bool3':'bool bx = b & (1 << x); bool by = b & (1 << y); return bx & by;',
 'bool4':'bool by = b & (1 << y); bool bx = b & (1 << x); return bx & by;',
 'yx':'return ((b & (1 << y)) != 0) & ((b & (1 << x)) != 0);',
 'cnst':'unsigned int const cy = y; unsigned int const cx = x; return ((b & (1 << cy)) != 0) & ((b & (1 << cx)) != 0);',
 'late':'unsigned int lx; unsigned int ly; unsigned short const cb = b; ly = y; lx = x; return ((cb & (1 << ly)) != 0) & ((cb & (1 << lx)) != 0);',
}
calls={'hl':'raw = Both(pad->buttons, input >> 8, input & 0xff);','lh':'raw = Both(pad->buttons, input & 0xff, input >> 8);'}
for bk,bv in bodies.items():
  for ck,cv in calls.items():
    for inl in ['static inline','inline']:
      t=base.replace(oldB,f'{inl} int Both(unsigned short b, unsigned int x, unsigned int y) {{ {bv} }}').replace(old,cv)
      open(f'iv_{bk}_{ck}_{inl.split()[0]}.cpp','w').write(t)
