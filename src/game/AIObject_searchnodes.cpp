// A fragment of AIObject.cpp (0x8004b5b4): CAIObject::GetNextPathNode and
// CAIObject::CleanupAStarPathWalk and
// CAIObject::AddChildrenToSearchNodeQueue, over the A* search nodes held in the list
// at +600 (inferred from offsets in a non-virtual view). GetNextPathNode drops
// the reached node while the goal (+616) is unchanged; otherwise it records
// the new goal, recomputes the path (limit 100) and skips the first node when
// the line to the second, both raised by 0.4, is clear. The current node is
// kept at +620. SPR_BEGIN (named by its symbol) is passed as the search queue;
// its declared type is unknown. Search node and path node offsets are
// inferred; CAIFilterRealVector3 temps follow AIObject_patrolwalk.cpp.
// AddChildrenToSearchNodeQueue makes a search node for each enabled child of
// the parent's path node (skipping the grandparent), keeps the cheaper of it
// and an open node already queued for the same path node, and returns the
// new node when it reaches the goal. The path node fields (+12 search node,
// +16 child count, +17 flag bits, +20 child indices) and search node fields
// (+4 parent, +20 cost, +24 closed flag) and their names are inferred.
class CLinkedListElement;

class CLinkedList {
public:
    bool IsEmpty();
    void RemoveElement(CLinkedListElement*);
    void AddElementAtEnd(CLinkedListElement*);
    void AddElementAtBeginning(CLinkedListElement*);

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

class CSearchNode;

class CPathNode {
public:
    CAIFilterRealPosition m_position;
    CSearchNode* m_searchNode;
    unsigned char m_childCount;
    unsigned char m_enabled : 1;
    unsigned short* m_children;
};

class CSearchNode : public CLinkedListElement {
public:
    static void* operator new(unsigned long);
    static void operator delete(void*, unsigned long);
    CSearchNode();
    ~CSearchNode();
    CSearchNode* Setup(CSearchNode*, CPathNode*, const CPathNode*);

    CSearchNode* m_parent;
    CPathNode* m_pathNode;
    float m_unknown0c;
    float m_unknown10;
    float m_cost;
    bool m_closed;

    /* inferred inline getter (the test reads like an inlined bool return) */
    bool IsClosed() const { return m_closed; }
};

class CSearchNodeQueue {
public:
    void Clear();
    CSearchNode* PopFirst();
    void ReplaceElement(CSearchNode*, CSearchNode*);
    void Insert(CSearchNode*);

    int m_count;
};

class CSearchNodeList : public CLinkedList {
};
class CAIFilterObject;

class CAIFilterGlobal {
public:
    CPathNode* GetAStarPathNodeByIndex(int) const;

    unsigned char unknown00[24];
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);
};

extern void* SPR_BEGIN; /* type unknown */
extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIObject {
public:
    bool ComputeAStarPath(CPathNode*, const CPathNode*, float, CSearchNodeQueue&, CSearchNodeList&);
    CPathNode* GetNextPathNode(CPathNode*, CPathNode*);
    void CleanupAStarPathWalk();
    CSearchNode* AddChildrenToSearchNodeQueue(CSearchNode*, const CPathNode*, CSearchNodeQueue&, CSearchNodeList&);

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

bool CAIObject::ComputeAStarPath(CPathNode* start, const CPathNode* goal, float maxCost,
                                 CSearchNodeQueue& queue, CSearchNodeList& list) {
    queue.m_count = 1;
    CSearchNode* node;
    CSearchNode* found = 0;
    while (!list.IsEmpty()) {
        CSearchNode* node = (CSearchNode*)list.m_head;
        list.RemoveElement(node);
        delete node;
    }
    if (start == goal) {
        found = new CSearchNode;
        found->Setup(0, start, goal);
    } else {
        CSearchNode* node = new CSearchNode;
        node->Setup(0, start, goal);
        queue.Insert(node);
        float limit = 0.25f * maxCost;
        while (queue.m_count > 1) {
            CSearchNode* best = queue.PopFirst();
            if (best->m_cost > maxCost && best->m_unknown0c > limit) {
                delete best;
                break;
            }
            best->m_closed = true;
            list.AddElementAtEnd(best);
            found = AddChildrenToSearchNodeQueue(best, goal, queue, list);
            if (found)
                break;
        }
        queue.Clear();
    }
    bool result = found != 0;
    if (found) {
        if (!found->IsClosed())
            found->m_closed = true;
        list.AddElementAtEnd(found);
    } else {
        CSearchNode* nearestNode = 0;
        float distance;
        float nearest = 0.0f;
        for (CSearchNode* node = (CSearchNode*)list.m_head; node; node = (CSearchNode*)node->m_next) {
            distance = node->m_unknown10;
            if (!nearestNode || distance < nearest) {
                nearestNode = node;
                nearest = distance;
            }
        }
        found = nearestNode;
    }
    if (found) {
        for (node = found; node; node = node->m_parent) {
            node->m_pathNode->m_searchNode = 0;
            node->m_closed = !node->m_closed;
        }
        CSearchNode* cur = (CSearchNode*)list.m_head;
        while (cur) {
            node = (CSearchNode*)cur->m_next;
            list.RemoveElement(cur);
            if (cur->m_closed) {
                cur->m_closed = !cur->m_closed;
                delete cur;
            }
            cur = node;
        }
        for (node = found; node; node = node->m_parent)
            list.AddElementAtBeginning(node);
    }
    return result;
}

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

CSearchNode* CAIObject::AddChildrenToSearchNodeQueue(CSearchNode* parent, const CPathNode* goal,
                                                     CSearchNodeQueue& queue, CSearchNodeList& list) {
    CPathNode* node = parent->m_pathNode;
    unsigned short* child = node->m_children;
    unsigned short* end = child + node->m_childCount;
    for (; child < end; child++) {
        CPathNode* next = g_aigAIFilterGlobalObject.GetAStarPathNodeByIndex(*child);
        if (!(next->m_enabled))
            continue;
        if (parent->m_parent && parent->m_parent->m_pathNode == next)
            continue;
        CSearchNode* search = new CSearchNode;
        CSearchNode* existing = search->Setup(parent, next, goal);
        if (existing && !existing->IsClosed()) {
            if (search->m_cost < existing->m_cost) {
                search->m_pathNode->m_searchNode = search;
                queue.ReplaceElement(existing, search);
                delete existing;
            } else {
                delete search;
            }
        } else if (existing) {
            delete search;
        } else if (goal == next) {
            return search;
        } else {
            queue.Insert(search);
        }
    }
    return 0;
}
