"""tr.py src.cpp [unit]: compile with the int3-patched mwcceppc under ptrace; at each colour call (0x507a30) dump
the simplify stack (pop order) with node index, degree and neighbours, and the colours after. Output JSON lines."""
import sys,os,ctypes,struct,json,signal,tempfile
ROOT='/home/kyle/workspace/moh-frontline'
TR=ROOT+'/scratch/loop4/ba/trace'
libc=ctypes.CDLL(None,use_errno=True)
libc.ptrace.restype=ctypes.c_long; libc.ptrace.argtypes=[ctypes.c_long,ctypes.c_long,ctypes.c_void_p,ctypes.c_void_p]
class R(ctypes.Structure): _fields_=[(n,ctypes.c_ulonglong) for n in 'r15 r14 r13 r12 rbp rbx r11 r10 r9 r8 rax rcx rdx rsi rdi orig_rax rip cs eflags rsp ss fs_base gs_base ds es fs gs'.split()]
ENTRY=0x507a30; NUM=0x4fd680
BPS={}
d=bytearray(open(ROOT+'/scratch/loop4/regrules2/nosched/mwcceppc.exe','rb').read())
for a in (ENTRY,NUM): BPS[a]=bytes([d[a-0x401000+0x400]])
src=sys.argv[1]; unit=sys.argv[2] if len(sys.argv)>2 else 'buddy_head'
t=json.load(open(f'{ROOT}/config/GMFE69/{unit}.json'))
w=tempfile.mkdtemp(); p=os.path.join(w,'probe.cpp'); open(p,'w').write(open(src).read())
dirs=t.get('include_dirs',[])+([] if 'include' in t.get('include_dirs',[]) else ['include'])
cmd=[ROOT+'/build/compiler/wibo',TR+'/mwcceppc.exe','-c',*t['compiler_flags'],'-I-',*[f'-I{ROOT}/{x}' for x in dirs],p,'-o',w+'/probe.o']
pid=os.fork()
if pid==0:
    os.chdir(w); libc.ptrace(0,0,None,None); os.execv(cmd[0],cmd)
os.waitpid(pid,0)
mem=None
def rd(a,n):
    mem.seek(a); return mem.read(n)
def u32(a): return struct.unpack('<I',rd(a,4))[0]
def s16(a): return struct.unpack('<h',rd(a,2))[0]
def wr(a,b):
    # POKEDATA 8 bytes
    a8=a&~7; old=libc.ptrace(2,pid,ctypes.c_void_p(a8),None)
    ob=bytearray(struct.pack('<q',old)); ob[a-a8:a-a8+len(b)]=b
    libc.ptrace(5,pid,ctypes.c_void_p(a8),ctypes.c_void_p(struct.unpack('<Q',bytes(ob))[0]))
regs=R(); pending=None; out=[]
libc.ptrace(7,pid,None,None)
armed=False; names=[]
while True:
    _,st=os.waitpid(pid,0)
    if os.WIFEXITED(st) or os.WIFSIGNALED(st): break
    sig=os.WSTOPSIG(st); fwd=0
    if sig==signal.SIGTRAP:
        if mem is None: mem=open(f'/proc/{pid}/mem','r+b',buffering=0)
        libc.ptrace(12,pid,None,ctypes.byref(regs)); ip=(regs.rip-1)&0xffffffff
        if ip==NUM:
            esp=regs.rsp&0xffffffff; obj=u32(esp+4); cls=rd(esp+8,1)[0]
            nm=u32(obj+10); name=rd(nm+10,40).split(b'\0')[0].decode('latin1') if nm else '?'
            names.append([cls,u32(0x5e8a7c+4*cls),name])
            wr(NUM,BPS[NUM]); regs.rip-=1; libc.ptrace(13,pid,None,ctypes.byref(regs))
            libc.ptrace(9,pid,None,None); os.waitpid(pid,0); wr(NUM,b'\xcc'); libc.ptrace(7,pid,None,None); continue
        if ip==ENTRY:
            esp=regs.rsp&0xffffffff; ret=u32(esp); lst=u32(esp+4)
            cls=rd(0x5e931f,1)[0]; arr=u32(0x5e87d0); lo=u32(0x5e8778+4*cls); hi=u32(0x5e8a7c+4*cls)
            ptrs={u32(arr+4*i):i for i in range(hi)}
            nodes=[]; n=lst
            while n:
                nodes.append(n); n=u32(n)
            rec={'cls':cls,'lo':lo,'hi':hi,'stack':[],'names':[x for x in names if x[0]==cls]}; names[:]=[x for x in names if x[0]!=cls]
            for n in nodes:
                k=s16(n+0x18); rec['stack'].append([ptrs.get(n,-1),s16(n+0x12),k,[s16(n+0x1a+2*j) for j in range(k)]])
            pending=(ret,rec,nodes,rd(ret,1))
            wr(ENTRY,BPS[ENTRY]); wr(ret,b'\xcc'); regs.rip-=1; libc.ptrace(13,pid,None,ctypes.byref(regs))
        elif pending and ip==pending[0]:
            ret,rec,nodes,ob=pending
            for e,n in zip(rec['stack'],nodes): e.append(s16(n+0x14))
            out.append(rec); pending=None
            wr(ret,ob); wr(ENTRY,b'\xcc'); regs.rip-=1; libc.ptrace(13,pid,None,ctypes.byref(regs))
        else: fwd=0  # exec trap
    else: fwd=sig
    libc.ptrace(7,pid,None,ctypes.c_void_p(fwd))
json.dump(out,open(sys.argv[3] if len(sys.argv)>3 else '/dev/stdout','w'))
import shutil; shutil.copy(w+'/probe.o','/tmp/claude-1000/-home-kyle-workspace-moh-frontline/e1f4a78d-97e2-4c48-9222-8d4f5cde9463/scratchpad/last.o')
