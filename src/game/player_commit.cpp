// A fragment of player.cpp (0x800a1b88): CPlayerObject::CommitUpdate. In
// multiplayer it places the animated player mesh (+1992) at the player's
// transform (lifted by g_MPAnimMeshZOffset) and sets its locomotion
// parameters: the horizontal distance moved since the last frame (from the
// previous position at +648) is added to the mesh's current speed, with a
// direction type from the normalised forward vector (3 backwards, 1/2 to
// either side by the sign of the cross product, 0 forwards or still).
// Unless the flag at bit 0x02 of +918 is set, a fiber from 0.4 above the
// position straight down is tested against the scene's world volume
// (pass-through disabled); a hit stops the player (both velocities zeroed,
// bit 0x40 of +916 set), runs AttemptUpdate(0) and returns. With the flag
// set the transform is optionally rebuilt from the yaw and the scripted
// position, the camera (+2000) gets the eye matrix times the transform, a
// perspective from the field of view (+1068, degrees) and an update, and the
// current weapon object updates its transforms. Then the velocity is
// constrained, the position and crouch height (the local capsule's end z)
// are stored, and the mounted weapon (+1504) is updated; when it fired it
// shows a muzzle flash (unless it is the weapon at +1500), counts the shot
// and, when the flag at +1784 is set, ejects a spent casing (a thrown
// object placed by the weapon node's transform times the hand matrix at
// +1792, with a random velocity and spin). The class and functions are named
// by the mangled symbols; the members, flag names and views are inferred
// (CPlayerObject as a view with ISceneNode's virtual slots). The static
// toggle is the original's function-static flag (its guard included); it is
// only ever set and cleared. Inferred helpers: the casing ejection is an
// inline member (its locals are allocated as an inline's), the eye-matrix
// reference local, the CVector3 double-pair view's copy constructor (one
// doubleword at a time, keeping the original's dead parameter copies) and
// its constructor from the union (a whole copy, used for the scripted
// position passed to SetPos). The rest of the file is in other units.
extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y ;
    }
    return x;
}

/* inferred: the vector's doubleword-pair view */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; }
    /* inferred: a whole (builtin) copy from the union view */
    CVector3(const CVector3Data& o) : d(o) {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    float Normalize() {
        float length = sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]);
        if (length != 0.0f) {
            float inv = 1.0f / length;
            d.v[0] *= inv;
            d.v[1] *= inv;
            d.v[2] *= inv;
        }
        return length;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start.d = start.d;
        m_end.d = end.d;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }
    CVector3 GetEnd() const { return m_end; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);
    static void InitClass();
    static bool s_ClassInit;
    void BuildRotZ(float);
    void SetPos(CVector3);
    void Translate(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);
    void TransformVector(CVector3&, CVector3) const;
    const CVector3& GetFront() const { return m_front; }
    const CVector3& GetPos() const { return m_pos; }

    CVector3 m_right;
    CVector3 m_front;
    CVector3 m_up;
    CVector3 m_pos;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    float x, y, z, w;
};

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[32];
};

class CVolFiber;

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const class CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const class CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const class CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const class CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber(const CLine3&);
    ~CVolFiber();

    unsigned char unknown04[76];
};

class CVolCapsule : public IVolume {
public:
    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

enum EClsnId {};

/* inferred: the scene node interface's virtual slots (placeholders where unused) */
class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual IVolume* GetLocalBoundingVolume(int) const;
    virtual IVolume* GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void SetCollisionId(EClsnId);
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual void vfcc();
    virtual void vfd0();
    virtual void vfd4();
    virtual void vfd8();
    virtual void vfdc();
    virtual void vfe0();
    virtual void vfe4();
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void vfec();
    virtual void vff0();
    virtual void vff4();
    virtual void vff8();
    virtual void vffc();
    virtual void vf100();
    virtual void vf104();
    virtual void SetPosition(CVector3);
};

class CAnimatedPlayer : public ISceneNode {
public:
    void GetFlocoParams(float&, long&);
    void SetFlocoParams(float, long);
};

class CCamera : public ISceneNode {
public:
    void SetPerspective(float, float);
    static const float ASPECT_4x3;

    unsigned char unknown004[60];
};

class CThrownObject : public ISceneNode {
public:
    void SetVelocity(CVector3&);

    unsigned char unknown004[692];
    CQuaternion m_rotation;
};

