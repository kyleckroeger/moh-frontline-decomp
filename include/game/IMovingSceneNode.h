// IMovingSceneNode: a scene node that moves (cameras, bullets, animated objects), shared by
// every file that uses it (PortReadiness step 3.3).
//
// Evidence: __RTTI__16IMovingSceneNode and __vt__16IMovingSceneNode: ISceneNode's 53 slots
// (AsMovingNode overridden at +0x6c/+0x70), then the 18 new slots declared below, in order.
// Every one of them and the destructor are weak in the original (Moh2.cpp's copies), so they
// are inline here with Moh2.cpp's bodies. No data of its own: the views are 0x24 bytes, as
// ISceneNode.
// Inferred: GetLightVolume's and the light-volume functions' pointer type name is the
// mangled one (BPDLightVolume); the bodies are Moh2.cpp's.
#ifndef GAME_IMOVINGSCENENODE_H
#define GAME_IMOVINGSCENENODE_H

#include "game/ISceneNode.h"

struct BPDLightVolume;

class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
    virtual void Reset() {}
    virtual void Halt() {}
    virtual void ApplyForceTo(CVector3, float) {}
    virtual void SetTMLocalToWorld(const CMatrix&) {}
    virtual void PreTransform(const CMatrix&) {}
    virtual void Transform(const CMatrix&) {}
    virtual void Move(CVector3) {}
    virtual void Rotate(CVector3, float) {}
    virtual void Pitch(float) {}
    virtual void Roll(float) {}
    virtual void Yaw(float) {}
    virtual void SetPosition(CVector3) {}
    virtual void SetBasis(CVector3, CVector3, CVector3) {}
    virtual void Orthonormalize() {}
    virtual CVector3 GetWorldLinearVelocity() const { return CVector3(0.0f, 0.0f, 0.0f); }
    virtual void EnterLightVolume(BPDLightVolume*) {}
    virtual void ExitLightVolume(BPDLightVolume*) {}
    virtual BPDLightVolume* GetLightVolume() { return 0; }
    virtual IMovingSceneNode* AsMovingNode() { return this; }
    virtual const IMovingSceneNode* AsMovingNode() const { return this; }
};

#endif
