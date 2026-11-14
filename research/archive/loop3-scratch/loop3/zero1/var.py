"""var.py template variants.txt: variants separated by lines '=== name'; prints real score + shared flag"""
import sys,os,subprocess,tempfile,re
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import score
T='/home/kyle/workspace/moh-frontline/config/GMFE69/ShellMenu_levelstats.json'
tm=open(sys.argv[1]).read()
parts=re.split(r'^=== (.*)$',open(sys.argv[2]).read(),flags=re.M)
os.makedirs('v',exist_ok=True)
for i in range(1,len(parts),2):
    name,body=parts[i].strip(),parts[i+1]
    src=tm.replace('@@BODY@@',body)
    open(f'v/{name}.cpp','w').write(src)
    print(name, score.real(T,src,'CalculateLevelStats__10CShellMenuFv','0x800e6374','432'),flush=True)
