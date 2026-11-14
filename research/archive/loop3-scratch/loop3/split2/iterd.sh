# iterd.sh FUNC DRAFT TAG: rounds of split2 sweep from a given draft
cd /home/kyle/workspace/moh-frontline
f="$1"; d="$2"; short=$(python3 -c "import re,sys;print(re.sub(r'\W','_',sys.argv[1])[:40])" "$f")
for n in 1 2 3 4 5; do
  o=scratch/loop3/split2/it$3/$short/r$n; mkdir -p $o
  (ulimit -v 3000000; timeout 2400 python3 scratch/loop3/split2/split2.py "$f" --draft "$d" --nomember --out $o) | cut -c1-240
  nb=$(ls $o/*.best.* 2>/dev/null | head -1)
  [ -z "$nb" ] && break
  d=$nb
done
echo "FINAL $f $d"
