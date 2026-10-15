// A fragment of AIObject.cpp (0x8004b5b4): CAIObject::GetNextPathNode and
// CAIObject::CleanupAStarPathWalk, over the A* search nodes held in the list
// at +600 (inferred from offsets in a non-virtual view). GetNextPathNode drops
// the reached node while the goal (+616) is unchanged; otherwise it records
// the new goal, recomputes the path (limit 100) and skips the first node when
// the line to the second, both raised by 0.4, is clear. The current node is
// kept at +620. SPR_BEGIN (named by its symbol) is passed as the search queue;
// its declared type is unknown. Search node and path node offsets are
// inferred; CAIFilterRealVector3 temps follow AIObject_patrolwalk.cpp.
class CLinkedListElement;

class CLinkedList {
public:
    void RemoveElement(CLinkedListElement*);

    CLinkedListElement* m_head;
};

class CLinkedListElement {
public:
    CLinkedListElement* m_next;
};

class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
} __attribute__((aligned(8)));

class CPathNode {
public:
    CAIFilterRealPosition m_position;
};

class CSearchNode : public CLinkedListElement {
public:
    ~CSearchNode();

    unsigned char unknown04[4];
    CPathNode* m_pathNode;
};

class CSearchNodeQueue;
class CSearchNodeList;
class CAIFilterObject;

class CAIFilterGlobal {
public:
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);
};

extern void* SPR_BEGIN; /* type unknown */

class CAIObject {
public:
    bool ComputeAStarPath(CPathNode*, const CPathNode*, float, CSearchNodeQueue&, CSearchNodeList&);
    CPathNode* GetNextPathNode(CPathNode*, CPathNode*);
    void CleanupAStarPathWalk();

    unsigned char unknown000[8];
    CAIFilterObject* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[564];
    CLinkedList m_searchNodes;
    unsigned char unknown25c[12];
    CPathNode* m_unknown268;
    CPathNode* m_unknown26c;
};

CPathNode* CAIObject::GetNextPathNode(CPathNode* start, CPathNode* end) {
    CSearchNode* first = (CSearchNode*)m_searchNodes.m_head;
    if (end == m_unknown268 && first) {
        m_searchNodes.RemoveElement(first);
        delete first;
        CSearchNode* head = (CSearchNode*)m_searchNodes.m_head;
        if (head)
            m_unknown26c = head->m_pathNode;
        else
            m_unknown26c = 0;
        return m_unknown26c;
    }
    if (end)
        m_unknown268 = end;
    bool found = ComputeAStarPath(start, m_unknown268, 100.0f, *(CSearchNodeQueue*)SPR_BEGIN,
                                  *(CSearchNodeList*)&m_searchNodes);
    first = (CSearchNode*)m_searchNodes.m_head;
    CSearchNode* next = (CSearchNode*)first->m_next;
    if (next) {
        CAIFilterRealVector3 to;
        CAIFilterRealVector3 from;
        float z = m_position.z;
        float y = m_position.y;
        float x = m_position.x;
        from.x = x;
        from.y = y;
        from.z = z;
        from.z += 0.4f;
        CPathNode* node = next->m_pathNode;
        float ny;
        float nz = node->m_position.z;
        ny = node->m_position.y;
        float nx = node->m_position.x;
        to.x = nx;
        to.y = ny;
        to.z = nz;
        to.z += 0.4f;
        if (!CAIFilterGlobal::CheckCollisionLineToEnvironment(from, to, m_filterObject)) {
            m_searchNodes.RemoveElement(first);
            delete first;
            m_unknown26c = next->m_pathNode;
            return m_unknown26c;
        }
    }
    m_unknown26c = first->m_pathNode;
    if (found)
        return m_unknown26c;
    return 0;
}

void CAIObject::CleanupAStarPathWalk() {
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
}
