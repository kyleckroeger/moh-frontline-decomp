"""sc.py <file> <function substring>: score a candidate file as the draft of that function"""
import csv,sys
sys.path.insert(0,'research/tools')
import score
f,k=sys.argv[1:3]
for row in csv.DictReader(open("research/drafts/index.tsv"),delimiter="\t"):
    if k in row["function"]:
        print(row["function"], f, score.real(row["compile_settings_from"], open(f,errors="replace").read(), row["function"], row["address"], row["size"]))
