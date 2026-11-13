// Tank_object.cpp: CTankObject, the whole file, with the class's vtable and the RTTI
// records of its chain (IDestructible .. CTankObject): the AI-doodad accessors and the
// hierarchical-tank cast (inline, emitted weak at the start of the file); firing a mapped
// weapon from its sub-object's attach point, the weapon slot map, adding a slot's weapon
// through the weapon factory and the look matrix; drawing the tank and each mapped weapon's
// muzzle flash, and the projectile-hit particle systems; the update hooks, the clean-up,
// the destructor and the two constructors.
//
// The names come from the mangled symbols; the class chain and the vtable order from
// __RTTI__11CTankObject and __vt__11CTankObject (91 slots: ISceneNode's and
// IMovingSceneNode's from the shared headers, CStaticObject's from +0x124, CHierObject's
// CleanUp and Begin*MotionUpdate, then CTankObject's InitAI). Which class first declares
// a slot follows __vt__13CStaticObject and __vt__11CHierObject. The bodies of the inline
// base virtuals follow the matched units that keep their weak copies (attachobject,
// hierobject_weak, bsbifunc_distance, bsbifunc_mmg, objcreate_setscript_static,
// bsbifunc_particle, bsbifunc_asweapon, bsbifunc_hierfire).
//
// Inferred, not in any mangled name: the data members (named by offset, as the fragments'
// views did: the script object at +0x118, the collision id at +0x1dc, the flag byte at
// +0x1e0, the attached light at +0x24c, the matrix at +0x40, the doodad at +0x740, the
// look sub-object at +0x794), the result types, and the views of CWeapon, CBullet,
// CParticleSystem, IVolume and the collision's hit. CTankObject declares its destructor
// first (the key function). The unit is compiled -inline auto,deferred, which emits the
// functions in reverse source order, so they are written last to first; the weak copies of
// the header inlines that the vtable references are weak duplicates of the original's.
#include "game/ISceneNode.h"
#include "game/IMovingSceneNode.h"

class CProjectileBullet;
class CStaticMesh;
class CThrownObject;
class CWeapon;
class CTankObject;
enum EBSEventEnum {};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

/* inferred views */
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

class CStaticObject : public IMovingSceneNode {
public:
    struct MotionFrame;

    virtual void MarkForDestruction(int);
    virtual ~CStaticObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const { return m_drawEnabled; }
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId id) { m_collisionId = id; }
    virtual BSObject* GetScriptObject() const { return m_script; }
    virtual void TriggerScriptEvent(int event, void* data, bool flag) {
        BSObjectTriggerEvent(m_script, event, data, 0, flag);
    }
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CStaticObject* AsStaticObject() { return this; }
    virtual const CStaticObject* AsStaticObject() const { return this; }
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const { return m_attachedLight; }
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject* script) { m_script = script; }
    virtual int GetObjectType() const;
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual CThrownObject* AsThrownObject() { return 0; }
    virtual CWeapon* AsWeaponObject() { return 0; }
    virtual CTankObject* AsHierTankObject() { return 0; }
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void AttemptPathMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CommitPathMotionUpdate();

    unsigned char unknown024[28];
    CMatrix m_tm;
    unsigned char unknown080[152];
    BSObject* m_script;
    unsigned char unknown11c[192];
    EClsnId m_collisionId;
    unsigned char m_drawEnabled : 1;
    unsigned char unknown1E0b40 : 6;
    unsigned char m_flag1E0 : 1;
    unsigned char unknown1e1[107];
    CLight* m_attachedLight;
};

class CAttachableObject : public CStaticObject {
public:
    virtual ~CAttachableObject();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual int GetObjectType() const { return 1; }
};

class CHierObject : public CAttachableObject {
public:
    CHierObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CHierObject();
    virtual ~CHierObject();
    virtual void BeginUpdate(float);
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual CHierObject* AsHierObject() { return this; }
    virtual const CHierObject* AsHierObject() const { return this; }
    virtual CVector3 GetWorldLinearVelocity() const;
    virtual BPDLightVolume* GetLightVolume();
    virtual void Init() { CStaticObject::Init(); }
    virtual int GetObjectType() const { return 8; }
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual void AttemptScriptMotionUpdate(float);
    virtual void AttemptAnimMotionUpdate(float);
    virtual void CommitScriptMotionUpdate();
    virtual void CommitAnimMotionUpdate();
    virtual void CleanUp();
    virtual void BeginScriptMotionUpdate(float);
    virtual void BeginAnimMotionUpdate(float);

    CHierObject* GetSubObject(int);
    void GetAttachPointMatrix(int, CMatrix&);

    unsigned char unknown250[1264];
};

/* inferred views */
struct WEAPONPROPSVIEW {
    unsigned char unknown00[14];
    short field0E;
};

enum EWeaponShootType {};

class CWeapon {
public:
    void Shoot(EWeaponShootType, float, float);
    void Update(float, bool);
    bool GetMuzzleFlashStatus();
    CStaticObject* GetMuzzleFlash() { return m_muzzleFlash; }

    unsigned char unknown000[640];
    WEAPONPROPSVIEW* m_properties;
    unsigned char unknown284[16];
    int m_mode;
    int m_type;
    unsigned char unknown29C[4];
    CVector3 m_direction;
    CVector3 m_position;
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flag20 : 1;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_fromTank : 1;
    unsigned char unknown2C1[3];
    float m_flashTime;
    unsigned char unknown2C8[4];
    CStaticObject* m_muzzleFlash;
};

class CWeaponFactory {
public:
    CWeapon* CreateWeaponByCRC(int, ISceneNode*, int);
    void DestroyWeapon(CWeapon*);

