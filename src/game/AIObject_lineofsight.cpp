// A fragment of AIObject.cpp (0x80048e98):
// CAIObject::SetupLineOfSightModeObject sets the object to look for (+1692)
// and the vision distance (+1760), warning and using 50 when the distance is
// not above 0.001. The class, function and printf are named by the symbols;
// the members are inferred from offsets. The rest of the file is in other
// units.
extern "C" int printf(const char*, ...);

class CAIObject {
public:
    void SetupLineOfSightModeObject(CAIObject*, float);

    unsigned char unknown000[1692];
    CAIObject* m_lineOfSightObject;
    unsigned char unknown6a0[64];
    float m_visionDistance;
};

void CAIObject::SetupLineOfSightModeObject(CAIObject* object, float distance) {
    m_lineOfSightObject = object;
    if (distance <= 0.001f) {
        printf("Warning: SLOSMO - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_visionDistance = distance;
}
