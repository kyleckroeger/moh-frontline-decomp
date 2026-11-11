// PathNodeList.cpp: its zero-initialised globals (0x8 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct CPathNodeList;
struct CPathNode;
class CPathNodeList {
public:
    static CPathNodeList* s_pnlCurrent;
    static CPathNode** s_pPathNodeMapEntries;
};

CPathNodeList* CPathNodeList::s_pnlCurrent;
CPathNode** CPathNodeList::s_pPathNodeMapEntries;
