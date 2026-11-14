cd /home/kyle/workspace/moh-frontline
f="$1"; short=$(python3 -c "import re,sys;print(re.sub(r'\W','_',sys.argv[1])[:40])" "$f")
d=$(python3 -c "
import csv,sys
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    if r['function']==sys.argv[1]: print(r['draft'])" "$f")
ext=${d##*.}
(ulimit -v 3000000; REAL=1 timeout 1800 python3 scratch/loop3/split2/autods.py "$f" "$d" scratch/loop3/split2/dsr/$short.$ext) > scratch/loop3/split2/dsr/$short.log 2>&1
tail -1 scratch/loop3/split2/dsr/$short.log
