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
// the new matrix). The classes and functions are named by the mangled
// symbols; the members, flag names and listener view are inferred, and the
// CVector3 view copies through a double pair as in player_weapon.cpp. The
// rest of the file is not part of this unit.
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

class CHierObject {
public:
    bool SetHierCollisionState(int);
    void CommitAnimMotionUpdate();
    void AttemptAnimMotionUpdate(float);

    unsigned char unknown000[64];
    CMatrix m_matrix;
    unsigned char unknown080[140];
    void* m_listenerData;
    unsigned char unknown110[4];
    MatrixListenerView* m_listener;
    unsigned char unknown118[8];
    CVector3 m_position;
    unsigned char unknown130[32];
    CQuaternion m_rotation;
    CVector3 m_velocity;
    unsigned char unknown170[16];
    CQuaternion m_animRotation;
    unsigned char unknown190[16];
    CQuaternion m_nextRotation;
    unsigned char unknown1b0[48];
    unsigned char m_flags1e0a : 4;
    bool m_transformDelta : 1;
    unsigned char m_flags1e0b : 3;
    unsigned char unknown1e1[67];
    int m_animating;
    unsigned char unknown228[4];
    int m_animMotion;
    unsigned char unknown230[4];
    float m_rotationRate;
    CQuaternion m_targetRotation;
    unsigned char unknown248[824];
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
    if (!m_animating)
        return;
    if (!m_animMotion)
        return;
    m_animRotation = m_nextRotation;
    m_animPosition += m_animDelta;
    m_position = m_animPosition;
    m_rotation = m_animRotation;
}

void CHierObject::AttemptAnimMotionUpdate(float dt) {
    if (m_animating && m_animMotion) {
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
