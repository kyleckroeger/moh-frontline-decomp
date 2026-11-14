D='static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & (1 << x)) != 0) & ((b & (1 << y)) != 0); }'
C='raw = Both(pad->buttons, input >> 8, input & 0xff);'
V={}
SB='static inline int Both(unsigned short b, unsigned int x, unsigned int y) { return ((b & (1 << y)) != 0) & ((b & (1 << x)) != 0); }'
V['sb_uc']=[(D,SB),(C,'raw = Both(pad->buttons, (unsigned char)(input >> 8), (unsigned char)input);')]
V['sb_uc2']=[(D,SB.replace('unsigned int x, unsigned int y','unsigned char x, unsigned char y')),(C,'raw = Both(pad->buttons, input >> 8, input);')]
V['uc2']=[(D,D.replace('unsigned int x, unsigned int y','unsigned char x, unsigned char y')),(C,'raw = Both(pad->buttons, input >> 8, input);')]
V['uc3']=[(D,D.replace('unsigned int x, unsigned int y','unsigned char y, unsigned char x')),(C,'raw = Both(pad->buttons, input, input >> 8);')]
V['uc4']=[(D,SB.replace('unsigned int x, unsigned int y','unsigned char y, unsigned char x')),(C,'raw = Both(pad->buttons, input, input >> 8);')]
