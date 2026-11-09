// The 8-byte adjustor thunks the compiler generates for multiple inheritance, as
// partial source: "addi r3, r3, -N; b target". They exist because the second base
// class sits at offset N, and they have no logic of their own. This minimal layout
// makes the compiler emit them; the real classes are in the matching source.
class CDrawContext;
class CDmaTag;
class CDmaPacket;
class CRenderBin32 {
public:
    int m_fields[7];
    virtual ~CRenderBin32();
};
class ISceneNode32 {
public:
    virtual ~ISceneNode32();
    virtual bool IsDrawEnabled() const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual void Draw(CDrawContext&);
    virtual void BeginUpdate(float);
};
class CParticleSystemManager : public CRenderBin32, public ISceneNode32 {
public:
    virtual ~CParticleSystemManager();
    virtual bool IsDrawEnabled() const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual void Draw(CDrawContext&);
    virtual void BeginUpdate(float);
};
CParticleSystemManager::~CParticleSystemManager() {}

class ISceneNode36 {
public:
    int m_fields[8];
    virtual ~ISceneNode36();
};
class CRenderBin36 {
public:
    virtual ~CRenderBin36();
    virtual bool IsUsed();
    virtual void Link(CDmaTag*, CDmaPacket&);
};
class CBulletDecalManager : public ISceneNode36, public CRenderBin36 {
public:
    virtual ~CBulletDecalManager();
    virtual bool IsUsed();
    virtual void Link(CDmaTag*, CDmaPacket&);
};
CBulletDecalManager::~CBulletDecalManager() {}
