import sys,json,os
sys.path.insert(0,'scratch/game'); import fast
t=json.load(open(sys.argv[1])); src=open(sys.argv[2]).read(); d=sys.argv[3]; os.makedirs(d,exist_ok=True)
o,log=fast.compile_object(t,src,d); print(o or log[:2000])
