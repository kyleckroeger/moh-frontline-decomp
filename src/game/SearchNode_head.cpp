// CSearchNode's operator delete and operator new, which take A* search nodes
// from and return them to the free list over the file's static pool, and the
// cost between two nodes, the real distance between their path nodes'
// positions. CSearchNode, CPathNode and CAIFilterRealPosition are named by
// the mangled symbols and the free list by its symbol; the node and
// path-node layouts are inferred, and the position class is built from three
// floats here (its constructors are inline; this one is inferred). Reset,
// first, links the whole pool into the free list, built in a register by an
// inferred inline helper (the pool is file-local in the original and declared
// extern here, as in SearchNode.cpp).
struct CPathNodePositionView {
    float x;
    float y;
    float z;
};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition(float px, float py, float pz) : x(px), y(py), z(pz) {}
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CPathNode {
public:
    CPathNodePositionView m_position;
};

class CSearchNode {
public:
    static void Reset();
    static void operator delete(void*, unsigned long);
    static void* operator new(unsigned long);
    float GetCostValue(const CSearchNode*);

    int unknown00;
    CSearchNode* m_next;
    CPathNode* m_node;
    unsigned char unknown0c[12];
    bool m_open;
    unsigned char unknown19[3];
};

extern CSearchNode* s_rsnAStarSearchNodeFreeList; // file-local in the original (defined in SearchNode.cpp)
extern CSearchNode s_rsnAStarSearchNodePool[4096];

inline CSearchNode* BuildFreeList() {
    CSearchNode* list = 0;
    for (int i = 0; i < 4096; i++) {
        s_rsnAStarSearchNodePool[i].m_next = list;
        s_rsnAStarSearchNodePool[i].m_node = 0;
        s_rsnAStarSearchNodePool[i].m_open = false;
        list = &s_rsnAStarSearchNodePool[i];
    }
    return list;
}

void CSearchNode::Reset() {
    s_rsnAStarSearchNodeFreeList = BuildFreeList();
}

void CSearchNode::operator delete(void* p, unsigned long) {
    ((CSearchNode*)p)->m_next = s_rsnAStarSearchNodeFreeList;
    s_rsnAStarSearchNodeFreeList = (CSearchNode*)p;
}

void* CSearchNode::operator new(unsigned long) {
    CSearchNode* node = s_rsnAStarSearchNodeFreeList;
    s_rsnAStarSearchNodeFreeList = node->m_next;
    return node;
}

float CSearchNode::GetCostValue(const CSearchNode* other) {
    CPathNodePositionView& a = m_node->m_position;
    CPathNodePositionView& b = other->m_node->m_position;
    CAIFilterRealPosition mine(a.x, a.y, a.z);
    CAIFilterRealPosition theirs(b.x, b.y, b.z);
    return theirs.GetDistanceXYZReal(mine);
}
