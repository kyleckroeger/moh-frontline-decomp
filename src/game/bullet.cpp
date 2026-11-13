// bullet.cpp: CBullet, the base of every bullet in the scene. The whole file,
// with the class's virtual table and RTTI record (slot order from
// __vt__7CBullet; the destructor is the key function). First the weak copies
// of the class's inline virtuals that the table needs: IsDrawEnabled (true),
// GetRightward, GetUp and GetTMLocalToWorld (each copies a row or the whole
// transform, a CMatrix at +80) and both AsBullet casts (itself). Then Draw
// (empty); Shutdown (the float at +144 cleared, -1 at +36 and FLT_MAX at
// +148); Init (the transform reset to identity, Shutdown's stores through a
// direct call the compiler inlines, two words cleared); the destructor (its
// virtual table pointer, then the inline IMovingSceneNode and ISceneNode
// destructors and IObserver's, and the object freed when asked) and the
// default constructor (the scene-node base constructors: table pointers and
// eight cleared words; the transform's class initialisation; the word at +40
// cleared).
// The classes and functions are named by the mangled symbols; the members
// and their names, the result types of the bullet's own virtuals and the
// by-value row getters are inferred. GetPosition and GetForward are inline
// too, but their weak copies come from other files. IMovingSceneNode's and
// ISceneNode's inline virtuals and destructors give weak copies here (their
// tables are weak in the original); they are weak duplicates, linked to the
// original copies. The float constants are items of the file's .sdata2 pool.
// The file is compiled with deferred inlining, so its functions are written
// here in reverse of their order in the original.
class CDrawContext;
class CCollision;
class CThrownBullet;
class CProjectileBullet;

#include "game/CVector3.h"

#include "game/CMatrix.h"

#include "game/IMovingSceneNode.h"

class CBullet : public IMovingSceneNode {
public:
    virtual ~CBullet();
    virtual void Draw(CDrawContext&);
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual bool IsDrawEnabled() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void Init();
    virtual void Shutdown();
    virtual void GetUp(CVector3&) const;
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
    virtual void SetExclusionPair(ISceneNode*);
    virtual void Penetrate(const CCollision&);
    CBullet();

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 RightRow() const { return m_tm.right; }

    int data24;
    int data28;
    unsigned char unknown2c[36];
    CMatrix m_tm;
    float data90;
    float data94;
};

CBullet::CBullet() {
    data28 = 0;
}

CBullet::~CBullet() {
}

void CBullet::Init() {
    m_tm.Ident();
    CBullet::Shutdown();
    data08 = 0;
    data04 = 0;
}

void CBullet::Shutdown() {
    data90 = 0.0f;
    data24 = -1;
    data94 = 3.4028235e38f;
}

void CBullet::Draw(CDrawContext&) {
}

__declspec(weak) CBullet* CBullet::AsBullet() {
    return this;
}

__declspec(weak) const CBullet* CBullet::AsBullet() const {
    return this;
}

__declspec(weak) void CBullet::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

__declspec(weak) void CBullet::GetUp(CVector3& v) const {
    v = UpRow();
}

__declspec(weak) void CBullet::GetRightward(CVector3& v) const {
    v = RightRow();
}

__declspec(weak) bool CBullet::IsDrawEnabled() const {
    return true;
}
