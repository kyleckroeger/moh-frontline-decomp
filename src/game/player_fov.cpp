// A fragment of player.cpp (0x8009d5f8): CPlayerObject::TransitionFOV starts
// moving the field of view (+1068) towards a new one, unless it is there
// already: the target is kept (+1076) and the rate (+1080) is the given speed,
// negated when the target is below the current field of view.
// UpdateDeathSequence runs the death timer (+952, out of 180): outside
// multiplayer it turns the view by cosine-shaped yaw and pitch steps, in
// multiplayer it blends the yaw and pitch from their starting values (+4796,
// +4800) towards the killer's (+1992) look direction; when the timer runs out
// it triggers script event 120, otherwise (single player) it fades the master
// volume. The class and functions are named by the symbols; the members are
// inferred from offsets, the virtual functions before TriggerScriptEvent are
// placeholders in ISceneNode's order (vtable not emitted here), and the Lerp
// helper is an inferred inline. The rest of the file is in other units.
extern "C" {
double cos(double);
double atan2(double, double);
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }

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
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class CSoldierObject {
public:
    void GetLookMatrix(CMatrix&);
};

extern bool g_bInMultiplayerMode;
void SoundSetMasterVolume(int);

class CPlayerObject {
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
    virtual void TriggerScriptEvent(int, void*, bool);
    void TransitionFOV(float, float);
    void UpdateDeathSequence(float);

    unsigned char unknown004[664];
    float m_yaw;
    float m_pitch;
    unsigned char unknown2a4[276];
    float m_deathTime;
    unsigned char unknown3bc[112];
    float m_fov;
    unsigned char unknown430[4];
    float m_targetFov;
    float m_fovRate;
    unsigned char unknown43c[908];
    CSoldierObject* m_killer;
    unsigned char unknown7cc[2800];
    float m_startYaw;
    float m_startPitch;
};

void CPlayerObject::TransitionFOV(float target, float speed) {
    if (m_fov == target)
        return;
    m_targetFov = target;
    if (m_targetFov < m_fov)
        m_fovRate = -speed;
    else
        m_fovRate = speed;
}

static inline float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void CPlayerObject::UpdateDeathSequence(float dt) {
    float time = m_deathTime / 180.0f;
    float left = 1.0f - time;
    float yaw = dt * (0.05f * (float)cos(3.1415927f * (2.5f * left)));
    float pitch = dt * ((0.035f * (float)cos(3.1415927f / (0.5f + left))) / (2.0f * left + 1.0f));
    m_deathTime -= dt;
    if (!g_bInMultiplayerMode) {
        m_yaw += yaw;
        m_pitch -= pitch;
    } else {
        CMatrix look;
        m_killer->GetLookMatrix(look);
        CVector3 front = look.front;
        float angle = atan2(front.x, front.y);
        m_yaw = Lerp(m_startYaw, angle, left);
        m_pitch = Lerp(m_startPitch, -3.1415927f * front.z, left);
    }
    if (m_deathTime <= 0.0f) {
        m_deathTime = 0.0f;
        TriggerScriptEvent(120, 0, false);
    } else if (!g_bInMultiplayerMode) {
        SoundSetMasterVolume((int)(127.0f * time));
    }
}
