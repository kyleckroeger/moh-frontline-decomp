# var.py base.cpp out_prefix  (reads variants from stdin as python dict {name: [(old,new),...]})
import sys,subprocess,json,os
base=open(sys.argv[1]).read(); V=eval(sys.stdin.read())
for k,reps in V.items():
    s=base
    for o,n in reps:
        assert o in s,(k,o); s=s.replace(o,n)
    import os; fn=os.path.abspath(f'{sys.argv[2]}{k}.cpp'); open(fn,'w').write(s)
    r=subprocess.run(['python3','/home/kyle/workspace/moh-frontline/scratch/game/fast.py',os.environ.get('T','/home/kyle/workspace/moh-frontline/scratch/loop3/systems4/t.json'),fn]+sys.argv[3:4] or ['CreateGerman__FP20TriggerObject_structib'],capture_output=True,text=True,cwd='/home/kyle/workspace/moh-frontline')
    print(k, r.stdout.strip() or r.stderr[-300:])
