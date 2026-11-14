import subprocess,sys
for f in sys.argv[1:]:
    out=subprocess.run(['python3','scratch/loop3/undef/layout.py',f,'.text,.data'],capture_output=True,text=True).stdout.splitlines()
    print('#####',f)
    sec=None; runs=[]
    for l in out:
        if l.startswith('=='): sec=l.split()[1]; print(' ',l); runs=[]; continue
        if l.strip().startswith('dataunit'):
            if '.data' in l and 'extab' not in l: print('   ',l.strip())
            continue
        a=l.split()
        if sec=='.text':
            tag=a[4] if len(a)>4 else '?'
            if tag=='UNMATCHED': print('    **UNMATCHED',a[3][:50],a[1])
            elif not runs or runs[-1][0]!=tag:
                runs.append([tag,1]); print('    ',tag)
        elif sec=='.data':
            print('     d',a[0],a[1],a[3][:40],a[4] if len(a)>4 else '')
