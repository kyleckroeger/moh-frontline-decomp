cd /home/kyle/workspace/moh-frontline
while read f; do sh scratch/loop3/split2/iter.sh "$f"; done < scratch/loop3/split2/rows.txt
