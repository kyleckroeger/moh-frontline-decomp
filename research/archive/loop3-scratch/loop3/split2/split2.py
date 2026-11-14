# split2.py: opsweep.py with extended split forms (agent split2, 2026-11-13).
#   split2.py FUNC [--draft f] [--out d] [--nomember] [--j 4]   (always split-only)
# Kinds generated:
#   split@k  : 'X = t0 o1 t1 ... on tn;' -> 'X = t0..tk; X o(k+1)= t(k+1); ... X on= tn;' for every k
#              X is a simple variable (optionally declared) OR a member/array/deref lvalue (a.x, p->f, arr[i], *p)
#   splitR   : commutative last op: 'X = tn; X on= (t0..t(n-1));'
#   ret@k    : 'return t0 o1 ... tn;' -> '{ T ret_ = t0..tk; ret_ o= t..; return ret_; }' (T = function return type)
#   retv     : 'return E;' -> '{ T ret_ = E; return ret_; }'
#   collapse : 'T v = E; return v;' / 'v = E; return v;' -> 'return E;'; 'v op= E; return v;' -> 'return v op (E);'
import sys, os, re
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/loop3/opswap')
import opsweep
from opsweep import tokenize, body_mask, is_unary, PREC, COMM, STOPKW

AOPS = ('+', '-', '*', '/', '&', '|', '^')
SKIPKW = STOPKW | {'else', 'goto', 'break', 'continue', 'default'}

SCALAR = re.compile(r'^(const )?(unsigned |signed )?(int|float|double|char|short|long|bool|unsigned|u?int\d+_t|[usfUS]\d+|BOOL|uint|ulong|ushort|uchar)$')

def indent_at(src, pos):
    ls = src.rfind('\n', 0, pos) + 1
    return re.match(r'[ \t]*', src[ls:]).group(0)

def stmt_start(toks, i):
    j = i - 1; depth = 0
    while j >= 0:
        t = toks[j][1]
        if t in (')', ']'): depth += 1
        elif t in ('(', '['):
            if depth == 0: return None
            depth -= 1
        elif depth == 0 and t in (';', '{', '}'): return j + 1
        j -= 1
    return 0

def scan_expr(toks, i):
    # from token i scan to the statement's ';' at depth 0; return (j_of_semicolon, top-level op indices) or None
    j = i; depth = 0; ops = []
    while j < len(toks):
        tt = toks[j][1]
        if tt in ('(', '['): depth += 1
        elif tt in (')', ']'): depth -= 1
        elif depth == 0 and tt in (';', ',', '?', '{', '}', '=', '+=', '-=', '*=', '/=', '&=', '|=', '^=', '<<=', '>>=', '%='): break
        elif depth == 0 and tt in PREC and not is_unary(toks, j): ops.append(j)
        j += 1
    if j >= len(toks) or toks[j][1] != ';': return None
    return j, ops

def chain(toks, ops):
    if not ops: return None
    lo = min(PREC[toks[o][1]] for o in ops)
    if lo < 6: return None
    ch = [x for x in ops if PREC[toks[x][1]] == lo]
    if any(toks[x][1] not in AOPS for x in ch): return None
    return ch

def terms(src, toks, a, b, ch):
    # a..b inclusive token range, ch top-level chain op indices -> (terms text list, ops list)
    bounds = [a] + [x + 1 for x in ch]; ends = [x - 1 for x in ch] + [b]
    return [src[toks[s][2]:toks[e][3]].strip() for s, e in zip(bounds, ends)], [toks[x][1] for x in ch]

def norm(s): return re.sub(r'\s+', '', s)

def func_ret_types(src, toks):
    # map token index of each top-level function-body '{' to its return type text (or None)
    res = {}; depth = 0; stack = []
    inb = body_mask(toks)
    for i, (k, t, a, b) in enumerate(toks):
        if t == '{':
            if not inb[i] and i and toks[i - 1][1] in (')', 'const'):
                j = i - 1
                if toks[j][1] == 'const': j -= 1
                d = 0
                while j >= 0:
                    if toks[j][1] == ')': d += 1
                    elif toks[j][1] == '(':
                        d -= 1
                        if d == 0: break
                    j -= 1
                # name tokens: id (:: id)*  (j is at '(')
                j -= 1
                if j < 0 or toks[j][0] != 'id': continue
                j -= 1
                while j >= 1 and toks[j][1] == '::' and toks[j - 1][0] == 'id': j -= 2
                if j >= 0 and toks[j][1] == '~': continue
                # j now at last type token
                e = j; s = j
                while s >= 0 and toks[s][1] not in (';', '}', '{', ')') : s -= 1
                seg = toks[s + 1:e + 1]
                cps = [n for n, x in enumerate(seg) if x[1] == ':']
                if cps: seg = seg[cps[-1] + 1:]
                ty = [x[1] for x in seg if x[1] not in ('static', 'inline', 'extern', 'virtual', 'asm')]
                tyt = ' '.join(ty).replace(' *', '*').replace(' &', '&')
                res[i] = tyt if tyt and (tyt.endswith('*') or SCALAR.match(tyt)) else None
    return res

def _istypeend(toks, j):
    # true when toks[j] (an id) is the function name's first token preceded by a type token
    return True

def body_owner(toks):
    # for each token, index of the enclosing top-level function '{'
    inb = body_mask(toks); own = [None] * len(toks); cur = None; d = 0
    for i, (k, t, a, b) in enumerate(toks):
        if t == '{':
            if not inb[i] and cur is None and i and toks[i - 1][1] in (')', 'const'): cur = i; d = 0
            if cur is not None: d += 1
        elif t == '}' and cur is not None:
            d -= 1
            if d == 0: cur = None; continue
        own[i] = cur
    return own

