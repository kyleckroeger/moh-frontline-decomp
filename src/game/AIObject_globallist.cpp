// A fragment of AIObject.cpp (0x8004ec04): CAIObject::UpdateReactionTime
// (without a target the reacting flag is cleared while the reaction time plus
// 3 is still ahead of the AI clock; with one, a reacting object's reaction
// time is reset to the clock), CAIObject's global object list
// (ResetGlobalList, GetGlobalListStart, RemoveFromGlobalList) and
// RemovePointersTo, which clears another object's target, path-walk target
// (+624) and the pointers at +1688 and +1692 when they refer to a removed
// object, raising script events 67 and 68. The list head is file-local in the
// original and is declared extern here; the members are inferred from offsets
// in a non-virtual view and named by offset where their use is unclear.
struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterObject {
public:
    unsigned char unknown00[12];
    BSObject* m_bsObject;
};

class CLinkedListElement;

class CLinkedList {
public:
    void RemoveElement(CLinkedListElement*);

    CLinkedListElement* m_head;
};

class CLinkedListElement {
public:
    unsigned char unknown00[4];
};

class CSearchNode : public CLinkedListElement {
public:
    ~CSearchNode();
};

class CAIObject {
public:
    void ResetGlobalList();
    CAIObject* GetGlobalListStart();
    void RemovePointersTo(CAIObject*);
    void RemoveFromGlobalList();
    bool UpdateReactionTime();

    unsigned char unknown000[4];
    CAIObject* m_next;
    CAIFilterObject* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[100];
    bool m_unknown0c0;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
    unsigned char unknown0c8[152];
    float m_reactionTime;
    unsigned char unknown164[112];
    int m_unknown1d4;
    unsigned char unknown1d8[20];
    int m_unknown1ec;
    unsigned char unknown1f0[40];
    int m_unknown218;
    unsigned char unknown21c[4];
    float m_unknown220;
    bool m_unknown224;
    bool m_unknown225;
    unsigned char unknown226[50];
    CLinkedList m_searchNodes;
    unsigned char unknown25c[8];
    int m_unknown264;
    int m_unknown268;
    int m_unknown26c;
    CAIObject* m_pathTarget;
    unsigned char unknown274[84];
    CAIFilterRealPosition m_arbitraryPoint;
    unsigned char unknown2d4[4];
    int m_unknown2d8;
    unsigned char unknown2dc[956];
    CAIObject* m_unknown698;
    CAIObject* m_unknown69c;
    unsigned char unknown6a0[56];
    bool m_unknown6d8;
    unsigned char unknown6d9[88];
    bool m_reacting;
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

extern CAIObject* g_paioAIObjectListStart; // file-local in the original
extern int g_iAIObjectCount;

bool CAIObject::UpdateReactionTime() {
    if (m_target == 0) {
        if (3.0f + m_reactionTime > g_aigAIFilterGlobalObject.m_time)
            m_reacting = false;
    } else if (m_reacting) {
        m_reactionTime = g_aigAIFilterGlobalObject.m_time;
    }
    return true;
}

void CAIObject::ResetGlobalList() {
    g_paioAIObjectListStart = 0;
    g_iAIObjectCount = 0;
}

CAIObject* CAIObject::GetGlobalListStart() {
    return g_paioAIObjectListStart;
}

void CAIObject::RemovePointersTo(CAIObject* object) {
    if (m_target == object) {
        BSObjectTriggerEvent(m_filterObject->m_bsObject, 67, 0, 0, true);
        m_unknown0c0 = false;
        if (m_target) {
            m_unknown6d8 = false;
            CAIObject* old = m_target;
            m_target = 0;
            if (m_unknown218 == 8 || m_unknown218 == 2) {
                if (m_unknown2d8 == 1 || m_unknown2d8 == 2)
                    m_unknown2d8 = 3;
            }
            if (old) {
                old->m_unknown1ec += -1;
                if (old->m_unknown1ec <= 15)
                    old->m_unknown1d4 = old->m_unknown1ec;
                else
                    old->m_unknown1d4 = 15;
            }
        }
    }
    if (m_pathTarget == object) {
        BSObjectTriggerEvent(m_filterObject->m_bsObject, 68, 0, 0, true);
        m_unknown264 = 0;
        m_unknown268 = 0;
        m_unknown26c = 0;
        float z = m_arbitraryPoint.z;
        float y = m_arbitraryPoint.y;
        float x = m_arbitraryPoint.x;
        m_moveTarget.x = x;
        m_moveTarget.y = y;
        m_moveTarget.z = z;
        m_unknown220 = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
        m_unknown224 = false;
        m_unknown225 = false;
        m_unknown218 = 0;
        CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
        while (node) {
            m_searchNodes.RemoveElement(node);
            delete node;
            node = (CSearchNode*)m_searchNodes.m_head;
        }
        m_pathTarget = 0;
    }
    if (m_unknown698 == object)
        m_unknown698 = 0;
    if (m_unknown69c == object) {
        BSObjectTriggerEvent(m_filterObject->m_bsObject, 67, 0, 0, true);
        m_unknown69c = 0;
    }
}

void CAIObject::RemoveFromGlobalList() {
    CAIObject* previous = 0;
    CAIObject* object = g_paioAIObjectListStart;
    g_iAIObjectCount--;
    while (object) {
        if (object == this)
            break;
        previous = object;
        object = object->m_next;
    }
    if (previous)
        previous->m_next = m_next;
    else
        g_paioAIObjectListStart = m_next;
    for (object = g_paioAIObjectListStart; object; object = object->m_next)
        object->RemovePointersTo(this);
}
