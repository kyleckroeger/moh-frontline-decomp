"""Fast per-function comparison for game-code experiments (research only).

fast.py <template.json> <source.cpp> <func>[:start:end] ...
Prints "<diff lines> <func>". Start/end default to the target symbol.
As a module, score(template, source_text, funcs, workdir) returns {func: count}.
"""
import json, os, re, subprocess, sys, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
OBJDUMP = str(ROOT / "build/tools/powerpc-eabi-objdump")
ELF = str(ROOT / "orig/GMFE69/Moh2RelGC.elf")
_cache = {}


def parse(text):
    """objdump -dr -> {func: [normalized instruction]}"""
    funcs, cur, pending = {}, None, None
    for line in text.splitlines():
        m = re.match(r'^[0-9a-f]+ <(.+)>:$', line)
        if m:
            cur = m.group(1); funcs[cur] = []; continue
        m = re.match(r'^\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)$', line)
        if m and cur:
            ins = re.sub(r'\s+', ' ', m.group(2).strip())
            ins = re.sub(r' [0-9a-f]+ <.*>$', ' <T>', ins)
            funcs[cur].append([ins, False]); continue
        if re.match(r'^\s*[0-9a-f]+: R_PPC', line) and cur and funcs[cur]:
            funcs[cur][-1][1] = True
    out = {}
    for f, ins in funcs.items():
        norm = []
        for i, reloc in ins:
            if reloc:  # the relocated field is filled at link time
                i = re.sub(r'-?\d+\((r?\d+)\)', r'X(\1)', i)
                i = re.sub(r',-?\d+$', ',X', i)
                i = re.sub(r'X\((r2|r13|r0|0)\)', 'X(sda)', i)
                i = re.sub(r'^li (r\d+),X$', r'addi \1,sda,X', i)
                i = re.sub(r'^addi (r\d+),(r13|r2),X$', r'addi \1,sda,X', i)
            norm.append(i)
        out[f] = norm
    return out


def target(func, start=None, end=None):
    key = (func, start, end)
    if key not in _cache:
        if start is None:
            from audit import load_target
            _, t = load_target()
            s = next(y for y in t.symbols() if y['name'] == func and y['type'] == 2 and y['section'])
            start, end = s['address'], s['address'] + s['size']
        text = subprocess.run([OBJDUMP, '-dr', f'--start-address={start:#x}', f'--stop-address={end:#x}', ELF],
                              capture_output=True, text=True).stdout
        text = re.sub(r'^([0-9a-f]+) <.+>:$', lambda m: f'{m.group(1)} <{func}>:', text, flags=re.M)
        _cache[key] = parse(text).get(func, [])
    return _cache[key]


def compile_object(template, source_text, workdir):
    from setup_compiler import setup_compiler
    compiler, wrapper = setup_compiler(template['compiler_version'], 'GC')
    src = Path(workdir) / ('probe.' + os.environ.get('SUFFIX', 'cpp'))
    src.write_text(source_text)
    cmd = [str(wrapper), str(compiler / 'mwcceppc.exe'), '-c', *template['compiler_flags'], '-I-',
           *[f'-I{ROOT / p}' for p in template.get('include_dirs', [])], str(src), '-o', 'probe.o']
    r = subprocess.run(cmd, cwd=workdir, capture_output=True, text=True)
    obj = Path(workdir) / 'probe.o'
    if r.returncode or not obj.exists():
        return None, r.stdout + r.stderr
    return str(obj), ''


def score(template, source_text, funcs, workdir):
    for _ in range(3):  # wibo occasionally fails under parallel load
        obj, log = compile_object(template, source_text, workdir)
        if obj is not None:
            break
    if obj is None:
        return {f[0]: 9999 for f in funcs}
    mine = parse(subprocess.run([OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout)
    out = {}
    for f in funcs:
        name = f[0]
        a, b = target(*f), mine.get(name)
        if b is None:
            out[name] = 9999; continue
        import difflib
        out[name] = sum(1 for l in difflib.unified_diff(a, b, n=0, lineterm='')
                        if l[:1] in '+-' and not l.startswith(('+++', '---')))
    return out


if __name__ == '__main__':
    template = json.load(open(sys.argv[1]))
    source = open(sys.argv[2]).read()
    funcs = []
    for spec in sys.argv[3:]:
        parts = spec.split(':')
        funcs.append((parts[0], int(parts[1], 16), int(parts[2], 16)) if len(parts) == 3 else (parts[0],))
    with tempfile.TemporaryDirectory() as d:
        for name, n in score(template, source, funcs, d).items():
            print(n, name)
