// A fragment of AIObject.cpp (0x8004aec4): CAIObject::SetCoverPoint releases
// the current cover point (clearing bit 1 of +424, flagging the point
// unoccupied and clearing the point, the movement mode and the at-point flag;
// the point is re-tested inside, as the code does), records the flag argument
// (+684) and, for an index of 0 or more, takes the global filter's cover point
// of that index: it stores the available peek type (+660), gets the modified
// position, flags the point occupied (firing script event 62 when that fails),
// stores its peek location (+688) and sets the at-point flag (+704) when the
// position is within 0.3. The classes and functions are named by the symbols;
// the views follow AIObject_storecover.cpp and the members are inferred from
// offsets. The rest of the file is in other units.
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIFilterRealPosition {
public:
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
} __attribute__((aligned(8)));

enum COVERPOINT_TYPE {};

class CCoverPoint {
public:
    void FlagAsUnoccupied(COVERPOINT_TYPE);
    bool FlagAsOccupied(COVERPOINT_TYPE);
    COVERPOINT_TYPE GetAvailablePeekPosition() const;
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    CAIFilterRealVector3 GetPeekLocation();
};

struct AIFilterScriptView {
    unsigned char unknown00[12];
    BSObject* script;
};

class CAIFilterGlobal {
public:
    CCoverPoint* GetCoverPointByIndex(int) const;

    unsigned char unknown00[24];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIObject {
public:
    void SetCoverPoint(int, bool);

    unsigned char unknown000[8];
    AIFilterScriptView* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[388];
    unsigned long m_flags1a8;
    unsigned char unknown1ac[108];
    int m_moveMode;
    unsigned char unknown21c[116];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    unsigned char unknown298[20];
    bool m_flag2ac;
    unsigned char unknown2ad[3];
    CAIFilterRealPosition m_peekLocation;
    unsigned char unknown2bc[4];
    bool m_atCoverPoint;
};

void CAIObject::SetCoverPoint(int index, bool flag) {
    if (m_coverPoint) {
        m_flags1a8 &= ~2;
        if (m_coverPoint) {
            m_coverPoint->FlagAsUnoccupied(m_coverType);
            m_coverPoint = 0;
            m_moveMode = 0;
            m_atCoverPoint = false;
        }
    }
    m_atCoverPoint = false;
    m_flag2ac = flag;
    if (index >= 0) {
        m_coverPoint = g_aigAIFilterGlobalObject.GetCoverPointByIndex(index);
        if (m_coverPoint) {
            CAIFilterRealVector3 position;
            m_coverType = m_coverPoint->GetAvailablePeekPosition();
            m_coverPoint->GetModifiedPosition(position, m_coverType);
            if (!m_coverPoint->FlagAsOccupied(m_coverType))
                BSObjectTriggerEvent(m_filter->script, 62, 0, 0, false);
            const CAIFilterRealVector3& peek = m_coverPoint->GetPeekLocation();
            float pz = peek.z;
            float py = peek.y;
            float px = peek.x;
            m_peekLocation.x = px;
            m_peekLocation.y = py;
            m_peekLocation.z = pz;
            if (m_position.GetDistanceXYZReal(position) < 0.3f)
                m_atCoverPoint = true;
        }
    }
}
