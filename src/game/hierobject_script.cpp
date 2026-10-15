// A fragment of hierobject.cpp (0x8009ad28): CHierObject's script motion:
// CommitScriptMotionUpdate (a scripted move uses up its time, never below
// zero, and advances the position by the step; a scripted turn makes the
// next rotation current and uses up its time), AttemptScriptMotionUpdate
// (the turn step is the remaining turn time capped by the frame time; the
// next rotation is the scripted rotation, interpolated from identity when
// the step is under one, applied to the current one; the matrix is rebuilt
// from it, the move step times the velocity (turned into the matrix's
// frame) and the position, multiplied by the two local matrices, and a
// listener is told) and BeginScriptMotionUpdate (a finished move or turn
// fires its script event with the event data and stops; the current
// rotation is normalised while turning). The classes and functions are named
// by the mangled symbols; the members, flag names, event fields and listener
// view are inferred, CHierObject declares placeholder virtuals up to
// TriggerScriptEvent (+100), and the CVector3 view copies through a double
// pair as in player_weapon.cpp. The rest of the file is not part of this
// unit.
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
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix;

class CQuaternion {
public:
    void Slerp(CQuaternion, CQuaternion, float);
    CQuaternion() {}
    CQuaternion(float ax, float ay, float az, float aw) : w(aw), x(ax), y(ay), z(az) {}
    void Normalize();
    void Set(float, float, float, float);
    void Rotate(CVector3&, CVector3);
    void Multiply(CQuaternion);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;
    void SetPos(CVector3);
    void Translate(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);

    float m[4][4];
};

/* inferred: the object told of the new matrix through its slot +16 */
class MatrixListenerView {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void Update(void*, const CMatrix&);
};

/* inferred: a motion key (32 bytes): position and rotation */
struct MotionKey {
    CVector3 m_position;
    CQuaternion m_rotation;
};

class CHierObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void TriggerScriptEvent(int, void*, bool);

    void CommitAnimMotionUpdate();
    void AttemptAnimMotionUpdate(float);
    void BeginAnimMotionUpdate(float);
    void CommitScriptMotionUpdate();
    void AttemptScriptMotionUpdate(float);
    void BeginScriptMotionUpdate(float);

    unsigned char unknown004[60];
    CMatrix m_matrix;
    unsigned char unknown080[140];
    void* m_listenerData;
    unsigned char unknown110[4];
    MatrixListenerView* m_listener;
    unsigned char unknown118[8];
    CVector3 m_position;
    CVector3 m_keyOffset;
    CQuaternion m_keyRotation;
    CQuaternion m_rotation;
    CVector3 m_velocity;
    unsigned char unknown170[16];
    CQuaternion m_animRotation;
    CQuaternion m_scriptRotation;
    CQuaternion m_nextRotation;
    unsigned char unknown1b0[8];
    float m_moveTime;
    float m_turnTime;
    unsigned char unknown1c0[4];
    int m_moveEvent;
    int m_turnEvent;
    unsigned char unknown1cc[4];
    float m_moveStep;
    float m_turnStep;
    unsigned char unknown1d8[8];
    unsigned char m_flags1e0a : 1;
    bool m_scriptMove : 1;
    bool m_scriptTurn : 1;
    unsigned char m_flags1e0c : 1;
    bool m_transformDelta : 1;
    bool m_flag04 : 1;
    unsigned char m_flags1e0b : 2;
    unsigned char unknown1e1[55];
    int m_keyEvent;
    unsigned char unknown21c[4];
    int m_lastKey;
    int m_keyCount;
    float m_keyTime;
    int m_keyFrames;
    int m_key;
    float m_rotationRate;
    CQuaternion m_targetRotation;
    MotionKey* m_keys;
    unsigned char unknown24c[832];
    void* m_eventData;
    unsigned char unknown590[96];
    CMatrix m_matrix5f0;
    CMatrix m_matrix630;
    CVector3 m_animPosition;
    CVector3 m_animDelta;
};
void CHierObject::CommitScriptMotionUpdate() {
    if (m_scriptMove) {
        float zero = 0.0f;
        m_moveTime -= m_moveStep;
        m_moveTime = m_moveTime < zero ? zero : m_moveTime;
        m_animPosition += m_animDelta;
    }
    if (m_scriptTurn) {
        float zero = 0.0f;
        m_animRotation = m_nextRotation;
        m_turnTime -= m_turnStep;
        m_turnTime = m_turnTime < zero ? zero : m_turnTime;
    }
}

void CHierObject::AttemptScriptMotionUpdate(float dt) {
    float moveStep = dt;
    float turnStep = dt;
    if (m_scriptTurn) {
        turnStep = m_turnTime < dt ? m_turnTime : dt;
        if (turnStep < 1.0f) {
            CQuaternion identity(0.0f, 0.0f, 0.0f, 1.0f);
            m_nextRotation.Slerp(m_scriptRotation, identity, turnStep);
        }
        else
            m_nextRotation = m_scriptRotation;
        m_nextRotation.Multiply(m_animRotation);
    } else {
        m_nextRotation = m_animRotation;
    }
    m_nextRotation.GetMatrix(m_matrix);
    if (m_scriptMove) {
        moveStep = m_moveTime < dt ? m_moveTime : dt;
        m_animDelta.x = moveStep * m_velocity.x;
        m_animDelta.y = moveStep * m_velocity.y;
        m_animDelta.z = moveStep * m_velocity.z;
        m_matrix.TransformPoint(m_animDelta, m_animDelta);
    } else {
        m_animDelta.x = m_animDelta.y = m_animDelta.z = 0.0f;
    }
    m_matrix.SetPos(m_animDelta);
    m_matrix.Translate(m_animPosition);
    m_matrix.Multiply(m_matrix, m_matrix630);
    m_matrix.Multiply(m_matrix, m_matrix5f0);
    m_moveStep = moveStep;
    m_turnStep = turnStep;
    if (m_listener && m_listenerData)
        m_listener->Update(m_listenerData, m_matrix);
}

void CHierObject::BeginScriptMotionUpdate(float) {
    if (m_scriptMove && m_moveTime <= 0.0f) {
        TriggerScriptEvent(m_moveEvent, m_eventData, false);
        m_moveTime = 0.0f;
        m_scriptMove = false;
        m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    }
    if (m_scriptTurn) {
        m_animRotation.Normalize();
        if (m_turnTime <= 0.0f) {
            TriggerScriptEvent(m_turnEvent, m_eventData, false);
            m_turnTime = 0.0f;
            m_scriptTurn = false;
            m_scriptRotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}
