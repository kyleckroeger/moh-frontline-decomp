cd /home/kyle/workspace/moh-frontline
F='__ct<Q23dwi21list_iter<9BuddyInfo>>__Q23dwi16list<9BuddyInfo>FiQ23dwi21list_iter<9BuddyInfo>Q23dwi21list_iter<9BuddyInfo>Pc'
D=scratch/loop3/sinit2
python3 $D/sc.py $D/t_lc.json __sinit_buddy_cpp __sinit_ "$@" 2>&1 | grep "_s.cpp"
python3 $D/sc.py $D/t_lc.json "$F" "__ct<" $(echo "$@" | sed 's/_s\.cpp/_l.cpp/g') 2>&1 | grep "_l.cpp"
