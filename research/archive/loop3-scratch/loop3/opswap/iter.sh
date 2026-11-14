# iter.sh FUNC: rounds of swap+split sweep starting from the index draft
cd /home/kyle/workspace/moh-frontline
f="$1"; short=$(python3 -c "import re,sys;print(re.sub(r'\W','_',sys.argv[1])[:40])" "$f")
d=$(python3 -c "
import csv,sys
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    if r['function']==sys.argv[1]: print(r['draft'])" "$f")
for n in 1 2 3 4 5; do
  o=scratch/loop3/opswap/it/$short/r$n; mkdir -p $o
  (ulimit -v 3000000; timeout 2400 python3 scratch/loop3/opswap/opsweep.py "$f" --draft "$d" --split --nomember --out $o) | cut -c1-200
  nb=$(ls $o/*.best.* 2>/dev/null | head -1)
  [ -z "$nb" ] && break
  d=$nb
done
echo "FINAL $f $d"
