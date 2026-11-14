# opsweep.py: commutative operand-swap sweep over a draft.
#   opsweep.py FUNC [--draft path] [--all] [--out dir] [--j 4]
# Finds every binary commutative operator (* + & | ^) in function bodies of the draft,
# builds a variant swapping its two operands (each wrapped in parens), scores each with
# score.real, then greedily combines the improving swaps. Also tries 'x op= y' -> 'x = y op x'.
# Writes best variant to OUT/FUNC_short.best.cpp and a log line per variant.
import sys, os, re, csv, json, tempfile
from concurrent.futures import ThreadPoolExecutor
ROOT = '/home/kyle/workspace/moh-frontline'
sys.path.insert(0, ROOT + '/scratch/game'); sys.path.insert(0, ROOT + '/research/tools')
os.chdir(ROOT)
import fast, score

TOK = re.compile(r'''(?P<ws>\s+)|(?P<com>//[^\n]*|/\*.*?\*/)|(?P<pp>^[ \t]*\#[^\n]*(?:\\\n[^\n]*)*)|(?P<str>"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')|(?P<num>(?:0[xX][0-9a-fA-F]+|\d+\.?\d*(?:[eE][+-]?\d+)?|\.\d+(?:[eE][+-]?\d+)?)[fFuUlL]*)|(?P<id>[A-Za-z_]\w*)|(?P<op>->\*?|\+\+|--|<<=|>>=|<<|>>|<=|>=|==|!=|&&|\|\||[-+*/%&|^]=|::|\.\.\.|[-+*/%&|^~!<>=?:;,.()\[\]{}])''', re.S | re.M)

def tokenize(s):
    out = []
    for m in TOK.finditer(s):
        k = m.lastgroup
        if k in ('ws', 'com', 'pp'): continue
        out.append((k, m.group(), m.start(), m.end()))
    return out

PREC = {'*': 13, '/': 13, '%': 13, '+': 12, '-': 12, '<<': 11, '>>': 11, '<': 10, '>': 10, '<=': 10, '>=': 10,
        '==': 9, '!=': 9, '&': 8, '^': 7, '|': 6, '&&': 5, '||': 4}
MIRROR = {'<': '>', '>': '<', '<=': '>=', '>=': '<=', '==': '==', '!=': '!='}
COMM = {'*', '+', '&', '|', '^'}
STOPKW = {'return', 'case', 'else', 'do', 'if', 'while', 'for', 'switch', 'throw', 'new', 'delete', 'sizeof'}

def is_unary(toks, i):
    if i == 0: return True
    k, t = toks[i - 1][0], toks[i - 1][1]
    if k in ('id',) and t in STOPKW: return True
    if k in ('id', 'num', 'str'): return False
    if t in (')', ']', '++', '--'): return False
    return True

def operand_left(toks, i, p):
    j = i - 1; depth = 0
    while j >= 0:
        k, t = toks[j][0], toks[j][1]
        if t in (')', ']'): depth += 1; j -= 1; continue
        if t in ('(', '['):
            if depth == 0: break
            depth -= 1; j -= 1; continue
        if depth > 0: j -= 1; continue
        if k == 'id' and t in STOPKW: break
        if t in (';', ',', '{', '}', '?', ':', '=', '+=', '-=', '*=', '/=', '&=', '|=', '^=', '<<=', '>>=', '%=', 'return'): break
        if t in PREC and not is_unary(toks, j):
            if PREC[t] < p: break
        j -= 1
    return j + 1

def operand_right(toks, i, p):
    j = i + 1; depth = 0; n = len(toks)
    while j < n:
        k, t = toks[j][0], toks[j][1]
        if t in ('(', '['): depth += 1; j += 1; continue
        if t in (')', ']'):
            if depth == 0: break
            depth -= 1; j += 1; continue
        if depth > 0: j += 1; continue
        if t in (';', ',', '{', '}', '?', ':', '=', '+=', '-=', '*=', '/=', '&=', '|=', '^=', '<<=', '>>=', '%='): break
        if t in PREC and not is_unary(toks, j) and j > i + 1:
            if PREC[t] <= p: break
        j += 1
    return j - 1

