// A fragment of static_obj.cpp (0x800b013c): CStaticObject's Draw (when
// the object is shown, points the draw context at its transform, sets the
// light volume when the mesh is lit and draws the mesh), CommitUpdate (tells
// the other matrix listener, commits the path, script or animation motion
// that is running and gives the attached object the transform moved by its
// offset), CommitPathMotionUpdate (saves the transform's position as the path
// motion's position, copies a speed value and recomputes the path destination
// and speed), CommitAnimMotionUpdate (while animation motion is active, saves
// the position and sets the rotation at +336 from the transform),
// CommitScriptMotionUpdate (saves the position, then for each active script
// motion - flag bits at +480 - steps its time down by its step, never below
// 0 (a .sdata2 pool entry), the rotation one also taking its target rotation
// and the value one advancing its value by step times rate) and
// AttemptUpdate (attempts the running motion; script motion only when the
// hold flag at +481 is clear). CStaticObject, CMatrix, CQuaternion,
// CStaticMesh, CLightVolumeManager and CDrawContext are named by the mangled
// symbols; CStaticObject declares its virtual functions in the order of
// __vt__13CStaticObject (placeholders named by offset for the slots not used
// here), its members, the flag names and the mesh, attached-object,
// listener and draw-context views are inferred, and the CVector3 view and
// by-value position getter are as in static_obj_setpos.cpp. The rest of the
// file is not part of this unit.
class CVector3 {
public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        double pair[2];
    };
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    void PreTranslate(CVector3);
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

    float x;
    float y;
    float z;
    float w;
};

class BPDLightVolume;

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();

    unsigned char unknown00[4];
};

class CStaticMesh {
public:
    static void SetLightVolume(BPDLightVolume*);
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

/* inferred: the object at +588, given the object's transform through its slot +232 */
class AttachedView {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
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
    virtual void SetTransform(CMatrix&);
};

/* inferred: the other matrix listener, told through its slot +16 */
class MatrixListenerView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void Update(void*, const CMatrix&);
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
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
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
    void UpdatePathMotionDestAndSpeed();

    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown004[44];
    MeshView* m_mesh;
    unsigned char unknown034[12];
    CMatrix m_tm;
    unsigned char unknown080[88];
    float m_speed216;
    unsigned char unknown0dc[40];
    int m_pathState;
    unsigned char unknown108[4];
    void* m_listenerData;
    MatrixListenerView* m_matrixListener;
    MatrixListenerView* m_listener;
    unsigned char unknown118[8];
    CVector3 m_animPosition;
    unsigned char unknown130[32];
    CQuaternion m_animRotation;
    unsigned char unknown160[16];
    CVector3 m_pathPosition;
    CQuaternion m_scriptRotation;
    unsigned char unknown190[16];
    CQuaternion m_scriptTargetRotation;
    float m_value1b0;
    float m_value1b4;
    float m_time1b8;
    float m_time1bc;
    float m_time1c0;
    unsigned char unknown1c4[12];
    float m_step1d0;
    float m_step1d4;
    float m_step1d8;
    unsigned char unknown1dc[4];
    unsigned char m_flag80 : 1;
    unsigned char m_scriptMove : 1;
    unsigned char m_scriptRotate : 1;
    unsigned char m_scriptValue : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_animMove : 1;
    unsigned char m_flags03 : 2;
    unsigned char m_flags1e1a : 2;
    unsigned char m_scriptHold : 1;
    unsigned char m_flags1e1b : 5;
    unsigned char unknown1e2[22];
    float m_pathSpeed;
    unsigned char unknown1fc[24];
    int m_drawArg;
    unsigned char unknown218[12];
    int m_animMotion;
    unsigned char unknown228[4];
    int m_animMotionActive;
    unsigned char unknown230[28];
    AttachedView* m_attached;
    CVector3 m_attachOffset;
    CLightVolumeManager m_lightVolumes;
};

void CStaticObject::Draw(CDrawContext& context) {
    if (m_flag80) {
        context.m_localToWorld = &m_tm;
        if (m_mesh->IsLit())
            CStaticMesh::SetLightVolume(m_lightVolumes.GetVolume());
        m_mesh->Draw(context, m_drawArg);
    }
}

void CStaticObject::CommitUpdate() {
    if (m_matrixListener != m_listener)
        m_matrixListener->Update(m_listenerData, m_tm);
    if (IsFollowingPath())
        CommitPathMotionUpdate();
    else if (m_scriptMove || m_scriptRotate || m_scriptValue)
        CommitScriptMotionUpdate();
    else if (m_animMove)
        CommitAnimMotionUpdate();
    if (m_attached) {
        CMatrix tm;
        GetTMLocalToWorld(tm);
        tm.PreTranslate(m_attachOffset);
        m_attached->SetTransform(tm);
    }
}

void CStaticObject::CommitPathMotionUpdate() {
    m_pathPosition = PositionRow();
    m_pathSpeed = m_speed216;
    UpdatePathMotionDestAndSpeed();
}

void CStaticObject::CommitAnimMotionUpdate() {
    if (m_animMotion && m_animMotionActive) {
        m_animPosition = PositionRow();
        m_animRotation.SetFromMatrix(m_tm);
    }
}

void CStaticObject::CommitScriptMotionUpdate() {
    m_pathPosition = PositionRow();
    if (m_scriptMove) {
        m_time1b8 -= m_step1d0;
        m_time1b8 = m_time1b8 < 0.0f ? 0.0f : m_time1b8;
    }
    if (m_scriptRotate) {
        m_scriptRotation = m_scriptTargetRotation;
        m_time1bc -= m_step1d4;
        m_time1bc = m_time1bc < 0.0f ? 0.0f : m_time1bc;
    }
    if (m_scriptValue) {
        m_value1b4 += m_step1d8 * m_value1b0;
        m_time1c0 -= m_step1d8;
        m_time1c0 = m_time1c0 < 0.0f ? 0.0f : m_time1c0;
    }
}

void CStaticObject::AttemptUpdate(float dt) {
    if (IsFollowingPath()) {
        AttemptPathMotionUpdate(dt);
    } else if (m_scriptMove || m_scriptRotate || m_scriptValue) {
        if (!m_scriptHold)
            AttemptScriptMotionUpdate(dt);
    } else if (m_animMove) {
        AttemptAnimMotionUpdate(dt);
    }
}
