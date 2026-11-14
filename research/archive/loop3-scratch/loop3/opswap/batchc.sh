cd /home/kyle/workspace/moh-frontline
while read f; do
  (ulimit -v 3000000; timeout 1500 python3 scratch/loop3/opswap/opsweep.py "$f" --cmp --cmponly --out scratch/loop3/opswap/outc) || echo "$f TIMEOUT/ERR"
done < scratch/loop3/opswap/rows.txt
