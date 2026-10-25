// A fragment of AIObject.cpp (0x80049288): CAIObject::CheckFieldOfViewCorpse
// and the weak copies of the inline CAIObject::GetAIStatusVar (its first
// caller is CheckFieldOfViewCorpse) and CAIStatus::GetVarValue. GetVarValue
// switches over the status variables: stored values, or the
// CAIObject::ComputeStatus* of the attacker against the target (statics of
// CAIStatus). Those functions and IsObjectInLineOfSight are defined here
// because the weak copies inline them; their own copies are placed by other
// units of this file (file duplicates). AIObject.cpp is compiled with
// deferred inlining (see AIObject_ctor), and CodeWarrior inlines at most
// three levels deep: GetAIStatusVar calls IsObjectInLineOfSight while
// GetVarValue inlines it, so ComputeStatusLOSValue reaches it through one more
// inline (CanSee, name and shape inferred). CheckFieldOfViewCorpse returns
// the first object in the global list with status variable 13 set (a corpse,
// inferred) that is in the field of view at the corpse vision distance
// (+1764; halved for the flag at +1840 of the object, times 0.75 when it is
// crouching) and angle (+1776), or none. The classes and functions are named
// by the symbols; the views follow AIObject_obstacle.cpp,
// AIObject_globallist.cpp and AIObject_lineofsight_obj.cpp (the list head,
// file-local in the original, is declared extern; the next object at +4).
extern "C" int printf(const char*, ...);

class CAIFilterRealPosition {
public:
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
} __attribute__((aligned(8)));

class CAIFilterObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020(CAIFilterRealVector3&, CAIFilterRealVector3&);
};

class CAIFilterGlobal {
public:
    static bool CheckVisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                             CAIFilterObject*);
};

enum aistatus_var {};

class CAIObject;

class CAIStatus {
public:
    int GetVarValue(aistatus_var) const;

    int m_values[14];

    static CAIObject* s_paioAttacker;
    static CAIObject* s_paioTarget;
};

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;

    bool IsObjectInLineOfSight(CAIFilterRealPosition&, float, bool);
    CAIObject* CheckFieldOfViewCorpse(); /* result type inferred */
    bool IsObjectInFieldOfView(CAIFilterRealPosition&, float, float, bool);
    int GetAIStatusVar(aistatus_var) const;
    // inferred wrapper (see the file comment)
    bool CanSee(CAIObject* other, float distance) { return IsObjectInLineOfSight(other->m_position, distance, other->IsCrouching()); }
    int ComputeStatusIsEnemy(CAIObject*);
    int ComputeStatusLOSValue(CAIObject*);
    int ComputeStatusThreatValue(CAIObject*);
    int ComputeStatusExponentialDistance(CAIObject*);
    int ComputeStatusDistance(CAIObject*);
    int ComputeStatusVisibility(CAIObject*);

    CAIObject* m_next;
    CAIFilterObject* m_filter;
    unsigned char unknown00c[8];
    unsigned char m_team;
    unsigned char unknown015[3];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[396];
    unsigned long m_flags;
    CAIStatus m_status;
    unsigned char unknown1ec[28];
    float m_sightDistance;
    unsigned char unknown20c[1240];
    float m_corpseDistance;
    unsigned char unknown6e8[8];
    float m_corpseAngle;
    unsigned char unknown6f4[60];
    bool m_unknown730;
};

inline int CAIStatus::GetVarValue(aistatus_var var) const {
    int value = 0;
    switch (var) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
    case 13:
        value = m_values[var];
        break;
    case 6:
        value = s_paioAttacker->ComputeStatusVisibility(s_paioTarget);
        break;
    case 7:
        value = s_paioAttacker->ComputeStatusDistance(s_paioTarget);
        break;
    case 9:
        value = s_paioAttacker->ComputeStatusThreatValue(s_paioTarget);
        break;
    case 10:
        value = s_paioAttacker->ComputeStatusLOSValue(s_paioTarget);
        break;
    case 11:
        value = s_paioAttacker->ComputeStatusExponentialDistance(s_paioTarget);
        break;
    case 12:
        value = s_paioAttacker->ComputeStatusIsEnemy(s_paioTarget);
        break;
    }
    return value;
}

bool CAIObject::IsObjectInLineOfSight(CAIFilterRealPosition& position, float distance, bool low) {
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(position) <= range))
        return false;
    CAIFilterRealVector3 unused;
    CAIFilterRealVector3 eye;
    m_filter->unknown020(unused, eye);
    float standing[2] = {0.8f, 1.2f};
    float crouching[2] = {0.8f, 0.1f};
    int count = 2;
    float* heights = standing;
    if (low) {
        count = 2;
        heights = crouching;
    }
    for (int i = 0; i < count; i++) {
        CAIFilterRealPosition target;
        float y;
        float z;
        float x;
        z = position.z;
        y = position.y;
        x = position.x;
        target.x = x;
        target.y = y;
        target.z = z;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter))
            return true;
    }
    return false;
}

static inline bool AreEnemyTeams(unsigned char team, unsigned char other) {
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

int CAIObject::ComputeStatusIsEnemy(CAIObject* other) {
    return AreEnemyTeams(m_team, other->m_team) ? 15 : 0;
}

int CAIObject::ComputeStatusLOSValue(CAIObject* other) {
    float distance = m_sightDistance;
    if (other->m_unknown730)
        distance *= 0.5f;
    if (other->IsCrouching())
        distance *= 0.75f;
    return CanSee(other, distance) ? 15 : 0;
}

int CAIObject::ComputeStatusThreatValue(CAIObject* other) {
    int value = other->GetAIStatusVar((aistatus_var)3);
    int distance = ComputeStatusExponentialDistance(other);
    int count = other->GetAIStatusVar((aistatus_var)8);
    int threat = value + (15 - distance) * 2;
    threat /= count + 3;
    if (threat < 0)
        threat = 0;
    else if (threat > 15)
        threat = 15;
    return threat;
}

int CAIObject::ComputeStatusDistance(CAIObject* other) {
    float distance = 0.1f * m_position.GetDistanceXYReal(other->m_position);
    if (distance >= 15.0f)
        return 15;
    return distance;
}

int CAIObject::ComputeStatusVisibility(CAIObject*) {
    return 0;
}


inline int CAIObject::GetAIStatusVar(aistatus_var var) const {
    return m_status.GetVarValue(var);
}

extern CAIObject* g_paioAIObjectListStart; // file-local in the original

CAIObject* CAIObject::CheckFieldOfViewCorpse() {
    for (CAIObject* object = g_paioAIObjectListStart; object; object = object->m_next) {
        if (object->GetAIStatusVar((aistatus_var)13)) {
            float distance;
            float angle;
            angle = m_corpseAngle;
            distance = m_corpseDistance;
            if (object->m_unknown730)
                distance *= 0.5f;
            if (object->IsCrouching())
                distance *= 0.75f;
            if (IsObjectInFieldOfView(object->m_position, distance, angle, object->IsCrouching()))
                return object;
        }
    }
    return 0;
}
