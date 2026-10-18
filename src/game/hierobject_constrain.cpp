// A fragment of hierobject.cpp (0x80099b78): CHierObject::ConstrainVelocity
// and OnCollision. ConstrainVelocity: when the object is active it
// constrains each attached scene node, clears the script steps, the next
// rotation and the animation delta, then resets the running motion: a path
// motion goes back to its start position with no speed, a script motion
// rebuilds the matrix from the animated rotation and position and the two
// local matrices, an animation motion stops and keeps its rotation; each
// child then takes the matrix and is constrained in turn.
// OnCollision ignores bullets (and everything while the first flag of +1416
// is set); with a volume it records each contact's collision state and
// tries collision damage. An active object hit by the player pushes back:
// when the direction from the player to the contact opposes the object's
// own motion (along its path, or out of the contact point moved through the
// script motion's inverse), the motion is undone as in ConstrainVelocity
// and each child takes the matrix and the collision; the active flag is then
// cleared. Other active collisions add the other node to the attached list
// once per contact while there is room. Every child then takes the
// collision. The classes and functions are named by the mangled symbols;
// CHierObject declares its virtual functions in the order of
// __vt__11CHierObject (placeholders named by offset), the members, flag
// names, the attached-node list (pointer, count and capacity), the entry
// type (a by-value assignment) and the listener, extent, volume, contact
// and collision views are inferred as in hierobject_update.cpp. CVector3
// holds its components in a union with a double pair (implicit copies as
// doubleword pairs, components kept in registers). The rest of the file is
// not part of this unit.
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
    CVector3(float ax, float ay, float az) {
        d.v[0] = ax;
        d.v[1] = ay;
        d.v[2] = az;
    }
    CVector3 operator*(float f) const { return CVector3(d.v[0] * f, d.v[1] * f, d.v[2] * f); }
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
    float Dot(const CVector3& v) const { return d.v[0] * v.d.v[0] + d.v[1] * v.d.v[1] + d.v[2] * v.d.v[2]; }
    CVector3& operator+=(const CVector3& v) {
        d.v[0] += v.d.v[0];
        d.v[1] += v.d.v[1];
        d.v[2] += v.d.v[2];
        return *this;
    }

    CVector3Pair d;
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
    void SetFromMatrix(const CMatrix&);
    void Multiply(CQuaternion);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

