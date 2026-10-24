// A fragment of AIObject.cpp (0x80048564): CAIObject::CheckLineOfSightModeList,
// CAIObject::CheckLineOfSightModeObject and CAIObject::IsTargetInLineOfSight.
// CheckLineOfSightModeList takes the next object of the global AI object list
// (from the last match at +1688, wrapping at the list start) whose status
// matches the watch list (+1780, count +1828) and is not this object
// (FindListObject, inlined; it remembers the match and gives up after a full
// turn) and returns it when it is in line of sight at the vision distance
// (+1756), halved for the flag at +1840 of the object and times 0.75 when it
// is crouching; otherwise 0 (the two zero results, after a test against the
// previous match, are kept as written). The list start is read through the
// inferred inline GetListStart (the global is file-local in the original;
// the accessor gives its register).
// CheckLineOfSightModeObject returns the watched object (+1692) when it is in
// line of sight: the threat (+196) through IsTargetInLineOfSight at the vision
// distance (+1760; its position is then copied to +336 and two flags set, and
// the flag at +1752 cleared when it is not seen), any other object through
// IsObjectInLineOfSight (inlined; the body follows
// AIObject_lineofsight_obj.cpp and is marked inline here so that it is not
// emitted again) at the vision distance, halved for the flag at +1840 of the
// object and times 0.75 when it is crouching (here it takes the object, whose
// position it reads, rather than the position: the address is not kept in a
// register). IsTargetInLineOfSight (an inline member, weak in the original,
// emitted after its caller) first tries the threat from the eye
// position (+688, when there is an eye source at +656 and the flag at +704),
// otherwise or failing that the inferred inline IsThreatInLineOfSight (the
// same distance scaling for the threat). The classes and functions are
// named by the symbols; the vtable is declared up to IsCrouching as in
// AIObject_obstacle.cpp, and the members follow that unit and
// AIObject_visionseek.cpp. The rest of the file is in other units.
extern "C" int printf(const char*, ...);

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

struct aistatus_match {
    int value[3];
};

class CAIStatus {
public:
    bool MatchStatusList(aistatus_match*, int);
};

class CAIObject;
extern CAIObject* g_paioAIObjectListStart;
static inline CAIObject* GetListStart() { return g_paioAIObjectListStart; }
class CAIObject;
extern CAIObject* g_paioAIObjectListStart; // file-local in the original

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;

    CAIObject* CheckLineOfSightModeList();
    CAIObject* CheckLineOfSightModeObject(); /* result type inferred */
    /* inferred: the next object of the global list matching the watch list */
    CAIObject* FindListObject() {
        CAIObject* first = GetListStart();
        CAIObject* object;
        if (m_listObject)
            object = m_listObject;
        else
            object = first;
        for (;;) {
            object = object->m_next;
            if (!object)
                object = first;
            if (object != this && object->m_status.MatchStatusList(m_list, m_listCount)) {
                m_listObject = object;
                return m_listObject;
            }
            if (m_listObject) {
                if (object == m_listObject)
                    return 0;
            } else if (object == first) {
                return 0;
            }
        }
    }
    bool IsTargetInLineOfSight(float);
    bool IsObjectInLineOfSight(CAIObject*, float, bool);
    bool IsObjectInLineOfSightFromEye(CAIFilterRealPosition&, float, bool);
    bool IsThreatInLineOfSight(float distance) {
        CAIObject* threat = m_threat;
        float range = distance;
        if (threat->m_unknown730)
            range *= 0.5f;
        if (threat->IsCrouching())
            range *= 0.75f;
        return IsObjectInLineOfSight(threat, range, threat->IsCrouching());
    }

    CAIObject* m_next;
    CAIFilterObject* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[160];
    CAIObject* m_threat;
    unsigned char unknown0c8[136];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[84];
    unsigned long m_flags;
    CAIStatus m_status;
    unsigned char unknown1b5[219];
    void* m_eyeSource;
    unsigned char unknown294[28];
    CAIFilterRealPosition m_eyePosition;
    unsigned char unknown2bc[4];
    bool m_hasEye;
    unsigned char unknown2c1[983];
    CAIObject* m_listObject;
    CAIObject* m_seekObject;
    unsigned char unknown6a0[56];
    bool m_unknown6d8;
    unsigned char unknown6d9[3];
    float m_listDistance;
    float m_visionDistance;
    unsigned char unknown6e4[16];
    aistatus_match m_list[4];
    int m_listCount;
    unsigned char unknown728[8];
    bool m_unknown730;
    bool m_unknown731;
};

inline bool CAIObject::IsObjectInLineOfSight(CAIObject* other, float distance, bool low) {
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(other->m_position) <= range))
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
        float z = other->m_position.z;
        y = other->m_position.y;
        float x = other->m_position.x;
        target.x = x;
        target.y = y;
        target.z = z;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter))
            return true;
    }
    return false;
}

inline bool CAIObject::IsObjectInLineOfSightFromEye(CAIFilterRealPosition& position, float distance, bool low) {
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(position) <= range))
        return false;
    float standing[2] = {0.8f, 1.2f};
    float crouching[2] = {0.8f, 0.1f};
    int count = 2;
    float* heights = standing;
    if (low) {
        count = 2;
        heights = crouching;
    }
    for (int i = 0; i < count; i++) {
        CAIFilterRealVector3 target;
        float y;
        float z = position.z;
        y = position.y;
        float x = position.x;
        target.x = x;
        target.y = y;
        target.z = z;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(m_eyePosition, target, m_filter))
            return true;
    }
    return false;
}

CAIObject* CAIObject::CheckLineOfSightModeList() {
    CAIObject* start = m_listObject;
    CAIObject* object = FindListObject();
    if (!object)
        return 0;
    float distance = m_listDistance;
    if (object->m_unknown730)
        distance *= 0.5f;
    if (object->IsCrouching())
        distance *= 0.75f;
    CAIObject* result;
    if (IsObjectInLineOfSight(object, distance, object->IsCrouching()))
        result = object;
    else if (object == start)
        result = 0;
    else
        result = 0;
    return result;
}

CAIObject* CAIObject::CheckLineOfSightModeObject() {
    CAIObject* object = m_seekObject;
    if (!object)
        return 0;
    bool visible;
    if (object == m_threat) {
        visible = IsTargetInLineOfSight(m_visionDistance);
        if (visible) {
            float z = m_seekObject->m_position.z;
            float y = m_seekObject->m_position.y;
            float x = m_seekObject->m_position.x;
            m_unknown150.x = x;
            m_unknown150.y = y;
            m_unknown150.z = z;
            m_unknown6d8 = 1;
            m_unknown731 = 1;
        } else {
            m_unknown6d8 = 0;
        }
    } else {
        float distance = m_visionDistance;
        if (object->m_unknown730)
            distance *= 0.5f;
        if (object->IsCrouching())
            distance *= 0.75f;
        visible = IsObjectInLineOfSight(object, distance, object->IsCrouching());
    }
    if (visible)
        return m_seekObject;
    return 0;
}

inline bool CAIObject::IsTargetInLineOfSight(float distance) {
    if (m_eyeSource && m_hasEye) {
        bool visible = IsObjectInLineOfSightFromEye(m_threat->m_position, distance, m_threat->IsCrouching());
        return visible || IsThreatInLineOfSight(distance);
    }
    return IsThreatInLineOfSight(distance);
}
