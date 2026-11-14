"""eh.py: decode CodeWarrior PPC extab records.
  eh.py orig ADDR                 -> original's record for the function at ADDR
  eh.py obj TMPL.json SRC FUNC    -> record our compile gives for FUNC (mangled name)
  eh.py cmp ADDR TMPL.json SRC FUNC
As a module: orig_record(addr), obj_record(tmpl, text, func) -> (header, ranges, actions) or None.
"""
import sys, struct, json, tempfile, bisect
R = '/home/kyle/workspace/moh-frontline/'
sys.path.insert(0, R + 'tools'); sys.path.insert(0, R + 'research/tools')
from formats import Elf32
from pathlib import Path

DSZ = {1: 4, 2: 8, 3: 12, 4: 8, 5: 12, 6: 12, 7: 12, 8: 16, 9: 20, 10: 8, 11: 12, 12: 12, 13: 4, 14: 2, 16: 16}
DOFF = {2: 4, 3: 8, 4: 4, 5: 8, 6: 8, 7: 8, 8: 12, 9: 16, 10: 4, 11: 8}
NAMES = {1: 'BRANCH', 2: 'DLOCAL', 3: 'DLOCALCOND', 4: 'DLOCALPTR', 5: 'DLOCALARR', 6: 'DBASE', 7: 'DMEMBER',
         8: 'DMEMBERCOND', 9: 'DMEMBERARR', 10: 'DELPTR', 11: 'DELPTRCOND', 12: 'CATCH', 13: 'ACTCATCH', 14: 'TERM'}


def decode(data, base, dtor_at):
    """data: bytes of the extab section, base: record offset; dtor_at(off) -> symbol name for a pointer at off."""
    field = struct.unpack_from('>H', data, base)[0]
    hdr = dict(gprs=field >> 11, fprs=(field >> 6) & 31, cr=(field >> 5) & 1, fp=(field >> 4) & 1, large=(field >> 3) & 1)
    ranges = []
    if hdr['large']:
        p = base + 4
        while True:
            if p + 4 > len(data) or struct.unpack_from('>I', data, p)[0] == 0: break
            s, n, a = struct.unpack_from('>IHH', data, p)
            ranges.append((s, s + n * 4, a)); p += 8
    else:
        p = base + 2
        while True:
            if p + 2 > len(data) or struct.unpack_from('>H', data, p)[0] == 0: break
            s, e, a = struct.unpack_from('>HHH', data, p)
            ranges.append((s, e, a)); p += 6
    chains = {}
    for _, _, a in ranges:
        if a in chains: continue
        out = []; q = base + a; seen = set()
        while q not in seen:
            seen.add(q)
            b = data[q]; t = b & 0x7F
            if t == 0: break
            d = dict(t=NAMES.get(t, str(t)))
            if t == 1:
                tgt = struct.unpack_from('>H', data, q + 2)[0]; d['to'] = tgt
                out.append(d); q = base + tgt; continue
            if t in (2, 4, 10):
                d['loc'] = struct.unpack_from('>h', data, q + 2)[0]
                if t != 2: d['reg'] = data[q + 1] >> 7
            elif t in (6, 7):
                d['reg'] = data[q + 1] >> 7; d['obj'] = struct.unpack_from('>h', data, q + 2)[0]
                d['off'] = struct.unpack_from('>i', data, q + 4)[0]
            elif t == 3:
                d['cond'], d['loc'] = struct.unpack_from('>hh', data, q + 2)
            elif t == 5:
                d['loc'], d['n'], d['esz'] = struct.unpack_from('>hHH', data, q + 2)
            elif t == 8:
                d['cond'], d['obj'] = struct.unpack_from('>hh', data, q + 2); d['off'] = struct.unpack_from('>i', data, q + 6)[0]
            elif t == 9:
                d['obj'] = struct.unpack_from('>h', data, q + 2)[0]
                d['off'], d['n'], d['esz'] = struct.unpack_from('>iii', data, q + 4)
            elif t == 11:
                d['cond'], d['obj'] = struct.unpack_from('>hh', data, q + 2)
            if t in DOFF: d['dtor'] = dtor_at(q + DOFF[t])
            out.append(d)
            if b & 0x80 or t not in DSZ: break
            q += DSZ[t]
        chains[a] = out
    return hdr, ranges, chains


