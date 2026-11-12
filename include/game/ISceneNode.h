// ISceneNode: the base of everything in the scene, shared by every file that uses it
// (PortReadiness step 3.3), with its bases IDestructible, ISubject and IObserver.
//
// Hierarchy, from the original's RTTI (__RTTI__10ISceneNode lists IDestructible, ISubject,
// IObserver, all at offset 0) and vtables:
//   __vt__13IDestructible: MarkForDestruction__13IDestructible, __dt__, Destroy
//   __vt__8ISubject:       MarkForDestruction__8ISubject (override), __dt__, Destroy
//   __vt__9IObserver:      MarkForDestruction__8ISubject, __dt__, Destroy
//   __vt__10ISceneNode:    the three above, then the 52 slots declared below, in order
// Out of line in the original: IDestructible::MarkForDestruction, ISubject::
// MarkForDestruction, ~ISubject, ~IObserver. Weak (inline) copies: ~IDestructible,
// IDestructible::Destroy, ~ISceneNode and every ISceneNode virtual (0x8001ada0..0x8001b064).
// Bodies follow src/game/Moh2.cpp, the matched unit that emits those weak copies.
//
// Inferred, not in any mangled name: the pointer return types (Moh2.cpp's), and the data
// members. ISceneNode is 0x24 bytes; the bases have no data (their destructors store only
// the vtable pointer), so +4..+0x20 are ISceneNode's, named by offset as the matched
// constructor views name them (the scene's list links are among them; not resolved).
#ifndef GAME_ISCENENODE_H
#define GAME_ISCENENODE_H

#include "game/CMatrix.h"

class CCollision;
class CDrawContext;
class CBullet;
class CLight;
class CStaticObject;
class CHierObject;
class CWorldObject;
class CAnimObject;
class CSoldierObject;
class CPlayerObject;
class CPlayerWeaponObject;
class CAnimatedPlayerObject;
class CCollisionVolume;
class CAIDoodad;
class IMovingSceneNode;
class IVolume;
struct BSObject;

// A collision class id (SetCollisionId__10ISceneNodeF7EClsnId); enumerators not known.
enum EClsnId {};

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible() {}
    virtual void Destroy() {}
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
    enum EVolumeType {};

    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}
    virtual void BeginUpdate(float) {}
    virtual void UpdateAI(float) {}
    virtual void CommitAI() {}
    virtual void ConstrainVelocity() {}
    virtual void AttemptUpdate(float) {}
    virtual void OnCollision(const CCollision&) {}
    virtual void CommitUpdate() {}
    virtual void Draw(CDrawContext&) {}
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const { return 0; }
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const { return 0; }
    virtual void GetTMLocalToWorld(CMatrix& matrix) const { matrix.Ident(); }
    virtual void GetPosition(CVector3& position) const { position.x = position.y = position.z = 0.0f; }
    virtual void GetRightward(CVector3& direction) const { direction.Set(1.0f, 0.0f, 0.0f); }
    virtual void GetForward(CVector3& direction) const { direction.Set(0.0f, 1.0f, 0.0f); }
    virtual void GetUpward(CVector3& direction) const { direction.Set(0.0f, 0.0f, 1.0f); }
    virtual bool IsVisible(CDrawContext&) const { return false; }
    virtual bool IsDrawEnabled() const { return false; }
    virtual EClsnId GetCollisionId() const { return (EClsnId)-1; }
    virtual void SetCollisionId(EClsnId) {}
    virtual BSObject* GetScriptObject() const { return 0; }
    virtual void TriggerScriptEvent(int, void*, bool) {}
    virtual void HandleBulletCollision(CBullet*, const CCollision&) {}
    virtual IMovingSceneNode* AsMovingNode() { return 0; }
    virtual const IMovingSceneNode* AsMovingNode() const { return 0; }
    virtual CStaticObject* AsStaticObject() { return 0; }
    virtual const CStaticObject* AsStaticObject() const { return 0; }
    virtual CHierObject* AsHierObject() { return 0; }
    virtual const CHierObject* AsHierObject() const { return 0; }
    virtual CWorldObject* AsWorldObject() { return 0; }
    virtual const CWorldObject* AsWorldObject() const { return 0; }
    virtual CAnimObject* AsAnimObject() { return 0; }
    virtual const CAnimObject* AsAnimObject() const { return 0; }
    virtual CSoldierObject* AsSoldierObject() { return 0; }
    virtual const CSoldierObject* AsSoldierObject() const { return 0; }
    virtual CPlayerObject* AsPlayerObject() { return 0; }
    virtual const CPlayerObject* AsPlayerObject() const { return 0; }
    virtual CPlayerWeaponObject* AsPlayerWeaponObject() { return 0; }
    virtual const CPlayerWeaponObject* AsPlayerWeaponObject() const { return 0; }
    virtual CAnimatedPlayerObject* AsAnimatedPlayerObject() { return 0; }
    virtual const CAnimatedPlayerObject* AsAnimatedPlayerObject() const { return 0; }
    virtual CLight* AsLight() { return 0; }
    virtual const CLight* AsLight() const { return 0; }
    virtual CBullet* AsBullet() { return 0; }
    virtual const CBullet* AsBullet() const { return 0; }
    virtual CCollisionVolume* AsCollisionVolume() { return 0; }
    virtual const CCollisionVolume* AsCollisionVolume() const { return 0; }
    virtual CAIDoodad* GetAIDoodad() { return 0; }
    virtual const CAIDoodad* GetAIDoodad() const { return 0; }
    virtual void SetAttachedLight(CLight*, CVector3) {}
    virtual CLight* GetAttachedLight() const { return 0; }

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

#endif