    unsigned char unknown00[28];
};

class CAIFilterGlobal {
public:
    void EnableAllAStarPathNodes();
    void DisableCollidingAStarPathNodes(const IVolume*);

    unsigned char unknown00[24];
};

class IVolume {
public:
    virtual void Unknown08();
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();

    unsigned char unknown04[28];
};

class CAITankDoodad {
public:
    CAITankDoodad();
    ~CAITankDoodad();
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CTankObject*);

    unsigned char unknown00[84];
};

extern CWeaponFactory g_WeaponFactory;
extern CAIFilterGlobal g_aigAIFilterGlobalObject;

extern "C" void* memset(void*, int, unsigned long);

class CTankObject : public CHierObject {
public:
    virtual ~CTankObject();
    CTankObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CTankObject();

    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CAIDoodad* GetAIDoodad() { return (CAIDoodad*)&m_aiDoodad; }
    virtual const CAIDoodad* GetAIDoodad() const { return (const CAIDoodad*)&m_aiDoodad; }
    virtual CTankObject* AsHierTankObject() { return this; }
    virtual void CleanUp();
    virtual void InitAI();

    void FireSubObject(int);
    void MapWeaponSlot(int, int);
    bool AddWeapon(int, int);
    void GetLookMatrix(CMatrix&);
    void VerifyAIDataSize();

    CAITankDoodad m_aiDoodad;
    int m_lookSubObject;
    int m_pathNodeTimer;
    int m_weaponSlot[8];
    CWeapon* m_weapons[3];
    unsigned char unknown7C8[8];
    CVolSphere m_sphere0;
    CVolSphere m_sphere1;
};

void CTankObject::VerifyAIDataSize() {
}

CTankObject::CTankObject() {
    memset(m_weaponSlot, -1, sizeof(m_weaponSlot));
    memset(m_weapons, 0, sizeof(m_weapons));
    m_lookSubObject = 0;
    m_pathNodeTimer = 0;
    m_flag1E0 = 1;
}

CTankObject::CTankObject(CStaticMesh* mesh, int a, int b, int c, int d, int e, CStaticObject::MotionFrame* frames)
    : CHierObject(mesh, a, b, c, d, e, frames) {
    memset(m_weaponSlot, -1, sizeof(m_weaponSlot));
    memset(m_weapons, 0, sizeof(m_weapons));
    m_lookSubObject = 0;
    m_pathNodeTimer = 0;
    m_flag1E0 = 1;
}

void CTankObject::InitAI() {
    m_aiDoodad.Init(this);
}

void CTankObject::CleanUp() {
    int i;

    g_aigAIFilterGlobalObject.EnableAllAStarPathNodes();
    m_aiDoodad.CleanUp();
    for (i = 0; i < 3; i++) {
        if (m_weapons[i])
            g_WeaponFactory.DestroyWeapon(m_weapons[i]);
    }
    CHierObject::CleanUp();
}

CTankObject::~CTankObject() {
}

void CTankObject::BeginUpdate(float dt) {
    int i;

    CHierObject::BeginUpdate(dt);
    for (i = 0; i < 3; i++) {
        if (m_weapons[i])
            m_weapons[i]->Update(dt, false);
    }
    m_aiDoodad.PreUpdate();
}

void CTankObject::UpdateAI(float dt) {
    m_aiDoodad.Update(dt);
    if (m_pathNodeTimer <= 0) {
        g_aigAIFilterGlobalObject.EnableAllAStarPathNodes();
        g_aigAIFilterGlobalObject.DisableCollidingAStarPathNodes(GetWorldBoundingVolume((EVolumeType)0));
        m_pathNodeTimer = 60;
    } else {
        m_pathNodeTimer -= (int)dt;
    }
}

void CTankObject::AttemptUpdate(float dt) {
    CHierObject::AttemptUpdate(dt);
}

void CTankObject::CommitUpdate() {
    CHierObject::CommitUpdate();
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

void CTankObject::GetLookMatrix(CMatrix& matrix) {
    if (m_lookSubObject) {
        matrix = GetSubObject(m_lookSubObject)->m_tm;
    } else {
        CVector3 pos;

        matrix = m_tm;
        pos = matrix.GetPos();
        pos.z += 1.0f;
        matrix.SetPos(pos);
    }
}

bool CTankObject::AddWeapon(int slot, int crc) {
    if (crc == (int)0xfb9630c9) {
        m_weapons[slot] = 0;
        return false;
    }
    m_weapons[slot] = g_WeaponFactory.CreateWeaponByCRC(crc, (ISceneNode*)this, 999);
    if (m_weapons[slot]) {
        if (m_weapons[slot]->m_properties->field0E)
            m_weapons[slot]->m_mode = 4;
        else
            m_weapons[slot]->m_mode = 10;
        return true;
    }
    return false;
}

void CTankObject::MapWeaponSlot(int subObject, int slot) {
    m_weaponSlot[subObject] = slot;
}

void CTankObject::FireSubObject(int subObject) {
    CMatrix tm;
    CMatrix attach;
    CHierObject* sub;
    int slot = m_weaponSlot[subObject];

    if (slot >= 0 && slot < 3) {
        sub = GetSubObject(subObject);

        m_weapons[slot]->m_fromTank = 1;
        tm = sub->m_tm;
        sub->GetAttachPointMatrix(0xe6e804d5, attach);
        tm.Multiply(attach, tm);
        m_weapons[slot]->m_direction = tm.GetFront();
        m_weapons[slot]->m_position = tm.GetPos();
        m_weapons[slot]->Shoot((EWeaponShootType)2, 1.0f, -1.0f);
    }
}
