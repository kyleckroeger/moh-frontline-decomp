// A fragment of animplayer.cpp (0x8010a39c): CAnimatedPlayer::Draw draws the
// player as a soldier unless the draw context's value at +0x114 is the one
// held by the player object at +0x44b0 (at its +0x768). HandleBulletCollision
// lets the player's own bullets through (the scene excludes the pair, type
// 5, and the bullet penetrates); a projectile hit places a blood particle
// system at the contact, oriented to its normal as in
// Soldier_object_bullet.cpp, and the hit is passed to the player. The
// classes and functions are named by the mangled symbols; the members and
// views are inferred, CAnimatedPlayer is a non-virtual view deriving from
// CSoldierObject, and the vector helpers and the inline frsqrte sqrtf are
// as in Soldier_object_bullet.cpp. The rest of the file is in other units.
class CDrawContext {
public:
    unsigned char unknown000[0x114];
    int m_value114;
};

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

class CVector3 {
public:
    CVector3() {}
    CVector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }

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
    void Ident();
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

/* inferred: a collision contact: normal and position */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_position;
};

class CCollision {
public:
    unsigned char unknown00[24];
    ContactView* m_contacts;
};

class ISceneNode;
class CPlayerObject;
struct BSObject;
class CThrownBullet;
class CProjectileBullet;

class ISceneNode {
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
    virtual BSObject* GetScriptObject() const;
};

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual void Render();
    virtual void Init();
    virtual void Link();
    virtual int IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    virtual ~CParticleSystem();
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
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void SetLocalToWorld(const CMatrix&);
    static CParticleSystem* Create(unsigned long);
};

class CBullet {
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
    virtual void GetForward(CVector3&) const;
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
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&) const;
    virtual void vf148();
    virtual void Penetrate(const CCollision&);
};

class CScene {
public:
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);

    unsigned char unknown000[328];
};

extern CScene g_scene;

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
    virtual void vf064();
    virtual void HandleBulletCollision(CBullet*, const CCollision&);

    unsigned char unknown004[0x764];
    int m_value768;
};

class CSoldierObject {
public:
    void Draw(CDrawContext&);
};

class CAnimatedPlayer : public CSoldierObject {
public:
    void Draw(CDrawContext&);
    void HandleBulletCollision(CBullet*, const CCollision&);

    unsigned char unknown0000[0x44b0];
    CPlayerObject* m_player;
};

void CAnimatedPlayer::Draw(CDrawContext& context) {
    if (context.m_value114 != m_player->m_value768)
        CSoldierObject::Draw(context);
}

void CAnimatedPlayer::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    if (m_player == (CPlayerObject*)bullet->GetFiredBy()) {
        g_scene.ExcludeCollision(*(ISceneNode*)this, *(ISceneNode*)bullet, 5);
        bullet->Penetrate(collision);
        return;
    }
    if (bullet->AsProjectile()) {
        ContactView* contact = collision.m_contacts;
        CParticleSystem* blood = CParticleSystem::Create(0x6da45fe8);
        if (blood) {
            CMatrix matrix;
            matrix.Ident();
            CVector3 right;
            CVector3 front;
            if ((float)__fabs(contact->m_normal.Dot(CVector3(1.0f, 0.0f, 0.0f))) <= 0.9999f) {
                front.Cross(contact->m_normal, CVector3(1.0f, 0.0f, 0.0f));
                front.Normalize();
                right.Cross(front, contact->m_normal);
                right.Normalize();
            } else {
                right.Cross(contact->m_normal, CVector3(0.0f, 0.0f, 1.0f));
                right.Normalize();
                front.Cross(contact->m_normal, right);
                front.Normalize();
            }
            matrix.SetRight(right);
            matrix.SetFront(front);
            matrix.SetUp(contact->m_normal);
            matrix.SetPos(contact->m_position);
            blood->SetLocalToWorld(matrix);
        }
    }
    m_player->HandleBulletCollision(bullet, collision);
}
