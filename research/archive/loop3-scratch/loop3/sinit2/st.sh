# st.sh file.cpp...: score sinit + list, and show stack stores for each
cd /home/kyle/workspace/moh-frontline
F='__ct<Q23dwi21list_iter<9BuddyInfo>>__Q23dwi16list<9BuddyInfo>FiQ23dwi21list_iter<9BuddyInfo>Q23dwi21list_iter<9BuddyInfo>Pc'
D=scratch/loop3/sinit2
for f in "$@"; do
 case $f in /*) ;; *) f=$D/$f;; esac
 b=$(basename $f .cpp)
 cat $D/pre.h > $D/tmp_l.cpp; cat $f >> $D/tmp_l.cpp; echo '#include "base_l.h"' >> $D/tmp_l.cpp
 cat $f > $D/tmp_s.cpp; echo '#include "base_s.h"' >> $D/tmp_s.cpp
 s=$(python3 $D/sc.py $D/t_lc.json __sinit_buddy_cpp __sinit_ -v $D/tmp_s.cpp)
 l=$(python3 $D/sc.py $D/t_lc.json "$F" "__ct<" -v $D/tmp_l.cpp)
 echo "$b sinit=$(echo "$s"|head -1|cut -d' ' -f1-3) list=$(echo "$l"|head -1|cut -d' ' -f1-3)  S:$(echo "$s"|sed 1d|cut -c42-|grep '^st[wb] r[0-9]*,[0-9]*(r1)$'|grep -v 'r0,52\|r31,44'|awk '{print $2}'|tr '\n' ' ') L:$(echo "$l"|sed 1d|cut -c42-|grep '^st[wb] r[0-9]*,[0-9]*(r1)$'|grep -v 'r0,52\|r31,44\|r30,40\|r29,36'|awk '{print $2}'|tr '\n' ' ')"
done
