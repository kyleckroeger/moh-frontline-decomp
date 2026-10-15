// A fragment of SearchNode.cpp (0x80056210): the CSearchNode destructor and
// constructor. The destructor unlinks the node from its path node, clears its
// open flag, link and path node, and returns the node to the free list when
// deleting; the constructor clears the link and path node. The file name is
// this project's; the original record is SearchNode.cpp. SearchNode.cpp holds
// operator new/delete and GetCostValue; Setup between the two parts is not
// part of this unit. The file's static data are defined here: the pool of
// 4096 nodes (built by its static initialisation, with its array destructor
// registered) and the free list, whose initialiser links the whole pool (as
// CSearchNode::Reset does, through the same inferred inline helper).
// CSearchNode and CPathNode are named by the mangled symbols; members are
// inferred from offsets.
// CSearchNode: A* search nodes from a fixed pool of 4096 with a free list
// (class operator new/delete), linked to path nodes, with the cost so far,
// the estimate to the goal and their sum. CSearchNode, CPathNode and
// CAIFilterRealPosition are named by the mangled symbols; members are
// inferred from offsets and are not original.
class CLinkedListElement {
public:
    CLinkedListElement* next;

    ~CLinkedListElement();
    CLinkedListElement();
};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
    float w;
};

class CSearchNode;

class CPathNode {
public:
    CAIFilterRealPosition GetPosition() const { return CAIFilterRealPosition(m_position[0], m_position[1], m_position[2]); }

    float m_position[3];
    CSearchNode* m_pSearchNode;
};

class CSearchNode : public CLinkedListElement {
public:
    CSearchNode();
    ~CSearchNode();
    static void* operator new(unsigned long);
    static void operator delete(void*, unsigned long);
    static void Reset();
    float GetCostValue(const CSearchNode*);
    CSearchNode* Setup(CSearchNode*, CPathNode*, const CPathNode*);

    CSearchNode* m_pParent;
    CPathNode* m_pPathNode;
    float m_cost;
    float m_estimate;
    float m_total;
    bool m_open;
};

static CSearchNode s_rsnAStarSearchNodePool[4096];

inline CSearchNode* BuildFreeList() {
    CSearchNode* list = 0;
    for (int i = 0; i < 4096; i++) {
        s_rsnAStarSearchNodePool[i].m_pParent = list;
        s_rsnAStarSearchNodePool[i].m_pPathNode = 0;
        s_rsnAStarSearchNodePool[i].m_open = false;
        list = &s_rsnAStarSearchNodePool[i];
    }
    return list;
}

static CSearchNode* s_rsnAStarSearchNodeFreeList = BuildFreeList();

void CSearchNode::Reset();

// Inlined into the deleting destructor; its out-of-line copy belongs to the
// SearchNode.cpp unit, so this fragment keeps an inline copy of the same body.
inline void CSearchNode::operator delete(void* p, unsigned long) {
    ((CSearchNode*)p)->m_pParent = s_rsnAStarSearchNodeFreeList;
    s_rsnAStarSearchNodeFreeList = (CSearchNode*)p;
}

void* CSearchNode::operator new(unsigned long);

float CSearchNode::GetCostValue(const CSearchNode* other);

CSearchNode* CSearchNode::Setup(CSearchNode* parent, CPathNode* node, const CPathNode* goal);

CSearchNode::~CSearchNode() {
    if (m_pPathNode->m_pSearchNode == this)
        m_pPathNode->m_pSearchNode = 0;
    if (m_open)
        m_open = !m_open;
    next = 0;
    m_pPathNode = 0;
}

CSearchNode::CSearchNode() {
    next = 0;
    m_pPathNode = 0;
}
