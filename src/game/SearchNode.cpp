// SearchNode.cpp (0x80055fc8): CSearchNode, A* search nodes from a fixed pool
// of 4096 with a free list (class operator new and delete), linked to path
// nodes, with the cost so far, the estimate to the goal and their sum. Reset
// links the whole pool into the free list (built in a register by an inferred
// inline helper); operator delete and new return nodes to and take them from
// the free list; GetCostValue is the real distance between two nodes' path
// node positions; Setup links a search node to its parent and path node
// (claiming the path node when it has no search node yet, and returning the
// previous one), takes the cost from the parent (GetCostValue, inlined, plus
// the parent's cost, or 0 at the root) and the estimate to the goal (the
// inferred inline GetEstimate) and their sum. The destructor unlinks the node
// from its path node, clears its open flag, link and path node, and returns
// the node to the free list when deleting (operator delete inlined); the
// constructor clears the link and path node. The file's statics are the pool
// (built by the static initialisation, with its array destructor registered)
// and the free list, whose initialiser links the whole pool. CSearchNode,
// CPathNode, CLinkedListElement and CAIFilterRealPosition are named by the
// mangled symbols; the node and path-node layouts are inferred, and the
// position class is built from three floats here (its constructors are
// inline; this one is inferred).
class CLinkedListElement {
public:
    CLinkedListElement* next;

    ~CLinkedListElement();
    CLinkedListElement();
};

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

class CSearchNode : public CLinkedListElement {
public:
    CSearchNode();
    ~CSearchNode();
    static void Reset();
    static void operator delete(void*, unsigned long);
    static void* operator new(unsigned long);
    float GetCostValue(const CSearchNode*);
    CSearchNode* Setup(CSearchNode*, CPathNode*, const CPathNode*);

    CSearchNode* m_next;
    CPathNode* m_node;
    float m_cost;
    float m_estimate;
    float m_total;
    bool m_open;
};

static CSearchNode s_rsnAStarSearchNodePool[4096];

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

static CSearchNode* s_rsnAStarSearchNodeFreeList = BuildFreeList();

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

CSearchNode::~CSearchNode() {
    if (m_node->m_searchNode == this)
        m_node->m_searchNode = 0;
    if (m_open)
        m_open = !m_open;
    next = 0;
    m_node = 0;
}

CSearchNode::CSearchNode() {
    next = 0;
    m_node = 0;
}
