#!/usr/bin/env python3
# move_header.py CLASS UNIT...: apply try_header's moved source for each unit; when the compiled
# object only adds weak copies of CLASS's members (and its table), list them as weak duplicates
# with their original addresses, and park the data they bring (the weak table, the copies'
# exception records) as stripped objects; keep the unit if one_unit.py verifies it, else revert.
import json, re, shutil, struct, subprocess, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from formats import Elf32
cls = sys.argv[1]; mangled = f"{len(cls)}{cls}"
_syms = [s for s in Elf32((ROOT / 'orig/GMFE69/Moh2RelGC.elf').read_bytes()).symbols() if s['name']]
orig = {s['name']: s['address'] for s in _syms}
orig_weak = {s['name'] for s in _syms if s['binding'] == 2}
def one(u):
    r = subprocess.run(['python3', 'research/tools/one_unit.py', u], cwd=ROOT, capture_output=True, text=True, timeout=900)
    lines = (r.stdout + r.stderr).strip().splitlines()
    # one_unit prints the failing check first, then the linked sections
    return next((l for l in lines if ': FAIL ' in l or 'Error' in l or 'VERIFIED' in l), lines[-1] if lines else '')

def relocations(obj, target):
    """{offset: (symbol, addend)} of the relocations on section TARGET."""
    secs = obj.sections; syms = list(obj.symbols())
    idx = next((s['index'] for s in secs if s['name'] == target), None)
    for s in secs:
        if s['type'] == 4 and s['info'] == idx:
            d = obj.contents(s)
            return {w: (syms[info >> 8], add) for w, info, add in (struct.unpack_from('>IIi', d, o) for o in range(0, len(d), 12))}
    return {}

def weak_layout(cfg, obj, weak):
    """Data a weak duplicate brings: its own data objects (a weak table) and the exception records
    of weak duplicate functions. List them as stripped objects (data_strip parks them at the
    section's end, outside the compared slice) and take the compiled sizes as linked sizes.
    Returns True when the manifest changed."""
    syms = list(obj.symbols()); secs = {s['name']: s for s in obj.sections}
    objects = lambda name: sorted((s['address'], s['name']) for s in syms
                                  if name in secs and s['section'] == secs[name]['index'] and s['type'] == 1 and s['size'])
    drop = {}
    for name in secs:
        drop[name] = {n for _, n in objects(name) if n in weak}
    if 'extabindex' in secs:  # entry: function, size, record
        xr = relocations(obj, 'extabindex')
        at = {a: n for a, n in objects('extabindex')}
        rec = {a: n for a, n in objects('extab')}
        for off, (fn, _) in xr.items():
            if off % 12 == 0 and fn['name'] in weak and off in at:
                drop['extabindex'].add(at[off])
                es, add = xr.get(off + 8, (None, 0))
                if es is not None and add + (es['address'] if es['type'] != 3 else 0) in rec:
                    drop.setdefault('extab', set()).add(rec[add + (es['address'] if es['type'] != 3 else 0)])
    changed = False
    for section in cfg['sections']:
        name = section['name']
        if name == '.text' or name not in secs or not drop.get(name):
            continue
        compiled = secs[name]['size']
        if compiled == section.get('linked_size', section['size']):
            continue
        section['linked_size'] = compiled
        section['stripped_objects'] = sorted(set(section.get('stripped_objects', [])) | drop[name])
        changed = True
    return changed
for u in sys.argv[2:]:
    cfgp = ROOT / f'config/GMFE69/{u}.json'; cfg = json.loads(cfgp.read_text())
    src = ROOT / cfg['source']; moved = ROOT / f'scratch/try/{u}/src/{src.name}'
    old_src, old_cfg = src.read_text(), cfgp.read_text()
    src.write_text(moved.read_text())
    if 'include' not in cfg['include_dirs']:
        cfg['include_dirs'].append('include')
    cfgp.write_text(json.dumps(cfg, indent=2) + '\n')
    res = one(u)
    if 'VERIFIED' not in res and 'function set' in res or 'dependencies' in res:
        obj = Elf32((ROOT / f'scratch/one/{u}/compiled.o').read_bytes())
        known = {f['name'] for f in cfg['functions']}
        extra = sorted({s['name'] for s in obj.symbols() if s['binding'] == 2 and 0 < s['section'] < 0xFF00
                        and s['type'] in (1, 2) and s['name'] not in known})
        if extra and all(n in orig_weak for n in extra):  # every extra copy is one of the original's weak copies
            cfg['weak_duplicates'] = sorted(set(cfg.get('weak_duplicates', [])) | set(extra))
            cfg['externals'] = {**cfg.get('externals', {}), **{n: hex(orig[n]) for n in extra}}
            cfgp.write_text(json.dumps(cfg, indent=2) + '\n')
            res = one(u)
    if 'lacks one retained original copy' in res and not cfg.get('strip_unused'):
        # weak duplicates need a stripping unit (project_build); try it
        cfg['strip_unused'] = True
        cfgp.write_text(json.dumps(cfg, indent=2) + '\n')
        res = one(u)
    if 'VERIFIED' not in res and cfg.get('weak_duplicates') and any(
            k in res for k in ('data layout differs', 'sections differ', 'source bytes differ', 'Trimmed data')):
        # a weak table or exception records of the weak copies now sit in the data sections
        cfg['strip_unused'] = True
        if weak_layout(cfg, Elf32((ROOT / f'scratch/one/{u}/compiled.o').read_bytes()), set(cfg['weak_duplicates'])):
            cfgp.write_text(json.dumps(cfg, indent=2) + '\n')
            res = one(u)
    if 'VERIFIED' in res:
        print('MOVED', u)
        subprocess.run(['git', 'add', str(src), str(cfgp)], cwd=ROOT)
    else:
        src.write_text(old_src); cfgp.write_text(old_cfg)
        print('KEPT VIEW', u, res[:120])
