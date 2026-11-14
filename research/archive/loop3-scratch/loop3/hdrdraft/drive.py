"""For each draft: greedily move its views onto the shared headers, score, check, brchk."""
import csv, json, os, re, subprocess, sys, tempfile, traceback
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path
ROOT = Path("/home/kyle/workspace/moh-frontline")
sys.path.insert(0, str(ROOT / "research/tools"))
os.chdir(ROOT)
import try_header as th, try_dwi as td, score, check_draft as cd
OUT = ROOT / "scratch/loop3/hdrdraft/out"
OUT.mkdir(parents=True, exist_ok=True)
ORDER = ["CVector3", "CMatrix", "CPlane", "ISceneNode", "IMovingSceneNode", "CCamera", "CPlayerObject",
         "dwi", "CScene", "CColor", "CRect", "CScreen", "CInputManager"]
MP = {"DrawFragCount__13UserInterfaceFb", "BIFunc_UpdateFragCount__FPPiPv", "BIFunc_MPAddPlayerHealth__FPPiPv",
      "InitMultiplayer__13CPlayerObjectFP12MPPlayerDatab"}
OBJDUMP = "build/tools/powerpc-eabi-objdump"

def settings(path):
    while not os.path.exists(path) and "_" in Path(path).stem:
        path = str(Path(path).with_name(Path(path).stem.rsplit("_", 1)[0] + ".json"))
    t = json.load(open(path))
    if "include" not in t.get("include_dirs", []):
        t["include_dirs"] = t.get("include_dirs", []) + ["include"]
    p = OUT / ("settings_" + Path(path).name)
    p.write_text(json.dumps(t))
    return str(p)

def has(text, c):
    if c == "dwi":
        return bool(re.search(r"namespace\s+dwi", text))
    return bool(re.search(r"^(?:class|struct)\s+" + c + r"\b[^;{]*\{", text, re.M))

def move(text, c, path):
    if c == "dwi":
        new, kinds = td.moved(text, {"cont", "hash"})
        return new
    return th.moved(text, c, path)

def sc(st, text, r):
    s = score.real(st, text, r["function"], r["address"], r["size"])
    return s[0] if s else None

def branches(st, text, r):
    t = json.load(open(st))
    a = int(r["address"], 16); n = int(r["size"])
    def load(lines):
        L = [l for l in lines if re.match(r'^\s*[0-9a-f]+:\t', l)]
        if not L: return []
        base = int(L[0].split(':')[0], 16); out = []
        for l in L:
            p = l.split('\t'); ins = p[2].split(' <')[0].strip() if len(p) > 2 else ''
            m = re.match(r'(b\w*)\s+(?:(cr\d),)?([0-9a-f]+)$', ins)
            if m and not ins.startswith('bl '): out.append('%s +%d' % (m.group(1), int(m.group(3), 16) - base))
            elif m: out.append('bl')
            else: out.append(None)
        return out
    with tempfile.TemporaryDirectory() as w:
        o, log = cd.fast.compile_object(t, text, w)
        if not o: return None
        d = subprocess.run([OBJDUMP, '-d', o], capture_output=True, text=True).stdout
        m = re.search(r'<' + re.escape(r['function']) + r'>:\n(.*?)(?:\n\n|\Z)', d, re.S)
        if not m: return None
        A = load(m.group(1).split('\n'))
    B = load(subprocess.run([OBJDUMP, '-d', '--start-address=%#x' % a, '--stop-address=%#x' % (a + n), 'orig/GMFE69/Moh2RelGC.elf'], capture_output=True, text=True).stdout.split('\n'))
    bd = [(i, x, y) for i, (x, y) in enumerate(zip(A, B)) if x != y and x and y and not (x.startswith('bl') and y.startswith('bl'))]
    return len(bd)

def work(r, pre=None, post=None):
    name = Path(r["draft"]).name
    try:
        text0 = open(r["draft"], errors="replace").read()
        base = text0
        if pre: text0 = pre(text0)
        st = settings(r["compile_settings_from"])
        old = sc(st, base, r)
        cur, cur_s, used, log = text0, old, [], []
        for c in ORDER:
            if not has(cur, c):
                continue
            try:
                cand = move(cur, c, r["draft"])
                if post and cand: cand = post(cand, c)
            except Exception as e:
                log.append(f"{c}:err"); continue
            if not cand or cand == cur:
                log.append(f"{c}:nochange"); continue
            s = sc(st, cand, r)
            if s is None and c != "dwi":
                th.EARLY_INCLUDE[c] = True
                try:
                    cand2 = move(cur, c, r["draft"])
                finally:
                    th.EARLY_INCLUDE[c] = c in ("CScreen", "CPlayerObject")
                s2 = sc(st, cand2, r) if cand2 else None
                if s2 is not None:
                    cand, s = cand2, s2
            if s is None:
                log.append(f"{c}:no_compile"); (OUT / f"{name}.{c}.nc.cpp").write_text(cand); continue
            log.append(f"{c}:{s}")
            if old is None or s <= cur_s:
                cur, cur_s = cand, s; used.append(c)
            else:
                (OUT / f"{name}.{c}.worse.cpp").write_text(cand)
        res = dict(draft=r["draft"], function=r["function"], old=old, new=cur_s, used=used, log=log)
        if used:
            (OUT / name).write_text(cur)
            cold = cd.compare(base, st, r["function"], r["address"], r["size"])["status"]
            cnew = cd.compare(cur, st, r["function"], r["address"], r["size"])["status"]
            res.update(check_old=cold, check_new=cnew, br_old=branches(st, base, r), br_new=branches(st, cur, r))
        return res
    except Exception:
        return dict(draft=r["draft"], function=r["function"], error=traceback.format_exc()[-500:])

if __name__ == "__main__":
    only = sys.argv[1:]
    rows = [r for r in csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t") if r["function"] not in MP]
    rows = [r for r in rows if any(has(open(r["draft"], errors="replace").read(), c) for c in ORDER)]
    if only:
        rows = [r for r in rows if any(k in r["draft"] for k in only)]
    print(len(rows), "drafts", flush=True)
    with ProcessPoolExecutor(3) as pool, open(OUT / os.environ.get("RES", "results.jsonl"), "a") as f:
        for res in pool.map(work, rows):
            print(json.dumps(res), flush=True); f.write(json.dumps(res) + "\n"); f.flush()
