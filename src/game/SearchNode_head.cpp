// CSearchNode's operator delete and operator new, which take A* search nodes
// from and return them to the free list over the file's static pool, and the
// cost between two nodes, the real distance between their path nodes'
// positions. CSearchNode, CPathNode and CAIFilterRealPosition are named by
// the mangled symbols and the free list by its symbol; the node and
// path-node layouts are inferred, and the position class is built from three
// floats here (its constructors are inline; this one is inferred). Reset,
// first, links the whole pool into the free list, built in a register by an
// inferred inline helper (the pool is file-local in the original and declared
// extern here, as in SearchNode.cpp). Setup, last, links a search node to
// its parent and path node (claiming the path node when it has no search
// node yet, and returning the previous one), takes the cost from the parent
// (GetCostValue, inlined, plus the parent's cost, or 0 at the root) and the
// estimate to the goal (the inferred inline GetEstimate) and their sum; the
// path node's search-node member (+12) and the cost members are inferred.
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

class CSearchNode;

class CPathNode {
public:
    CPathNodePositionView m_position;
    CSearchNode* m_searchNode;
};

class CSearchNode {
public:
    static void Reset();
    static void operator delete(void*, unsigned long);
    static void* operator new(unsigned long);
    float GetCostValue(const CSearchNode*);
    CSearchNode* Setup(CSearchNode*, CPathNode*, const CPathNode*);

    int unknown00;
    CSearchNode* m_next;
    CPathNode* m_node;
    float m_cost;
    float m_estimate;
    float m_total;
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
    CAIFilterRealPosition mine(a.x, a.y, a.z);
    CPathNodePositionView& b = other->m_node->m_position;
    CAIFilterRealPosition theirs(b.x, b.y, b.z);
    return theirs.GetDistanceXYZReal(mine);
}

// inferred inline: the distance from this node's path node to the goal
inline float GetEstimate(const CSearchNode* node, const CPathNode* goal) {
    const CPathNodePositionView& c = node->m_node->m_position;
    const CPathNodePositionView& d = goal->m_position;
    CAIFilterRealPosition mine(c.x, c.y, c.z);
    CAIFilterRealPosition target(d.x, d.y, d.z);
    return mine.GetDistanceXYZReal(target);
}

CSearchNode* CSearchNode::Setup(CSearchNode* parent, CPathNode* node, const CPathNode* goal) {
    CSearchNode* previous = node->m_searchNode;
    m_next = parent;
    m_node = node;
    if (!previous)
        node->m_searchNode = this;
    if (parent) {
        m_cost = GetCostValue(parent);
        m_cost += parent->m_cost;
    } else {
        m_cost = 0.0f;
    }
    m_estimate = GetEstimate(this, goal);
    m_total = m_cost + m_estimate;
    return previous;
}
