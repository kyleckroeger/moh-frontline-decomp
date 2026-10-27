// A fragment of static_obj.cpp (0x800af260-0x800af48c): CStaticObject::SetMovementPath
// and CStaticObject::ScaleFromScript.
// SetMovementPath (a script call) picks a spline path by id and starts the
// traversal (optionally also the look traversal and following the player),
// or stops/finishes the movement for the negative ids -1/-2. Class and
// function names are from the symbols; the member meanings are inferred.
// ScaleFromScript starts a scripted value change: the script-value flag is
// set, the event to fire is kept, and the target value and the time divided
// by it are stored. The rest of the file is in other units.
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

class CStaticObject {
public:
    void SetMovementPath(int, bool, bool, float, float, float, float, BSObject*, float, bool);
    void ScaleFromScript(float, float, EBSEventEnum);

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
    unsigned char unknown108[168];
    float m_value1b0;
    unsigned char unknown1b4[12];
    float m_time1c0;
    unsigned char unknown1c4[8];
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
