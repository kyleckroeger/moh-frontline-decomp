cd /home/kyle/workspace/moh-frontline
while read f; do sh scratch/loop3/opswap/iter.sh "$f"; done < scratch/loop3/opswap/rows.txt
