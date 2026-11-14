cd /home/kyle/workspace/moh-frontline
while read f; do
  (ulimit -v 3000000; timeout 1500 python3 scratch/loop3/opswap/opsweep.py "$f" --pairs --out scratch/loop3/opswap/outp) || echo "$f TIMEOUT/ERR"
done < scratch/loop3/opswap/rows_small.txt
