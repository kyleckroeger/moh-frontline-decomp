"""score_draft.py <function substring>...: differing lines of each draft in research/drafts against the original.

Run from the repository root after tools/setup.py and tools/audit.py. A score of 0
is not proof: small-data constants are invisible here, so only one_unit.py and
tools/reconstruct.py prove a draft.
"""
import csv, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import score

for row in csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t"):
    if any(k in row["function"] for k in sys.argv[1:]):
        text = open(row["draft"], errors="replace").read()
        print(row["function"], row["draft"], score.real(row["compile_settings_from"], text, row["function"], row["address"], row["size"]))
