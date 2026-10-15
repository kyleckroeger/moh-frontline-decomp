// A fragment of projectilebullet.cpp (0x800d0c40): CProjectileBullet's
// GetVelocity (the direction at +96 scaled by the speed at +180),
// SetExclusionPair (re-includes the collision with the previous excluded node
// in g_scene, then stores the new one), GetScriptBulletType (maps the
// projectile type at +164 to the script's bullet type, with a warning for an
// unknown type) and IsVisible (the second fiber volume's visibility test).
// The classes, functions and globals are named by the mangled symbols; the
// members and their names are inferred views (non-virtual here; the volume's
// TestVisibility is IVolume's slot +68), and the type numbers are not named.
// The rest of the file is not part of this unit.
class CDrawContext;
class CBullet;
class CScene;
class ISceneNode;

void DebugMsg(const char*, ...);

/* inferred: CVector3 as four floats (copies move doubleword pairs) */
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CScene {
public:
    void IncludeCollision(const ISceneNode&, const ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
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
    virtual bool TestVisibility(CDrawContext&) const;
};

class CVolFiber : public IVolume {
public:
    unsigned char unknown04[76];
};

class CProjectileBullet {
public:
    void GetVelocity(CVector3&);
    void SetExclusionPair(ISceneNode*);
    int GetScriptBulletType() const;
    bool IsVisible(CDrawContext&) const;

    unsigned char unknown000[96];
    CVector3 m_direction;
    unsigned char unknown070[52];
    unsigned int m_type;
    unsigned char unknown0a8[8];
    ISceneNode* m_excluded;
    float m_speed;
    unsigned char unknown0b8[8];
    CVolFiber m_fiberA;
    CVolFiber m_fiberB;
};

void CProjectileBullet::GetVelocity(CVector3& velocity) {
    CVector3 direction = m_direction;
    float speed = m_speed;
    velocity.x = speed * direction.x;
    velocity.y = speed * direction.y;
    velocity.z = speed * direction.z;
}

void CProjectileBullet::SetExclusionPair(ISceneNode* node) {
    if (m_excluded)
        g_scene.IncludeCollision(*m_excluded, *(ISceneNode*)this);
    m_excluded = node;
}

int CProjectileBullet::GetScriptBulletType() const {
    switch (m_type) {
    case 0:
    case 1:
        return 10;
    case 2:
    case 3:
        return 0;
    case 4:
    case 5:
        return 4;
    case 6:
        return 12;
    case 7:
    case 8:
        return 13;
    case 9:
        return 11;
    case 10:
    case 13:
        return 5;
    case 11:
        return 14;
    case 12:
        return 15;
    case 14:
    case 15:
        return 6;
    case 16:
    case 17:
        return 1;
    case 18:
    case 19:
        return 16;
    case 20:
    case 21:
        return 19;
    case 22:
    case 23:
        return 20;
    case 24:
    case 25:
        return 2;
    case 26:
    case 27:
        return 18;
    case 28:
        return 17;
    case 29:
        return 27;
    default:
        DebugMsg("WARNING: Unknown proj bullet type in GetScriptBulletType.  Type %d\n", m_type);
        return 4;
    }
}

bool CProjectileBullet::IsVisible(CDrawContext& context) const {
    return m_fiberB.TestVisibility(context);
}
