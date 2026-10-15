// A fragment of AIObject.cpp (0x80048270): CAIObject::SetupVisionSeekingMode
// stores the sought object and position (+1692, +1712) and the vision
// distance (+1760; 50 with a warning when it is not above 0.001), then the
// seeking direction (+1728): the flat direction from 1.5 above the object's
// position to the sought position, turned a quarter turn and normalised; the
// random offsets (+1744, +1748) start at -0.25 and -0.75. The classes and
// functions are named by the symbols; the members follow
// AIObject_visionseek.cpp, the position's assignment follows AIObject.cpp and
// the raised position's type follows AIObject_storecover.cpp; its empty
// constructor and destructor are inferred (the code keeps the raised position
// on the stack, which an object with a destructor is). The rest of the file
// is in other units.
extern "C" {
#include <math.h>
int printf(const char*, ...);
}

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& other) {
        float x_, y_, z_;
        z_ = other.z;
        y_ = other.y;
        x_ = other.x;
        x = x_;
        y = y_;
        z = z_;
        return *this;
    }

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3() {}
    ~CAIFilterRealVector3() {}

    float w;
} __attribute__((aligned(8)));

class CAIObject {
public:
    void SetupVisionSeekingMode(CAIObject*, CAIFilterRealPosition&, float);

    unsigned char unknown000[24];
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

void CAIObject::SetupVisionSeekingMode(CAIObject* object, CAIFilterRealPosition& position, float distance) {
    m_seekPosition = position;
    m_seekObject = object;
    if (distance <= 0.001f) {
        printf("Warning: SVSM - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_visionDistance = distance;
    CAIFilterRealVector3 eye;
    float z = m_position.z;
    float y = m_position.y;
    float x = m_position.x;
    eye.x = x;
    eye.y = y;
    eye.z = z;
    eye.z += 1.5f;
    m_seekDirection.x = position.x - eye.x;
    m_seekDirection.y = position.y - eye.y;
    m_seekDirection.z = position.z - eye.z;
    m_seekDirection.z = 0.0f;
    float sideways = m_seekDirection.x;
    m_seekDirection.x = m_seekDirection.y;
    m_seekDirection.y = -sideways;
    float length = sqrtf(m_seekDirection.x * m_seekDirection.x + m_seekDirection.y * m_seekDirection.y +
                         m_seekDirection.z * m_seekDirection.z);
    if (length != 0.0f) {
        m_seekDirection.x *= 1.0f / length;
        m_seekDirection.y *= 1.0f / length;
        m_seekDirection.z *= 1.0f / length;
    }
    m_seekSideways = -0.25f;
    m_seekUp = -0.75f;
}