struct TriggerObject_struct;
CThrownObject* CreateSpecialThrownObject(TriggerObject_struct*, int, int);
float MathFunRandomReal(float, float);

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual void Render();
    virtual void Init();
    virtual void Link();
    virtual int IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    virtual ~CParticleSystem();
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
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void SetLocalToWorld(const CMatrix&);
    static CParticleSystem* Create(unsigned long);
};

class CShellMenu {
public:
    void Set_shotsFired(unsigned int, bool);

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;

class CScene {
public:
    unsigned char unknown00[24];
    ISceneNode* m_world;
};

extern CScene g_scene;

class CCDBObject {
public:
    static void DisablePassThru(bool);
};

class CWeapon {
public:
    void Update(float, bool);

    unsigned char unknown000[704];
    unsigned char m_flag2c0a : 1;
    unsigned char m_fired : 1;
    unsigned char m_flags2c0 : 6;
};

class CPlayerWeaponObject {
public:
    void UpdateWeaponTransforms();
};

extern bool g_bInMultiplayerMode;
extern float g_MPAnimMeshZOffset;
extern ISceneNode* g_pStaticObjectMuzzleFlashPlayer;

class CPlayerObject : public ISceneNode {
public:
    virtual ~CPlayerObject();
    void CommitUpdate();

    /* inferred inline helper */
    void EjectCasing() {
        CThrownObject* thrown = CreateSpecialThrownObject(0, 0x9ab689e6, -1);
        if (thrown) {
            thrown->SetCollisionId((EClsnId)-1);
            CMatrix matrix;
            m_weaponNode->GetTMLocalToWorld(matrix);
            matrix.Multiply(m_handMatrix, matrix);
            float rx = MathFunRandomReal(-0.5f, 0.5f);
            float ry = MathFunRandomReal(-0.5f, 0.5f);
            float rz = MathFunRandomReal(-0.5f, 0.5f);
            CVector3 velocity(0.016683351f * MathFunRandomReal(2.0f, 2.5f), 0.016683351f * MathFunRandomReal(0.0f, 0.5f), 0.016683351f * MathFunRandomReal(1.0f, 2.0f));
            matrix.TransformVector(velocity, velocity);
            thrown->SetPosition(matrix.GetPos());
            thrown->SetVelocity(velocity);
            CMatrix rotation(matrix);
            rotation.RotateX(rx);
            rotation.RotateY(ry);
            rotation.RotateZ(rz);
            thrown->m_rotation.SetFromMatrix(rotation);
            thrown->BeginUpdate(1.0f);
        }
    }
    CPlayerWeaponObject* CurrentPlayerWeapon() const {
        if (m_usingMountedWeapon)
            return 0;
        if (m_currentWeapon >= 0)
            return m_weapons[m_currentWeapon];
        return 0;
    }

