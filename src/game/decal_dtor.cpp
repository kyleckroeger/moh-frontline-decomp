// A fragment of decal.cpp (0x80083c50): the CBulletDecalManager destructor and
// default constructor, then the CBulletDecalTextureBin destructor and default
// constructor. The manager is a scene node with a render bin as its second
// base (CUcodeRenderBin at +36, its table pointer at +64; the secondary table
// follows ISceneNode's 53 primary slots) and holds 13 texture bins of 108
// bytes at +68. Its destructor destroys the bins through the array
// destructor, then the render-bin and scene-node bases (inline destructors);
// its constructor runs both bases' inline constructors, sets the table
// pointers and builds the bins through the array constructor. The texture
// bin's destructor resets its table pointer through the inline
// CUcodeRenderBin and CRenderBin destructors; its constructor runs
// CRenderBinData's inline constructor (link words cleared, priority 3) and
// clears the word at +36. The file name is this project's; the original
// record is decal.cpp. CRenderBinData is named by CRenderBin's RTTI record;
// the other classes and functions are named by the mangled symbols; the
// members are inferred. Weak virtuals whose bodies are not part of this view
// (the scene-node defaults of Moh2.cpp, CRenderBin's Init and IsUsed) are
// declared without one, which also keeps those weak tables out of this unit;
// CBulletDecalManager declares Draw and the texture bin Render (their own
// overrides, defined elsewhere) first so their global tables are not emitted
// here. The compiler's copies of the bases' weak inline destructors are weak
// duplicates, linked to the original copies.
class CDmaTag;
class CDmaPacket;
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
    CRenderBinData() : m_prev(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    int m_prev;
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

class CBulletDecalTextureBin : public CUcodeRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CBulletDecalTextureBin();
    CBulletDecalTextureBin();

    int data20;
    int data24;
    unsigned char unknown28[68];
};

class CBulletDecalManager : public ISceneNode, public CUcodeRenderBin {
public:
    virtual void Draw(CDrawContext&);
    virtual ~CBulletDecalManager();
    CBulletDecalManager();

    CBulletDecalTextureBin m_bins[13];
};

CBulletDecalManager::~CBulletDecalManager() {
}

CBulletDecalManager::CBulletDecalManager() {
}

CBulletDecalTextureBin::~CBulletDecalTextureBin() {
}

CBulletDecalTextureBin::CBulletDecalTextureBin() {
    data24 = 0;
}
