from gv import *
N="    dwi::list_iter<BuddyInfo> node(0);\n"; S="    CBuddySoldier* soldier;\n"
reads={'op':"        soldier = (*node).soldier;\n",'dir':"        soldier = node.m_node->m_data.soldier;\n"}
for order in ['NS','SN']:
  for rn,r in reads.items():
    d=DR.replace(N+S,(S+N) if order=='SN' else N+S).replace("        soldier = (*node).soldier;\n",r)
    print(order,rn,score(put(d),order+rn))
