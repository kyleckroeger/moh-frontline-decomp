// A fragment of AIObject.cpp (0x8004a0ec): CAIObject::WalkToCoverPointIndirect
// and WalkToCoverPointDirect.
// The indirect walk: every 0.1 seconds (+540) it checks the cover point's usefulness (as
// CheckCoverpointUsefulness, inlined); then, once within 0.2 of the stored
// target (+80) and not yet arrived (bit 2 of +552), fires script event 60
// (cover types 0x40 and 0x80) or 59 with the cover type, marks the arrival,
// sets the movement mode 3 and hands the cover point's modified position to
// the filter object (its virtual slot at 0x1c, a placeholder named by
// offset). WalkToCoverPointDirect does the same check, then fires event 58
// or 57 once the squared distance is about 24.01 (within 0.09; bit 1) and,
// setting the flag at +704, event 56 once it is below 0.09 (bit 0). The classes and functions are named by the symbols;
// CheckCoverpointUsefulness's body follows AIObject_coveruseful.cpp (it is
// later in the original file; this unit marks it inline so that it is
// inlined without being emitted), the members follow AIObject_coverwalk.cpp
// and the event meanings are inferred. The rest of the file is in other
// units.
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
bool MathFunCloseToZero(float, float);

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
};

enum COVERPOINT_TYPE {};

class CAIObject;

class CCoverPoint {
public:
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    bool ProvidesCover(const CAIFilterRealPosition&, const CAIFilterRealPosition*, const CAIObject*) const;
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

class CAIFilterGlobal {
public:
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

    unsigned char unknown000[8];
    CAIFilterObjectView* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealVector3 m_target;
    unsigned char unknown060[100];
    CAIObject* m_threat;
    unsigned char unknown0c8[336];
    int m_moveMode;
    float m_nextCheck21c;
    unsigned char unknown220[8];
    unsigned long m_state;
    unsigned char unknown22c[100];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    unsigned char unknown298[20];
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
