#!/usr/bin/env python3
# move_header.py CLASS UNIT...: apply try_header's moved source for each unit; when the compiled
# object only adds weak copies of CLASS's members (and its table), list them as weak duplicates
# with their original addresses; keep the unit if one_unit.py verifies it, else revert.
import json, re, shutil, subprocess, sys
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
    return (r.stdout + r.stderr).strip().splitlines()[-1] if (r.stdout + r.stderr).strip() else ''
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
    if 'VERIFIED' in res:
        print('MOVED', u)
        subprocess.run(['git', 'add', str(src), str(cfgp)], cwd=ROOT)
    else:
        src.write_text(old_src); cfgp.write_text(old_cfg)
        print('KEPT VIEW', u, res[:120])
