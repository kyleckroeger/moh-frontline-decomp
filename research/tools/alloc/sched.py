"""sched.py <src> <unit> [out.txt] [-f substr]: dump MWCC 1.3.2's instruction scheduler (Scheduler.c) decisions.

Runs an int3-patched copy of the real compiler under ptrace (no code change otherwise); the copy is made
on first use in $SCHED_CC (default scratch/sched_cc). $SCHED_OBJ=path keeps the object.
Also prints every pcode inserted/deleted between the two passes (0x4dc1b0/0x4dc1f0/0x4dc230, with the
caller; 0x57a596 = copy coalescing): such an edit clears the block's 'scheduled' flag (8) and makes the
post-RA pass reschedule it with the real registers.
Breakpoints: 0x506e10 scheduleinstructions(pass: 0 = before RA, 1 = after RA), 0x506f20 schedule block,
0x506ff9 dependency graph built (dump nodes in input order), 0x50703b selector returned (pick + cycle).
Per node: lat (own latency), slack (+0x14 = critical height max - height; a node with cycle < slack is
'not urgent'), h (+0x16 height = longest latency path to block end), np (unscheduled preds), succs.
Selector (0x5070f0): first ready node in input order, then a later ready node replaces it if
(best not urgent and cand urgent) or more successors with np==1 or equal and higher h
(or equal and lower opcode class byte when the global at 0x5e86a0 is set).
"""
import sys, os, ctypes, struct, signal, tempfile
ROOT = '/home/kyle/workspace/moh-frontline'
CC = os.environ.get('SCHED_CC', ROOT + '/scratch/sched_cc')
REAL = ROOT + '/build/compiler/GC/1.3.2'
libc = ctypes.CDLL(None, use_errno=True)
libc.ptrace.restype = ctypes.c_long
libc.ptrace.argtypes = [ctypes.c_long, ctypes.c_long, ctypes.c_void_p, ctypes.c_void_p]
class R(ctypes.Structure):
    _fields_ = [(n, ctypes.c_ulonglong) for n in 'r15 r14 r13 r12 rbp rbx r11 r10 r9 r8 rax rcx rdx rsi rdi orig_rax rip cs eflags rsp ss fs_base gs_base ds es fs gs'.split()]
ORIG = {0x506e10: 0x53, 0x506f20: 0x53, 0x506ff9: 0xff, 0x50703b: 0x89, 0x4dc1b0: 0x53, 0x4dc1f0: 0x53, 0x4dc230: 0x53}
if not os.path.exists(CC + '/mwcceppc.exe'):  # make the int3-patched copy of the real compiler once
    import shutil
    os.makedirs(CC, exist_ok=True)
    for f in os.listdir(REAL): shutil.copy(os.path.join(REAL, f), CC)
    d = bytearray(open(REAL + '/mwcceppc.exe', 'rb').read())
    for va, b in ORIG.items():
        assert d[va - 0x401000 + 0x400] == b; d[va - 0x401000 + 0x400] = 0xcc
    open(CC + '/mwcceppc.exe', 'wb').write(d)
args = [a for a in sys.argv[1:]]
filt = None
if '-f' in args:
    i = args.index('-f'); filt = args[i + 1]; del args[i:i + 2]
src = args[0]; unit = args[1]; outp = args[2] if len(args) > 2 else None
import json
t = json.load(open(f'{ROOT}/config/GMFE69/{unit}.json'))
w = tempfile.mkdtemp(); p = os.path.join(w, 'probe.cpp'); open(p, 'w').write(open(src).read())
dirs = t.get('include_dirs', []) + ([] if 'include' in t.get('include_dirs', []) else ['include'])
cmd = [ROOT + '/build/compiler/wibo', CC + '/mwcceppc.exe', '-c', *t['compiler_flags'], '-I-',
       *[f'-I{ROOT}/{x}' for x in dirs], p, '-o', w + '/probe.o']
pid = os.fork()
if pid == 0:
    os.chdir(w); libc.ptrace(0, 0, None, None); os.execv(cmd[0], cmd)
os.waitpid(pid, 0)
mem = None
def rd(a, n): mem.seek(a); return mem.read(n)
def u32(a): return struct.unpack('<I', rd(a, 4))[0]
def s32(a): return struct.unpack('<i', rd(a, 4))[0]
def s16(a): return struct.unpack('<h', rd(a, 2))[0]
def u16(a): return struct.unpack('<H', rd(a, 2))[0]
def cstr(a): return rd(a, 24).split(b'\0')[0].decode('latin1')
def wr(a, b):
    a8 = a & ~7; old = libc.ptrace(2, pid, ctypes.c_void_p(a8), None)
    ob = bytearray(struct.pack('<q', old)); ob[a - a8:a - a8 + len(b)] = b
    libc.ptrace(5, pid, ctypes.c_void_p(a8), ctypes.c_void_p(struct.unpack('<Q', bytes(ob))[0]))
