// CAnimObject, the first functions of the file: empty overrides, attachment
// flags, the transform setter and light-volume and scene bookkeeping, then
// IsVisible (submits the draw data with the transform to the mesh drawer,
// which decides when there is one; otherwise the camera's point test on the
// position, read through the virtual GetPosition). CAnimObject and the
// referenced classes are named by the mangled symbols. This is an inferred,
// non-virtual view: only the members these functions touch are declared, at
// their offsets, and the class's virtual table is not reproduced here
// (IsVisible reaches the drawer's and the scene node's virtual functions
// through inferred views). The empty overrides are inline in the class in
// the original (weak symbols, emitted for the vtable), so they are defined
// __declspec(weak) here. CVector3 view: four floats, 8-byte aligned,
// overlaid with two doubles. The union is inferred from the code, not the
// original declaration: whole-vector copies in this file (the row accessors)
// move each vector as two lfd/stfd pairs, which an implicit copy through the
// double pair reproduces.
#include "game/CVector3.h"

#include "game/CMatrix.h"


class CStaticObject;
class BPDLightVolume;

class CLightVolumeManager {
public:
    void* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);
};

class ISceneNode {
public:
    enum EVolumeType { VolumeTypeUnknown = 0 };
};

class ISubject {
public:
    void MarkForDestruction(int);
};

class CScene {
public:
    void Remove(ISceneNode&);

    char data[328];
};

extern CScene g_scene;

class CDrawContext;

class CCamera {
public:
    bool IsPointVisible(CVector3, CDrawContext&) const;
};

class CDrawContext {
public:
    char data000[212];
    CCamera* m_camera;
};

// Inferred: the object drawing the animated mesh, called through the virtuals
// at +16 and +68 of its table (the earlier slots are placeholders named by
// offset).
class AnimDrawerView {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010(void*, const CMatrix&);
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
    virtual bool unknown044(CDrawContext&);
};

// Inferred: the scene-node virtual table as far as GetPosition (+64).
class SceneNodeVirtualView {
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
    virtual void GetPosition(CVector3&) const;
};

class CAnimObject {
public:
    bool IsVisible(CDrawContext&) const;
    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    const CAnimObject* AsAnimObject() const;
    CAnimObject* AsAnimObject();
    int AttachObject(CStaticObject*, char*, int);
    int DetachObject(char*);
    int GetAttachPointMatrix(char*, CMatrix*);
    void AttachFloatingBone();
    void DetachFloatingBone();
    void PreTransform(const CMatrix&);
    void Transform(const CMatrix&);
    void Rotate(CVector3, float);
    void Pitch(float);
    void Roll(float);
    void SetBasis(CVector3, CVector3, CVector3);
    void Destroy();
    void MarkForDestruction(int);
    void SetAttached(bool);
    void AttachToLocation(const CMatrix&);
    void SetTMLocalToWorld(const CMatrix&);
    void* GetLightVolume();
    void ExitLightVolume(BPDLightVolume*);
    void EnterLightVolume(BPDLightVolume*);
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;
    void Orthonormalize();
    void SetYaw(float);
    float GetYaw() const;
    void Yaw(float);
    void Move(CVector3);
    void SetPosition(CVector3);
    void Reset();
    int GetCollisionId() const;
    bool IsDrawEnabled() const;

    CVector3 GetUp() const { return m_tm.up; }
    CVector3 GetFwd() const { return m_tm.front; }
    CVector3 GetRight() const { return m_tm.right; }
    CVector3 GetPos() const { return m_tm.pos; }

    char field0000[9024];
    CMatrix m_tm;
    void* m_drawData;
    char field2384[4];
    AnimDrawerView* m_drawer;
    char field238C[8];
    int m_collisionId;
    char field2398[24];
    int m_yaw;
    char field23B4[56];
    int m_field23EC;
    char field23F0[100];
    bool m_field2454;
    char field2455[5];
    bool m_attached;
    bool m_attachedToLocation;
    char field245C[4];
    CMatrix m_attachLocation;
    char field24A0[24];
    CLightVolumeManager m_lightVolumes;
};

__declspec(weak) void CAnimObject::UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&) {
}

__declspec(weak) const CAnimObject* CAnimObject::AsAnimObject() const {
    return this;
}

__declspec(weak) CAnimObject* CAnimObject::AsAnimObject() {
    return this;
}

__declspec(weak) int CAnimObject::AttachObject(CStaticObject*, char*, int) {
    return 0;
}

__declspec(weak) int CAnimObject::DetachObject(char*) {
    return 0;
}

__declspec(weak) int CAnimObject::GetAttachPointMatrix(char*, CMatrix*) {
    return 0;
}

__declspec(weak) void CAnimObject::AttachFloatingBone() {
}

__declspec(weak) void CAnimObject::DetachFloatingBone() {
}

__declspec(weak) void CAnimObject::PreTransform(const CMatrix&) {
}

__declspec(weak) void CAnimObject::Transform(const CMatrix&) {
}

__declspec(weak) void CAnimObject::Rotate(CVector3, float) {
}

__declspec(weak) void CAnimObject::Pitch(float) {
}

__declspec(weak) void CAnimObject::Roll(float) {
}

__declspec(weak) void CAnimObject::SetBasis(CVector3, CVector3, CVector3) {
}

void CAnimObject::Destroy() {
}

void CAnimObject::MarkForDestruction(int flag) {
    // The ISubject and ISceneNode bases are at the start of the object.
    ((ISubject*)this)->MarkForDestruction(flag);
    g_scene.Remove(*(ISceneNode*)this);
}

void CAnimObject::SetAttached(bool attached) {
    m_attached = attached;
    m_attachedToLocation = false;
}

void CAnimObject::AttachToLocation(const CMatrix& location) {
    m_attached = true;
    m_attachedToLocation = true;
    m_attachLocation = location;
}

void CAnimObject::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
}

void* CAnimObject::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}

void CAnimObject::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

void CAnimObject::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}

void CAnimObject::GetUpward(CVector3& v) const {
    v = GetUp();
}

void CAnimObject::GetForward(CVector3& v) const {
    v = GetFwd();
}

void CAnimObject::GetRightward(CVector3& v) const {
    v = GetRight();
}

void CAnimObject::GetPosition(CVector3& v) const {
    v = GetPos();
}

void CAnimObject::Orthonormalize() {
    m_tm.Orthonormalize();
}

void CAnimObject::SetYaw(float yaw) {
    m_yaw = (int)(2670176.75f * yaw) & 0xFFFFFF;
    m_field23EC = m_yaw;
}

float CAnimObject::GetYaw() const {
    return 6.2831855f * m_yaw / 16777216.0f;
}

void CAnimObject::Yaw(float angle) {
    m_tm.RotateZ(angle);
    m_yaw = (m_yaw + (int)(2670176.75f * angle)) & 0xFFFFFF;
}

void CAnimObject::Move(CVector3 delta) {
    m_tm.Translate(delta);
}

void CAnimObject::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
}

void CAnimObject::Reset() {
    m_tm.Ident();
    m_yaw = 0;
    m_field2454 = false;
}

int CAnimObject::GetCollisionId() const {
    return m_collisionId;
}

bool CAnimObject::IsDrawEnabled() const {
    return true;
}

bool CAnimObject::IsVisible(CDrawContext& context) const {
    if (m_drawData)
        m_drawer->unknown010(m_drawData, m_tm);
    if (m_drawer)
        return m_drawer->unknown044(context);
    CVector3 position;
    ((const SceneNodeVirtualView*)this)->GetPosition(position);
    return context.m_camera->IsPointVisible(position, context);
}
