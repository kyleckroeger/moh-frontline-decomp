// CCamera: the view (position, projection, frustum planes), shared by every file that uses it
// (PortReadiness step 3.3).
//
// Evidence:
// - __vt__7CCamera (0x8018460c): IMovingSceneNode's 71 slots, no new ones; CCamera overrides
//   the destructor, BeginUpdate, AttemptUpdate, CommitUpdate, Draw, GetTMLocalToWorld,
//   GetPosition/GetRightward/GetForward/GetUpward, IsVisible, IsDrawEnabled, Reset and
//   IMovingSceneNode's SetTMLocalToWorld..Orthonormalize. All out of line in the original
//   (camera.cpp: units camera_dtor and camera_rows), as are UpdateWorldToCamera (camera_frustum),
//   SetPerspective and IsPointVisible, and the static ASPECT_4x3.
// - The table is emitted with the destructor, the first override declared here. Units that
//   define it but must not emit the table (camera_dtor.cpp) keep their own view, which
//   declares Draw first.
// - Field offsets: the matched views and the layout lint (research/layout/classes.tsv):
//   matrices at +0x40/+0x80/+0xc0/+0x100, the four flags at +0x140..+0x143, the planes at
//   +0x150, the script object at +0x290 (camera_dtor.cpp destroys it). 0x2a0 bytes (Moh2.cpp).
// - The constructor is inline (no symbol); Moh2.cpp's form.
//
// Inferred, not in any mangled name: the field names (camera_rows.cpp's and
// camera_frustum.cpp's; +0x141 is cleared by every move and set by UpdateWorldToCamera,
// +0x142 is cleared by SetPerspective), data294, and the unknown ranges.
#ifndef GAME_CCAMERA_H
#define GAME_CCAMERA_H

#include "game/IMovingSceneNode.h"
#include "game/CPlane.h"

class CDrawContext;

class CCamera : public IMovingSceneNode {
public:
    CCamera() : m_tm(true), m_worldToCamera(true), m_flag141(true), m_projectionValid(false), m_flag143(false), m_script(0), data294(0) {}
    virtual ~CCamera();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual void Reset();
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    void UpdateWorldToCamera();
    void SetPerspective(float, float);
    bool IsPointVisible(CVector3, CDrawContext&) const;

    static const float ASPECT_4x3;

    float m_near;
    float m_far;
    float m_width;
    float m_height;
    unsigned char unknown34[12];
    CMatrix m_tm;
    CMatrix m_worldToCamera;
    CMatrix m_projection;
    CMatrix m_worldToScreen;
    bool m_perspective;
    bool m_flag141;
    bool m_projectionValid;
    bool m_flag143;
    unsigned char unknown144[12];
    CPlane m_planes[10];
    BSObject* m_script;
    int data294;
    unsigned char unknown298[8];
};

#endif
