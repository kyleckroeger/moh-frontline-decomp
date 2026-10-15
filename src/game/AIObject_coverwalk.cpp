// A fragment of AIObject.cpp (0x8004ab24): CAIObject::CommitCoverPointWalkToStored
// records the squared XY distance from the position (+24) to the stored
// target (+80), clears two flags, sets the next checks 1 and 0.3 seconds
// ahead, disengages the obstacle avoidance (re-testing the flag it just
// cleared, as the code does), sets the movement mode 9, hands the target to
// the object at +8 through its virtual slot at 0x1c (a placeholder named by
// offset) and clears the low three bits of the state at +552. The classes and
// functions are named by the symbols; the members follow AIObject_spline.cpp
// and AIObject_obstacle.cpp and are otherwise inferred from offsets. The rest
// of the file is in other units.
class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

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
    unsigned char unknown22c[92];
    float m_nextCheck288;
    unsigned char unknown28c[88];
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