    unsigned char unknown004[124];
    CMatrix m_tm;
    CMatrix m_eyeMatrix;
    unsigned char unknown100[176];
    CVolCapsule m_localCapsule;
    unsigned char unknown200[88];
    CVector3 m_velocity;
    CVector3 m_localVelocity;
    unsigned char unknown278[16];
    CVector3 m_position;
    float m_crouchHeight;
    float m_rotation;
    unsigned char unknown2a0[244];
    unsigned char m_flag394a : 1;
    unsigned char m_flag394b : 1;
    unsigned char m_flags394 : 6;
    unsigned char unknown395;
    unsigned char m_usingMountedWeapon : 1;
    unsigned char m_flags396 : 5;
    unsigned char m_flag396g : 1;
    unsigned char m_flag396h : 1;
    unsigned char m_flag397a : 1;
    unsigned char m_flag397b : 1;
    unsigned char m_flags397 : 6;
    unsigned char unknown398[148];
    float m_fov;
    unsigned char unknown430[44];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[184];
    ISceneNode* m_weaponNode;
    unsigned char unknown5d0[12];
    CWeapon* m_lastWeapon;
    CWeapon* m_mountedWeapon;
    unsigned char unknown5e4[276];
    bool m_throwHelmet;
    unsigned char unknown6f9[7];
    CMatrix m_handMatrix;
    CVector3 m_scriptPosition;
    unsigned char unknown750[8];
    float m_delta;
    unsigned char unknown75c[108];
    CAnimatedPlayer* m_mesh;
    unsigned char unknown7cc[4];
    CCamera m_camera;
};

void CPlayerObject::CommitUpdate() {
    if (g_bInMultiplayerMode) {
        if (m_flag397b) {
            CVector3 position = m_tm.GetPos();
            position.d.v[2] += g_MPAnimMeshZOffset;
            m_mesh->SetPosition(position);
        } else {
            CMatrix tm(m_tm);
            tm.Translate(CVector3(0.0f, 0.0f, g_MPAnimMeshZOffset));
            m_mesh->SetTMLocalToWorld(tm);
        }
        CVector3 direction = m_tm.GetPos();
        direction.d.v[0] -= m_position.d.v[0];
        direction.d.v[1] -= m_position.d.v[1];
        direction.d.v[2] -= m_position.d.v[2];
        direction.d.v[2] = 0.0f;
        float speed = direction.Normalize();
        long type = 0;
        if (speed > 1.1920929e-07f) {
            CVector3 front = m_tm.GetFront();
            front.Normalize();
            float dot = front.d.v[0] * direction.d.v[0] + front.d.v[1] * direction.d.v[1] + front.d.v[2] * direction.d.v[2];
            if (dot < -0.7071f) {
                type = 3;
            } else if (dot < 0.7071f) {
                front.d.v[2] = 0.0f;
                direction.d.v[2] = 0.0f;
                front.d.v[0] = front.d.v[1] * direction.d.v[2] - front.d.v[2] * direction.d.v[1];
                front.d.v[1] = front.d.v[2] * direction.d.v[0] - front.d.v[0] * direction.d.v[2];
                front.d.v[2] = front.d.v[0] * direction.d.v[1] - front.d.v[1] * direction.d.v[0];
                if (front.d.v[2] > 0.0f)
                    type = 2;
                else
                    type = 1;
            }
        }
        float offset;
        long current;
        m_mesh->GetFlocoParams(offset, current);
        m_mesh->SetFlocoParams(speed + offset, type);
    }
    if (!m_flag396g) {
        IVolume* world = g_scene.m_world->GetWorldBoundingVolume(0);
        CVector3 start = m_tm.GetPos();
        start.d.v[2] += 0.4f;
        CLine3 line(start, CVector3(start.d.v[0], start.d.v[1], -1e30f));
        CVolFiber fiber(line);
        CCollision collision;
        CCDBObject::DisablePassThru(true);
        int result = world->TestCollision(fiber, collision, false);
        CCDBObject::DisablePassThru(false);
        static bool toggle = true;
        if (result == 1) {
            if (toggle)
                toggle = false;
            m_localVelocity.d.v[0] = m_localVelocity.d.v[1] = m_localVelocity.d.v[2] = 0.0f;
            m_velocity.d.v[0] = m_velocity.d.v[1] = m_velocity.d.v[2] = 0.0f;
            m_flag394b = 1;
            AttemptUpdate(0.0f);
            return;
        }
        toggle = true;
    } else {
        if (m_flag396h) {
            m_tm.BuildRotZ(m_rotation);
            m_tm.SetPos(m_scriptPosition.d);
        }
        const CMatrix& eye = m_eyeMatrix;
        CMatrix tm(m_tm);
        CMatrix view;
        view.Multiply(eye, tm);
        m_camera.SetTMLocalToWorld(view);
        float fov = 0.017453292f * m_fov;
        m_camera.SetPerspective(fov, fov * CCamera::ASPECT_4x3);
        m_camera.AttemptUpdate(1.0f);
        CPlayerWeaponObject* weapon = CurrentPlayerWeapon();
        if (!m_flag397a && weapon)
            weapon->UpdateWeaponTransforms();
    }
    ConstrainVelocity();
    m_position = CVector3(m_tm.GetPos());
    m_crouchHeight = m_localCapsule.m_axis.GetEnd().d.v[2];
    if (m_mountedWeapon) {
        m_mountedWeapon->Update(m_delta, !m_usingMountedWeapon);
        if (m_mountedWeapon->m_fired) {
            if (m_mountedWeapon != m_lastWeapon) {
                CParticleSystem* flash = CParticleSystem::Create(0x2c0c51fb);
                if (flash) {
                    CMatrix matrix;
                    g_pStaticObjectMuzzleFlashPlayer->GetTMLocalToWorld(matrix);
                    flash->SetLocalToWorld(matrix);
                }
            }
            g_Shell.Set_shotsFired(1, false);
            if (m_throwHelmet) {
                EjectCasing();
            }
        }
    }
}
