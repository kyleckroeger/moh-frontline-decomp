// A fragment of static_obj.cpp (0x800b194c): CStaticObject's AnimUpdate
// (advances the frame counter at +532 - backwards when the reverse flag is
// set - by whole frame times; at the last frame it fires the animation's
// script event once and stops, otherwise it wraps around the frame count)
// and Init (takes the script rotation from the transform, sets the scale
// value to 1 and a value at +232 to 0.5, shows the object, clears the
// script motions, velocity, path state and two flags). CStaticObject,
// CMatrix, CQuaternion, CLightVolumeManager and CDrawContext are named by
// the mangled symbols; CStaticObject declares its virtual functions in the
// order of __vt__13CStaticObject (placeholders named by offset for the slots
// not used here), its members, the flag names and the mesh, key, extent and
// listener views are inferred, and the CVector3 view copies through a double
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

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    void PreTranslate(CVector3);
    void SetPos(CVector3);
    void Translate(CVector3);
    void TransformPoint(CVector3&, CVector3) const;
    void Multiply(const CMatrix&, const CMatrix&);
    void BuildScale(float);
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    CQuaternion() {}
    CQuaternion(float ax, float ay, float az, float aw) : w(aw), x(ax), y(ay), z(az) {}
    void Slerp(CQuaternion, CQuaternion, float);
    void Multiply(CQuaternion);
    void Normalize();
    void Rotate(CVector3&, CVector3);
    void Set(float, float, float, float);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

class BPDLightVolume;

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();
    void Update(float);

    unsigned char unknown00[4];
};

/* inferred: the draw context's transform pointer at +192 */
class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
};

/* inferred: the object's mesh (+48), drawn through its slot +12 */
class MeshView {
public:
    virtual void unknown08();
    virtual void Draw(CDrawContext&, int);
    virtual void unknown10();
    virtual bool IsLit();
};

/* inferred: a motion key (32 bytes): position and rotation */
struct MotionKey {
    CVector3 m_position;
    CQuaternion m_rotation;
};

/* inferred: the other matrix listener, told through its slot +16 */
class MatrixListenerView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void Update(void*, const CMatrix&);
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
    virtual void vf40();
    virtual void vf44();
    virtual void GetExtents(CVector3&, CVector3&) const;
};

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

class CStaticObject {
public:
    virtual void MarkForDestruction(int);
    virtual ~CStaticObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void GetLocalBoundingVolume(int) const;
    virtual void GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual void vf130();
    virtual void vf134();
    virtual void vf138();
    virtual void vf13c();
    virtual void vf140();
    virtual void vf144();
    virtual void vf148();
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void AttemptPathMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CommitPathMotionUpdate();

    bool IsFollowingPath() const { return m_pathState != 0 && m_pathState != 1; }
    CVector3 PositionRow() const { return m_tm.position; }
    void AnimUpdate(float);
    void Init();
    void BeginAnimMotionUpdate(float);
    void UpdatePathMotionDestAndSpeed();


    unsigned char unknown004[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char unknown01c[20];
    MeshView* m_mesh;
    unsigned char unknown034[12];
    CMatrix m_tm;
    unsigned char unknown080[88];
    float m_speed216;
    unsigned char unknown0dc[12];
    float m_valueE8;
    unsigned char unknown0ec[24];
    int m_pathState;
    unsigned char unknown108[4];
    void* m_listenerData;
    MatrixListenerView* m_matrixListener;
    MatrixListenerView* m_listener;
    unsigned char unknown118[8];
    CVector3 m_animPosition;
    CVector3 m_keyOffset;
    CQuaternion m_keyRotation;
    CQuaternion m_animRotation;
    CVector3 m_velocity;
    CVector3 m_pathPosition;
    CQuaternion m_scriptRotation;
    CQuaternion m_scriptTurn;
    CQuaternion m_scriptTargetRotation;
    float m_value1b0;
    float m_value1b4;
    float m_time1b8;
    float m_time1bc;
    float m_time1c0;
    int m_moveEvent;
    int m_turnEvent;
    int m_valueEvent;
    float m_step1d0;
    float m_step1d4;
    float m_step1d8;
    unsigned char unknown1dc[4];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_transformDelta : 1;
    unsigned char m_animMove : 1;
    unsigned char m_reverse : 1;
    unsigned char m_flag01 : 1;
    unsigned char m_flag1e1 : 1;
    unsigned char m_flags1e1a : 1;
    unsigned char m_scriptHold : 1;
    unsigned char m_flags1e1b : 5;
    unsigned char unknown1e2[22];
    float m_pathSpeed;
    int m_animEvent;
    unsigned char unknown200[4];
    int m_animLast;
    int m_animCount;
    float m_animTime;
    int m_animFrames;
    int m_frame;
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
    void* m_attached;
    CVector3 m_attachOffset;
    CLightVolumeManager m_lightVolumes;
};


void CStaticObject::AnimUpdate(float dt) {
    if (m_animCount && m_animFrames) {
        m_animTime += dt;
        while (m_animTime >= (float)m_animFrames) {
            m_animTime -= (float)m_animFrames;
            if (m_reverse)
                m_frame--;
            else
                m_frame++;
        }
        if (m_animLast) {
            if (m_frame >= m_animLast && !m_reverse) {
                if (m_animEvent != -1) {
                    TriggerScriptEvent(m_animEvent, 0, false);
                    m_animEvent = -1;
                }
                m_frame = m_animLast - 1;
                m_animFrames = 0;
            } else if (m_frame <= m_animLast && m_reverse) {
                if (m_animEvent != -1) {
                    TriggerScriptEvent(m_animEvent, 0, false);
                    m_animEvent = -1;
                }
                m_frame = m_animLast;
                m_animFrames = 0;
            }
        } else if (m_reverse) {
            while (m_frame < 0)
                m_frame += m_animCount;
        } else {
            while (m_frame >= m_animCount)
                m_frame -= m_animCount;
        }
    }
}

void CStaticObject::Init() {
    m_scriptRotation.SetFromMatrix(m_tm);
    m_value1b4 = 1.0f;
    m_flag80 = true;
    m_scriptValue = false;
    m_scriptMove = false;
    m_scriptRotate = false;
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    m_pathState = 0;
    m_flag01 = false;
    m_valueE8 = 0.5f;
    m_flag1e1 = false;
}
