import sys, csv
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import score
fn, path = sys.argv[1], sys.argv[2]
for row in csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t"):
    if row["function"] == fn:
        print(score.real(row["compile_settings_from"], open(path, errors="replace").read(), row["function"], row["address"], row["size"]))
