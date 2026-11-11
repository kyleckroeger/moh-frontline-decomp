#!/bin/sh
# landlock.sh acquire <agent> | release <agent> | status -- landing lock for parallel agents.
# mkdir is atomic, so only one agent at a time builds, lands, edits research/ or pushes.
L=/home/kyle/workspace/moh-frontline/scratch/loop3/LANDLOCK
case "$1" in
acquire)
  n=0
  until mkdir "$L" 2>/dev/null; do
    n=$((n+1)); [ $n -ge 240 ] && { echo "TIMEOUT: held by $(cat $L/owner 2>/dev/null)"; exit 1; }
    sleep 5
  done
  echo "$2 $(date '+%F %T')" > "$L/owner"; echo "ACQUIRED by $2";;
release)
  [ "$(cut -d' ' -f1 "$L/owner" 2>/dev/null)" = "$2" ] || { echo "NOT OWNER: $(cat $L/owner 2>/dev/null)"; exit 1; }
  rm -rf "$L"; echo RELEASED;;
status) cat "$L/owner" 2>/dev/null || echo free;;
*) echo "usage: landlock.sh acquire|release <agent> | status"; exit 2;;
esac
