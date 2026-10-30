// CAISoldierObject (0x8004f31c): SetUpperLocation keeps a position at +1864;
// ChooseTarget keeps the current target (its distance halved when its call
// at virtual slot +24 is true and it is nearer than 10, less 2 as a bias) and
// looks through the given object list for a nearer enemy (by the two team
// bytes at +20), alive (+436), in line of sight (the range halved for the
// flag at +1840 and times 0.75 when crouching), then sets and returns the
// target or clears it. The team test, the virtual-table view and the inline
// position assignment are inferred. Then the target match lists: the soldier's list of up to a few
// (status, value, weight) matches used when choosing targets, copied from
// the caller or reset to built-in lists by the current target mode. The
// class, enum and structure names come from the mangled symbols; the member
// names and the match fields are inferred. Init and Update follow: Init runs
// the base initialisation, sets the reaction counter (15 times the ratio of
// two members, rounded up), copies a few settings and resets the match list
// for mode 3; Update refreshes the target position when reacting, steps the
// current walk mode (spline, A* path, cover points, arbitrary point), applies
// obstacle avoidance and resets the counter. This is a non-virtual view of
// the class and of a CAIObject base (its virtual table is not reproduced;
// members inferred from offsets); the constants are entries of the file's
// .sdata2 pool. The functions before and after these are not reconstructed
// here.
enum aifilter_target_mode {};

struct aistatus_match {
    int field0;
    int field4;
    int field8;
};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& o) {
        float z = o.z;
        float y = o.y;
        float x = o.x;
        this->x = x;
        this->y = y;
        this->z = z;
        return *this;
    }

    float x;
    float y;
    float z;
};

class CAIObject {
public:
    float GetDistanceXYZReal(CAIObject*);
    bool IsObjectInLineOfSight(CAIFilterRealPosition&, float, bool);
    void SetTarget(CAIObject*);
    void Init();
    bool UpdateReactionTime();
    void UpdateTargetPosition();
    void WalkNextSplinePathPoint();
    void WalkAStarPath();
    void WalkToCoverPointDirect();
    void WalkToCoverPointIndirect();
    void WalkToCoverPoint();
    void CheckCoverpointUsefulness();
    void WalkToArbitraryPoint();
    void ProcessObstacleAvoidance();

    unsigned char unknown0000[4];
    CAIObject* m_next;
    unsigned char unknown0008[12];
    unsigned char data014;
    unsigned char data015;
    unsigned char unknown0016[2];
    CAIFilterRealPosition m_position;
    unsigned char unknown0024[160];
    CAIObject* m_target;
    unsigned char unknown00c8[236];
    int data1b4;
    int data1b8;
    int data1bc;
    int data1c0;
    int data1c4;
    int data1c8;
    unsigned char unknown01cc[8];
    int data1d4;
    unsigned char unknown01d8[16];
    int data1e8;
    unsigned char unknown01ec[40];
    int data214;
    int m_walkMode;
    unsigned char unknown021c[200];
    bool m_avoidObstacles;
    unsigned char unknown02e5[1099];
    bool m_unknown730;
    unsigned char unknown0731[23];
    CAIFilterRealPosition m_upperLocation;
    unsigned char unknown0754[20];
    float data768;
    float data76c;
};

/* inferred: the AI object's virtual functions as far as ChooseTarget needs
   them (the table order of CAIObject, AIObject_visionseek.cpp); the call at
   +24 is not named */
class CAIObjectVirtuals {
public:
    virtual ~CAIObjectVirtuals();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;
    virtual bool vf18() const;
};

inline CAIObjectVirtuals* Virtuals(CAIObject* object) {
    return (CAIObjectVirtuals*)object;
}

class CAISoldierObject : public CAIObject {
public:
    void SetUpperLocation(CAIFilterRealPosition&);
    CAIObject* ChooseTarget(CAIObject*, float);
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    void Init();
    void Update(float);
    void PreUpdate();

    aifilter_target_mode m_targetMode;
    aistatus_match m_matches[4];
    int m_matchCount;
};

void CAISoldierObject::SetUpperLocation(CAIFilterRealPosition& location) {
    m_upperLocation = location;
}

inline bool IsEnemyTeam(int team, int other) {
    switch (team) {
    case 1:
        switch (other) {
        case 0:
        case 1:
            return false;
        case 2:
            return true;
        }
        break;
    case 2:
        switch (other) {
        case 0:
        case 2:
            return false;
        case 1:
            return true;
        }
        break;
    case 0:
        switch (other) {
        case 0:
            return false;
        case 1:
        case 2:
            return true;
        }
        break;
    }
    return false;
}

