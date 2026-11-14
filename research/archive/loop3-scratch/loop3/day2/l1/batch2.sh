#!/bin/sh
cd /home/kyle/workspace/moh-frontline
tail -n +2 scratch/loop3/day2/late1.tsv | sort -n | while IFS='	' read s r f d; do
  case "$f" in *MP*|*Multiplayer*|TRKSupp*) continue;; esac
  row=$(awk -F'\t' -v f="$f" '$4==f' research/drafts/index.tsv | head -1)
  [ -z "$row" ] && continue
  j=$(echo "$row" | cut -f5); dr=$(echo "$row" | cut -f6)
  suf=cpp; case "$dr" in *.c) suf=c;; esac
  echo "=== $f ($s)"
  (ulimit -v 2500000; timeout 600 python3 scratch/loop3/day2/l1/flagx.py "$j" "$f" "$dr" $suf 2>&1 | tail -2)
done
