// A fragment of AIObject.cpp (0x80048e10):
// CAIObject::SetupLineOfSightModeList copies a list of 12-byte status
// matches (+1780) and its count (+1828), clears a word at +1688 and sets the
// list's vision distance (+1756); SetupLineOfSightModeObject sets the object
// to look for (+1692) and its vision distance (+1760). Both warn and use 50
// when the distance is not above 0.001. CAIObject::IsObjectInFieldOfView
// defaults the angle to pi/2 and the distance to 50 (with a warning), then
// works as IsObjectInLineOfSight (AIObject_lineofsight_obj.cpp; the same
// distance warning and the height tables) with a field-of-view test after the
// eye position: the unit direction from the eye to the position raised 0.8
// against the filter's normalised look direction (the first vector of its
// virtual slot 0x20) must not be below the cosine of the angle. The class,
// functions and printf are named by the symbols; the members, the filter's
// slot and the vector views are inferred. The rest of the file is in other
// units.
extern "C" int printf(const char*, ...);
extern "C" void* memcpy(void*, const void*, unsigned long);

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

extern "C" double cos(double);

class CAIFilterRealPosition {
public:
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(const CAIFilterRealVector3& o) { *(CVector3Pair*)this = *(const CVector3Pair*)&o; }
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    float Dot(const CAIFilterRealVector3& v) const { return x * v.x + y * v.y + z * v.z; }

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
    unsigned char unknown00[12];
};

class CAIObject {
public:
    void SetupLineOfSightModeList(aistatus_match*, int, float);
    void SetupLineOfSightModeObject(CAIObject*, float);
    bool IsObjectInFieldOfView(CAIFilterRealPosition&, float, float, bool);

    unsigned char unknown000[8];
    CAIFilterObject* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[396];
    unsigned long m_flags;
    unsigned char unknown1b4[1252];
    int m_value698;
    CAIObject* m_lineOfSightObject;
    unsigned char unknown6a0[60];
    float m_listVisionDistance;
    float m_visionDistance;
    unsigned char unknown6e4[16];
    aistatus_match m_matches[4];
    int m_matchCount;
};

void CAIObject::SetupLineOfSightModeList(aistatus_match* matches, int count, float distance) {
    memcpy(m_matches, matches, count * sizeof(aistatus_match));
    m_matchCount = count;
    m_value698 = 0;
    if (distance <= 0.001f) {
        printf("Warning: SLOSML - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_listVisionDistance = distance;
}

void CAIObject::SetupLineOfSightModeObject(CAIObject* object, float distance) {
    m_lineOfSightObject = object;
    if (distance <= 0.001f) {
        printf("Warning: SLOSMO - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_visionDistance = distance;
}

bool CAIObject::IsObjectInFieldOfView(CAIFilterRealPosition& position, float distance, float angle, bool low) {
    if (angle <= 0.001f)
        angle = 1.5707964f;
    if (distance <= 0.001f) {
        printf("Warning: IOIFOV - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(position) <= range))
        return false;
    CAIFilterRealVector3 look;
    CAIFilterRealVector3 eye;
    m_filter->unknown020(look, eye);
    CAIFilterRealVector3 target;
    float z = position.z;
    float y = position.y;
    float x = position.x;
    target.x = x;
    target.y = y;
    target.z = z;
    target.z += 0.8f;
    CAIFilterRealVector3 direction(target);
    direction.x -= eye.x;
    direction.y -= eye.y;
    direction.z -= eye.z;
    direction.Normalize();
    look.Normalize();
    float dot = direction.x * look.x + direction.y * look.y + direction.z * look.z;
    if (dot < (float)cos(angle))
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
        float tz = position.z;
        float ty = position.y;
        float tx = position.x;
        target.x = tx;
        target.y = ty;
        target.z = tz;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter))
            return true;
    }
    return false;
}
