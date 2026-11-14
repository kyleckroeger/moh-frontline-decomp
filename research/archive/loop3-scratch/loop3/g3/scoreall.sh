cd /home/kyle/workspace/moh-frontline
for f in $(cut -f3 scratch/loop3/day2/group3.tsv | tail -n +2); do
  (ulimit -v 3000000; timeout 300 python3 research/tools/score_draft.py "$f" 2>&1 | tail -1 | awk "{print \$(NF-1), \$NF, \$1}" | cut -c1-90)
done
