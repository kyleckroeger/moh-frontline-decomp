// A fragment of hierobject.cpp (0x80099b78): CHierObject::ConstrainVelocity.
// When the object is active it constrains each attached scene node, clears
// the script steps, the next rotation and the animation delta, then resets
// the running motion: a path motion goes back to its start position with no
// speed, a script motion rebuilds the matrix from the animated rotation and
// position and the two local matrices, an animation motion stops and keeps
// its rotation; each child then takes the matrix and is constrained in turn.
// The classes and functions are named by the mangled symbols; CHierObject
// declares its virtual functions in the order of __vt__11CHierObject
// (placeholders named by offset), the members, flag names, the attached-node
// list (a fast_vec-like pointer, capacity and count) and the listener,
// extent and volume views are inferred as in hierobject_update.cpp, and the
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
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3 operator*(float f) const { return CVector3(x * f, y * f, z * f); }
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
    void SetFromMatrix(const CMatrix&);
    void Multiply(CQuaternion);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

inline CVector3 operator*(float f, const CVector3& v) { return CVector3(f * v.x, f * v.y, f * v.z); }

class CMatrix {
public:
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
    virtual void unknown028();
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
    /* inferred inline helper: its temporary's stack slot needs it */
    void ResetPathMotion() {
        m_matrix.SetPos(m_pathPosition);
        m_speed = m_pathSpeed = 0.0f;
    }
    void BeginScriptMotionUpdate(float);
    CVector3 GetWorldLinearVelocity() const;
    bool IsMoving() const { return m_pathState != 0 && m_pathState != 1; }
    bool IsScripted() const { return m_scriptMove || m_scriptTurn || m_flag10; }

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
    int m_attachedCapacity;
    int m_attachedCount;
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
        m_animDelta.x = m_animDelta.y = m_animDelta.z = 0.0f;
        if (IsMoving()) {
            ResetPathMotion();
        } else if (m_scriptMove || m_scriptTurn) {
            m_animRotation.GetMatrix(m_matrix);
            m_matrix.Translate(m_animPosition);
            m_matrix.Multiply(m_matrix, m_matrix630);
            m_matrix.Multiply(m_matrix, m_matrix5f0);
        } else if (m_flag04) {
            m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
            m_targetRotation = m_rotation;
        }
    }
    for (CHierObject* child = m_firstChild; child; child = child->m_nextSibling) {
        child->m_matrix5f0 = m_matrix;
        child->ConstrainVelocity();
    }
}
