"""hdrtest.py ctor_file [all]: header = HEAD header (+push_front change) with the list range ctor replaced by ctor_file.
prints buddy range ctor diff and, with 'all', units whose symbol names change vs HEAD header."""
import sys,json,tempfile,subprocess,re,os
ARG=os.path.abspath(sys.argv[1]); os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); import fast,varmap,score
base=subprocess.run(['git','show','HEAD:include/game/DwiContainers.h'],capture_output=True,text=True).stdout
i=base.index("    template <class I>\n    list(int capacity, I first, I last, char* name)"); j=base.index("    // Through clear(): an expanded inline")
def mk(ctor):
    h=base[:i]+ctor+base[j:]
    return h.replace("        list_node<T>* pos = m_head.m_next;\n        list_node<T>* prev = pos->m_prev;","        list_node<T>* pos = begin().m_node;\n        list_node<T>* prev = pos->m_prev;")
D='scratch/loop4/ba/incv'; os.makedirs(D+'/game',exist_ok=True)
def comp(u,h,src=None):
    open(D+'/game/DwiContainers.h','w').write(h)
    t=json.load(open(f'config/GMFE69/{u}.json')) if os.path.exists(f'config/GMFE69/{u}.json') else json.load(open('scratch/loop4/ba/landing/buddy.json'))
    t['include_dirs']=[D]+t['include_dirs']
    obj,log=fast.compile_object(t,open(src or t['source']).read(),tempfile.mkdtemp())
    return obj
h=mk(open(ARG).read())
obj=comp('buddy',h,'scratch/loop4/ba/buddy_land.cpp')
if not obj: print('compile fail'); sys.exit()
n='__ct<Q23dwi21list_iter<9BuddyInfo>>__Q23dwi16list<9BuddyInfo>FiQ23dwi21list_iter<9BuddyInfo>Q23dwi21list_iter<9BuddyInfo>Pc'
m=varmap.rows(['--disassemble='+n,obj]); o=varmap.rows(['--start-address=0x80058988','--stop-address=0x80058af0',fast.ELF])
print('ctor diff',sum(1 for x,y in zip(o,m) if not score.eq(x,y))+abs(len(o)-len(m)), 'len',len(m),len(o))
if len(sys.argv)>2:
    syms=lambda obj: re.sub(r'^[0-9a-f]+ ','',subprocess.run([fast.OBJDUMP,'-tdr',obj],capture_output=True,text=True).stdout.split('SYMBOL TABLE')[-1],flags=re.M)
    for line in open('/tmp/claude-1000/-home-kyle-workspace-moh-frontline/e1f4a78d-97e2-4c48-9222-8d4f5cde9463/scratchpad/units.txt'):
        u=line.split()[0]
        if u.startswith('buddy'): continue
        a=syms(comp(u,base)); b=syms(comp(u,h))
        if a!=b: print('  numbering changes:',u)
    print('done')
