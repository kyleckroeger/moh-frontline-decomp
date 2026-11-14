// The end of particlesystem.cpp (0x8008128c): weak copies of header inlines
// emitted in this file: CParticleSystem::IsEmitting (a flag bit at +136) and
// the empty default setters (render type, texture, emission rate and delay,
// lifetimes, particle colour, size, position, velocity and acceleration, fog
// and system acceleration), then the static initialisation of the file's
// matrices (each inline constructor initialises the matrix class once; the
// model-to-screen matrix is set to the identity), the particle system manager
// (a CUcodeRenderBin with ISceneNode as a second base at +32: base tables,
// members cleared, priority 7, destructor registered) and the camera forward
// vector (0, 0, -1; its constants are entries of the file's .sdata2 pool).
// CParticleSystem, ShapeFile, CColor, CVector3, ERenderType and the globals
// are named by the mangled symbols; CParticleSystem is a non-virtual view and
// its flag bit-field, the manager's members, the vector constructor and the
// identity-matrix constructor are inferred. The weak this-adjusting thunks
// after the initialiser (for the ISceneNode base at +32: IsDrawEnabled,
// IsVisible, Draw, BeginUpdate and the destructor) belong to the manager's
// table, which is emitted with its key function IsUsed; IsUsed is defined here
// only for that and is placed by particlesystem_manager_init.cpp (a file
// duplicate, like the table). ISceneNode's virtual functions are named by its table.
struct ShapeFile;

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

#include "game/CVector3.h"

class CParticleSystem {
public:
    enum ERenderType {};

    bool IsEmitting() const;
    void SetRenderType(ERenderType);
    void SetTexture(ShapeFile*);
    void SetEmmisionRate(float);
    void SetEmmisionDelay(float);
    void SetSystemLifetime(float);
    void SetParticleLifetime(float);
    void SetParticleColor(CColor, CColor);
    void SetParticleSize(CVector3, CVector3, float);
    void SetParticlePosition(CVector3, CVector3);
    void SetParticleVelocity(CVector3, CVector3);
    void SetParticleAcceleration(CVector3);
    void SetFogEnable(bool);
    void SetSystemAcceleration(CVector3);

    unsigned char unknown000[136];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

__declspec(weak) bool CParticleSystem::IsEmitting() const {
    return m_emitting;
}

__declspec(weak) void CParticleSystem::SetRenderType(ERenderType) {
}

__declspec(weak) void CParticleSystem::SetTexture(ShapeFile*) {
}

__declspec(weak) void CParticleSystem::SetEmmisionRate(float) {
}

__declspec(weak) void CParticleSystem::SetEmmisionDelay(float) {
}

__declspec(weak) void CParticleSystem::SetSystemLifetime(float) {
}

__declspec(weak) void CParticleSystem::SetParticleLifetime(float) {
}

__declspec(weak) void CParticleSystem::SetParticleColor(CColor, CColor) {
}

__declspec(weak) void CParticleSystem::SetParticleSize(CVector3, CVector3, float) {
}

__declspec(weak) void CParticleSystem::SetParticlePosition(CVector3, CVector3) {
}

__declspec(weak) void CParticleSystem::SetParticleVelocity(CVector3, CVector3) {
}

__declspec(weak) void CParticleSystem::SetParticleAcceleration(CVector3) {
}

__declspec(weak) void CParticleSystem::SetFogEnable(bool) {
}

__declspec(weak) void CParticleSystem::SetSystemAcceleration(CVector3) {
}

class CDmaTag;
class CDmaPacket;
class CDrawContext;
class CCollision;
class CBullet;
class CLight;
struct BSObject;
enum EClsnId {};

#include "game/CMatrix.h"


class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual void MarkForDestruction(int);
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}
    enum EVolumeType {};
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual void* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual void* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CRenderBinData {
public:
    CRenderBinData() : m_prev(0), m_next(0), m_children(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    CRenderBinData* m_prev;
    CRenderBinData* m_next;
    CRenderBinData* m_children;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

// No Render of its own: the manager's table (0x801848d4) lists CRenderBin::Render.
// Link is declared (its weak default is in compartment_bins.cpp) so that this class's
// weak table is not emitted here.
class CUcodeRenderBin : public CRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual ~CUcodeRenderBin() {}
};

class CParticleSystemManager : public CUcodeRenderBin, public ISceneNode {
public:
    virtual bool IsUsed();
    virtual void Init();
    CParticleSystemManager()
        : data54(0), data58(0), data5c(0), data60(0), data64(0), data68(0), data6c(0), data70(0), data74(0),
          data78(0) {
        m_priority = 7;
    }
    virtual ~CParticleSystemManager();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual void BeginUpdate(float);

    int unknown44[4];
    int data54;
    int data58;
    int data5c;
    int data60;
    int data64;
    int data68;
    int data6c;
    int data70;
    int data74;
    int data78;
};

/* The manager's key function (declared first: inferred); its matched copy
   is placed by particlesystem_manager_init.cpp. Defining it here emits the
   table and the this-adjusting thunks after the initialiser. */
bool CParticleSystemManager::IsUsed() {
    return m_children != 0;
}

static CMatrix g_ModelToScreenMatrix(true);
static CVector3 g_XAxis;
static CVector3 g_YAxis;
static CVector3 g_ZAxis;
static CMatrix g_WorldToCamera;
static CParticleSystemManager g_ParticleSystemManager;
static CVector3 vCameraForward(0.0f, 0.0f, -1.0f);