def find_splits(src):
    toks = tokenize(src); inb = body_mask(toks); res = []
    rtypes = func_ret_types(src, toks); own = body_owner(toks)
    for i, (k, t, a, b) in enumerate(toks):
        if not inb[i]: continue
        if t == '=':
            s = stmt_start(toks, i)
            if s is None or s >= i: continue
            lv = toks[s:i]
            # cut labels / case prefixes
            cps = [n for n, x in enumerate(lv) if x[1] == ':']
            if cps: lv = lv[cps[-1] + 1:]; s = s + cps[-1] + 1
            if not lv: continue
            if any(x[0] == 'id' and x[1] in SKIPKW for x in lv): continue
            if any(x[1] in ('(', ')', '++', '--', ',', '=') for x in lv): continue
            complex_ = any(x[1] in ('.', '->', '[') for x in lv) or lv[0][1] == '*'
            if complex_:
                X = src[lv[0][2]:lv[-1][3]].strip()
                declpre = ''
            else:
                if lv[-1][0] != 'id': continue
                X = lv[-1][1]
            r = scan_expr(toks, i + 1)
            if not r: continue
            j, ops = r
            ch = chain(toks, ops)
            if not ch: continue
            ts, os_ = terms(src, toks, i + 1, j - 1, ch)
            if complex_:
                if any(norm(X) in norm(x) for x in ts): continue
                if any(re.search(r'\b%s\b' % re.escape(id_), ''.join(ts)) and False for id_ in []): pass
            else:
                ids = {x[1] for x in toks[i + 1:j] if x[0] == 'id'}
                if X in ids: continue
            ind = indent_at(src, toks[s][2])
            n = len(os_)
            for kk in range(n):
                head = ts[0] + ''.join(f' {os_[m]} {ts[m + 1]}' for m in range(kk))
                tail = ''.join(f';\n{ind}{X} {os_[m]}= {ts[m + 1]}' for m in range(kk, n))
                res.append((f'split@{kk}' + ('m' if complex_ else ''), toks[i + 1][2], toks[j][3], f'{head}{tail};', f'{X} = ... split@{kk}/{n}: {head[:40]}'))
            if os_[-1] in COMM and n >= 1:
                lt = ts[0] + ''.join(f' {os_[m]} {ts[m + 1]}' for m in range(n - 1))
                lt2 = lt if n == 1 else f'({lt})'
                res.append(('splitR' + ('m' if complex_ else ''), toks[i + 1][2], toks[j][3], f'{ts[-1]};\n{ind}{X} {os_[-1]}= {lt2};', f'{X} = {ts[-1][:30]} r'))
        elif t == 'return':
            o = own[i]
            T = rtypes.get(o) if o is not None else None
            if not T: continue
            r = scan_expr(toks, i + 1)
            if not r: continue
            j, ops = r
            if j == i + 1: continue
            ind = indent_at(src, a)
            E = src[toks[i + 1][2]:toks[j - 1][3]].strip()
            # collapse: 'return v;' after 'v = E;' / 'T v = E;' / 'v op= E;'
            if j == i + 2 and toks[i + 1][0] == 'id' and i >= 1 and toks[i - 1][1] == ';':
                v = toks[i + 1][1]
                ps = stmt_start(toks, i - 1)
                if ps is not None and ps < i - 1:
                    pv = toks[ps:i - 1]
                    eqs = [n for n, x in enumerate(pv) if x[1] in ('=', '+=', '-=', '*=', '/=', '&=', '|=', '^=')]
                    okc = bool(eqs) and eqs[0] > 0 and not any(x[0] == 'id' and x[1] in SKIPKW for x in pv[:eqs[0]]) and not any(x[1] in ('(', ',', '.', '->', '[', ':') for x in pv[:eqs[0]]) and pv[eqs[0] - 1][1] == v
                    if okc:
                        opt = pv[eqs[0]][1]
                        PE = src[pv[eqs[0] + 1][2]:pv[-1][3]].strip()
                        if opt == '=':
                            new = f'return {PE};'
                        elif eqs[0] == 1:
                            new = f'return {v} {opt[0]} ({PE});'
                        else:
                            new = None
                        if new:
                            res.append(('splitC', toks[ps][2], toks[j][3], new, f'collapse {v}'))
            if j > i + 2: res.append(('splitV', a, toks[j][3], f'{{\n{ind}    {T} ret_ = {E};\n{ind}    return ret_;\n{ind}}}', f'retv {E[:40]}'))
            ch = chain(toks, ops)
            if not ch: continue
            ts, os_ = terms(src, toks, i + 1, j - 1, ch)
            n = len(os_)
            for kk in range(n):
                head = ts[0] + ''.join(f' {os_[m]} {ts[m + 1]}' for m in range(kk))
                tail = ''.join(f'\n{ind}    ret_ {os_[m]}= {ts[m + 1]};' for m in range(kk, n))
                res.append((f'splitT@{kk}', a, toks[j][3], f'{{\n{ind}    {T} ret_ = {head};{tail}\n{ind}    return ret_;\n{ind}}}', f'ret@{kk}/{n} {head[:40]}'))
    return res

opsweep.find_splits = find_splits

if __name__ == '__main__':
    if '--list' in sys.argv:
        src = open(sys.argv[sys.argv.index('--list') + 1]).read()
        for x in find_splits(src):
            print(x[0], repr(src[x[1]:x[2]][:70]), '=>', repr(x[3][:90]))
        sys.exit()
    for f in ('--split', '--splitonly'):
        if f not in sys.argv: sys.argv.append(f)
    opsweep.main()