def body_mask(toks):
    # brace depth >= 1 inside something that looks like a function body: mark tokens in braces
    # opened right after ')' or 'const' (function bodies) and nested braces.
    inb = [False] * len(toks); stack = []
    for i, (k, t, a, b) in enumerate(toks):
        if t == '{':
            prev = toks[i - 1][1] if i else ''
            fn = bool(stack and stack[-1]) or prev in (')', 'const')
            # constructor init lists: ') : a(b) {'
            if not fn and prev in (')',): fn = True
            if not fn and i and toks[i-1][0] in ('id', 'num') or prev == ')':
                # look back for ':' init list ending with ')' handled above
                pass
            stack.append(fn)
        elif t == '}':
            if stack: stack.pop()
        else:
            inb[i] = bool(stack and stack[-1])
    return inb

def class_ranges(src):
    toks = tokenize(src); out = []
    for i, (k, t, a, b) in enumerate(toks):
        if t in ('struct', 'class', 'union'):
            j = i + 1
            while j < len(toks) and toks[j][1] not in ('{', ';', '(', ')', ','): j += 1
            if j < len(toks) and toks[j][1] == '{':
                d = 0
                for m in range(j, len(toks)):
                    if toks[m][1] == '{': d += 1
                    elif toks[m][1] == '}':
                        d -= 1
                        if d == 0: out.append((toks[j][2], toks[m][3])); break
    return out

def find_swaps(src):
    toks = tokenize(src); inb = body_mask(toks); res = []
    for i, (k, t, a, b) in enumerate(toks):
        if not inb[i]: continue
        if t in COMM and not is_unary(toks, i):
            if i + 1 < len(toks) and toks[i+1][1] in ('>', ')', ',', '*', '&', '>>', 'const'): continue
            p = PREC[t]
            l = operand_left(toks, i, p); r = operand_right(toks, i, p)
            if l >= i or r <= i: continue
            L = src[toks[l][2]:toks[i - 1][3]]; R = src[toks[i + 1][2]:toks[r][3]]
            if t == '*' and re.fullmatch(r'\s*(const\s+)?[A-Za-z_]\w*\s*', L) and toks[r][0] == 'id' and r + 1 < len(toks) and toks[r+1][1] in ('=', ';', ',', ')') and (l == 0 or toks[l-1][1] in (';', '{', '}', '(', ',')):
                continue  # 'T * p =' declaration
            if t == '&' and (l == 0 or toks[l-1][1] in (';', '{', '}', '(', ',')) and toks[r][0] == 'id' and r + 1 < len(toks) and toks[r+1][1] in ('=', ';', ',', ')'):
                continue  # 'T & r =' declaration
            w = lambda s: s if re.fullmatch(r'\s*[\w.]+\s*', s) else '(' + s + ')'
            res.append(('swap', toks[l][2], toks[r][3], w(R) + ' ' + t + ' ' + w(L), f'{L.strip()} {t} {R.strip()}'))
        if t in MIRROR and '--cmp' in sys.argv and not is_unary(toks, i):
            p = PREC[t]
            l = operand_left(toks, i, p); r = operand_right(toks, i, p)
            if l >= i or r <= i: continue
            L = src[toks[l][2]:toks[i - 1][3]]; R = src[toks[i + 1][2]:toks[r][3]]
            w = lambda s: s if re.fullmatch(r'\s*[\w.]+\s*', s) else '(' + s + ')'
            res.append(('cmp', toks[l][2], toks[r][3], w(R) + ' ' + MIRROR[t] + ' ' + w(L), f'{L.strip()} {t} {R.strip()}'))
        if t in ('*=', '+=', '&=', '|=', '^='):
            l = operand_left(toks, i, 0); r = operand_right(toks, i, 0)
            if l >= i or r <= i: continue
            L = src[toks[l][2]:toks[i - 1][3]]; R = src[toks[i + 1][2]:toks[r][3]]
            if not re.fullmatch(r'\s*[\w.\[\]>-]+\s*', L): continue
            op = t[0]
            w = lambda s: s if re.fullmatch(r'\s*[\w.]+\s*', s) else '(' + s + ')'
            res.append(('opeq', toks[l][2], toks[r][3], f'{L.strip()} = {w(R)} {op} {L.strip()}', f'{L.strip()} {t} {R.strip()}'))
    return res

