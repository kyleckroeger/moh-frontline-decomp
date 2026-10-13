// A fragment of AIObject.cpp (0x80049bfc): CAIObject::SetupFieldOfViewCorpse
// (the corpse vision distance at +1764 and angle at +1776, same defaults and
// warnings as below), CAIObject::SetupFieldOfViewModeList
// (copies the match entries, 12 bytes each, to +1780 and their count to
// +1828, clears the watched object at +1688, then the same distance and angle
// defaults as below into +1756 and +1772), CAIObject::SetupFieldOfViewModeObject
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
extern "C" void* memcpy(void*, const void*, unsigned long);

/* Named by the mangled symbol; the 12-byte size comes from the copy, the
   member layout and the array bound below are inferred. */
struct aistatus_match {
    int value[3];
};

class CAIObject {
public:
    void SetupFieldOfViewCorpse(float, float);
    void SetupFieldOfViewModeList(aistatus_match*, int, float, float);
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
    unsigned char unknown694[4];
    CAIObject* m_listObject;
    CAIObject* m_fovObject;
    unsigned char unknown6a0[60];
    float m_listDistance;
    float m_visionDistance;
    float m_corpseDistance;
    float m_fovAngle;
    float m_listAngle;
    float m_corpseAngle;
    aistatus_match m_list[4];
    int m_listCount;
};

void CAIObject::SetupFieldOfViewCorpse(float distance, float angle) {
    if (distance <= 0.001f) {
        printf("Warning: SFOVC - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_corpseDistance = distance;
    if (angle <= 0.001f) {
        printf("Warning: FOV is zero, changing to default\n");
        angle = 90.0f;
    }
    m_corpseAngle = 0.017453292f * angle;
}

void CAIObject::SetupFieldOfViewModeList(aistatus_match* list, int count, float distance, float angle) {
    memcpy(m_list, list, count * sizeof(aistatus_match));
    m_listCount = count;
    m_listObject = 0;
    if (distance <= 0.001f) {
        printf("Warning: SFOVML - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_listDistance = distance;
    if (angle <= 0.001f)
        angle = 90.0f;
    m_listAngle = 0.017453292f * angle;
}

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
