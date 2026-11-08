// A fragment of static_obj.cpp (0x800af468): CStaticObject::ScaleFromScript
// starts a scripted value change: the script-value flag is set, the event to
// fire is kept, and the target value and the time divided by it are stored.
// The class, function and EBSEventEnum are named by the symbols; the members
// follow static_obj_attempt.cpp (their meanings inferred). The rest of the
// file is in other units.
enum EBSEventEnum {};

/* Inferred: four floats overlaid with two doubles (copied as lfd/stfd pairs). */
class CVector3 {
public:
    union {
        float v[4];
        double pair[2];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void BuildRotX(float);
    void RotateY(float);
    void RotateZ(float);
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);

    float x;
    float y;
    float z;
    float w;
};

/* Inferred: the smaller of two times, ignoring zero (a zero second time
   keeps the first, a non-positive first time gives the second). */
inline void KeepSmaller(float& a, float b) {
    bool first = false;
    if ((a < b && a > 0.0f) || b == 0.0f)
        first = true;
    if (first)
        a = a;
    else
        a = b;
}

inline void SmallestRatio(const CVector3& a, const CVector3& b, float& tx) {
    float tz, ty, q, p;
    tx = 0.0f;
    ty = 0.0f;
    tz = 0.0f;
    if (a.d.v[0] != 0.0f && b.d.v[0] != 0.0f)
        tx = b.d.v[0] / a.d.v[0];
    if ((p = a.d.v[1]) != 0.0f && (q = b.d.v[1]) != 0.0f)
        ty = q / p;
    if ((p = a.d.v[2]) != 0.0f && (q = b.d.v[2]) != 0.0f)
        tz = q / p;
    KeepSmaller(tx, ty);
    KeepSmaller(tx, tz);
}
class CStaticObject {
public:
    void ScaleFromScript(float, float, EBSEventEnum);
    void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);

    unsigned char unknown000[400];
    CQuaternion m_scriptTurn;
    unsigned char unknown1a0[16];
    float m_value1b0;
    unsigned char unknown1b4[8];
    float m_time1bc;
    float m_time1c0;
    unsigned char unknown1c4[4];
    int m_turnEvent;
    int m_valueEvent;
    unsigned char unknown1d0[16];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_flags1e0 : 4;
};

void CStaticObject::ScaleFromScript(float value, float time, EBSEventEnum event) {
    m_scriptValue = true;
    m_valueEvent = event;
    m_value1b0 = value;
    m_time1c0 = time / value;
}

void CStaticObject::RotateFromScript(CVector3& angles, CVector3& rates, EBSEventEnum event) {
    CVector3 a = angles;
    CVector3 b = rates;
    a.d.v[0] = -a.d.v[0];
    a.d.v[1] = -a.d.v[1];
    a.d.v[2] = -a.d.v[2];
    b.d.v[0] = -b.d.v[0];
    b.d.v[1] = -b.d.v[1];
    b.d.v[2] = -b.d.v[2];
    m_scriptRotate = true;
    m_turnEvent = event;
    if (b.d.v[0] == 0.0f && b.d.v[1] == 0.0f && b.d.v[2] == 0.0f) {
        m_time1bc = 3.4028235e38f;
    } else {
        float t;
        SmallestRatio(a, b, t);
        m_time1bc = t;
    }
    CMatrix m;
    m.BuildRotX(a.d.v[0]);
    m.RotateY(a.d.v[1]);
    m.RotateZ(a.d.v[2]);
    m_scriptTurn.SetFromMatrix(m);
}
