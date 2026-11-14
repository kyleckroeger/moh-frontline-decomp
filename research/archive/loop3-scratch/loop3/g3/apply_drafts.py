import csv,sys,os,shutil
sys.path.insert(0,'research/tools'); import check_draft as c, score
changes=[ # function, new draft path, src, settings, real
 ('GenerateTestSplinePath__13','research/drafts/GenerateTestSplinePath.80050af4.cpp','scratch/loop3/g3/gtsp/k.cpp',None),
 ('YUV_WriteRow__','research/drafts/YUV_WriteRow.801083dc.cpp','scratch/loop3/g3/yuv/b.cpp',None),
 ('TRKSuppAccessFile','research/drafts/TRKSuppAccessFile.80135324.c','scratch/loop3/g3/trk/draft.c','config/GMFE69/support.json'),
]
rows=list(csv.reader(open('research/drafts/index.tsv'),delimiter='\t'))
hdr=rows[0]
st=list(csv.reader(open('research/drafts/status.tsv'),delimiter='\t'))
for key,dst,src,tm in changes:
    for r in rows[1:]:
        if key in r[3]:
            old=r[5]
            if tm: r[4]=tm
            if old!=dst and os.path.exists(old): os.remove(old)
            shutil.copy(src,dst); r[5]=dst
            res=score.real(r[4],open(dst).read(),r[3],r[2],r[1])
            r[0]=str(res[0])
            cmp=c.compare(open(dst).read(),r[4],r[3],r[2],r[1])
            print(r[3][:40],res,cmp['status'],round(cmp['similarity'],3))
            for s in st[1:]:
                if s[5]==r[3]:
                    s[0]=cmp['status']; s[1]='%.3f'%cmp['similarity']; s[2]='%.3f'%cmp['offsets']; s[3]=str(cmp['lines']); s[6]=str(cmp['calls_same']); s[7]=str(sum(cmp['missing'].values())); s[8]=str(sum(cmp['extra'].values())); s[9]=dst
with open('research/drafts/index.tsv','w',newline='') as f: csv.writer(f,delimiter='\t',lineterminator='\n').writerows(rows)
with open('research/drafts/status.tsv','w',newline='') as f: csv.writer(f,delimiter='\t',lineterminator='\n').writerows(st)