def find_splits(src):
    # 'v = L op R;' / 'T v = L op R;' -> 'v = L; v op= R;' and (commutative) 'v = R; v op= L;'
    toks = tokenize(src); inb = body_mask(toks); res = []
    for i, (k, t, a, b) in enumerate(toks):
        if t != '=' or not inb[i] or i < 1 or toks[i - 1][0] != 'id': continue
        v = toks[i - 1][1]
        if i >= 2 and toks[i - 2][1] in ('.', '->', '::'): continue
        j = i + 1; depth = 0; ops = []
        while j < len(toks):
            tt = toks[j][1]
            if tt in ('(', '['): depth += 1
            elif tt in (')', ']'): depth -= 1
            elif depth == 0 and tt in (';', ',', '?', '{', '}'): break
            elif depth == 0 and tt in PREC and not is_unary(toks, j): ops.append(j)
            j += 1
        if j >= len(toks) or toks[j][1] != ';' or not ops: continue
        lo = min(PREC[toks[o][1]] for o in ops)
        if lo < 6: continue
        o = [x for x in ops if PREC[toks[x][1]] == lo][-1]
        op = toks[o][1]
        if op not in ('+', '-', '*', '/', '&', '|', '^'): continue
        L = src[toks[i + 1][2]:toks[o - 1][3]]; R = src[toks[o + 1][2]:toks[j - 1][3]]
        ids = {x[1] for x in toks[i + 1:j] if x[0] == 'id'}
        if v in ids: continue
        ind = re.search(r'[ \t]*$', src[:toks[i - 1][2]][:]).group(0)
        line_start = src.rfind('\n', 0, toks[i - 1][2]) + 1
        indent = re.match(r'[ \t]*', src[line_start:]).group(0)
        res.append(('split', toks[i + 1][2], toks[j][3], f'{L.strip()};\n{indent}{v} {op}= {R.strip()};', f'{v} = {L.strip()} {op} {R.strip()}'))
        if op in COMM:
            res.append(('splitR', toks[i + 1][2], toks[j][3], f'{R.strip()};\n{indent}{v} {op}= {L.strip()};', f'{v} = {R.strip()} {op}r {L.strip()}'))
    return res

def apply(src, edits):
    # edits: list of (start,end,new); must not overlap... nested swaps: apply outermost-safe by
    # applying in descending start, skipping overlaps.
    out = src; last = None
    for s, e, new in sorted(edits, key=lambda x: -x[0]):
        if last is not None and e > last: return None
        out = out[:s] + new + out[e:]; last = s
    return out

