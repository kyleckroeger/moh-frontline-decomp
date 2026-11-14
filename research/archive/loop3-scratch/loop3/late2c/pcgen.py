import itertools,random,sys,subprocess,os,json
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game')
os.chdir('/home/kyle/workspace/moh-frontline')
import score
base=open('scratch/loop3/late2c/v/pc_f.c').read()
a=base.index('void SNDI_precalcaztospkrvol() {'); e=base.index('\n}\n',a)+3
pre,post=base[:a],base[e:]
def blk(ind,stmts):
    return ''.join(ind+s+'\n' for s in stmts)
def gen(k):
    N='sndgs.speakers'
    # block1
    def two(x,y,flip): return (y,x) if flip else (x,y)
    b1d=[ "int before = az[0] - azimuth;", "int after = azimuth + (0x10000 - az[%s - 1]);"%N]
    if k['b1_ba']: b1d=b1d[::-1]
    s1=k['b1_sum']
    den1='(before + after)' if s1==0 else '(after + before)'
    b1ab=["int a = after * 127 / %s;"%den1,"int b = before * 127 / %s;"%den1]
    if k['b1_ab']: b1ab=b1ab[::-1]
    b1=b1d+b1ab+["","sndaztospkr[i][0] = shape(a);","sndaztospkr[i][%s - 1] = shape(b);"%N]
    if k['b1_st']: b1[-2],b1[-1]=b1[-1],b1[-2]
    b2d=["int before = azimuth - az[%s - 1];"%N,"int after = 0x10000 - azimuth + az[0];"]
    if k['b2_ba']: b2d=b2d[::-1]
    den2='(after + before)' if k['b2_sum']==0 else '(before + after)'
    b2ab=["int a = before * 127 / %s;"%den2,"int b = after * 127 / %s;"%den2]
    if k['b2_ab']: b2ab=b2ab[::-1]
    b2=b2d+b2ab+["","sndaztospkr[i][0] = shape(a);","sndaztospkr[i][%s - 1] = shape(b);"%N]
    if k['b2_st']: b2[-2],b2[-1]=b2[-1],b2[-2]
    b3d=["int after = az[1] - azimuth;","int before = azimuth - az[0];"]
    if k['b3_ba']: b3d=b3d[::-1]
    den3='(before + after)' if k['b3_sum']==0 else '(after + before)'
    ca='int const a' if k['b3_const'] else 'int a'
    b3ab=["%s = after * 127 / %s;"%(ca,den3),"int b = before * 127 / %s;"%den3]
    if k['b3_ab']: b3ab=b3ab[::-1]
    b3=b3d+b3ab+["","level[0] = shape(a);","level[1] = shape(b);"]
    if k['b3_st']: b3[-2],b3[-1]=b3[-1],b3[-2]
    az_decl = "int azimuth = i * 256;" if k['azf']==0 else "int azimuth = i << 8;"
    f="void SNDI_precalcaztospkrvol() {\n    int i;\n\n    for (i = 0; i < 256; i++) {\n        %s\n"%az_decl
    f+="""        if (sndgs.speakers == 1) {
            sndaztospkr[i][0] = 127;
        } else {
            int k;
            unsigned short* az;

            for (k = 0; k < sndgs.speakers; k++)
                sndaztospkr[i][k] = 0;
            az = sndgs.azimuths[sndgs.speakers - 1];
            if (azimuth <= az[0]) {
"""+blk(' '*16,b1)+"""            } else if (azimuth >= az[sndgs.speakers - 1]) {
"""+blk(' '*16,b2)+"""            } else {
                signed char* level = sndaztospkr[i];

                for (k = 0; k < sndgs.speakers; k++) {
                    if (azimuth >= az[0] && azimuth <= az[1]) {
"""+blk(' '*24,b3)+"""                    }
                    az++;
                    level++;
                }
            }
        }
    }
}
"""
    return pre+f.replace(' '*16+'\n','\n').replace(' '*24+'\n','\n')+post
K=['b1_ba','b1_sum','b1_ab','b1_st','b2_ba','b2_sum','b2_ab','b2_st','b3_ba','b3_sum','b3_const','b3_ab','b3_st','azf']
def sc(k):
    r=score.real('config/GMFE69/satospkr.json',gen(k),'SNDI_precalcaztospkrvol__Fv','8015f538','708')
    return 9999 if r is None else r[0]+abs(r[1])*5
cur={x:0 for x in K}; cur['b3_const']=1
best=sc(cur); print('base',best,flush=True)
improved=True
while improved:
    improved=False
    for x in K:
        t=dict(cur); t[x]^=1
        s=sc(t)
        if s<best:
            best=s; cur=t; improved=True; print(best,cur,flush=True)
open('scratch/loop3/late2c/pc_best.c','w').write(gen(cur)); print('final',best,cur)
