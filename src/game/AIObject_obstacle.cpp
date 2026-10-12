// A fragment of AIObject.cpp (0x80049d4c): CAIObject::SetupFieldOfViewModeObject
// (the watched object, the vision distance - 50 with a warning when it is not
// above 0.001 - and the angle, 90 by default, in radians; the constants and
// the message are pool entries), then CAIObject::ProcessObstacleAvoidance
// and EngageObstacleAvoidance, driving the object's CObstacleAvoidance (+744)
// from a position at +80. The members are inferred from offsets in a
// non-virtual view; the position copied to +1616, the objective position at
// +1672, the engaged flag at +740 and the squared distance at +1660 are named
// by offset, as are the field-of-view members (+1692, +1760, +1768).
class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

class CObstacleAvoidance {
public:
    void RunEnvironmentCollisionChecks();
    void DetermineNewObjectivePosition(CAIFilterRealPosition&);
    void PrepareForAvoidance(CAIFilterRealPosition&);
};

extern "C" int printf(const char*, ...);

class CAIObject {
public:
    void SetupFieldOfViewModeObject(CAIObject*, float, float);
    void ProcessObstacleAvoidance();
    void EngageObstacleAvoidance(bool, float, float, float);

    unsigned char unknown000[80];
    CAIFilterRealPosition m_unknown050;
    unsigned char unknown05c[648];
    bool m_unknown2e4;
    unsigned char unknown2e5[3];
    CObstacleAvoidance m_obstacleAvoidance;
    unsigned char unknown2e9[871];
    CAIFilterRealPosition m_unknown650;
    unsigned char unknown65c[32];
    float m_unknown67c;
    unsigned char unknown680[8];
    CAIFilterRealPosition m_unknown688;
    unsigned char unknown694[8];
    CAIObject* m_fovObject;
    unsigned char unknown6a0[64];
    float m_visionDistance;
    unsigned char unknown6e4[4];
    float m_fovAngle;
};

void CAIObject::SetupFieldOfViewModeObject(CAIObject* object, float distance, float angle) {
    m_fovObject = object;
    if (distance <= 0.001f) {
        printf("Warning: SFOVMO - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_visionDistance = distance;
    if (angle <= 0.001f)
        angle = 90.0f;
    m_fovAngle = 0.017453292f * angle;
}

void CAIObject::ProcessObstacleAvoidance() {
    float z = m_unknown050.z;
    float y = m_unknown050.y;
    float x = m_unknown050.x;
    m_unknown650.x = x;
    m_unknown650.y = y;
    m_unknown650.z = z;
    m_obstacleAvoidance.RunEnvironmentCollisionChecks();
    m_obstacleAvoidance.DetermineNewObjectivePosition(m_unknown688);
}

void CAIObject::EngageObstacleAvoidance(bool engage, float, float, float distance) {
    m_unknown2e4 = engage;
    if (m_unknown2e4) {
        m_obstacleAvoidance.PrepareForAvoidance(m_unknown050);
        m_unknown67c = distance * distance;
    }
}
