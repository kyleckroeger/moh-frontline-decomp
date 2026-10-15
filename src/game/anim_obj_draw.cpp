// A fragment of anim_obj.cpp (0x80095208): CAnimObject's Draw (unless the
// configuration turns animated objects off, points the draw context at the
// transform, updates the skeleton matrices, draws the skinned animation and
// updates the local bounding volume from the returned extents),
// SetNodeState (forwarded to the animation), SetMoveTarget,
// UpdateAnimation (advances the animation, adds the extracted rotation -
// wrapped to -pi..pi - to the turn and makes the extracted translation the
// velocity unless a force or stop is active), FinishAnimationState (starts
// the queued animation state, or fires the script event),
// QueueAnimationState and SetAnimationState (queues the state when the
// animation reports 0xf0f0f0f0 and starts the state it names instead). The
// file is compiled with deferred inlining (FinishAnimationState inlines the
// later SetAnimationState), so the functions are written in reverse image
// order. The classes, functions and g_config are named by the symbols;
// CAnimObject declares its virtual functions in the order of
// __vt__11CAnimObject (placeholders named by offset for the slots not used
// here), the members, flag names, return types and the configuration,
// draw-context and volume views are inferred, the layout follows
// anim_obj_ctor.cpp, and the CVector3 view copies through a double pair as
// in player_weapon.cpp. The rest of the file is not part of this unit.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

/* inferred: the configuration flag at +3 that stops animated objects drawing */
struct ConfigView {
    unsigned char unknown00[3];
    bool noAnimObjects;
    unsigned char unknown04[16];
};

extern ConfigView g_config;

/* inferred: the draw context's transform pointer at +192 */
class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
};

class ISceneNode {
public:
    enum EVolumeType {};
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    virtual bool ExtractRotation(long*);
    virtual bool ExtractTranslation(CVector3*);

    unsigned long SetAnimationState(unsigned short, float, unsigned short&, long, void*);
    void SetGeomNodeState(long, long);
};

class CAnimSkinned : public CAnimated {
public:
    void UpdateSkeletonMatrices(CMatrix*, CVector3*, CVector3*);

    unsigned char unknown04[44];
};

/* inferred: the animated volume's flag at +32 */
struct AnimatedVolumeView {
    unsigned char unknown00[32];
    bool m_flag20;
};

class CAnimObject {
public:
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
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
    virtual void UpdateAnimation(float);
    virtual void SetScript(BSObject*);
    virtual void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);

    void SetNodeState(long, long);
    void SetMoveTarget(const CVector3&);
    bool FinishAnimationState(long, bool);
    void QueueAnimationState(unsigned short, float, long, void*);
    void SetAnimationState(unsigned short, float, long, void*);

    unsigned char unknown0004[44];
    CAnimSkinned m_skinned;
    unsigned char unknown0060[3808];
    CMatrix m_matrices[80];
    CMatrix m_tm;
    unsigned char unknown2380[16];
    BSObject* m_script;
    unsigned char unknown2394[4];
    AnimatedVolumeView* m_volume;
    unsigned char unknown239c[4];
    CVector3 m_moveTarget;
    unsigned char unknown23b0[24];
    CVector3 m_velocity;
    CVector3 m_animDelta;
    float m_animScale;
    unsigned char unknown23ec[4];
    float m_turn;
    unsigned char unknown23f4[94];
    bool m_finished;
    bool m_finishPending;
    bool m_onGround;
    unsigned char unknown2455;
    bool m_forced;
    bool m_noRotation;
    unsigned char unknown2458;
    bool m_useTranslation;
    unsigned char unknown245a[70];
    bool m_queued;
    unsigned char unknown24a1;
    unsigned short m_queuedState;
    float m_queuedTime;
    long m_queuedValue;
    void* m_queuedData;
    bool m_finishing;
    bool m_hasMoveTarget;
    bool m_keepZ;
    unsigned char unknown24b3;
    bool m_moved;
    bool m_stopped;
};

void CAnimObject::SetAnimationState(unsigned short state, float time, long value, void* data) {
    unsigned short next;
    if (m_skinned.SetAnimationState(state, time, next, value, data) == 0xf0f0f0f0) {
        QueueAnimationState(state, time, value, data);
        m_skinned.SetAnimationState(next, time, next, value, data);
    } else {
        m_queued = false;
    }
    m_finishPending = false;
}

void CAnimObject::QueueAnimationState(unsigned short state, float time, long value, void* data) {
    m_queuedState = state;
    m_queuedTime = time;
    m_queuedValue = value;
    m_queuedData = data;
    m_queued = true;
}

bool CAnimObject::FinishAnimationState(long event, bool keep) {
    if (!m_finished || !m_finishPending) {
        if (m_queued && !keep) {
            m_queued = false;
            m_finishing = true;
            SetAnimationState(m_queuedState, m_queuedTime, m_queuedValue, m_queuedData);
            m_finished = true;
        } else {
            m_finishing = false;
            BSObjectTriggerEvent(m_script, event, 0, 0, false);
            if (!keep)
                m_finishPending = true;
        }
    }
    return false;
}

void CAnimObject::UpdateAnimation(float dt) {
    m_turn = 0.0f;
    m_noRotation = false;
    m_animDelta.x = m_animDelta.y = m_animDelta.z = 0.0f;
    m_animScale = 1.0f;
    m_skinned.UpdateAnimation(dt);
    long rotation = 0;
    if (!m_noRotation && m_skinned.ExtractRotation(&rotation)) {
        float angle = 6.2831855f * (float)rotation / 16777216.0f;
        if (angle > 3.1415927f)
            angle -= 6.2831855f;
        else if (angle < -3.1415927f)
            angle += 6.2831855f;
        m_turn += angle;
    }
    CVector3 translation;
    bool translated = false;
    if (m_skinned.ExtractTranslation(&translation) && m_useTranslation)
        translated = true;
    float inv = 1.0f / dt;
    m_moved = false;
    if (!m_forced && !m_stopped) {
        if (translated) {
            m_velocity.x = inv * (m_animScale * translation.x + m_animDelta.x);
            m_velocity.y = inv * (m_animScale * translation.y + m_animDelta.y);
            if (!m_keepZ)
                m_velocity.z = inv * translation.z;
            m_moved = true;
        } else {
            m_velocity.x = 0.0f;
            m_velocity.y = 0.0f;
            if (!m_keepZ)
                m_velocity.z = 0.0f;
        }
    } else if (m_stopped) {
        m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    }
    m_turn *= inv;
    if (m_volume)
        m_volume->m_flag20 = false;
}

void CAnimObject::SetMoveTarget(const CVector3& target) {
    m_moveTarget = target;
    m_hasMoveTarget = true;
}

void CAnimObject::SetNodeState(long node, long state) {
    m_skinned.SetGeomNodeState(node, state);
}

void CAnimObject::Draw(CDrawContext& context) {
    if (!g_config.noAnimObjects) {
        context.m_localToWorld = &m_tm;
        CVector3 min;
        CVector3 max;
        m_skinned.UpdateSkeletonMatrices(m_matrices, &min, &max);
        m_skinned.Draw(context);
        UpdateLocalBoundingVolume((ISceneNode::EVolumeType)3, min, max);
    }
}