CAIObject* CAISoldierObject::ChooseTarget(CAIObject* list, float range) {
    float value;
    CAIObject* best = 0;
    float bestDistance = 0.0f;
    CAIObject* object = m_target;
    if (object) {
        best = object;
        value = GetDistanceXYZReal(object);
        if (Virtuals(object)->vf18() && value < 10.0f)
            value *= 0.5f;
        bestDistance = value - 2.0f;
    }
    for (object = list; object; object = object->m_next) {
        if (object == this || object == best)
            continue;
        if (!IsEnemyTeam(data014, object->data014))
            continue;
        if (object->data1b4 <= 0)
            continue;
        value = range;
        if (object->m_unknown730)
            value *= 0.5f;
        if (Virtuals(object)->IsCrouching())
            value *= 0.75f;
        if (!IsObjectInLineOfSight(object->m_position, value, Virtuals(object)->IsCrouching()))
            continue;
        value = GetDistanceXYZReal(object);
        if (Virtuals(object)->vf18() && value < 10.0f)
            value *= 0.5f;
        if (!best || value < bestDistance) {
            best = object;
            bestDistance = value;
        }
    }
    if (best) {
        SetTarget(best);
        return best;
    }
    SetTarget(0);
    return 0;
}

void CAISoldierObject::SetupTargetMatchList(aifilter_target_mode, aistatus_match* matches, int count) {
    aistatus_match* match = m_matches;

    m_matchCount = count;
    for (int i = 0; i < count; i++, match++, matches++)
        *match = *matches;
}

void CAISoldierObject::ResetTargetMatchList(aifilter_target_mode) {
    switch (m_targetMode) {
    case 2:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 9;
        m_matches[0].field8 = 3;
        m_matches[1].field0 = 3;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 3;
        m_matches[2].field0 = 1;
        m_matches[2].field4 = 5;
        m_matches[2].field8 = 3;
        m_matches[3].field0 = 2;
        m_matches[3].field4 = 5;
        m_matches[3].field8 = 3;
        m_matchCount = 4;
        break;
    case 3:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 9;
        m_matches[0].field8 = 3;
        m_matches[1].field0 = 3;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 3;
        m_matches[2].field0 = 1;
        m_matches[2].field4 = 5;
        m_matches[2].field8 = 3;
        m_matches[3].field0 = 2;
        m_matches[3].field4 = 5;
        m_matches[3].field8 = 3;
        m_matchCount = 4;
        break;
    case 4:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 4;
        m_matches[0].field8 = 2;
        m_matches[1].field0 = 1;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 2;
        m_matchCount = 2;
        break;
    case 5:
        m_matches[0].field0 = 4;
        m_matches[0].field4 = 5;
        m_matches[0].field8 = 3;
        m_matches[1].field0 = 4;
        m_matches[1].field4 = 11;
        m_matches[1].field8 = 2;
        m_matches[2].field0 = 3;
        m_matches[2].field4 = 4;
        m_matches[2].field8 = 3;
        m_matches[3].field0 = 1;
        m_matches[3].field4 = 4;
        m_matches[3].field8 = 3;
        m_matchCount = 4;
        break;
    case 6:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 4;
        m_matches[0].field8 = 2;
        m_matches[1].field0 = 1;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 2;
        m_matchCount = 2;
        break;
    }
}

void CAISoldierObject::Init() {
    CAIObject::Init();
    data1b4 = 15.0f * (data768 / data76c) + 0.999f;
    data1b8 = data015;
    data1bc = data214;
    data1c0 = 4;
    data1c4 = 1;
    data1c8 = data014;
    data1d4 = 0;
    data1e8 = 0;
    m_targetMode = (aifilter_target_mode)3;
    ResetTargetMatchList((aifilter_target_mode)3);
}

void CAISoldierObject::Update(float) {
    if (UpdateReactionTime() && m_target)
        UpdateTargetPosition();
    switch (m_walkMode) {
    case 1:
        WalkNextSplinePathPoint();
        break;
    case 2:
        WalkAStarPath();
        break;
    case 3:
        WalkToCoverPointDirect();
        break;
    case 4:
        WalkToCoverPointIndirect();
        break;
    case 9:
        WalkToCoverPoint();
        break;
    case 5:
        CheckCoverpointUsefulness();
        break;
    case 8:
        WalkToArbitraryPoint();
        break;
    }
    if (m_avoidObstacles)
        ProcessObstacleAvoidance();
    data1b4 = 15.0f * (data768 / data76c) + 0.999f;
    data1c0 = 4;
    data1c4 = 1;
}

void CAISoldierObject::PreUpdate() {
}
