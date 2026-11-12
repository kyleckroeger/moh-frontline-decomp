// A fragment of static_obj.cpp (0x800af260-0x800af7c0): CStaticObject::SetMovementPath,
// ScaleFromScript, RotateFromScript and TranslateFromScript.
// SetMovementPath (a script call) picks a spline path by id and starts the
// traversal (optionally also the look traversal and following the player),
// or stops/finishes the movement for the negative ids -1/-2. Class and
// function names are from the symbols; the member meanings are inferred.
// ScaleFromScript starts a scripted value change: the script-value flag is
// set, the event to fire is kept, and the target value and the time divided
// by it are stored. RotateFromScript and TranslateFromScript start a scripted
// rotation / translation: flag, event, the time until the first axis arrives
// (CalculateMovementTime, an inline member as in Rising Sun's
// StaticObjectTranslate.cpp; FLT_MAX when every rate is zero), and the
// rotation target as a quaternion / the velocity. The rest of the file is in
// other units.
/* Inferred view: copied as two doubles through a union (the velocity
   store loads both halves before storing). */
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class BSObject;
class CAISplinePath;
class CAIFilterObject;
class CPlayerObject;

class CAIFilterGlobal {
public:
    CAISplinePath* GetSplinePath(unsigned long, bool) const;

    unsigned char unknown00[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;
// The bool reaches GetSplinePath through a reference, so it is re-masked
// (clrlwi) before the call.
static inline CAISplinePath* FindSplinePath(unsigned long id, const bool& reverse) {
    return g_aigAIFilterGlobalObject.GetSplinePath(id, reverse);
}

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown00[16];
};

extern CScene g_scene;

class CAISplinePathTraversal {
public:
    void PrepareForTraversalForward(CAISplinePath*, bool, CAIFilterObject*);

    int m_state;
    unsigned char unknown04[12];
    CAISplinePath* m_path;
    unsigned char unknown14[20];
};

enum EBSEventEnum {};

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

class CStaticObject {
public:
    void SetMovementPath(int, bool, bool, float, float, float, float, BSObject*, float, bool);
    void ScaleFromScript(float, float, EBSEventEnum);
    void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    float CalculateMovementTime(const CVector3&, const CVector3&);

    unsigned char unknown000[136];
    CAISplinePathTraversal m_lookTraversal;
    CAISplinePathTraversal m_traversal;
    float m_speed;
    float m_maxSpeed;
    float m_acceleration;
    float m_deceleration;
    unsigned char unknown0e8[4];
    CPlayerObject* m_followPlayer;
    float m_followDistanceSquared;
    float m_unknownf4;
    float m_unknownf8;
    float m_unknownfc;
    float m_unknown100;
    int m_movementState;
    unsigned char unknown108[88];
    CVector3 m_translateVelocity;
    unsigned char unknown170[32];
    CQuaternion m_scriptTurn;
    unsigned char unknown1a0[16];
    float m_value1b0;
    unsigned char unknown1b4[4];
    float m_translateTime;
    float m_rotateTime;
    float m_time1c0;
    int m_translateEvent;
    int m_turnEvent;
    int m_valueEvent;
    unsigned char unknown1d0[16];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_flags1e0 : 4;
};

void CStaticObject::SetMovementPath(int path, bool flag, bool follow, float maxSpeed, float acceleration,
                                    float f3, float f4, BSObject*, float followDistance, bool keepSpeed) {
    if (path > 0) {
        CAISplinePath* spline = FindSplinePath(path, flag);
        if (spline != m_traversal.m_path) {
            m_traversal.PrepareForTraversalForward(spline, false, 0);
            if (follow) {
                m_lookTraversal.PrepareForTraversalForward(spline, false, 0);
                m_lookTraversal.m_state = 3;
            }
        } else if (m_traversal.m_state == 0 || m_movementState == 0) {
            return;
        }
        if (!keepSpeed)
            m_speed = 0.0f;
        m_maxSpeed = maxSpeed;
        m_acceleration = maxSpeed;
        m_deceleration = acceleration;
        m_unknownf4 = 0.0f;
        m_unknownf8 = 0.0f;
        m_unknownfc = f3;
        m_unknown100 = f4;
        if (follow) {
            m_movementState = 3;
            m_followPlayer = g_scene.GetPlayer(0);
            m_followDistanceSquared = followDistance * followDistance;
        } else {
            m_movementState = 2;
        }
    } else if (path == -1) {
        if (m_movementState == 0)
            m_traversal.m_state = 0;
        else
            m_movementState = 6;
    } else if (path == -2) {
        m_speed = 0.0f;
        m_acceleration = 0.0f;
        m_movementState = 0;
        m_traversal.m_state = 0;
    } else {
        if (m_movementState == 0)
            m_traversal.m_state = 0;
        else
            m_movementState = 5;
    }
}

void CStaticObject::ScaleFromScript(float value, float time, EBSEventEnum event) {
    m_scriptValue = true;
    m_valueEvent = event;
    m_value1b0 = value;
    m_time1c0 = time / value;
}

// Seconds until the first axis with a non-zero rate and distance arrives
// (zero times are ignored).
inline float CStaticObject::CalculateMovementTime(const CVector3& rate, const CVector3& distance) {
    float tx = 0.0f;
    float ty = 0.0f;
    float tz = 0.0f;
    if (rate.x != 0.0f && distance.x != 0.0f)
        tx = distance.x / rate.x;
    if (rate.y != 0.0f && distance.y != 0.0f)
        ty = distance.y / rate.y;
    if (rate.z != 0.0f && distance.z != 0.0f)
        tz = distance.z / rate.z;
    float t = ((tx < ty && tx > 0.0f) || ty == 0.0f) ? tx : ty;
    return ((t < tz && t > 0.0f) || tz == 0.0f) ? t : tz;
}

void CStaticObject::RotateFromScript(CVector3& angles, CVector3& rates, EBSEventEnum event) {
    CVector3 a = angles;
    CVector3 b = rates;
    a.x = -a.x;
    a.y = -a.y;
    a.z = -a.z;
    b.x = -b.x;
    b.y = -b.y;
    b.z = -b.z;
    m_scriptRotate = true;
    m_turnEvent = event;
    if (b.x == 0.0f && b.y == 0.0f && b.z == 0.0f) {
        m_rotateTime = 3.4028235e38f;
    } else {
        m_rotateTime = CalculateMovementTime(a, b);
    }
    CMatrix m;
    m.BuildRotX(a.x);
    m.RotateY(a.y);
    m.RotateZ(a.z);
    m_scriptTurn.SetFromMatrix(m);
}

void CStaticObject::TranslateFromScript(CVector3& velocity, CVector3& distance, EBSEventEnum event) {
    m_scriptMove = 1;
    m_translateEvent = event;
    if (distance.x == 0.0f && distance.y == 0.0f && distance.z == 0.0f) {
        m_translateTime = 3.4028235e38f;
    } else {
        m_translateTime = CalculateMovementTime(velocity, distance);
    }
    m_translateVelocity = velocity;
}
