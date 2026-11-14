# run.py: variants file with blocks separated by '=== name' lines; each block: inline text, then optional '--- call' line with replacement call text
import sys,subprocess,os
base=open('scratch/loop3/ui1/base.txt').read(); tail=open('scratch/loop3/ui1/'+os.environ.get('TAIL','tail.txt')).read()
txt=open(sys.argv[1]).read()
files=[]
for blk in txt.split('=== ')[1:]:
    name,rest=blk.split('\n',1)
    inl,_,rep=rest.partition('--- call\n')
    t=tail
    if rep.strip():
        old,new=rep.split('\n--->\n')
        assert old.strip() in t,(name,old)
        t=t.replace(old.strip(),new.strip())
    p=f'scratch/loop3/ui1/w_{name}.cpp'; open(p,'w').write(base+inl+t); files.append(p)
subprocess.run(['python3','scratch/loop3/unlockB2/dd.py','config/GMFE69/UserInterface.json','Init__13UserInterfaceFv',*files])