opn = {}
def opname(op):
    if op not in opn: opn[op] = cstr(u32(0x5bee80 + op * 0x12 - 8))
    return opn[op]
objn = {}
def objname(o):
    if not o: return '0'
    if o not in objn:
        try:
            nm = u32(o + 0x4) if False else 0
        except Exception: nm = 0
        objn[o] = 'o%d' % len(objn)
    return objn[o]
RC = {0: 'spr', 1: 'cr', 2: 'v', 3: 'f', 4: 'r'}
def insn(pc):
    op = s16(pc + 0x20); n = s16(pc + 0x22); a = pc + 0x24; ops = []
    for k in range(n):
        kind = rd(a, 1)[0]; cls = rd(a + 1, 1)[0]; fl = u16(a + 2)
        if kind == 0:
            ops.append('%s%d%s' % (RC.get(cls, 'c%d_' % cls), s16(a + 4), '' if not (fl & 2) else ''))
        elif kind == 1: ops.append('spr%d' % s16(a + 4))
        elif kind == 2: ops.append(str(s32(a + 4)))
        elif kind == 3: ops.append('m[%s]' % rd(a+4,8).hex())
        elif kind == 4: ops.append('L%x' % u32(a + 4))
        else: ops.append('k%d' % kind)
        a += 12
    return '%s %s' % (opname(op), ','.join(ops))
regs = R(); out = []
def emit(s): out.append(s)
libc.ptrace(7, pid, None, None)
idx = {}; blk = 0; npass = None; armed = set(ORIG)
def rearm(ip):
    wr(ip, bytes([ORIG[ip]])); regs.rip -= 1; libc.ptrace(13, pid, None, ctypes.byref(regs))
    libc.ptrace(9, pid, None, None); os.waitpid(pid, 0); wr(ip, b'\xcc')
first = True
while True:
    _, st = os.waitpid(pid, 0)
    if os.WIFEXITED(st) or os.WIFSIGNALED(st): break
    sig = os.WSTOPSIG(st); fwd = 0
    if sig == signal.SIGTRAP:
        if mem is None: mem = open(f'/proc/{pid}/mem', 'r+b', buffering=0)
        libc.ptrace(12, pid, None, ctypes.byref(regs)); ip = (regs.rip - 1) & 0xffffffff
        esp = regs.rsp & 0xffffffff
        if ip in ORIG:
            if ip == 0x506e10:
                npass = rd(esp + 4, 1)[0]; emit('=== PASS %s' % ('pre-RA' if npass == 0 else 'post-RA'))
            elif ip == 0x506f20:
                blk += 1; emit('--- block %d (%x)' % (blk, u32(esp + 4)))
            elif ip == 0x506ff9:
                head = regs.rbp & 0xffffffff; nodes = []; n = head
                while n: nodes.append(n); n = u32(n)
                idx = {n: i for i, n in enumerate(nodes)}
                for i, n in enumerate(nodes):
                    succ = []; e = u32(n + 8)
                    while e: succ.append('%d/%d' % (idx.get(u32(e + 4), -1), u16(e + 8))); e = u32(e)
                    emit('  n%-3d lat=%d slack=%d h=%d np=%d  %-34s -> %s' % (i, u16(n + 0x10), u16(n + 0x14), u16(n + 0x16), s16(n + 0x18), insn(u32(n + 0xc)), ' '.join(reversed(succ))))
            elif ip in (0x4dc1b0, 0x4dc1f0, 0x4dc230):
                if npass == 0:
                    a1 = u32(esp + 4)
                    if ip == 0x4dc230: emit('  [deleted from block %x by %x] %s' % (u32(a1 + 8), u32(esp), insn(a1)))
                    else: emit('  [inserted %s block %x] %s' % ('after' if ip == 0x4dc1b0 else 'before', u32((a1 if ip == 0x4dc1f0 else a1) + 8), insn(u32(esp + 8))))
            elif ip == 0x50703b:
                sel = regs.rax & 0xffffffff; cyc = u16(esp + 0xc)
                emit('  pick c%d n%d %s' % (cyc, idx.get(sel, -1), insn(u32(sel + 0xc)) if sel else ''))
            rearm(ip)
        else: fwd = 0
    else: fwd = sig
    libc.ptrace(7, pid, None, ctypes.c_void_p(fwd))
txt = '\n'.join(out) + '\n'
if filt:
    keep = []; cur = []
    for l in out:
        if l.startswith('---') or l.startswith('==='):
            if cur and any(filt in x for x in cur): keep += cur
            cur = [l]
        else: cur.append(l)
    if cur and any(filt in x for x in cur): keep += cur
    txt = '\n'.join(keep) + '\n'
(open(outp, 'w') if outp else sys.stdout).write(txt)
import shutil
if os.path.exists(w + '/probe.o') and os.environ.get('SCHED_OBJ'): shutil.copy(w + '/probe.o', os.environ['SCHED_OBJ'])
shutil.rmtree(w, ignore_errors=True)
