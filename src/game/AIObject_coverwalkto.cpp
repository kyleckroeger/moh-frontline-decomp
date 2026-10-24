// A fragment of AIObject.cpp (0x8004a0ec): CAIObject::WalkToCoverPointIndirect,
// WalkToCoverPointDirect and WalkToCoverPoint.
// The indirect walk: every 0.1 seconds (+540) it checks the cover point's usefulness (as
// CheckCoverpointUsefulness, inlined); then, once within 0.2 of the stored
// target (+80) and not yet arrived (bit 2 of +552), fires script event 60
// (cover types 0x40 and 0x80) or 59 with the cover type, marks the arrival,
// sets the movement mode 3 and hands the cover point's modified position to
// the filter object (its virtual slot at 0x1c, a placeholder named by
// offset). WalkToCoverPointDirect does the same check, then fires event 58
// or 57 once the squared distance is about 24.01 (within 0.09; bit 1) and,
// setting the flag at +704, event 56 once it is below 0.09 (bit 0).
// WalkToCoverPoint does the same usefulness check (stopping when the cover
// point is gone), updates the A* walk, and measures the distance to the cover
// point's position (+664) and the unit direction to it. About 4.9 away (not
// 5.5; bit 1), when the cover point's facing (the second row of its object's
// orientation matrix) is towards the soldier, the soldier faces the cover
// point (+128) and the remaining path cost (the search nodes from +600) is
// below 1.25 times the distance and the line between the two positions raised
// by 0.5 is clear, it fires event 58 or 57 and moves the target to the cover
// point's modified position; about 0.8 away (not 1.2; bit 2) it does the same
// with events 60 or 59 and no line test; below 0.3 (bit 0) it fires event 56.
// The classes and functions are named by the symbols;
// CheckCoverpointUsefulness's body follows AIObject_coveruseful.cpp (it is
// later in the original file; this unit marks it inline so that it is
// inlined without being emitted), the members follow AIObject_coverwalk.cpp
// and the event meanings are inferred, as are the path cost and the row
// getters (inline helpers: the row temporary built inside GetFront sits below
// the inlined check's position, as in the original frame). The rest of the
// file is in other units.
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
bool MathFunCloseToZero(float, float);

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(const CAIFilterRealVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }
    CAIFilterRealVector3& operator=(const CAIFilterRealVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
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
    CAIFilterRealVector3 GetRow(int i) const { return *(const CAIFilterRealVector3*)m[i]; }
    void GetFront(CAIFilterRealVector3& v) const { v = GetRow(1); }

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

struct CoverPointObjectView {
    unsigned char unknown00[28];
    CQuaternion orientation;
};

class CLinkedListElement {
public:
    CLinkedListElement* m_next;
};

class CSearchNode : public CLinkedListElement {
public:
    float GetCostValue(const CSearchNode*);
};

enum COVERPOINT_TYPE {};

class CAIObject;

class CCoverPoint {
public:
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    bool ProvidesCover(const CAIFilterRealPosition&, const CAIFilterRealPosition*, const CAIObject*) const;

    unsigned char unknown00[4];
    CoverPointObjectView* m_object;
};

/* inferred: the filter object, its script at +12 and virtual slot 0x1c */
class CAIFilterObjectView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c(CAIFilterRealPosition&);

    unsigned char unknown04[8];
    BSObject* script;
};

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);

    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;


class CAIObject {
public:
    void WalkToCoverPointIndirect();
    void WalkToCoverPointDirect();
    void CheckCoverpointUsefulness();
    void WalkToCoverPoint();
    void UpdateAStarPathWalk();

    unsigned char unknown000[8];
    CAIFilterObjectView* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealVector3 m_position;
    unsigned char unknown028[40];
    CAIFilterRealVector3 m_target;
    unsigned char unknown060[32];
    CAIFilterRealVector3 m_facing;
    unsigned char unknown090[52];
    CAIObject* m_threat;
    unsigned char unknown0c8[336];
    int m_moveMode;
    float m_nextCheck21c;
    unsigned char unknown220[8];
    unsigned long m_state;
    unsigned char unknown22c[44];
    CSearchNode* m_path;
    unsigned char unknown25c[48];
    bool m_flag28c;
    unsigned char unknown28d[3];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    CAIFilterRealVector3 m_coverTarget;
    unsigned char unknown2a8[4];
    bool m_checkCover;
    unsigned char unknown2ad[19];
    bool m_flag2c0;
    unsigned char unknown2c1[3];
    float m_nextCoverCheck;
};

inline void CAIObject::CheckCoverpointUsefulness() {
    if (m_checkCover) {
        if (!m_coverPoint) {
            BSObjectTriggerEvent(m_filter->script, 62, 0, 0, true);
        } else if (!(g_aigAIFilterGlobalObject.m_time < m_nextCoverCheck)) {
            m_nextCoverCheck = 0.2f + g_aigAIFilterGlobalObject.m_time;
            if (m_threat) {
                CAIFilterRealVector3 position;
                m_coverPoint->GetModifiedPosition(position, m_coverType);
                if (!m_coverPoint->ProvidesCover(position, &m_threat->m_position, this))
                    BSObjectTriggerEvent(m_filter->script, 62, 0, 0, true);
            }
        }
    }
}

