// A fragment of AIObject.cpp (0x800487b0): CAIObject::CheckLineOfSightModeObject
// returns the watched object (+1692) when it is in line of sight: the
// threat (+196) through IsTargetInLineOfSight at the vision distance (+1760;
// its position is then copied to +336 and two flags set, and the flag at
// +1752 cleared when it is not seen), any other object through
// IsObjectInLineOfSight (inlined; the body follows
// AIObject_lineofsight_obj.cpp and is marked inline here so that it is not
// emitted again) at the vision distance, halved for the flag at +1840 of the
// object and times 0.75 when it is crouching. The classes and functions are
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

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;

    CAIObject* CheckLineOfSightModeObject(); /* result type inferred */
    bool IsTargetInLineOfSight(float);
    bool IsObjectInLineOfSight(CAIFilterRealPosition&, float, bool);

    unsigned char unknown004[4];
    CAIFilterObject* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[160];
    CAIObject* m_threat;
    unsigned char unknown0c8[136];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[84];
    unsigned long m_flags;
    unsigned char unknown1b4[1256];
    CAIObject* m_seekObject;
    unsigned char unknown6a0[56];
    bool m_unknown6d8;
    unsigned char unknown6d9[7];
    float m_visionDistance;
    unsigned char unknown6e4[76];
    bool m_unknown730;
    bool m_unknown731;
};

inline bool CAIObject::IsObjectInLineOfSight(CAIFilterRealPosition& position, float distance, bool low) {
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
        float z = position.z;
        y = position.y;
        float x = position.x;
        target.x = x;
        target.y = y;
        target.z = z;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter))
            return true;
    }
    return false;
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
        visible = IsObjectInLineOfSight(object->m_position, distance, object->IsCrouching());
    }
    if (visible)
        return m_seekObject;
    return 0;
}
