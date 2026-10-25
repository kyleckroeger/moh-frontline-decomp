// A fragment of decal.cpp (0x8008397c): CBulletDecalManager::Reset (two
// scene-node words cleared, the manager added to the scene again, no decal
// active and every texture bin emptied). The file name is this project's;
// the original record is decal.cpp. The classes, functions and g_scene are
// named by the mangled symbols (CRenderBinData by CRenderBin's RTTI record);
// the members are inferred views (as in decal_dtor.cpp and decal_link.cpp).
// Each class declares its destructor (defined in decal_dtor.cpp) first so
// its table is not emitted here.
class CDmaTag;
class CDmaPacket;
class CRenderList;
class CBullet;
class CCollision;
class CDrawContext;
class CLight;
class CPlayerObject;
class CStaticObject;
class CMatrix;
class CVector3;
struct AIDoodadView;
enum EClsnId {};

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
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

/* ISceneNode's virtuals in table order (slots 20 to 216). They are the weak
   inline defaults of Moh2.cpp in the original; their bodies are not repeated
   here, so they are declared without one. */
class ISceneNode : public IObserver {
public:
    enum EVolumeType {};
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}
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
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
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
    virtual AIDoodadView* GetAIDoodad();
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

class CRenderBin;

/* CRenderBin's non-polymorphic base, named by CRenderBin's RTTI record; the
   members are inferred (CRenderBin::Init walks children from +8 through the
   siblings at +4, IsUsed tests +16, +20 is the render priority, 3 by
   default). */
struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. Init's and IsUsed's bodies are not part of this view, so they
   are declared without one, which also keeps CRenderBin's table out of this
   unit. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    CUcodeRenderBin() {}
    virtual ~CUcodeRenderBin() {}
    virtual void Link(CDmaTag*, CDmaPacket&);
};


struct SurfaceInfo {
    unsigned char unknown00[12];
    short m_type : 11;
    short unknown0c : 5;
};

struct SClsnContact {
    unsigned char unknown00[36];
    SurfaceInfo* m_surface;
};

class CCollision {
public:
    unsigned char unknown00[24];
    SClsnContact* m_contact;
};

class CSurfaceType {
public:
    static int GetDecalIndex(int);
};

/* Only Load and the 64-byte size are part of this view. */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[64];
};

/* Inferred: a decal's four vertices (position, colour, texture coordinates)
   and the next decal of its bin. */
struct SDecalVertex {
    float x, y, z;
    unsigned char r, g, b, a;
    float u, v;
};

struct BulletDecal {
    SDecalVertex m_vertices[4];
    BulletDecal* m_next;
    int m_bin;
};

class CBulletDecalTextureBin : public CUcodeRenderBin {
public:
    virtual ~CBulletDecalTextureBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    int m_count;
    BulletDecal* m_decals;
    CTexture m_texture;
    int unknown68;
};

class CBulletDecalManager : public ISceneNode, public CUcodeRenderBin {
public:
    virtual ~CBulletDecalManager();
    virtual void Draw(CDrawContext&);
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    void InitBulletDecalFromCollision(BulletDecal*, const SClsnContact*, bool);
    void AddBulletDecal(const CCollision&, bool);
    void Reset();

    CBulletDecalTextureBin m_bins[13];
    BulletDecal* m_decals;
    int m_active;
};


/* Only Add and the object's size are part of this view. */
class CScene {
public:
    void Add(ISceneNode&);

    unsigned char unknown00[0x148];
};

extern CScene g_scene;

void CBulletDecalManager::AddBulletDecal(const CCollision& collision, bool flag) {
    SClsnContact* contact = collision.m_contact;
    if (contact) {
        BulletDecal* decal;
        if (m_active >= 512) {
            decal = &m_decals[m_active & 511];
            CBulletDecalTextureBin& bin = m_bins[decal->m_bin];
            decal = &m_decals[m_active & 511];
            bin.m_count--;
            BulletDecal* d = bin.m_decals;
            if (decal == d) {
                bin.m_decals = 0;
            } else {
                for (; d;) {
                    BulletDecal* next = d->m_next;
                    if (next == decal) {
                        d->m_next = next->m_next;
                        break;
                    }
                    d = next;
                }
            }
        } else {
            decal = &m_decals[m_active];
        }
        m_active++;
        InitBulletDecalFromCollision(decal, contact, flag);
        int index = CSurfaceType::GetDecalIndex(contact->m_surface->m_type);
        decal->m_next = m_bins[index].m_decals;
        m_bins[index].m_decals = decal;
        m_bins[index].m_count++;
    }
}

void CBulletDecalManager::Reset() {
    data08 = 0;
    data04 = 0;
    g_scene.Add(*this);
    m_active = 0;
    for (CBulletDecalTextureBin* bin = (CBulletDecalTextureBin*)m_children; bin; bin = (CBulletDecalTextureBin*)bin->m_next) {
        bin->m_decals = 0;
        bin->m_count = 0;
    }
}
