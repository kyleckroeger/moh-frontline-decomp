// CTankObject::Draw and HandleBulletCollision (a fragment of Tank_object.cpp,
// between the weapon and update fragments): draw the tank, then each mapped
// weapon's muzzle flash while it is showing, at the sub-object's attach point
// with a random turn about Y (scaled by 3 for weapon type 44); and, for a
// projectile hit, spawn the hit particle systems at the collision point
// before the static object's handling. The names come from the mangled
// symbols; CStaticObject's virtual functions are placed at their
// __vt__13CStaticObject offsets (Draw at +48, SetTMLocalToWorld at +232),
// CBullet's AsProjectile at +320 of __vt__7CBullet and CParticleSystem's
// SetLocalToWorld at +140 of __vt__15CParticleSystem (vtable pointer after 28
// bytes of members), with the other entries as placeholders named by offset.
// The weapon's fields, the collision's hit view and the muzzle-flash getter
// are inferred; CTankObject declares its destructor first so this file does
// not emit its vtable.
class CCollision;
class CDrawContext;
class CProjectileBullet;

/* inferred views */
struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

#include "game/CVector3.h"


#include "game/CMatrix.h"


struct HITVIEW {
    unsigned char unknown00[16];
    CVector3 point;
};

struct COLLISIONVIEW {
    unsigned char unknown00[24];
    HITVIEW* hit;
};

class CBullet {
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
    virtual CProjectileBullet* AsProjectile();
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

unsigned char MathFunTestPercent(long long, long long);
long long MathFunGetRandomPercent();

float MathFunRandomReal(float, float);

class CStaticObject {
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
    virtual void Draw(CDrawContext&);
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
    virtual void SetTMLocalToWorld(const CMatrix&);
    void HandleBulletCollision(CBullet*, const CCollision&);
};

class CHierObject : public CStaticObject {
public:
    void Draw(CDrawContext&);
    CHierObject* GetSubObject(int);
    void GetAttachPointMatrix(int, CMatrix&);

    unsigned char unknown004[60];
    CMatrix m_tm;
};

class CWeapon {
public:
    bool GetMuzzleFlashStatus();
    CStaticObject* GetMuzzleFlash() { return m_muzzleFlash; }

    unsigned char unknown000[664];
    int m_type;
    unsigned char unknown29C[40];
    float m_flashTime;
    unsigned char unknown2C8[4];
    CStaticObject* m_muzzleFlash;
};

class CTankObject : public CHierObject {
public:
    virtual ~CTankObject();
    void Draw(CDrawContext&);
    void HandleBulletCollision(CBullet*, const CCollision&);

    unsigned char unknown080[1948 - 128];
    int m_weaponSlot[8];
    CWeapon* m_weapons[3];
};

void CTankObject::Draw(CDrawContext& context) {
    int i;
    int slot;
    CStaticObject* flash;

    CHierObject::Draw(context);
    for (i = 0; i <= 7; i++) {
        slot = m_weaponSlot[i];

        if (slot >= 0 && slot < 3 && m_weapons[slot]) {
            flash = m_weapons[slot]->GetMuzzleFlash();

            if (flash && m_weapons[slot]->GetMuzzleFlashStatus() && !((int)m_weapons[slot]->m_flashTime & 3)) {
                CMatrix tm;
                tm.Ident();
                CMatrix attach;
                attach.Ident();
                float angle = MathFunRandomReal(0.0f, 6.28f);
                CHierObject* sub = GetSubObject(i);

                if (sub) {
                    tm = sub->m_tm;
                    sub->GetAttachPointMatrix(0xe6e804d5, attach);
                    tm.Multiply(attach, tm);
                    tm.RotateY(angle);
                    if (m_weapons[slot]->m_type == 44) {
                        CMatrix scale;

                        scale.Ident();
                        scale.Scale(3.0f);
                        tm.Multiply(scale, tm);
                    }
                    flash->SetTMLocalToWorld(tm);
                    flash->Draw(context);
                }
            }
        }
    }
}

void CTankObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    if (bullet->AsProjectile()) {
        CMatrix tm;
        HITVIEW* hit = ((const COLLISIONVIEW&)collision).hit;
        CParticleSystem* system;
        int i;

        tm.Ident();
        tm.SetPos(hit->point);
        if (MathFunTestPercent(MathFunGetRandomPercent(), 30)) {
            system = CParticleSystem::Create(0xe9ac3f2e);
            if (system)
                system->SetLocalToWorld(tm);
        }
        for (i = 0; i < 2; i++) {
            if (MathFunTestPercent(MathFunGetRandomPercent(), 65))
                system = CParticleSystem::Create(0xace69e6e);
            else
                system = CParticleSystem::Create(0xc74092f9);
            if (system)
                system->SetLocalToWorld(tm);
        }
    }
    CStaticObject::HandleBulletCollision(bullet, collision);
}