void CAIObject::WalkToCoverPointIndirect() {
    if (!(g_aigAIFilterGlobalObject.m_time < m_nextCheck21c)) {
        m_nextCheck21c = 0.1f + g_aigAIFilterGlobalObject.m_time;
        CheckCoverpointUsefulness();
    }
    float distance = m_position.GetDistanceSquaredXYZReal(m_target);
    if (!(m_state & 4) && distance < 0.040000003f) {
        if (m_coverType & 0xc0)
            BSObjectTriggerEvent(m_filter->script, 60, (void*)m_coverType, 0, false);
        else
            BSObjectTriggerEvent(m_filter->script, 59, (void*)m_coverType, 0, false);
        m_state |= 4;
        m_moveMode = 3;
        m_coverPoint->GetModifiedPosition(m_target, m_coverType);
        m_filter->vf1c(m_target);
    }
}

void CAIObject::WalkToCoverPointDirect() {
    if (!(g_aigAIFilterGlobalObject.m_time < m_nextCheck21c)) {
        m_nextCheck21c = 0.1f + g_aigAIFilterGlobalObject.m_time;
        CheckCoverpointUsefulness();
    }
    float distance = m_position.GetDistanceSquaredXYZReal(m_target);
    if (!(m_state & 2) && MathFunCloseToZero(distance - 24.01f, 0.09f)) {
        if (m_coverType & 0xc0)
            BSObjectTriggerEvent(m_filter->script, 58, (void*)m_coverType, 0, false);
        else
            BSObjectTriggerEvent(m_filter->script, 57, (void*)m_coverType, 0, false);
        m_state |= 2;
    }
    if (!(m_state & 1) && distance < 0.09f) {
        m_flag2c0 = true;
        BSObjectTriggerEvent(m_filter->script, 56, (void*)m_coverType, 0, true);
        m_state |= 1;
    }
}

inline float PathCost(CSearchNode* node) {
    if (!node)
        return 0.0f;
    float cost = 0.0f;
    for (CSearchNode* next = (CSearchNode*)node->m_next; next; next = (CSearchNode*)next->m_next) {
        cost += next->GetCostValue(node);
        node = next;
    }
    return cost;
}

void CAIObject::WalkToCoverPoint() {
    if (!(g_aigAIFilterGlobalObject.m_time < m_nextCheck21c)) {
        m_nextCheck21c = 0.1f + g_aigAIFilterGlobalObject.m_time;
        CheckCoverpointUsefulness();
        if (!m_coverPoint)
            return;
    }
    UpdateAStarPathWalk();
    if (!m_coverPoint)
        return;
    float distance = m_position.GetDistanceXYZReal(m_coverTarget);
    CoverPointObjectView* object = m_coverPoint->m_object;
    CMatrix matrix;
    object->orientation.GetMatrix(matrix);
    CAIFilterRealVector3 front;
    matrix.GetFront(front);
    CAIFilterRealVector3 facing = m_facing;
    float dx = m_coverTarget.x - m_position.x;
    float dy = m_coverTarget.y - m_position.y;
    float dz = m_coverTarget.z - m_position.z;
    float length = sqrtf(dx * dx + dy * dy + dz * dz);
    if (length != 0.0f) {
        float inverse = 1.0f / length;
        dx *= inverse;
        dy *= inverse;
        dz *= inverse;
    }
    if (!(m_state & 2) && MathFunCloseToZero(distance - 4.9f, 0.3f)) {
        if (MathFunCloseToZero(distance - 4.9f - 0.6f, 0.3f)) {
            m_state |= 2;
        } else if (dx * front.x + dy * front.y + dz * front.z > 0.01f) {
            if (facing.x * dx + facing.y * dy + facing.z * dz > 0.7071f) {
                if (PathCost(m_path) < 1.25f * distance) {
                    CAIFilterRealVector3 to = m_coverTarget;
                    CAIFilterRealVector3 from = m_position;
                    to.z += 0.5f;
                    from.z += 0.5f;
                    if (!CAIFilterGlobal::CheckCollisionLineToEnvironment(from, to, (CAIFilterObject*)m_filter)) {
                        if (m_coverType & 0xc0)
                            BSObjectTriggerEvent(m_filter->script, 58, (void*)m_coverType, 0, false);
                        else
                            BSObjectTriggerEvent(m_filter->script, 57, (void*)m_coverType, 0, false);
                        m_flag28c = true;
                        m_flag2c0 = true;
                        m_state |= 2;
                        m_coverPoint->GetModifiedPosition(m_coverTarget, m_coverType);
                        float z = m_coverTarget.z;
                        float y = m_coverTarget.y;
                        float x = m_coverTarget.x;
                        m_target.x = x;
                        m_target.y = y;
                        m_target.z = z;
                        m_filter->vf1c(m_target);
                    }
                }
            }
        }
    }
    if (!(m_state & 4) && MathFunCloseToZero(distance - 0.8f, 0.2f)) {
        if (MathFunCloseToZero(distance - 0.8f - 0.4f, 0.2f)) {
            m_state |= 4;
        } else if (front.x * dx + front.y * dy + front.z * dz > 0.01f) {
            if (m_coverType & 0xc0)
                BSObjectTriggerEvent(m_filter->script, 60, (void*)m_coverType, 0, false);
            else
                BSObjectTriggerEvent(m_filter->script, 59, (void*)m_coverType, 0, false);
            m_flag28c = true;
            m_flag2c0 = true;
            m_state |= 4;
            m_coverPoint->GetModifiedPosition(m_coverTarget, m_coverType);
            float z = m_coverTarget.z;
            float y = m_coverTarget.y;
            float x = m_coverTarget.x;
            m_target.x = x;
            m_target.y = y;
            m_target.z = z;
            m_filter->vf1c(m_target);
        }
    }
    if (!(m_state & 1) && distance < 0.3f) {
        if (dx * front.x + dy * front.y + dz * front.z > 0.01f) {
            m_flag2c0 = true;
            BSObjectTriggerEvent(m_filter->script, 56, (void*)m_coverType, 0, true);
            m_state |= 1;
        }
    }
}