inline CVector3 operator*(float f, const CVector3& v) { return CVector3(f * v.d.v[0], f * v.d.v[1], f * v.d.v[2]); }

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    void FastInverse(const CMatrix&);
    void TransformPoint(CVector3&, CVector3) const;
    void SetPos(CVector3);
    void Translate(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    CMatrix& operator=(const CMatrix&);
    CVector3 GetFront() const { return front; }
    CVector3 GetPos() const { return pos; }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

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

class CPlayerObject {
public:
    void SetPlayersMountedMGPositions(CMatrix*);
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

/* inferred: the object's bounding volume (80 bytes), its extents at +72 */
class VolumeView {
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
    virtual void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[76];
};

/* inferred: an attached scene node, constrained through its slot +32 */
class ISceneNodeView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void ConstrainVelocity();
};

class CCollision;
class CBullet;
class CPlayerObject;
class CAnimObject;

class ISceneNode {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void GetPosition(CVector3&) const;
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual CAnimObject* AsAnimObject();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual CPlayerObject* AsPlayerObject();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual CBullet* AsBullet();
};

/* inferred: a collision contact: position, collision state, next contact */
struct ContactView {
    unsigned char unknown00[16];
    CVector3 m_position;
    unsigned char unknown20[4];
    int m_state;
    unsigned char unknown28[4];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_node0;
    ISceneNode* m_node1;
    unsigned char unknown08[16];
    ContactView* m_contacts;
};

class CStaticObject {
public:
    void AttemptCollisionDamage(const CCollision&);
};

/* inferred: an entry of the attached list */
struct AttachedEntry {
    AttachedEntry(ISceneNodeView* node) : m_node(node) {}
    AttachedEntry& operator=(AttachedEntry other) {
        m_node = other.m_node;
        return *this;
    }

    ISceneNodeView* m_node;
};

class CHierObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
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
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual void unknown140();
    virtual void unknown144();
    virtual void unknown148();
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void AttemptPathMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CommitPathMotionUpdate();

    void BeginAnimMotionUpdate(float);
    void SetHierCollisionState(int);
    /* inferred: adds a node to the attached list while there is room */
    bool AttachedFull() const { return m_attachedCount >= m_attachedCapacity; }
    void StoreAttached(AttachedEntry entry) { ((AttachedEntry*)m_attached)[m_attachedCount++] = entry; }
    void AddAttached(AttachedEntry entry) {
        if (!AttachedFull())
            StoreAttached(entry);
    }
    /* inferred inline helper: its temporary's stack slot needs it */
    void ResetPathMotion() {
        m_matrix.SetPos(m_pathPosition);
        m_speed = m_pathSpeed = 0.0f;
    }
    void BeginScriptMotionUpdate(float);
    CVector3 GetWorldLinearVelocity() const;
    bool IsMoving() const { return m_pathState != 0 && m_pathState != 1; }

    unsigned char pad004[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char pad01c[36];
    CMatrix m_matrix;
    unsigned char pad080[88];
    float m_speed;
    unsigned char pad0dc[40];
    int m_pathState;
    unsigned char pad108[4];
    void* m_listenerData;
    MatrixListenerView* m_matrixListener;
    MatrixListenerView* m_listener;
    unsigned char pad118[8];
    CVector3 m_position;
    CVector3 m_keyOffset;
    CQuaternion m_keyRotation;
    CQuaternion m_rotation;
    CVector3 m_velocity;
    CVector3 m_pathPosition;
    CQuaternion m_animRotation;
    CQuaternion m_scriptRotation;
    CQuaternion m_nextRotation;
    unsigned char pad1b0[8];
    float m_moveTime;
    float m_turnTime;
    unsigned char pad1c0[4];
    int m_moveEvent;
    int m_turnEvent;
    unsigned char pad1cc[4];
    float m_moveStep;
    float m_turnStep;
    unsigned char pad1d8[8];
    unsigned char m_flags1e0a : 1;
    bool m_scriptMove : 1;
    bool m_scriptTurn : 1;
    bool m_flag10 : 1;
    bool m_transformDelta : 1;
    bool m_flag04 : 1;
    unsigned char m_flags1e0b : 2;
    unsigned char pad1e1[23];
    float m_pathSpeed;
    unsigned char pad1fc[28];
    int m_keyEvent;
    unsigned char pad21c[4];
    int m_lastKey;
    int m_keyCount;
    float m_keyTime;
    int m_keyFrames;
    int m_key;
    float m_rotationRate;
    CQuaternion m_targetRotation;
    MotionKey* m_keys;
    unsigned char pad24c[820];
    CHierObject* m_firstChild;
    CHierObject* m_nextSibling;
    unsigned char m_flags588a : 1;
    bool m_mountedMG : 1;
    bool m_hasVolume : 1;
    bool m_flag588 : 1;
    unsigned char m_flags588b : 4;
    unsigned char pad589[3];
    void* m_eventData;
    unsigned char pad590[4];
    VolumeView m_volume;
    unsigned char pad5e4[12];
    CMatrix m_matrix5f0;
    CMatrix m_matrix630;
    CVector3 m_animPosition;
    CVector3 m_animDelta;
    ISceneNodeView** m_attached;
    int m_attachedUnknown;
    int m_attachedCount;
    int m_attachedCapacity;
};

void CHierObject::ConstrainVelocity() {
    if (m_flag588) {
        ISceneNodeView** end = m_attached + m_attachedCount;
        for (ISceneNodeView** it = m_attached; it != end; ++it) {
            if (*it)
                (*it)->ConstrainVelocity();
        }
        m_moveStep = 0.0f;
        m_turnStep = 0.0f;
        m_nextRotation = m_animRotation;
        m_animDelta.d.v[0] = m_animDelta.d.v[1] = m_animDelta.d.v[2] = 0.0f;
        if (IsMoving()) {
            ResetPathMotion();
        } else if (m_scriptMove || m_scriptTurn) {
            m_animRotation.GetMatrix(m_matrix);
            m_matrix.Translate(m_animPosition);
            m_matrix.Multiply(m_matrix, m_matrix630);
            m_matrix.Multiply(m_matrix, m_matrix5f0);
        } else if (m_flag04) {
            m_velocity.d.v[0] = m_velocity.d.v[1] = m_velocity.d.v[2] = 0.0f;
            m_targetRotation = m_rotation;
        }
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
        child->m_matrix5f0 = m_matrix;
        child->ConstrainVelocity();
    }
}

void CHierObject::OnCollision(const CCollision& collision) {
    if (m_flags588a)
        return;
    ISceneNode* other = collision.m_node0 == (ISceneNode*)this ? collision.m_node1 : collision.m_node0;
    if (other->AsBullet())
        return;
    if (m_hasVolume) {
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next)
            SetHierCollisionState(contact->m_state);
        ((CStaticObject*)this)->AttemptCollisionDamage(collision);
    }
    if (m_flag588) {
        if (m_hasVolume && other->AsPlayerObject()) {
            CVector3 contactPos = collision.m_contacts->m_position;
            CVector3 otherPos;
            other->GetPosition(otherPos);
            CVector3 push(contactPos.d.v[0] - otherPos.d.v[0], contactPos.d.v[1] - otherPos.d.v[1], contactPos.d.v[2] - otherPos.d.v[2]);
            push.Normalize();
            CVector3 normal;
            if (IsMoving()) {
                const CVector3& pos = m_matrix.GetPos();
                normal.d.v[0] = pos.d.v[0] - m_pathPosition.d.v[0];
                normal.d.v[1] = pos.d.v[1] - m_pathPosition.d.v[1];
                normal.d.v[2] = pos.d.v[2] - m_pathPosition.d.v[2];
                normal.Normalize();
            } else if (m_scriptMove || m_scriptTurn || m_flag10) {
                CMatrix inverse;
                inverse.FastInverse(m_matrix);
                CVector3 local;
                inverse.TransformPoint(local, contactPos);
                m_animRotation.GetMatrix(inverse);
                inverse.SetPos(m_pathPosition);
                inverse.TransformPoint(local, local);
                normal.d.v[0] = contactPos.d.v[0] - local.d.v[0];
                normal.d.v[1] = contactPos.d.v[1] - local.d.v[1];
                normal.d.v[2] = contactPos.d.v[2] - local.d.v[2];
                normal.Normalize();
            }
            if (push.Dot(normal) < 0.0f) {
                m_moveStep = 0.0f;
                m_turnStep = 0.0f;
                m_nextRotation = m_animRotation;
                m_animDelta.d.v[0] = m_animDelta.d.v[1] = m_animDelta.d.v[2] = 0.0f;
                if (IsMoving()) {
                    ResetPathMotion();
                } else if (m_scriptMove || m_scriptTurn) {
                    m_animRotation.GetMatrix(m_matrix);
                    m_matrix.Translate(m_animPosition);
                    m_matrix.Multiply(m_matrix, m_matrix630);
                    m_matrix.Multiply(m_matrix, m_matrix5f0);
                } else if (m_flag04) {
                    m_velocity.d.v[0] = m_velocity.d.v[1] = m_velocity.d.v[2] = 0.0f;
                    m_targetRotation = m_rotation;
                }
                for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
                    child->m_matrix5f0 = m_matrix;
                    child->OnCollision(collision);
                }
            }
            m_flag588 = 0;
            return;
        }
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next)
            AddAttached(AttachedEntry((ISceneNodeView*)other->AsAnimObject()));
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling)
        child->OnCollision(collision);
}
