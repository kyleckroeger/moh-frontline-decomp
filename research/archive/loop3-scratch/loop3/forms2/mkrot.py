R='/home/kyle/workspace/moh-frontline/'
src=open(R+'research/drafts/Rotate.800904e8.cpp').read()
b=open(R+'scratch/loop3/fpr2/rv/best1.txt').read()
h='inline float MulV(float a, float b) { return a * b; }\n\n'
key='inline void CMatrix::BuildRot'
a=src.index(key); src=src[:a]+h+src[a:]; a=src.index(key)
a=src.index('{',a)+2; e=src.index('    m[0][0] = txx + c;',a)
src=src[:a]+b+'\n'+src[e:]
open(R+'scratch/loop3/forms2/rot21.cpp','w').write(src)
