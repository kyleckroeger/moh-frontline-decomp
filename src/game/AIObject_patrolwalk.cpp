// A fragment of AIObject.cpp (0x8004c218): CAIObject::StartAStarPathWalk
// and CAIObject::SetupAStarPathWalkForPatrol restart the walk as
// StartWalkToArbitraryPoint does (move target from the arbitrary point,
// squared distance, cleared flags and search nodes). StartAStarPathWalk
// first refreshes the arbitrary point from its +736 source, then refreshes it
// from the threat or +336 by type and asks for an A* walk from a zero vector
// along the normalised direction away from the point, recording the walk
// state (2 when started, 0 when not). The patrol variant makes the patrol
// point the arbitrary point of type 9 (refreshing it from the threat or +336
// as WalkToArbitraryPoint does for the other types), then asks for an A*
// walk along the normalised direction to it and its reverse.
// The classes and functions are named by the symbols; the view follows
// AIObject_arbitrary.cpp and the point refresh is an inferred inline helper
// shared with WalkToArbitraryPoint. The rest of the file is in other units.
extern "C" {
#include <math.h>
}

struct BSObject;
enum aifilter_target_mode {};
enum EMovePointType {};
struct aistatus_match;
struct BS_STRUCT_Vector_struct;

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

union CVector3Pair {
    double pair[2];
    float v[4];
};

/* the copy goes through a double pair, as in player_weapon.cpp */
class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(const CAIFilterRealVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }

    float w;
} __attribute__((aligned(8)));

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    virtual void SetMoveTarget(const CAIFilterRealPosition&) {}
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);

    unsigned char unknown04[8];
    BSObject* m_script;
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

class CAIFilterGlobal {
public:
    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIObject {
public:
    bool StartAStarPathWalk();
    bool SetupAStarPathWalkForPatrol(const CAIFilterRealVector3&);
    bool SetupAStarPathWalk(const CAIFilterRealVector3&, const CAIFilterRealVector3&, bool);

    unsigned char unknown000[8];
    CAIFilterObject* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[104];
    CAIObject* m_unknown0c4;
    unsigned char unknown0c8[136];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[188];
    int m_unknown218;
    float m_unknown21c;
    float m_unknown220;
    bool m_unknown224;
    bool m_unknown225;
    unsigned char unknown226[50];
    CLinkedList m_searchNodes;
    unsigned char unknown25c[8];
    int m_unknown264;
    int m_unknown268;
    int m_unknown26c;
    unsigned char unknown270[88];
    CAIFilterRealPosition m_arbitraryPoint;
    unsigned char unknown2d4[4];
    EMovePointType m_arbitraryPointType;
    float m_unknown2dc;
    CAIFilterRealPosition* m_unknown2e0;
    unsigned char unknown2e4[1012];
    bool m_unknown6d8;
};

bool CAIObject::StartAStarPathWalk() {
    CAIFilterRealVector3 zero;
    CAIFilterRealVector3 direction;
    m_unknown218 = 0;
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
    if (m_unknown2e0) {
        m_arbitraryPoint.x = m_unknown2e0->x;
        m_arbitraryPoint.y = m_unknown2e0->y;
        m_arbitraryPoint.z = m_unknown2e0->z;
    }
    switch (m_arbitraryPointType) {
    case 0:
    case 1:
    case 2:
    default:
        if (m_unknown0c4 && m_unknown6d8) {
            float tz = m_unknown0c4->m_position.z;
            float ty = m_unknown0c4->m_position.y;
            float tx = m_unknown0c4->m_position.x;
            m_arbitraryPoint.x = tx;
            m_arbitraryPoint.y = ty;
            m_arbitraryPoint.z = tz;
        } else {
            float tz = m_unknown150.z;
            float ty = m_unknown150.y;
            float tx = m_unknown150.x;
            m_arbitraryPoint.x = tx;
            m_arbitraryPoint.y = ty;
            m_arbitraryPoint.z = tz;
        }
        m_unknown2e0->x = m_arbitraryPoint.x;
        m_unknown2e0->y = m_arbitraryPoint.y;
        m_unknown2e0->z = m_arbitraryPoint.z;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        break;
    }
    zero.x = zero.y = zero.z = 0.0f;
    direction.x = m_position.x - m_arbitraryPoint.x;
    direction.y = m_position.y - m_arbitraryPoint.y;
    direction.z = m_position.z - m_arbitraryPoint.z;
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length != 0.0f) {
        direction.x *= 1.0f / length;
        direction.y *= 1.0f / length;
        direction.z *= 1.0f / length;
    }
    if (!SetupAStarPathWalk(zero, direction, true)) {
        m_unknown218 = 0;
        return false;
    }
    m_unknown218 = 2;
    return true;
}

bool CAIObject::SetupAStarPathWalkForPatrol(const CAIFilterRealVector3& point) {
    CAIFilterRealVector3 direction;
    CAIFilterRealVector3 back;
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
    m_arbitraryPointType = (EMovePointType)9;
    CAIFilterRealVector3 patrol(point);
    m_arbitraryPoint.x = patrol.x;
    m_arbitraryPoint.y = patrol.y;
    m_arbitraryPoint.z = patrol.z;
    switch (m_arbitraryPointType) {
    case 0:
    case 1:
    case 2:
    default:
        if (m_unknown0c4 && m_unknown6d8) {
            float tz = m_unknown0c4->m_position.z;
            float ty = m_unknown0c4->m_position.y;
            float tx = m_unknown0c4->m_position.x;
            m_arbitraryPoint.x = tx;
            m_arbitraryPoint.y = ty;
            m_arbitraryPoint.z = tz;
        } else {
            float tz = m_unknown150.z;
            float ty = m_unknown150.y;
            float tx = m_unknown150.x;
            m_arbitraryPoint.x = tx;
            m_arbitraryPoint.y = ty;
            m_arbitraryPoint.z = tz;
        }
        m_unknown2e0->x = m_arbitraryPoint.x;
        m_unknown2e0->y = m_arbitraryPoint.y;
        m_unknown2e0->z = m_arbitraryPoint.z;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        break;
    }
    direction.x = m_arbitraryPoint.x - m_position.x;
    direction.y = m_arbitraryPoint.y - m_position.y;
    direction.z = m_arbitraryPoint.z - m_position.z;
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length != 0.0f) {
        direction.x *= 1.0f / length;
        direction.y *= 1.0f / length;
        direction.z *= 1.0f / length;
    }
    back.x = -1.0f * direction.x;
    back.y = -1.0f * direction.y;
    back.z = -1.0f * direction.z;
    if (!SetupAStarPathWalk(direction, back, true))
        return false;
    return true;
}
