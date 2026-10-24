// A fragment of AIObject.cpp (0x800478b8): CAIObject::ShouldILeaveMMG says to
// leave a mounted machine gun when there is a threat (+196), the flag at +1752
// is set and the cosine between the gun point's forward row and the unit
// direction to the threat is below 0.866. CAIObject::ChooseMMGPoint picks the
// nearest free mounted-machine-gun point: of the trigger objects whose core
// is of type 5 with the class CRC 0x5b607813 and whose low flag bit is clear,
// it skips those facing away from the threat (+196; the cosine between the
// point's forward row and the unit direction to the threat below 0.866) and
// those within 5 of another AI object not on the same side (+20, 0 = none),
// and returns the closest within 25. The classes, functions and globals are
// named by the symbols; the trigger core, CAIObject members and the vector and
// matrix views are shared with ShouldILeaveMMG (the forward row's dead 16-byte
// copy is the temporary bound to a const reference); the threat direction is
// assigned component by component (its FPR numbers). The rest of the file is
// in other units.
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

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { d.v[0] = ax; d.v[1] = ay; d.v[2] = az; }
    CVector3(const CVector3& other) { d.pair[0] = other.d.pair[0]; d.pair[1] = other.d.pair[1]; }
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            d.v[0] *= scale;
            d.v[1] *= scale;
            d.v[2] *= scale;
        }
    }
    float Dot(const CVector3& o) const { return d.v[0] * o.d.v[0] + d.v[1] * o.d.v[1] + d.v[2] * o.d.v[2]; }

    CVector3Pair d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    CVector3 GetRow(int i) const { return *(const CVector3*)m[i]; }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

class CAIFilterRealPosition {
public:
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

struct TriggerCoreView {
    unsigned char unknown00[12];
    int m_type;
    float x;
    float y;
    float z;
    CQuaternion rotation;
    unsigned char unknown2c[28];
    unsigned long m_crc;
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
    void* bsObject;
};

extern unsigned int g_pNumTriggers;
extern TriggerObject_struct g_pTriggerObjects[2000];

class CAIObject;
extern CAIObject* g_paioAIObjectListStart; // file-local in the original

class CAIObject {
public:
    bool ShouldILeaveMMG(TriggerObject_struct*);
    TriggerObject_struct* ChooseMMGPoint();

    unsigned char unknown000[4];
    CAIObject* m_next;
    unsigned char unknown008[12];
    unsigned char m_team;
    unsigned char unknown015[3];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[160];
    CAIObject* m_target;
    unsigned char unknown0c8[1552];
    bool m_unknown6d8;
};

bool CAIObject::ShouldILeaveMMG(TriggerObject_struct* trigger) {
    if (m_target && m_unknown6d8) {
        TriggerCoreView* core = trigger->core;
        CAIFilterRealPosition position;
        float x;
        float y;
        float z;
        z = core->z;
        y = core->y;
        x = core->x;
        position.x = x;
        position.y = y;
        position.z = z;
        CMatrix matrix;
        core->rotation.GetMatrix(matrix);
        const CVector3& row = matrix.GetRow(1);
        CVector3 forward(*&row);
        forward.Normalize();
        CVector3 delta;
        delta.d.v[0] = m_target->m_position.x - position.x;
        delta.d.v[1] = m_target->m_position.y - position.y;
        delta.d.v[2] = m_target->m_position.z - position.z;
        delta.Normalize();
        if (delta.Dot(forward) < 0.866f)
            return true;
    }
    return false;
}

TriggerObject_struct* CAIObject::ChooseMMGPoint() {
    TriggerObject_struct* trigger;
    TriggerObject_struct* chosen = 0;
    float best = 25.0f;
    for (trigger = g_pTriggerObjects; trigger != g_pTriggerObjects + g_pNumTriggers; trigger++) {
        /* the tests read the core through the trigger (its register order) */
        if (trigger->core->m_type == 5 && trigger->core->m_crc == 0x5b607813 && !(trigger->flags & 1)) {
            TriggerCoreView* core = trigger->core;
            CAIFilterRealPosition position;
            float y;
            float z = core->z;
            y = core->y;
            float x = core->x;
            position.x = x;
            position.y = y;
            position.z = z;
            CMatrix matrix;
            core->rotation.GetMatrix(matrix);
            const CVector3& row = matrix.GetRow(1);
            CVector3 forward(*&row);
            forward.Normalize();
            if (m_target) {
                CVector3 delta;
                delta.d.v[0] = m_target->m_position.x - position.x;
                delta.d.v[1] = m_target->m_position.y - position.y;
                delta.d.v[2] = m_target->m_position.z - position.z;
                delta.Normalize();
                if (delta.Dot(forward) < 0.866f)
                    continue;
            }
            CAIObject* object;
            bool occupied = false;
            for (object = g_paioAIObjectListStart; object; object = object->m_next) {
                if (object != this && (!object->m_team || object->m_team != m_team) &&
                    position.GetDistanceXYZReal(object->m_position) < 5.0f) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                float distance = m_position.GetDistanceXYZReal(position);
                if (distance < best) {
                    chosen = trigger;
                    best = distance;
                }
            }
        }
    }
    return chosen;
}
