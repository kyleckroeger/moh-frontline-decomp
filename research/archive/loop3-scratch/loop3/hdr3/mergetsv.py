"""mergetsv.py OLD NEW [MOVED_NOTE]: rows of NEW replace OLD's by unit; kept rows get detail 'moved (hdr3, after the CVector3 base form)'."""
import sys
old, new = sys.argv[1], sys.argv[2]
note = sys.argv[3] if len(sys.argv) > 3 else ""
rows = {r.split("\t")[0]: r for r in open(old).read().splitlines()[1:]}
for r in open(new).read().splitlines()[1:]:
    f = r.split("\t")
    while len(f) < 4: f.append("")
    if f[2] == "kept" and note: f[3] = note
    rows[f[0]] = "\t".join(f)
open(old, "w").write("unit\tsource\tstatus\tdetail\n" + "\n".join(sorted(rows.values())) + "\n")
