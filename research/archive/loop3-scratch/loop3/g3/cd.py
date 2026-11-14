"""cd.py KEY src: check_draft compare for a scratch source"""
import sys,csv; sys.path.insert(0,'research/tools'); import check_draft as c
k,src=sys.argv[1:3]
for row in csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t"):
    if k in row["function"]:
        row=dict(row); row['draft']=src
        c.report(row, c.compare(open(src).read(), row["compile_settings_from"], row["function"], row["address"], row["size"]))
