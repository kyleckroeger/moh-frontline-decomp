// A fragment of AIObject.cpp (0x8004aa28): CAIObject::CheckCoverpointUsefulness
// and StartCoverPointWalkToStored (starts the cover point walk; defined elsewhere).
// While the cover check is on (+684), fires script event 62 on the object's
// script when there is no cover point (+656), or, every 0.2 seconds (the
// next check at +708), when the threat (+196) is set and the cover point,
// at its position modified by the cover type (+660), no longer gives cover
// from the threat's position. The classes and functions are named by the
// symbols; CAIFilterRealVector3 and COVERPOINT_TYPE follow
// bsbifunc_coverpoint.cpp and the members are inferred from offsets. The
// rest of the file is in other units.
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIFilterRealPosition {
public:
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

struct AIFilterScriptView {
    unsigned char unknown00[12];
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
    void CheckCoverpointUsefulness();
    void StartCoverPointWalkToStored(bool);
    void StartCoverPointWalk(bool);

    unsigned char unknown000[8];
    AIFilterScriptView* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[160];
    CAIObject* m_threat;
    unsigned char unknown0c8[456];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    unsigned char unknown298[20];
    bool m_checkCover;
    unsigned char unknown2ad[23];
    float m_nextCoverCheck;
};

void CAIObject::CheckCoverpointUsefulness() {
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

void CAIObject::StartCoverPointWalkToStored(bool walk) {
    StartCoverPointWalk(walk);
}
