// A fragment of player.cpp (0x8009d0f8): CPlayerObject::BeginCrouch, while
// the flag at bit 0x08 of +919 is set, sets the two crouch flags (bits 0x80
// and 0x40 of +917) and a downward height speed (+676), slower in
// multiplayer. CPlayerObject::BeginStand restores the standing capsule axis
// from a function-static CLine3 when the space above is clear. The class is
// named by the mangled symbol; its members and flag names are inferred from
// offsets in a non-virtual view. The rest of the file is not part of this
// unit; its earlier .bss is stood in for by the manifest's leading_filler.
extern bool g_bInMultiplayerMode;

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; }
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    CLine3(const CLine3& other) {
        m_start.d = other.m_start.d;
        m_end.d = other.m_end.d;
        m_dir.d = other.m_dir.d;
        m_value30 = other.m_value30;
        m_value34 = other.m_value34;
        m_flag38 = other.m_flag38;
        m_flag39 = other.m_flag39;
    }
    ~CLine3() {}
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start = other.m_start;
            m_end = other.m_end;
            m_dir = other.m_dir;
            m_value30 = other.m_value30;
            m_value34 = other.m_value34;
            m_flag38 = other.m_flag38;
            m_flag39 = other.m_flag39;
        }
        return *this;
    }
    void SetSE(CVector3 start, CVector3 end) {
        m_start.d = start.d;
        m_end.d = end.d;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class CMatrix {
public:
    unsigned char unknown00[64];
};

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[32];
};

class CVolCapsule;

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const class CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const class CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
};

class CVolCapsule : public IVolume {
public:
    CLine3& GetAxis() { return m_axis; }
    void SetAxis(const CLine3& axis) { m_axis = axis; }
    float GetRadius() const { return m_radius; }
    void SetRadius(float radius) { m_radius = radius; }
    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual IVolume* GetLocalBoundingVolume(int) const;
    virtual IVolume* GetWorldBoundingVolume(int) const;
};

class CScene {
public:
    unsigned char unknown00[24];
    ISceneNode* m_world;
};

extern CScene g_scene;

class CCDBObject {
public:
    static void DisablePassThru(bool);
};

class CPlayerObject {
public:
    void BeginCrouch();
    void BeginStand();

    unsigned char unknown000[128];
    CMatrix m_matrix;
    unsigned char unknown0c0[240];
    CVolCapsule m_localCapsule;
    CVolCapsule m_worldCapsule;
    unsigned char unknown250[84];
    float m_heightSpeed;
    unsigned char unknown2a8[237];
    bool m_crouching : 1;
    bool m_crouchStarted : 1;
    unsigned char m_flags395 : 6;
    unsigned char unknown396;
    unsigned char m_flags397a : 4;
    bool m_flag397b : 1;
    unsigned char m_flags397b : 3;
};

void CPlayerObject::BeginCrouch() {
    if (!m_flag397b)
        return;
    m_crouching = true;
    m_crouchStarted = true;
    m_heightSpeed = -(g_bInMultiplayerMode ? 0.02666667f : 0.07333334f);
}

void CPlayerObject::BeginStand() {
    CLine3 crouchAxis = m_localCapsule.GetAxis();
    float radius = m_localCapsule.GetRadius();
    float speed = g_bInMultiplayerMode ? 0.02666667f : 0.07333334f;
    static CLine3 standingAxis(CVector3(0.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 1.1f));
    m_localCapsule.SetAxis(standingAxis);
    m_localCapsule.m_radius = radius;
    m_worldCapsule.TransformedCopy(m_localCapsule, m_matrix);
    IVolume* world = g_scene.m_world->GetWorldBoundingVolume(0);
    CCollision collision;
    CCDBObject::DisablePassThru(true);
    int result = world->TestCollision(m_worldCapsule, collision, false);
    CCDBObject::DisablePassThru(false);
    m_localCapsule.SetAxis(crouchAxis);
    m_localCapsule.m_radius = radius;
    m_worldCapsule.TransformedCopy(m_localCapsule, m_matrix);
    if (result != 3) {
        m_crouching = false;
        m_crouchStarted = true;
        m_heightSpeed = speed;
    }
}
