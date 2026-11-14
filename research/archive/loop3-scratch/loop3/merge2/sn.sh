#!/bin/sh
R=/home/kyle/workspace/moh-frontline; cd $R; M=scratch/loop3/merge2
sh research/tools/landlock.sh acquire merge2 || exit 1
if [ -n "$(git status --porcelain)" ]; then echo DIRTY; sh research/tools/landlock.sh release merge2; exit 1; fi
git pull --ff-only -q
cp $M/src/SearchNode.cpp src/game/SearchNode.cpp
git rm -q src/game/SearchNode_head.cpp config/GMFE69/SearchNode_tail.json
python3 -c "
import json
p='config/GMFE69/project.json'; m=json.load(open(p)); m['units'].remove('SearchNode_tail.json'); open(p,'w').write(json.dumps(m,indent=2)+'\n')"
(ulimit -v 3000000; timeout 900 python3 tools/port_unit.py SearchNode src/game/SearchNode.cpp --like $M/tpl_searchnode.json --original-file SearchNode.cpp --file-index 2327 --category reconstructed_game) 2>&1 | tail -2
python3 -c "
import json
p='config/GMFE69/SearchNode.json'; c=json.load(open(p)); c.pop('adapted_from',None); open(p,'w').write(json.dumps(c,indent=2)+'\n')"
out=$( (ulimit -v 3000000; timeout 900 python3 research/tools/one_unit.py SearchNode) 2>&1 ); echo "$out" | tail -2
subj="Merge SearchNode.cpp: 2 fragments into one unit"
if echo "$out" | grep -q "^SearchNode: VERIFIED"; then
  echo "- 2026-11-13 [loop merge2] merged SearchNode.cpp (SearchNode_head + SearchNode, 1,100 B) into unit SearchNode (source src/game/SearchNode.cpp): the pool and free list static again; CSearchNode on CLinkedListElement with the head's member names; operator delete is inlined into the deleting destructor from its own definition." >> research/log/autopilot.md
  (ulimit -v 6000000; timeout 3000 sh research/tools/land.sh "$subj" "CSearchNode's functions, the pool, the free list and their static initialisation in one unit; s_rsnAStarSearchNodePool and s_rsnAStarSearchNodeFreeList are file-static again. Complete image identical.") 2>&1 | tail -2
fi
if [ "$(git log -1 --format=%s)" = "$subj" ]; then git push 2>&1 | tail -1; else echo "FAILED SearchNode; reverting"; git reset -q; git checkout -q -- .; git clean -fdq config src; fi
sh research/tools/landlock.sh release merge2