def main():
    a = sys.argv[1:]
    func = a[0]
    rows = {r['function']: r for r in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t')}
    r = rows[func]
    draft = a[a.index('--draft') + 1] if '--draft' in a else r['draft']
    outd = a[a.index('--out') + 1] if '--out' in a else 'scratch/loop3/opswap/out'
    jobs = int(a[a.index('--j') + 1]) if '--j' in a else 4
    os.makedirs(outd, exist_ok=True)
    pass
    src = open(draft, errors='replace').read()
    t = r['compile_settings_from']
    import difflib, subprocess
    tgt = fast.target(func)
    tj = json.load(open(t))
    def sc(text):
        try:
            v = score.real(t, text, func, r['address'], r['size'])
        except Exception as e:
            return None
        if v is None: return None
        w = tempfile.mkdtemp()
        obj, _ = fast.compile_object(tj, text, w)
        m = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout).get(func, [])
        sm = difflib.SequenceMatcher(None, tgt, m, autojunk=False)
        dl = sum((i2 - i1) + (j2 - j1) for op, i1, i2, j1, j2 in sm.get_opcodes() if op != 'equal')
        subprocess.run(['rm', '-rf', w])
        return (v[0] + abs(v[1]), dl)
    base = sc(src)
    sw = find_swaps(src)
    if '--split' in a:
        sw = sw + find_splits(src)
    if '--splitonly' in a:
        sw = [x for x in sw if x[0].startswith('split')]
    if '--nomember' in a:
        cr = class_ranges(src)
        sw = [x for x in sw if not any(lo <= x[1] < hi for lo, hi in cr)]
    if '--cmponly' in a:
        sw = [x for x in sw if x[0] == 'cmp']
    short = re.sub(r'\W', '_', func)[:40]
    log = open(f'{outd}/{short}.log', 'w')
    print(f'base {base} swaps {len(sw)} {draft}', file=log, flush=True)
    if base is None:
        print(func, 'BASEFAIL'); return
    def one(k):
        kind, s, e, new, desc = sw[k]
        v = apply(src, [(s, e, new)])
        return k, sc(v)
    singles = {}
    with ThreadPoolExecutor(jobs) as ex:
        for k, v in ex.map(one, range(len(sw))):
            singles[k] = v
            print(f'single {k} {v} {sw[k][0]} {sw[k][4][:100]!r}', file=log, flush=True)
    if '--pairs' in a:
        ok = [k for k, v in singles.items() if v is not None]
        pairs = [(x, y) for i, x in enumerate(ok) for y in ok[i + 1:]]
        def pr(xy):
            v = apply(src, [(sw[j][1], sw[j][2], sw[j][3]) for j in xy])
            return xy, (sc(v) if v else None)
        with ThreadPoolExecutor(jobs) as ex:
            for xy, v in ex.map(pr, pairs):
                print(f'pair {xy} {v}', file=log, flush=True)
                if v is not None and v < base:
                    print(func, 'PAIR', xy, base, '->', v, sw[xy[0]][4][:50], '|', sw[xy[1]][4][:50], flush=True)
    good = sorted([k for k, v in singles.items() if v is not None and v < base], key=lambda k: singles[k])
    neutral = [k for k, v in singles.items() if v is not None and v == base]
    cur = []; best = base; bestsrc = src
    for k in good:
        ed = [(sw[j][1], sw[j][2], sw[j][3]) for j in cur + [k]]
        v = apply(src, ed)
        if v is None: continue
        s2 = sc(v)
        print(f'greedy +{k} -> {s2}', file=log, flush=True)
        if s2 is not None and s2 < best:
            best, bestsrc, cur = s2, v, cur + [k]
    if best < base:
        # second pass: neutral singles on top of the combination
        def two(k):
            v = apply(src, [(sw[j][1], sw[j][2], sw[j][3]) for j in cur + [k]])
            return k, (sc(v) if v else None), v
        with ThreadPoolExecutor(jobs) as ex:
            for k, s2, v in ex.map(two, [k for k in neutral + good if k not in cur]):
                print(f'pass2 +{k} -> {s2}', file=log, flush=True)
                if s2 is not None and s2 < best:
                    best, bestsrc = s2, v; cur = cur + [k]
        ext = '.c' if draft.endswith('.c') else '.cpp'
        open(f'{outd}/{short}.best{ext}', 'w').write(bestsrc)
    print(f'final {best} via {cur}', file=log, flush=True)
    print(func, base, '->', best, 'swaps', len(sw), 'improving', len(good), [(k, singles[k], sw[k][4][:60]) for k in cur], flush=True)

if __name__ == '__main__':
    main()
