// A fragment of AIObject.cpp (0x80048e10):
// CAIObject::SetupLineOfSightModeList copies a list of 12-byte status
// matches (+1780) and its count (+1828), clears a word at +1688 and sets the
// list's vision distance (+1756); SetupLineOfSightModeObject sets the object
// to look for (+1692) and its vision distance (+1760). Both warn and use 50
// when the distance is not above 0.001. The class, function and printf are named by the symbols;
// the members are inferred from offsets. The rest of the file is in other
// units.
extern "C" int printf(const char*, ...);
extern "C" void* memcpy(void*, const void*, unsigned long);

struct aistatus_match {
    unsigned char unknown00[12];
};

class CAIObject {
public:
    void SetupLineOfSightModeList(aistatus_match*, int, float);
    void SetupLineOfSightModeObject(CAIObject*, float);

    unsigned char unknown000[1688];
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
