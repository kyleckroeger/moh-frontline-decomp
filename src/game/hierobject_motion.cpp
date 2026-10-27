// A fragment of hierobject.cpp (0x8009a3bc): CHierObject's
// SetHierCollisionState (the object, or the first descendant in its child
// list, whose collision state (+1420) is the given one marks itself and every
// ancestor on the way (bit 0x10 of +1416); the compiler inlines the
// recursion several levels deep), CommitAnimMotionUpdate (when an animated motion is running, the next
// rotation becomes current, the position advances by the step and both are
// copied to the object's position and rotation) and AttemptAnimMotionUpdate
// (the step is the velocity times the time, the rotation is interpolated
// toward the current one at the rotation rate, and the matrix is rebuilt
// from it, the position and the step, optionally turned into the matrix's
// frame, then multiplied by the two local matrices; a listener is told of
// the new matrix) and BeginAnimMotionUpdate (when keyed motion is set up:
// restarts from the animated position and rotation, advances the key time by
// whole key lengths, clamps to the last key (firing the key event once and
// clearing flag 0x04) or wraps around the key count, then interpolates the
// position and slerps the rotation between the current and next keys,
// optionally offsets and turns them by the key transform, and sets the
// velocity, target rotation and rotation rate for the step). The classes and
// functions are named by the mangled symbols; the members, flag names, key
// and listener views and the lerp helper are inferred, and the CVector3 view
// copies through a double pair as in player_weapon.cpp. The rest of the file
// is not part of this unit.
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

    bool SetHierCollisionState(int);
    void CommitAnimMotionUpdate();
    void AttemptAnimMotionUpdate(float);
    void BeginAnimMotionUpdate(float);

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
    unsigned char unknown190[16];
    CQuaternion m_nextRotation;
    unsigned char unknown1b0[48];
    unsigned char m_flags1e0a : 4;
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
    unsigned char unknown24c[820];
    CHierObject* m_child;
    CHierObject* m_sibling;
    unsigned char m_flags588a : 3;
    unsigned char m_inCollisionState : 1;
    unsigned char m_flags588b : 4;
    unsigned char unknown589[3];
    int m_collisionState;
    unsigned char unknown590[96];
    CMatrix m_matrix5f0;
    CMatrix m_matrix630;
    CVector3 m_animPosition;
    CVector3 m_animDelta;
};

bool CHierObject::SetHierCollisionState(int state) {
    if (state == m_collisionState) {
        m_inCollisionState = 1;
        return true;
    }
    for (CHierObject* child = m_child; child; child = child->m_sibling) {
        if (child->SetHierCollisionState(state)) {
            m_inCollisionState = 1;
            return true;
        }
    }
    return false;
}

void CHierObject::CommitAnimMotionUpdate() {
    if (!m_keyCount)
        return;
    if (!m_keyFrames)
        return;
    m_animRotation = m_nextRotation;
    m_animPosition += m_animDelta;
    m_position = m_animPosition;
    m_rotation = m_animRotation;
}

void CHierObject::AttemptAnimMotionUpdate(float dt) {
    if (m_keyCount && m_keyFrames) {
        m_animDelta.x = dt * m_velocity.x;
        m_animDelta.y = dt * m_velocity.y;
        m_animDelta.z = dt * m_velocity.z;
        CQuaternion rotation;
        rotation.Slerp(m_targetRotation, m_rotation, dt * m_rotationRate);
        m_nextRotation = rotation;
        rotation.GetMatrix(m_matrix);
        if (m_transformDelta)
            m_matrix.TransformPoint(m_animDelta, m_animDelta);
        m_matrix.SetPos(m_position);
        m_matrix.Translate(m_animDelta);
        m_matrix.Multiply(m_matrix, m_matrix630);
        m_matrix.Multiply(m_matrix, m_matrix5f0);
    }
    if (m_listener && m_listenerData)
        m_listener->Update(m_listenerData, m_matrix);
}

inline void LerpTo(CVector3& r, const CVector3& a, const CVector3& b, float t) {
    float fx, fy, fz;
    fx = a.x;
    fy = a.y;
    fz = a.z;
    r.x = fx + t * (b.x - fx);
    r.y = fy + t * (b.y - fy);
    r.z = fz + t * (b.z - fz);
}

void CHierObject::BeginAnimMotionUpdate(float dt) {
    if (m_keyCount && m_keyFrames) {
        m_position = m_animPosition;
        m_rotation = m_animRotation;
        m_keyTime += dt;
        while (m_keyTime >= (float)m_keyFrames) {
            m_keyTime -= (float)m_keyFrames;
            m_key++;
        }
        int next = m_key + 1;
        if (m_lastKey) {
            if (next >= m_lastKey)
                next = m_lastKey - 1;
            if (m_key >= m_lastKey) {
                if (m_keyEvent != -1) {
                    TriggerScriptEvent(m_keyEvent, 0, false);
                    m_keyEvent = -1;
                }
                m_key = m_lastKey - 1;
                m_flag04 = false;
            }
        } else {
            m_key %= m_keyCount;
            next %= m_keyCount;
        }
        CVector3 from = m_keys[m_key].m_position;
        CVector3 to = m_keys[next].m_position;
        float t = m_keyTime / (float)m_keyFrames;
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f)
            t = 1.0f;
        CVector3 pos;
        LerpTo(pos, from, to, t);
        CQuaternion current;
        current = m_keys[m_key].m_rotation;
        CQuaternion target;
        target = m_keys[next].m_rotation;
        CQuaternion rotation;
        rotation.Slerp(target, current, t);
        if (m_transformDelta) {
            m_keyRotation.Rotate(pos, pos);
            pos.x += m_keyOffset.x;
            pos.y += m_keyOffset.y;
            pos.z += m_keyOffset.z;
            rotation.Multiply(m_keyRotation);
        }
        m_velocity.x = pos.x - m_position.x;
        float rate = 1.0f / dt;
        m_velocity.y = pos.y - m_position.y;
        m_velocity.z = pos.z - m_position.z;
        m_targetRotation = rotation;
        m_velocity.x *= rate;
        m_velocity.y *= rate;
        m_velocity.z *= rate;
        m_rotationRate = rate;
    }
}
