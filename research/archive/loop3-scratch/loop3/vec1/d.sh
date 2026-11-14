#!/bin/sh
# d.sh tmpl src func addr size : print only differing rows with context
python3 /home/kyle/workspace/moh-frontline/scratch/loop3/scripting/tools/dump.py "$@" 0 5000 | grep -n '' | awk '{print}' > /tmp/claude-1000/dd.$$; grep -n ' \* ' /tmp/claude-1000/dd.$$ | cut -d: -f1 | while read n; do sed -n "$((n-3)),$((n+3))p" /tmp/claude-1000/dd.$$; echo --; done | awk '!seen[$0]++'; rm -f /tmp/claude-1000/dd.$$
