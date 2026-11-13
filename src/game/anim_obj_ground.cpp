// A fragment of anim_obj.cpp (0x8009491c): CAnimObject::DetectGround probes
// for the ground under an animated object. When ground detection is on and
// the object is not rising (not jumping, or falling), it casts a fiber from
// one unit above to the point 0.05 below the object's position into the
// world volume; on a miss it casts a second fiber 100 units up and down and,
// if that one hits nothing either, marks the object stopped. A stopped object
// loses its velocity; an object whose first probe found ground lands (ending
// the jump, sending event 133 if it was falling) and takes the surface sound
// of the contact (1 by default), and the function returns how far below 0.99
// the hit lies. The classes, functions and globals are named by the mangled
// symbols and the virtuals by the vtables; the members, the collision,
// contact and surface views and the CVector3/CLine3 views (as in
// weapon_bullet.cpp; CLine3 has an inline destructor whose weak copy the
// linker took from another file) are inferred. The rest of the file is in
// other units.
#include "game/CVector3.h"


#include "game/CMatrix.h"


class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.x = m_end.x - m_start.x;
        m_dir.y = m_end.y - m_start.y;
        m_dir.z = m_end.z - m_start.z;
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class CCollision;
class CVolFiber;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber(const CLine3&);
    virtual ~CVolFiber();

    unsigned char unknown04[12];
    CLine3 m_line;
};

class CSurfaceType {
public:
    static short GetSoundIndex(int);
};

struct SurfaceView {
    unsigned char unknown00[12];
    short m_type : 11;
};

struct ContactView {
    unsigned char unknown00[36];
    SurfaceView* m_surface;
};

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[12];
    float m_t;
    unsigned char unknown10[8];
    ContactView* m_contacts;
    unsigned char unknown1c[4];
};

class ISceneNode {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual IVolume* GetWorldBoundingVolume(int) const;
};

#include "game/CScene.h"


extern CScene g_scene;

class CAnimObject {
public:
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void TriggerScriptEvent(int, void*, bool);

    float DetectGround();

    unsigned char unknown0004[9020];
    CMatrix m_tm;
    unsigned char unknown2380[72];
    CVector3 m_velocity;
    unsigned char unknown23d8[124];
    bool m_onGround;
    bool m_falling;
    bool m_jumping;
    unsigned char unknown2457[92];
    bool m_detectGround;
    unsigned char unknown24b4;
    bool m_stopped;
    unsigned char unknown24b6[26];
    short m_surfaceSound;
};

float CAnimObject::DetectGround() {
    float ground = 0.0f;
    CCollision collision;
    int hit = 1;
    m_stopped = false;
    if (m_detectGround && (!m_jumping || m_velocity.z < 0.0f)) {
        IVolume* world = g_scene.m_world->GetWorldBoundingVolume(0);
        CVector3 bottom = m_tm.GetPos();
        bottom.z -= 0.05f;
        CLine3 line(CVector3(bottom.x, bottom.y, 1.0f + bottom.z), bottom);
        CVolFiber fiber(line);
        hit = world->TestCollision(fiber, collision, true);
        if (hit == 1) {
            CLine3 up(CVector3(bottom.x, bottom.y, 100.0f + bottom.z), CVector3(bottom.x, bottom.y, bottom.z - 100.0f));
            CVolFiber fiber2(up);
            CCollision collision2;
            if (world->TestCollision(fiber2, collision2, true) == 1)
                m_stopped = true;
        }
    }
    if (m_stopped) {
        m_velocity.x = 0.0f;
        m_velocity.y = 0.0f;
        m_velocity.z = 0.0f;
    } else if (hit != 1) {
        m_onGround = true;
        m_jumping = false;
        ground = 0.99f - collision.m_t;
        if (m_falling) {
            TriggerScriptEvent(133, 0, false);
            m_falling = false;
        }
        m_surfaceSound = 1;
        if (collision.m_contacts && collision.m_contacts->m_surface && collision.m_contacts->m_surface->m_type >= 0)
            m_surfaceSound = CSurfaceType::GetSoundIndex(collision.m_contacts->m_surface->m_type);
    }
    return ground;
}
