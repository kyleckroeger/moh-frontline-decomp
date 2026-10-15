// A fragment of AIObject.cpp (0x8004b8fc): CAIObject::WalkAStarPath updates
// the A* path walk, then, when within the stopping distance (+732) of the
// walk's destination (+632; in 3D for walk mode 4, else in XY) less 0.5,
// marks the walk arrived (+652) and fires script event 152 on the object's
// script. The classes and functions are named by the symbols; the members
// are inferred from offsets. The rest of the file is in other units.
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIFilterRealPosition {
public:
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

/* inferred: the AI's game object, its script at +12 */
struct AIGameObjectView {
    unsigned char unknown00[12];
    BSObject* m_script;
};

class CAIObject {
public:
    void WalkAStarPath();
    void UpdateAStarPathWalk();

    unsigned char unknown000[8];
    AIGameObjectView* m_object;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[596];
    CAIFilterRealPosition m_walkDestination;
    unsigned char unknown284[8];
    bool m_walkArrived;
    unsigned char unknown28d[75];
    int m_walkMode;
    float m_walkStopDistance;
};

void CAIObject::WalkAStarPath() {
    UpdateAStarPathWalk();
    float distance;
    if (m_walkMode == 4)
        distance = m_position.GetDistanceXYZReal(m_walkDestination);
    else
        distance = m_position.GetDistanceXYReal(m_walkDestination);
    if (distance - m_walkStopDistance < 0.5f) {
        m_walkArrived = true;
        BSObjectTriggerEvent(m_object->m_script, 152, 0, 0, false);
    }
}
