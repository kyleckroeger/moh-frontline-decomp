import sys,json,subprocess,os
sys.path.insert(0,'scratch/game'); import fast
t=json.load(open(sys.argv[1])); src=open(sys.argv[2]).read(); out=sys.argv[3]
os.makedirs(out,exist_ok=True)
o,log=fast.compile_object(t,src,out); print(o,log[:2000])
