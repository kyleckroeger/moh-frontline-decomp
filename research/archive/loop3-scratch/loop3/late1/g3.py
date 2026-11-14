from gv import *
body_tail=DR[DR.index("        object = soldier->GetScriptObject();"):]
decls="    BSObject* object;\n    float x;\n    float y;\n    float z;\n    int index;\n    int i;\n    BSStateView* state;\n"
V={}
V['raw_end']="inline void DumpReadyBuddies() {\n    CBuddySoldier* soldier;\n    dwi::list_node<BuddyInfo>* node;\n"+decls+"    dwi::list_node<BuddyInfo>* const end = &g_ready_buddies.m_head;\n    DebugMsg(\"BUDDY_DEBUG: Dumping ready list...\\n\");\n    i = 0;\n    for (node = g_ready_buddies.m_head.m_next; node != end; node = node->m_next) {\n        soldier = node->m_data.soldier;\n"+body_tail.replace("++node","node = node->m_next")
V['raw_end2']="inline void DumpReadyBuddies() {\n    dwi::list_node<BuddyInfo>* node;\n    CBuddySoldier* soldier;\n"+decls+"    DebugMsg(\"BUDDY_DEBUG: Dumping ready list...\\n\");\n    i = 0;\n    for (node = g_ready_buddies.m_head.m_next; node != &g_ready_buddies.m_head; node = node->m_next) {\n        soldier = node->m_data.soldier;\n"+body_tail
V['param']="inline void DumpBuddyList(dwi::list<BuddyInfo>& buddies) {\n    dwi::list<BuddyInfo>* const list = &buddies;\n    dwi::list_iter<BuddyInfo> node(0);\n    CBuddySoldier* soldier;\n"+decls+"    DebugMsg(\"BUDDY_DEBUG: Dumping ready list...\\n\");\n    i = 0;\n    for (node = list->begin(); node != list->end(); ++node) {\n        soldier = (*node).soldier;\n"+body_tail+"inline void DumpReadyBuddies() { DumpBuddyList(g_ready_buddies); }\n"
V['param2']=V['param'].replace("    dwi::list_iter<BuddyInfo> node(0);\n    CBuddySoldier* soldier;\n","    CBuddySoldier* soldier;\n    dwi::list_iter<BuddyInfo> node(0);\n")
for n,d in V.items():
    print(n,score(put(d),n))
