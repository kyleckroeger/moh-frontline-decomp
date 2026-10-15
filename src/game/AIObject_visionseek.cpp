// A fragment of AIObject.cpp (0x80048134): CAIObject::CheckVisionSeekingMode
// makes two tries at seeing the sought position (+1712): each picks random
// offsets (sideways along +1728 by up to 0.25, up by up to 0.75, kept at
// +1744 and +1748) and, when the line from 1.5 above the position to the
// offset point is clear and the sought position is within the vision
// distance (+1760; 50 with a warning when it is not above 0.001), returns
// the sought object (+1692). The classes and functions are named by the
// symbols; the members are inferred from offsets (the sought object, the
// vision distance and the position copy follow AIObject_obstacle.cpp and
// AIObject_storecover.cpp, whose 8-byte aligned CAIFilterRealVector3 the
// raised position also is, per the stack layout), as is the result type. The rest of the file is
// in other units.
extern "C" int printf(const char*, ...);

float MathFunRandomRealSigned(float, float);

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

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static bool CheckVisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                             CAIFilterObject*);
};

class CAIObject {
public:
    CAIObject* CheckVisionSeekingMode(); /* result type inferred */

    unsigned char unknown000[8];
    CAIFilterObject* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[1656];
    CAIObject* m_seekObject;
    unsigned char unknown6a0[16];
    CAIFilterRealPosition m_seekPosition;
    unsigned char unknown6bc[4];
    CAIFilterRealPosition m_seekDirection;
    unsigned char unknown6cc[4];
    float m_seekSideways;
    float m_seekUp;
    unsigned char unknown6d8[8];
    float m_visionDistance;
};

CAIObject* CAIObject::CheckVisionSeekingMode() {
    for (int i = 0; i < 2; i++) {
        CAIFilterRealVector3 eye;
        CAIFilterRealPosition target;
        m_seekSideways = MathFunRandomRealSigned(-0.25f, 0.25f);
        m_seekUp = MathFunRandomRealSigned(-0.75f, 0.75f);
        target.x = m_seekSideways * m_seekDirection.x + m_seekPosition.x;
        target.y = m_seekSideways * m_seekDirection.y + m_seekPosition.y;
        target.z = m_seekUp + m_seekPosition.z;
        float z = m_position.z;
        float y = m_position.y;
        float x = m_position.x;
        eye.x = x;
        eye.y = y;
        eye.z = z;
        eye.z += 1.5f;
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter)) {
            float distance = m_visionDistance;
            if (distance <= 0.001f) {
                printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
                distance = 50.0f;
            }
            if (m_position.GetDistanceXYZReal(m_seekPosition) <= distance)
                return m_seekObject;
        }
    }
    return 0;
}
