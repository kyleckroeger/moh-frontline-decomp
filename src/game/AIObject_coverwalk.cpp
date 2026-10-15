// A fragment of AIObject.cpp (0x8004ab24): CAIObject::CommitCoverPointWalkToStored
// records the squared XY distance from the position (+24) to the stored
// target (+80), clears two flags, sets the next checks 1 and 0.3 seconds
// ahead, disengages the obstacle avoidance (re-testing the flag it just
// cleared, as the code does; EngageObstacleAvoidance inlined), sets the
// movement mode 9, hands the target to the object at +8 through its virtual
// slot at 0x1c (a placeholder named by offset) and clears the low three bits
// of the state at +552. CAIObject::StartCoverPointWalk restarts the walk as
// StartAStarPathWalk does, moves the arbitrary point (+712) to the cover
// point's modified position (also kept at +664), and asks for an A* walk from
// a zero vector along the reversed second row of the cover point object's
// orientation matrix; when that fails the target is the arbitrary point. It
// then commits the walk (CommitCoverPointWalkToStored, inlined) or only its
// distance, check and avoidance part, flags the cover point occupied (firing
// script event 62 when that fails) and returns whether the A* walk started.
// The classes and functions are named by the symbols; the members follow
// AIObject_spline.cpp, AIObject_obstacle.cpp, AIObject_patrolwalk.cpp and
// AIObject_setcover.cpp and are otherwise inferred from offsets, as are the
// cover point's object and its orientation (+4, +28). CMatrix follows
// bsbifunc_attach.cpp. The rest of the file is in other units.
class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

union CVector3Pair {
    double pair[2];
    float v[4];
};

/* the copy goes through a double pair, as in AIObject_patrolwalk.cpp */
class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(const CAIFilterRealVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }

    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

/* inferred: the cover point's object, oriented by the quaternion at +28 */
struct CoverPointObjectView {
    unsigned char unknown00[28];
    CQuaternion orientation;
};

enum COVERPOINT_TYPE {};

class CCoverPoint {
public:
    bool FlagAsOccupied(COVERPOINT_TYPE);
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;

    unsigned char unknown00[4];
    CoverPointObjectView* m_object;
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

struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CObstacleAvoidance {
public:
    void PrepareForAvoidance(CAIFilterRealPosition&);
};

/* inferred: the object at +8, its virtual function at slot 0x1c */
class CAIFilterObjectView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c(CAIFilterRealPosition&);

    unsigned char unknown04[8];
    BSObject* m_script;
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIObject {
public:
    void CommitCoverPointWalkToStored();
    bool StartCoverPointWalk(bool);
    bool SetupAStarPathWalk(const CAIFilterRealVector3&, const CAIFilterRealVector3&, bool);

    unsigned char unknown000[8];
    CAIFilterObjectView* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_target;
    unsigned char unknown05c[444];
    int m_moveMode;
    float m_nextCheck21c;
    float m_distanceSquared;
    bool m_flag224;
    bool m_flag225;
    unsigned char unknown226[2];
    unsigned long m_state;
    unsigned char unknown22c[44];
    CLinkedList m_searchNodes;
    unsigned char unknown25c[8];
    int m_unknown264;
    int m_unknown268;
    int m_unknown26c;
    unsigned char unknown270[24];
    float m_nextCheck288;
    unsigned char unknown28c[4];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    CAIFilterRealPosition m_coverPosition;
    unsigned char unknown2a4[28];
    bool m_atCoverPoint;
    unsigned char unknown2c1[7];
    CAIFilterRealVector3 m_arbitraryPoint;
    int m_arbitraryPointType;
    unsigned char unknown2dc[8];
    bool m_avoiding;
    unsigned char unknown2e5[3];
    CObstacleAvoidance m_obstacleAvoidance;
    unsigned char unknown2e9[915];
    float m_unknown67c;
};

void CAIObject::CommitCoverPointWalkToStored() {
    m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_target);
    m_flag224 = false;
    m_flag225 = false;
    m_nextCheck288 = 1.0f + g_aigAIFilterGlobalObject.m_time;
    m_nextCheck21c = 0.3f + g_aigAIFilterGlobalObject.m_time;
    m_avoiding = false;
    if (m_avoiding) {
        m_obstacleAvoidance.PrepareForAvoidance(m_target);
        m_unknown67c = 0.0f;
    }
    m_moveMode = 9;
    m_filterObject->vf1c(m_target);
    m_state &= ~7;
}

bool CAIObject::StartCoverPointWalk(bool commit) {
    CAIFilterRealVector3 zero;
    m_atCoverPoint = false;
    m_moveMode = 0;
    m_unknown264 = 0;
    m_unknown268 = 0;
    m_unknown26c = 0;
    float z = m_arbitraryPoint.z;
    float y = m_arbitraryPoint.y;
    float x = m_arbitraryPoint.x;
    m_target.x = x;
    m_target.y = y;
    m_target.z = z;
    m_distanceSquared = m_position.GetDistanceSquaredXYZReal(m_target);
    m_flag224 = false;
    m_flag225 = false;
    m_moveMode = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    m_coverPoint->GetModifiedPosition(m_arbitraryPoint, m_coverType);
    float pz = m_arbitraryPoint.z;
    float py = m_arbitraryPoint.y;
    float px = m_arbitraryPoint.x;
    m_coverPosition.x = px;
    m_coverPosition.y = py;
    m_coverPosition.z = pz;
    zero.x = zero.y = zero.z = 0.0f;
    CoverPointObjectView* object = m_coverPoint->m_object;
    CMatrix matrix;
    object->orientation.GetMatrix(matrix);
    CAIFilterRealVector3 direction(CAIFilterRealVector3(*(const CAIFilterRealVector3*)matrix.m[1]));
    direction.x *= -1.0f;
    direction.y *= -1.0f;
    direction.z *= -1.0f;
    m_arbitraryPointType = 0;
    bool result = true;
    if (!SetupAStarPathWalk(zero, direction, false)) {
        result = false;
        float tz = m_arbitraryPoint.z;
        float ty = m_arbitraryPoint.y;
        float tx = m_arbitraryPoint.x;
        m_target.x = tx;
        m_target.y = ty;
        m_target.z = tz;
    }
    if (commit) {
        CommitCoverPointWalkToStored();
    } else {
        m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_target);
        m_flag224 = false;
        m_flag225 = false;
        m_nextCheck288 = 1.0f + g_aigAIFilterGlobalObject.m_time;
        m_nextCheck21c = 0.3f + g_aigAIFilterGlobalObject.m_time;
        m_avoiding = false;
        if (m_avoiding) {
            m_obstacleAvoidance.PrepareForAvoidance(m_target);
            m_unknown67c = 0.0f;
        }
    }
    if (!m_coverPoint->FlagAsOccupied(m_coverType))
        BSObjectTriggerEvent(m_filterObject->m_script, 62, 0, 0, false);
    return result;
}