_orig = None
def _o():
    global _orig
    if _orig is None:
        e = Elf32(Path(R + 'orig/GMFE69/Moh2RelGC.elf').read_bytes())
        secs = {s['name']: s for s in e.sections}
        syms = sorted((s['address'], s['name']) for s in e.symbols() if s['name'] and s['type'] == 2)
        _orig = (e, secs, syms)
    return _orig


def orig_record(addr):
    e, secs, syms = _o()
    xi = secs['extabindex']; xd = e.contents(xi)
    x = secs['extab']; data = e.contents(x)
    for i in range(0, len(xd) - 11, 12):
        f, sz, ex = struct.unpack_from('>III', xd, i)
        if f == addr:
            if sz >> 31: return 'direct'
            def dtor_at(off):
                v = struct.unpack_from('>I', data, off)[0]
                j = bisect.bisect_right(syms, (v, '\xff')) - 1
                return syms[j][1] if j >= 0 and syms[j][0] == v else hex(v)
            return decode(data, ex - x['address'], dtor_at)
    return None


def obj_record_path(obj, func):
    e = Elf32(Path(obj).read_bytes())
    secs = e.sections; syms = list(e.symbols())
    idx = {s['name']: i for i, s in enumerate(secs)}
    def rels(target):
        for s in secs:
            if s['type'] == 4 and s['info'] == idx.get(target):
                d = e.contents(s); out = {}
                for o in range(0, len(d), 12):
                    w, info, add = struct.unpack_from('>IIi', d, o); out[w] = (syms[info >> 8], add)
                return out
        return {}
    if 'extabindex' not in idx: return None
    xr = rels('extabindex'); er = rels('extab'); data = e.contents(secs[idx['extab']])
    xd = e.contents(secs[idx['extabindex']])
    for i in range(0, len(xd) - 11, 12):
        s, _ = xr.get(i, ({'name': ''}, 0))
        if s['name'] == func or s['name'].split('__')[0] == func.split('__')[0] and func.startswith('quick_sort') or func.startswith('__sinit_') and s['name'].startswith('__sinit_'):
            if struct.unpack_from('>I', xd, i + 4)[0] >> 31: return 'direct'
            es, add = xr[i + 8]
            base = add + (es['address'] if es['type'] != 3 else 0)
            def dtor_at(off):
                r = er.get(off)
                return r[0]['name'] + (('+%d' % r[1]) if r[1] else '') if r else '0'
            return decode(data, base, dtor_at)
    return None


def obj_record(tmpl, text, func):
    import fast
    with tempfile.TemporaryDirectory() as w:
        obj, log = fast.compile_object(json.load(open(tmpl)), text, w)
        if not obj: return 'COMPILE FAIL ' + log[-300:]
        return obj_record_path(obj, func)


def fmt(rec):
    if not isinstance(rec, tuple): return str(rec)
    h, ranges, chains = rec
    out = ['hdr %s ranges %d' % (h, len(ranges))]
    for s, e, a in ranges: out.append('  [%x-%x] -> %d' % (s, e, a))
    for a, ch in chains.items():
        out.append('  act %d: ' % a + ' ; '.join(' '.join('%s=%s' % kv for kv in d.items()) for d in ch))
    return '\n'.join(out)


def summary(rec):
    """unique (type,dtor) set with counts, offset-free."""
    if not isinstance(rec, tuple): return str(rec)
    from collections import Counter
    c = Counter()
    for ch in rec[2].values():
        for d in ch:
            if 'dtor' in d: c[(d['t'], d['dtor'])] += 1
    return ', '.join('%s:%s' % k for k in sorted(c))


if __name__ == '__main__':
    m = sys.argv[1]
    if m == 'orig': print(fmt(orig_record(int(sys.argv[2], 16))))
    elif m == 'obj': print(fmt(obj_record(sys.argv[2], open(sys.argv[3]).read(), sys.argv[4])))
    elif m == 'cmp':
        print('ORIG'); print(fmt(orig_record(int(sys.argv[2], 16))))
        print('OURS'); print(fmt(obj_record(sys.argv[3], open(sys.argv[4]).read(), sys.argv[5])))
