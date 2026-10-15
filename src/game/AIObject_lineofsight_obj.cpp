// A fragment of AIObject.cpp (0x80048408): CAIObject::IsObjectInLineOfSight.
// Unless the flag 8 of +432 is set, a position within the vision distance
// (50 with a warning when it is not above 0.001) is in line of sight when
// the line from the filter's eye position to it, raised by either of two
// heights (0.8 or 1.2 standing, 0.8 or 0.1 when the flag is set), is clear.
// The classes and functions are named by the symbols; the filter's virtual
// function at slot 0x20 (a placeholder named by offset, giving two
// positions, the second used as the eye), the members, the height tables'
// meaning and the parameter meanings are inferred; the position types follow
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
    bool IsObjectInLineOfSight(CAIFilterRealPosition&, float, bool);

    unsigned char unknown000[8];
    CAIFilterObject* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[396];
    unsigned long m_flags;
};

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
        float z = position.z;
        float y = position.y;
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
