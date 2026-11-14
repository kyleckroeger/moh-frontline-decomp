D='static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & (1 << x)) != 0) & ((b & (1 << y)) != 0); }'
C='raw = Both(pad->buttons, input >> 8, input & 0xff);'
V={}
V['swapbody']=[(D,'static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & (1 << y)) != 0) & ((b & (1 << x)) != 0); }')]
V['swapargs']=[(D,'static inline int Both(unsigned short b, unsigned int y, unsigned int x) { return ((b & (1 << x)) != 0) & ((b & (1 << y)) != 0); }'),(C,'raw = Both(pad->buttons, input & 0xff, input >> 8);')]
V['swapboth']=[(D,'static inline int Both(unsigned short b, unsigned int y, unsigned int x) { return ((b & (1 << y)) != 0) & ((b & (1 << x)) != 0); }'),(C,'raw = Both(pad->buttons, input & 0xff, input >> 8);')]
V['masks']=[(D,'static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & x) != 0) & ((b & y) != 0); }'),(C,'raw = Both(pad->buttons, 1 << (input >> 8), 1 << (input & 0xff));')]
V['masks2']=[(D,'static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & x) != 0) & ((b & y) != 0); }'),(C,'raw = Both(pad->buttons, 1 << (input & 0xff), 1 << (input >> 8));')]
V['locals']=[(C,'unsigned int hi = input >> 8;\n                unsigned int lo = input & 0xff;\n                raw = Both(pad->buttons, hi, lo);')]
V['locals2']=[(C,'unsigned int lo = input & 0xff;\n                unsigned int hi = input >> 8;\n                raw = Both(pad->buttons, hi, lo);')]
V['shortb']=[(D,D.replace('unsigned short b','int b'))]
V['pairbool']=[(D,'static inline bool Pressed(unsigned short b, unsigned int x) { return (b & (1 << x)) != 0; }\nstatic inline int Both(unsigned short b, unsigned int x, unsigned int y) { return Pressed(b, x) & Pressed(b, y); }')]
