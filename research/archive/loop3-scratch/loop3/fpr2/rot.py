import sys
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
import s as S
S.ROWS['rot']=('config/GMFE69/matrix_rotate.json','Rotate__7CMatrixF8CVector3f','800904e8','376')
S.ROWS['br']=('config/GMFE69/matrix_translate.json','BuildRot__7CMatrixF8CVector3f','80090980','288')
rot=open(R+'scratch/loop3/unlockC/rot/landed.cpp').read(); mt=open(R+'src/game/matrix_translate.cpp').read()
def put(src,key,b,helpers=''):
    a=src.index(key); src=src[:a]+helpers+src[a:]; a=src.index(key)
    a=src.index('{',a)+2; e=src.index('    m[0][0] = txx + c;' if 'inline' in key else '    right.x = txx + c;',a)
    return src[:a]+b+src[e:]
def both(b,helpers='',show=False):
    r2=S.run('br',put(mt,'void CMatrix::BuildRot(',b,helpers),show)
    r1=S.run('rot',put(rot,'inline void CMatrix::BuildRot',b,helpers),show)
    return r1[0] if r1 else None, r2[0] if r2 else None
if __name__=='__main__':
    b=open(sys.argv[1]).read(); h=open(sys.argv[2]).read() if len(sys.argv)>2 else ''
    print(both(b,h,True))
