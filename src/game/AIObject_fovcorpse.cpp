// A fragment of AIObject.cpp (0x80049288): CAIObject::CheckFieldOfViewCorpse
// returns the first object in the global list with status variable 13 set
// (a corpse, inferred) that is in the field of view at the corpse vision
// distance (+1764; halved for the flag at +1840 of the object, times 0.75
// when it is crouching) and angle (+1776), or none. The classes and
// functions are named by the symbols; the view follows AIObject_obstacle.cpp
// (the vtable declared up to IsCrouching) and AIObject_globallist.cpp (the
// list head, file-local in the original, is declared extern; the next
// object at +4). The rest of the file is in other units.
class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

enum aistatus_var {};

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;

    CAIObject* CheckFieldOfViewCorpse(); /* result type inferred */
    bool IsObjectInFieldOfView(CAIFilterRealPosition&, float, float, bool);
    int GetAIStatusVar(aistatus_var) const;

    CAIObject* m_next;
    unsigned char unknown008[16];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[1728];
    float m_corpseDistance;
    unsigned char unknown6e8[8];
    float m_corpseAngle;
    unsigned char unknown6f4[60];
    bool m_unknown730;
};

extern CAIObject* g_paioAIObjectListStart; // file-local in the original

CAIObject* CAIObject::CheckFieldOfViewCorpse() {
    for (CAIObject* object = g_paioAIObjectListStart; object; object = object->m_next) {
        if (object->GetAIStatusVar((aistatus_var)13)) {
            float distance;
            float angle;
            angle = m_corpseAngle;
            distance = m_corpseDistance;
            if (object->m_unknown730)
                distance *= 0.5f;
            if (object->IsCrouching())
                distance *= 0.75f;
            if (IsObjectInFieldOfView(object->m_position, distance, angle, object->IsCrouching()))
                return object;
        }
    }
    return 0;
}
