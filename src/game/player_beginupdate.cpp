// A fragment of player.cpp (0x800a27f8): CPlayerObject::BeginUpdate, the
// player's per-frame input and movement step. It updates every weapon
// object's light volumes, tests a fiber from the position down past the
// capsule radius against the scene's world volume (on ground unless the
// test reports 1, or with bit 0x40 of +916 set), reads the controller
// (+1892) through g_inputMgr: the jump event (276) and the zoom cheat timer,
// the field-of-view transition and the zoom (sniper) field of view, the
// aim button (bit 0x08 of +916), crouch/stand, peeking up and ducking,
// clamps the height speed (+676), and sets the camera's height offset.
// Then, by state: aiming turns the view (scaled by the zoomed weapon),
// mounting a gun checks the view cone against the gun's yaw limit (+1044),
// a mounted gun turns its yaw/pitch (+1052/+1056, clamped to the limits)
// with the turn speed, moves the gun node and the player's matrices
// (SetPlayersMountedMGPositions) and places the muzzle flash; otherwise
// walking sets the local velocity (+616) from the stick (ladder, ground or
// air control, a slow-down factor at +1884), and the view rates. Falling is
// tracked from the start height (+1040) to a landing that may hurt (fall
// damage event 232 with hit directions and rumble); an applied force is
// added; jumping adds the jump speed. The local velocity becomes the world
// velocity (+600); a dying player stops. The view yaw/pitch (+668/+672)
// integrate the rates, with the weapon's recoil (+1972..+1980, +1088,
// +1092) recovering over time; the move path and the eye matrix (+192) are
// updated, the camera forward is read, the fire button fires (with a
// mounted gun's oscillating muzzle, function-static osc/osc_add/max_osc/
// osc_dist) or releases the weapon, the use/reload/next/previous buttons
// send their script events, and the speed class event (130) is sent when
// it changes. Last the position and crouch height are stored for the next
// frame, the AI doodad is pre-updated, the sounds are played and the hint
// button shows a hint. The class, functions and globals are named by the
// symbols; the member and flag names, the views (CPlayerObject as an
// IMovingSceneNode view, the gun's script node, the weapon object, the
// input manager, shell and cheat layouts) and the inline accessors marked
// below are inferred. The function is too large for CW's auto-inliner, so
// the CVector3/CLine3 helpers it calls out of line (emitted weak after it)
// are declared here without bodies. The rest of the file is in other units.
extern bool g_bInMultiplayerMode;

extern "C" double cos(double);
inline double fabs(double x) { return __fabs(x); }
extern "C" double pow(double, double);

namespace std {
float sqrtf(float);
}

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; }
    CVector3(float, float, float);
    /* inferred: a whole (builtin) copy from the union view (the original
       copies these vectors as two doubles loaded together) */
    CVector3(const CVector3Data& o) : d(o) {}
    void Scale(const CVector3&, float);
    void Add(const CVector3&);
    void Sub(const CVector3&, const CVector3&);
    void Normalize();
    float Length() const;
    CVector3 operator=(const CVector3&);

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(CVector3, CVector3);
    ~CLine3() {}
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
    static void InitClass();
    static bool s_ClassInit;
    CMatrix& operator=(const CMatrix&);
    void Ident();
    void Multiply(const CMatrix&, const CMatrix&);
    void RotateY(float);
    void RotateZ(float);
    void Rotate(CVector3, float);
    void BuildRotX(float);
    void BuildTrans(CVector3);
    void BuildScale(CVector3);
    void Translate(CVector3);
    void SetPos(CVector3);
    void FastInverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;
    CVector3 GetFront() const { return CVector3(m_front.d); }
    CVector3 GetPos() const { return CVector3(m_pos.d); }

    CVector3 m_right;
    CVector3 m_front;
    CVector3 m_up;
    CVector3 m_pos;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    void GetMatrix(CMatrix&) const;

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
    float GetRadius() const { return m_radius; }
    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

enum EClsnId {};
class CDrawContext;
class CBullet;
class CLight;
class CHierObject;
class CPlayerObject;
class BSObject;
struct BSGO_Basic;

class ISceneNode {
public:
    enum EVolumeType {};
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
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
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
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual CHierObject* AsHierObject();
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
    virtual void* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class CHierObject {
public:
    void DoBeginUpdate(float, bool);
    void SetRotation(CQuaternion&);
};

/* inferred: a mounted gun's script node (position floats, rotation) */
struct MountNodeView {
    unsigned char unknown00[16];
    float m_position[3];
    CQuaternion m_rotation;
};

/* inferred: the weapon node's script object */
struct ScriptNodeView {
    unsigned char unknown00[8];
    struct {
        unsigned char unknown00[4];
        MountNodeView* m_node;
    }* m_data;
};

/* inferred: a hierarchy object's transform at +1520 */
struct CHierObjectView {
    unsigned char unknown000[1520];
    CMatrix m_tm;
};

class CLightVolumeManager {
public:
    void Update(float);
};

enum EWeaponShootType {};

class CWeapon {
public:
    void Shoot(EWeaponShootType, float, float);

    unsigned char unknown000[644];
    short m_clipAmmo;
    unsigned char unknown286[58];
    unsigned char m_2c0_80 : 1;
    unsigned char m_2c0_rest : 7;
};

class CPlayerWeaponObject {
public:
    int GetZoomType() const;
    void Shoot(EWeaponShootType, int);

    unsigned char unknown0000[9400];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown24b9[3275];
    float m_zoomFov;
    float m_zoomTurnScale;
    unsigned char unknown318c[8];
    float m_recoilRecovery;
    unsigned char unknown3198[20];
    int m_zoomType;
    unsigned char unknown31b0[34];
    unsigned char m_31d2_80 : 1;
    unsigned char m_31d2_40 : 1;
    unsigned char m_31d2_rest : 6;
};

class UserInterface {
public:
    void AddHitDirection(unsigned int);
};

class CCamera : public IMovingSceneNode {
public:
    unsigned char unknown004[668];
};

class CAIPlayerDoodad {
public:
    void PreUpdate();

    unsigned char unknown00[84];
};

class CInputManager {
public:
    int GetValue(unsigned long, unsigned long);
    void SetFeedback(unsigned long, long, long, float, float);

    unsigned char unknown0000[9290];
    bool m_inverted;
    unsigned char m_zooming[5];
};

extern CInputManager g_inputMgr;

class CShellMenu {
public:
    unsigned char Get_currentControllerSetting();
    int PassPlayersMultiPlayerControllerValue(int);

    unsigned char unknown0000[5215];
    struct {
        bool m_inverted;
        unsigned char unknown01[27];
    } m_controllers[4];
};

extern CShellMenu g_Shell;

/* inferred: the cheat flags */
struct CheatsView {
    bool m_invincible;
    unsigned char unknown01;
    bool m_zoomCheat;
    bool m_zoomFlag;
    float m_zoomTimer;
    unsigned char unknown08[12];
};

extern CheatsView g_cheats;

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

template <class T> T MathFunClamp(T, T, T);
float MathFunAtan2F(float, float);
float MathFunRandomReal(float, float);
float MathFunRandomRealSigned(float, float);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void TriggerActivateMessages(BSGO_Basic*, bool);
void DisplayHint();

extern float MMG_TURNSPEED;
extern IMovingSceneNode* g_pStaticObjectMuzzleFlashPlayer;
extern ISceneNode* g_pCamera;
extern CVector3 g_CameraOffset;

/* inferred: the player's script object (its basic object at +12) */
struct PlayerScriptView {
    unsigned char unknown00[12];
    BSGO_Basic* m_basic;
};

class CPlayerObject : public IMovingSceneNode {
public:
    virtual ~CPlayerObject();
    void BeginUpdate(float);
    void TransitionFOV(float, float);
    void ShakeCamera(float&, float&, float);
    void SetPlayersMountedMGPositions(CMatrix*);
    void PeekUp(float);
    void DuckDown(float);
    void BeginStand();
    void BeginCrouch();
    void PeekLeftRight();
    void UpdateMovePath(float);
    void UpdateDeathSequence(float);
    void FireMyWeapon(float);
    void PlaySounds(bool, bool, float, float);
    CWeapon* GetCurrentWeapon() const;
    CPlayerWeaponObject* GetCurrentPlayerWeapon() const;
    /* inferred inline flag accessor */
    bool IsDisabled() const { return m_397_40; }

    unsigned char unknown004[32];
    CAIPlayerDoodad m_doodad;
    float m_value078;
    float m_value07c;
    CMatrix m_tm;
    CMatrix m_eyeMatrix;
    unsigned char unknown100[176];
    CVolCapsule m_localCapsule;
    CVolCapsule m_worldCapsule;
    BSObject* m_script;
    unsigned char unknown254[4];
    CVector3 m_velocity;
    CVector3 m_localVelocity;
    CVector3 m_force;
    CVector3 m_position;
    float m_crouchHeight;
    float m_yaw;
    float m_pitch;
    float m_heightSpeed;
    unsigned char unknown2a8[236];
    unsigned char m_394_80 : 1;
    unsigned char m_394_40 : 1;
    unsigned char m_394_20 : 1;
    unsigned char m_394_10 : 1;
    unsigned char m_394_08 : 1;
    unsigned char m_394_04 : 1;
    unsigned char m_394_02 : 1;
    unsigned char m_394_01 : 1;
    unsigned char m_395_80 : 1;
    unsigned char m_395_40 : 1;
    unsigned char m_395_20 : 1;
    unsigned char m_395_10 : 1;
    unsigned char m_395_08 : 1;
    unsigned char m_395_04 : 1;
    unsigned char m_395_02 : 1;
    unsigned char m_395_01 : 1;
    unsigned char m_usingMountedWeapon : 1;
    unsigned char m_396_40 : 1;
    unsigned char m_396_20 : 1;
    unsigned char m_396_10 : 1;
    unsigned char m_396_08 : 1;
    unsigned char m_396_04 : 1;
    unsigned char m_396_02 : 1;
    unsigned char m_396_01 : 1;
    unsigned char m_397_80 : 1;
    unsigned char m_397_40 : 1;
    unsigned char m_397_20 : 1;
    unsigned char m_397_10 : 1;
    unsigned char m_397_08 : 1;
    unsigned char m_397_04 : 1;
    unsigned char m_397_02 : 1;
    unsigned char m_397_01 : 1;
    unsigned char m_398_80 : 1;
    unsigned char m_398_40 : 1;
    unsigned char m_398_rest : 6;
    unsigned char unknown399[31];
    float m_deathTime;
    unsigned char unknown3bc[4];
    CVector3 m_cameraForward;
    unsigned char unknown3d0[16];
    CVector3 m_lastGroundPosition;
    unsigned char unknown3f0[32];
    float m_fallStartZ;
    float m_mgYawLimit;
    float m_mgPitchLimit;
    float m_mgYaw;
    float m_mgPitch;
    unsigned char unknown424[8];
    float m_fov;
    float m_zoomFov;
    float m_targetFov;
    float m_fovSpeed;
    unsigned char unknown43c[4];
    float m_recoilYaw;
    float m_recoilPitch;
    unsigned char unknown448[4];
    float m_peek;
    float m_duck;
    float m_fireDelay;
    float m_useTimer;
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[180];
    CHierObject* m_mgObject;
    IMovingSceneNode* m_weaponNode;
    unsigned char unknown5d0[12];
    CWeapon* m_lastWeapon;
    CWeapon* m_mountedWeapon;
    unsigned char unknown5e4[12];
    CMatrix m_flashMatrix;
    CMatrix m_mountMatrix;
    unsigned char unknown670[68];
    float m_turnScale;
    unsigned char unknown6b8[24];
    CVector3 m_flashScale;
    CVector3 m_mgDirection;
    unsigned char unknown6f0[100];
    float m_fireValue;
    float m_delta;
    float m_fallScale;
    unsigned char unknown760[4];
    unsigned long m_controller;
    unsigned char unknown768[72];
    int m_speedState;
    float m_recoil;
    float m_lastPitch;
    float m_recoilStep;
    unsigned char unknown7c0[16];
    CCamera m_camera;
    UserInterface m_ui;
    unsigned char unknownA71[2115];
    bool m_jumpEventSent;
    unsigned char unknown12b5[3];
    int m_jumpTimer;
};

/* inferred inline: the zoom type, 3 under the zoom cheat */
inline int CPlayerWeaponObject::GetZoomType() const { return g_cheats.m_zoomCheat ? 3 : m_zoomType; }

void CPlayerObject::BeginUpdate(float dt) {
    for (int i = 0; i < 45; i++) {
        if (m_weapons[i])
            m_weapons[i]->m_lightVolumes.Update(dt);
    }
    bool onGround = false;
    float pitchRate = 0.0f;
    float yawRate = 0.0f;
    if (!m_394_01) {
        IVolume* world = g_scene.m_world->GetWorldBoundingVolume((ISceneNode::EVolumeType)0);
        CVector3 position(m_tm.m_pos);
        float radius = m_localCapsule.m_radius;
        float z = position.d.v[2];
        CLine3 line(position, CVector3(position.d.v[0], position.d.v[1], z - radius - 0.05f));
        CVolFiber fiber(line);
        CCollision collision;
        CCDBObject::DisablePassThru(true);
        int result = world->TestCollision(fiber, collision, false);
        CCDBObject::DisablePassThru(false);
        bool ground = false;
        if (result != 1 || m_394_40)
            ground = true;
        onGround = ground;
    }
    if (g_inputMgr.GetValue(m_controller, 22)) {
        if (m_usingMountedWeapon)
            m_mgPitch = 0.0f;
        else if (m_396_01)
            m_pitch = 0.0f;
    }
    bool jump = (bool)g_inputMgr.GetValue(m_controller, 11);
    if (jump && m_394_40 && !m_jumpEventSent) {
        BSObjectTriggerEvent(m_script, 276, 0, 0, false);
        m_jumpEventSent = true;
    }
    if (g_cheats.m_zoomFlag) {
        if (g_cheats.m_zoomTimer > 0.0f)
            g_cheats.m_zoomTimer -= dt;
        if (jump)
            g_cheats.m_zoomTimer = 45.0f;
        else if (g_cheats.m_zoomTimer <= 0.0f)
            g_cheats.m_zoomTimer = 0.0f;
    }
    if (fabs(m_fovSpeed) > 0.0) {
        m_fov = 0.016683351f * (m_fovSpeed * dt) + m_fov;
        if (m_fovSpeed > 0.0f) {
            if (m_fov >= m_targetFov) {
                m_fovSpeed = 0.0f;
                m_fov = m_targetFov;
            }
        } else if (m_fov <= m_targetFov) {
            m_fovSpeed = 0.0f;
            m_fov = m_targetFov;
        }
    }
    if (g_bInMultiplayerMode && m_jumpTimer > 0)
        m_jumpTimer -= (int)dt;
    CWeapon* weapon = GetCurrentWeapon();
    if (weapon && weapon->m_2c0_80 && (unsigned int)weapon->m_clipAmmo != 0)
        m_396_40 = 0;
    g_inputMgr.m_zooming[m_controller] = m_396_40;
    if (m_396_40) {
        float zoom = (float)g_inputMgr.GetValue(m_controller, 143);
        zoom -= (float)g_inputMgr.GetValue(m_controller, 142);
        if (fabs(zoom) > 0.0) {
            m_zoomFov = 59.94f * (0.01f * zoom * dt) + m_zoomFov;
            m_zoomFov = MathFunClamp<float>(m_zoomFov, 5.0f, 35.0f);
        }
        TransitionFOV(m_zoomFov, 140.0f);
    } else if (weapon && weapon->m_2c0_80) {
        TransitionFOV(35.0f, 140.0f);
    } else if (m_394_08) {
        CPlayerWeaponObject* playerWeapon = GetCurrentPlayerWeapon();
        if (playerWeapon && playerWeapon->GetZoomType() == 2)
            TransitionFOV(playerWeapon->m_zoomFov, 140.0f);
        else
            TransitionFOV(35.0f, 140.0f);
    } else {
        TransitionFOV(35.0f, 140.0f);
    }
    m_394_04 = m_394_08;
    int crouch = g_inputMgr.GetValue(m_controller, 16);
    if (GetCurrentPlayerWeapon() && GetCurrentPlayerWeapon()->GetZoomType() && !m_397_01)
        m_394_08 = crouch > 0;
    else
        m_394_08 = 0;
    if (!m_394_08)
        m_395_10 = 0;
    if (m_usingMountedWeapon || m_396_08 || !m_396_01)
        m_394_08 = 0;
    if (!m_394_04 && m_394_08) {
        m_lastGroundPosition = m_tm.GetPos();
        m_value078 = 0.0f;
        m_value07c = 0.0f;
    }
    if (m_395_80 && !m_397_08) {
        BeginStand();
    } else {
        bool crouch = (bool)g_inputMgr.GetValue(m_controller, 12);
        if (crouch && m_396_01 && onGround) {
            if (m_395_80)
                BeginStand();
            else
                BeginCrouch();
        }
    }
    if (!m_395_40 && m_396_01) {
        if (m_395_80) {
            float peek = 0.0f;
            if (!g_bInMultiplayerMode) {
                if (m_394_08 && g_Shell.Get_currentControllerSetting() != 2) {
                    peek = 0.0078125f * (float)-g_inputMgr.GetValue(m_controller, 7);
                    if (peek < 0.01f)
                        peek = 0.0f;
                }
            } else if (m_394_08 && g_Shell.PassPlayersMultiPlayerControllerValue(m_controller) != 2) {
                peek = 0.0078125f * (float)-g_inputMgr.GetValue(m_controller, 7);
                if (peek < 0.01f)
                    peek = 0.0f;
            }
            PeekUp(peek);
        } else if (m_397_08) {
            float duck = 0.0f;
            if (!g_bInMultiplayerMode) {
                if (m_394_08 && g_Shell.Get_currentControllerSetting() != 2 && onGround) {
                    duck = 0.0078125f * (float)g_inputMgr.GetValue(m_controller, 7);
                    if (duck < 0.01f)
                        duck = 0.0f;
                }
            } else if (m_394_08 && g_Shell.PassPlayersMultiPlayerControllerValue(m_controller) != 2 && onGround) {
                duck = 0.0078125f * (float)g_inputMgr.GetValue(m_controller, 7);
                if (duck < 0.01f)
                    duck = 0.0f;
            }
            DuckDown(duck);
        }
        if (g_bInMultiplayerMode) {
            if (m_heightSpeed > 0.026666669f)
                m_heightSpeed = 0.026666669f;
            else if (m_heightSpeed < -0.026666669f)
                m_heightSpeed = -0.026666669f;
        } else {
            if (m_heightSpeed > 0.073333338f)
                m_heightSpeed = 0.073333338f;
            else if (m_heightSpeed < -0.073333338f)
                m_heightSpeed = -0.073333338f;
        }
        m_heightSpeed = m_heightSpeed / dt;
    }
    g_CameraOffset.d.v[2] = 0.1f + m_localCapsule.m_axis.GetEnd().d.v[2];
    if (m_394_08) {
        if (m_396_01) {
            if (g_bInMultiplayerMode) {
                float invert = g_Shell.m_controllers[m_controller].m_inverted ? -1.0f : 1.0f;
                if (g_Shell.PassPlayersMultiPlayerControllerValue(m_controller) == 2)
                    pitchRate = 0.00035156251f * (invert * (float)g_inputMgr.GetValue(m_controller, 138));
                else
                    pitchRate = 0.00035156251f * (invert * (float)g_inputMgr.GetValue(m_controller, 10));
            } else {
                float invert = g_inputMgr.m_inverted ? -1.0f : 1.0f;
                if (g_Shell.Get_currentControllerSetting() == 2)
                    pitchRate = 0.00035156251f * (invert * (float)g_inputMgr.GetValue(m_controller, 138));
                else
                    pitchRate = 0.00035156251f * (invert * (float)g_inputMgr.GetValue(m_controller, 10));
            }
            yawRate = 0.00035156251f * (float)-g_inputMgr.GetValue(m_controller, 9);
            CPlayerWeaponObject* playerWeapon = GetCurrentPlayerWeapon();
            if (playerWeapon) {
                float scale = 1.0f;
                if (m_396_40)
                    scale = 0.26000002f * ((m_fov - 5.0f) / 30.0f) + 0.04f;
                else if (playerWeapon->GetZoomType() == 2)
                    scale = playerWeapon->m_zoomTurnScale;
                if (m_398_40)
                    scale *= 2.0f;
                pitchRate *= scale;
                yawRate *= scale;
            }
        } else {
            pitchRate = 0.0f;
            yawRate = 0.0f;
        }
        if (!m_395_02) {
            m_localVelocity.d.v[0] = 0.0f;
            m_localVelocity.d.v[1] = 0.0f;
            if (!m_396_02)
                PeekLeftRight();
        } else {
            m_lastGroundPosition = m_tm.GetPos();
        }
        if (m_394_40) {
            m_395_02 = 0;
            m_localVelocity.d.v[2] = 0.0f;
        }
        jump = false;
    } else if (m_396_08) {
        CVector3 target;
        m_weaponNode->GetPosition(target);
        CMatrix matrix;
        m_weaponNode->GetTMLocalToWorld(matrix);
        matrix.Multiply(m_mountMatrix, matrix);
        CVector3 front;
        CVector3 direction;
        front = matrix.GetFront();
        direction = m_tm.GetPos();
        direction.Sub(target, direction);
        direction.d.v[2] = 0.0f;
        direction.Normalize();
        float dot = direction.d.v[0] * front.d.v[0] + direction.d.v[1] * front.d.v[1] + direction.d.v[2] * front.d.v[2];
        if (dot < (float)cos(m_mgYawLimit) && m_mgYawLimit < 3.1415927f) {
            BSObjectTriggerEvent(m_script, 168, 0, 0, true);
        } else {
            m_usingMountedWeapon = 1;
            m_396_08 = 0;
            if (m_currentWeapon >= 0)
                m_weapons[m_currentWeapon]->m_31d2_40 = 0;
            m_mgYaw = 0.0f;
            m_mgPitch = 0.0f;
            m_localVelocity.d.v[0] = m_localVelocity.d.v[1] = m_localVelocity.d.v[2] = 0.0f;
            m_pitch = 0.0f;
            m_tm = matrix;
            g_CameraOffset.d.v[2] = g_CameraOffset.d.v[1] = g_CameraOffset.d.v[0] = 0.0f;
        }
    } else if (m_usingMountedWeapon) {
        if (m_396_10) {
            CHierObjectView* mg = (CHierObjectView*)m_weaponNode->AsHierObject();
            CMatrix matrix;
            CMatrix view;
            m_mgObject->DoBeginUpdate(dt, true);
            matrix = mg->m_tm;
            view = matrix;
            CVector3 front(view.m_front.d);
            float oldAngle = MathFunAtan2F(m_mgDirection.d.v[0], m_mgDirection.d.v[1]);
            float newAngle = MathFunAtan2F(front.d.v[0], front.d.v[1]);
            m_mgDirection = front;
            m_mgYaw -= newAngle - oldAngle;
            if (m_mgYaw > 3.1415927f)
                m_mgYaw -= 6.2831855f;
            if (m_mgYaw < -3.1415927f)
                m_mgYaw += 6.2831855f;
        }
        float turnSpeed = m_mountedWeapon == m_lastWeapon ? 0.045f : MMG_TURNSPEED;
        float invert = g_inputMgr.m_inverted ? -1.0f : 1.0f;
        if (g_Shell.Get_currentControllerSetting() == 2)
            pitchRate = m_turnScale * ((0.0078125f * turnSpeed) * (invert * (float)g_inputMgr.GetValue(m_controller, 138)));
        else
            pitchRate = m_turnScale * ((0.0078125f * turnSpeed) * (invert * (float)g_inputMgr.GetValue(m_controller, 10)));
        yawRate = m_turnScale * ((0.0078125f * turnSpeed) * (float)-g_inputMgr.GetValue(m_controller, 9));
        if (m_mountedWeapon == m_lastWeapon) {
            if (pitchRate == 0.0f && yawRate == 0.0f) {
                if (m_397_02) {
                    BSObjectTriggerEvent(m_script, 23, 0, 0, true);
                    m_397_02 = 0;
                }
            } else if (!m_397_02) {
                BSObjectTriggerEvent(m_script, 22, 0, 0, true);
                m_397_02 = 1;
            }
        }
        ShakeCamera(pitchRate, yawRate, dt);
        m_mgYaw = yawRate * dt + m_mgYaw;
        m_mgPitch = pitchRate * dt + m_mgPitch;
        if (m_mgYawLimit < 3.1415927f) {
            m_mgYaw = m_mgYaw >= m_mgYawLimit ? m_mgYawLimit : m_mgYaw;
            m_mgYaw = m_mgYaw <= -m_mgYawLimit ? -m_mgYawLimit : m_mgYaw;
        }
        m_mgPitch = m_mgPitch >= m_mgPitchLimit ? m_mgPitchLimit : m_mgPitch;
        m_mgPitch = m_mgPitch <= -m_mgPitchLimit ? -m_mgPitchLimit : m_mgPitch;
        if (m_mgYaw > 6.2831855f)
            m_mgYaw -= 6.2831855f;
        else if (m_mgYaw < -6.2831855f)
            m_mgYaw += 6.2831855f;
        CMatrix mount;
        if (!m_396_10) {
            ScriptNodeView* script = (ScriptNodeView*)m_weaponNode->GetScriptObject();
            MountNodeView* node = script->m_data->m_node;
            node->m_rotation.GetMatrix(mount);
            mount.RotateZ(-m_mgYaw);
            CMatrix axis(mount);
            axis.Multiply(m_mountMatrix, axis);
            mount.Rotate(axis.m_right, -m_mgPitch);
            CVector3 position;
            position.d.v[0] = node->m_position[0];
            position.d.v[1] = node->m_position[1];
            position.d.v[2] = node->m_position[2];
            mount.SetPos(position);
            m_weaponNode->SetTMLocalToWorld(mount);
            SetPlayersMountedMGPositions(&mount);
        } else {
            CHierObject* mg = m_weaponNode->AsHierObject();
            mount.Ident();
            mount.RotateZ(-m_mgYaw);
            CMatrix axis(mount);
            axis.Multiply(m_mountMatrix, axis);
            mount.Rotate(axis.m_right, -m_mgPitch);
            CQuaternion rotation;
            rotation.SetFromMatrix(mount);
            mg->SetRotation(rotation);
            mg->DoBeginUpdate(dt, false);
            m_weaponNode->GetTMLocalToWorld(mount);
            SetPlayersMountedMGPositions(&mount);
        }
        m_localVelocity.d.v[0] = m_localVelocity.d.v[1] = m_localVelocity.d.v[2] = 0.0f;
        g_CameraOffset.d.v[2] = g_CameraOffset.d.v[1] = g_CameraOffset.d.v[0] = 0.0f;
        if (m_mountedWeapon != m_lastWeapon) {
            float angle = MathFunRandomReal(0.0f, 6.2831855f);
            if (g_pStaticObjectMuzzleFlashPlayer) {
                CMatrix flash;
                CMatrix weaponMatrix;
                flash.BuildScale(m_flashScale.d);
                flash.RotateY(angle);
                flash.Multiply(flash, m_flashMatrix);
                m_weaponNode->GetTMLocalToWorld(weaponMatrix);
                flash.Multiply(flash, weaponMatrix);
                g_pStaticObjectMuzzleFlashPlayer->SetTMLocalToWorld(flash);
            }
        }
    } else {
        m_peek = 0.0f;
        m_duck = 0.0f;
        if (onGround)
            m_395_02 = 0;
        if (!m_396_02) {
            float strafe = 0.0078125f * (float)g_inputMgr.GetValue(m_controller, 8);
            float forward = 0.0078125f * (float)-g_inputMgr.GetValue(m_controller, 7);
            if (m_394_01) {
                m_localVelocity.d.v[0] = 0.1f * (0.09175843f * strafe);
                m_localVelocity.d.v[1] *= 0.5f;
                m_localVelocity.d.v[2] = m_pitch < 0.0f ? forward : -forward;
                m_localVelocity.d.v[2] *= m_localVelocity.d.v[2] > 0.0f ? 0.04170838f : 0.025025027f;
            } else if (!m_395_02) {
                if (onGround) {
                    m_localVelocity.d.v[0] = 0.09175843f * strafe;
                    m_localVelocity.d.v[1] = forward;
                    if (g_bInMultiplayerMode)
                        m_localVelocity.d.v[1] *= 0.09175843f;
                    else
                        m_localVelocity.d.v[1] *= m_localVelocity.d.v[1] > 0.0f ? 0.09175843f : 0.066733405f;
                    m_fallScale = 1.0f;
                } else {
                    m_localVelocity.d.v[0] = 0.09175843f * (0.1f * strafe) + m_localVelocity.d.v[0];
                    m_localVelocity.d.v[1] = 0.09175843f * (0.1f * forward) + m_localVelocity.d.v[1];
                    float lengthSq = m_localVelocity.d.v[0] * m_localVelocity.d.v[0] + m_localVelocity.d.v[1] * m_localVelocity.d.v[1];
                    if (lengthSq > 0.0084196096f) {
                        float scale = 0.09175843f / std::sqrtf(lengthSq);
                        m_localVelocity.d.v[0] *= scale;
                        m_localVelocity.d.v[1] *= scale;
                    }
                }
                if (m_394_40)
                    m_localVelocity.d.v[2] = 0.0f;
                if (m_394_20) {
                    if (m_fallScale > 0.01f)
                        m_fallScale *= (float)pow(0.99f, dt);
                    if (m_fallScale < 0.01f)
                        m_fallScale = 0.01f;
                    m_localVelocity.d.v[0] *= m_fallScale;
                    m_localVelocity.d.v[1] *= m_fallScale;
                }
            }
        } else {
            m_localVelocity.d.v[0] = m_localVelocity.d.v[1] = m_localVelocity.d.v[2] = 0.0f;
        }
        if (jump && m_394_01)
            m_394_01 = 0;
        if (m_395_80 && onGround) {
            m_localVelocity.d.v[0] *= 0.25f;
            m_localVelocity.d.v[1] *= 0.25f;
        }
        if (m_396_01) {
            float invert;
            if (g_bInMultiplayerMode)
                invert = g_Shell.m_controllers[m_controller].m_inverted ? -1.0f : 1.0f;
            else
                invert = g_inputMgr.m_inverted ? -1.0f : 1.0f;
            pitchRate = 0.00035156251f * (invert * (float)g_inputMgr.GetValue(m_controller, 10));
            yawRate = 0.00035156251f * (float)-g_inputMgr.GetValue(m_controller, 9);
        } else {
            pitchRate = 0.0f;
            yawRate = 0.0f;
        }
    }
    if (!m_396_02 && !m_394_40 && !m_396_08 && !m_usingMountedWeapon && !m_394_01) {
        if (m_394_10)
            m_localVelocity.d.v[2] = -0.005455351f * dt + m_localVelocity.d.v[2];
        if (!m_395_01 && m_localVelocity.d.v[2] <= 0.0f) {
            m_fallStartZ = m_tm.GetPos().d.v[2];
            m_395_01 = 1;
        }
    }
    float fall = 0.0f;
    if (m_395_01 && (onGround || m_394_20)) {
        m_395_01 = 0;
        fall = m_fallStartZ - m_tm.GetPos().d.v[2];
        if (fall > 4.0f) {
            if (g_cheats.m_invincible) {
                TriggerScriptEvent(232, 0, false);
            } else if (m_397_20) {
                m_ui.AddHitDirection(1);
                m_ui.AddHitDirection(3);
                m_ui.AddHitDirection(4);
                m_ui.AddHitDirection(2);
                g_inputMgr.SetFeedback(m_controller, 12, 0, 1.0f, 0.1f);
                TriggerScriptEvent(232, (void*)((int)(fall - 4.0f) * 5), false);
            }
        }
    }
    if (m_395_04 && !m_396_02) {
        m_395_04 = 0;
        m_395_02 = 1;
        CMatrix inverse;
        inverse.FastInverse(m_tm);
        CVector3 force;
        inverse.TransformVector(force, m_force.d);
        m_localVelocity.Add(force);
        if (m_localVelocity.d.v[2] < 0.0f)
            m_localVelocity.d.v[2] = MathFunClamp<float>(m_localVelocity.d.v[2], -0.21f, 0.0f);
        else
            m_localVelocity.d.v[2] = MathFunClamp<float>(m_localVelocity.d.v[2], 0.0f, 0.21f);
    }
    if (onGround && jump && !m_396_02 && !m_395_80 && !m_395_40) {
        m_localVelocity.d.v[2] += 0.09175843f;
        if (m_395_02) {
            if (m_localVelocity.d.v[2] < 0.0f)
                m_localVelocity.d.v[2] = MathFunClamp<float>(m_localVelocity.d.v[2], -0.21f, 0.0f);
            else
                m_localVelocity.d.v[2] = MathFunClamp<float>(m_localVelocity.d.v[2], 0.0f, 0.21f);
        }
    } else {
        jump = false;
    }
    if (m_395_40 && !m_396_02 && m_heightSpeed > 0.0f) {
        m_localVelocity.d.v[1] = 0.0f;
        m_localVelocity.d.v[0] = 0.0f;
    }
    m_tm.TransformVector(m_velocity, m_localVelocity.d);
    if (m_deathTime > 0.0f || m_397_40) {
        m_velocity.d.v[0] = 0.0f;
        m_velocity.d.v[1] = 0.0f;
        m_localVelocity.d.v[0] = 0.0f;
        m_localVelocity.d.v[1] = 0.0f;
        if (m_deathTime > 0.0f)
            UpdateDeathSequence(dt);
    }
    if (!m_usingMountedWeapon) {
        ShakeCamera(pitchRate, yawRate, dt);
        m_pitch = pitchRate * dt + m_pitch;
        m_yaw = yawRate * dt + m_yaw;
        if (m_yaw > (double)6.2831855f)
            m_yaw -= (double)6.2831855f;
        else if (m_yaw < -(double)6.2831855f)
            m_yaw += (double)6.2831855f;
        if (m_recoil < 0.0f) {
            if (m_pitch > m_lastPitch)
                m_recoil += m_pitch - m_lastPitch;
            float step;
            if (m_recoil < 0.0f) {
                CPlayerWeaponObject* playerWeapon = GetCurrentPlayerWeapon();
                step = 0.003f * playerWeapon->m_recoilRecovery * dt;
                if (-step < m_recoil) {
                    step = -m_recoil;
                    m_recoil = 0.0f;
                } else {
                    m_recoil += step;
                }
            } else {
                step = 0.0f;
                m_recoil = 0.0f;
            }
            m_pitch += step;
        }
        m_pitch = MathFunClamp<float>(m_pitch, -1.45f, 1.45f);
        float pitch = m_pitch;
        float recoil = MathFunClamp<float>(m_recoilPitch * dt, 0.0f, m_recoilPitch);
        m_pitch -= recoil;
        if (fabs(m_recoilYaw) > 0.0) {
            m_yaw += MathFunRandomRealSigned(-m_recoilYaw / 2.0f, m_recoilYaw / 2.0f);
            m_recoilYaw = 0.0f;
        }
        m_recoilStep = -recoil;
        m_pitch = MathFunClamp<float>(m_pitch, -1.45f, 1.45f);
        if (fabs(m_recoilPitch) < 0.001f)
            m_recoilPitch = 0.0f;
        m_recoil += m_pitch - pitch;
        m_lastPitch = m_pitch;
        m_recoilPitch -= recoil;
        if (m_recoilPitch < 0.0f)
            m_recoilPitch = 0.0f;
    }
    UpdateMovePath(dt);
    if (!m_396_02 || m_396_01) {
        m_eyeMatrix.BuildRotX(-m_pitch);
        m_eyeMatrix.Translate(g_CameraOffset.d);
    } else {
        m_eyeMatrix.BuildTrans(g_CameraOffset.d);
    }
    CVector3 cameraPosition;
    g_pCamera->GetPosition(cameraPosition);
    m_camera.GetForward(m_cameraForward);
    float fire = (float)g_inputMgr.GetValue(m_controller, 14);
    if (m_useTimer >= 0.0f && !m_397_80) {
        m_useTimer += dt;
        if (m_useTimer > 120.0)
            m_useTimer = 120.0f;
    }
    bool fired = false;
    if (!IsDisabled()) {
        if (m_fireDelay >= 0.0 && !m_397_80) {
            if (m_fireDelay < dt) {
                FireMyWeapon(0.0f);
                BSObjectTriggerEvent(m_script, 160, 0, 0, true);
                fired = true;
                m_fireDelay = -1.0f;
            } else {
                m_fireDelay -= dt;
            }
        }
        if (!fired && !m_397_80 && (m_currentWeapon >= 0 || m_usingMountedWeapon)) {
            if (fire) {
                if (m_jumpTimer <= 0) {
                    BSObjectTriggerEvent(m_script, 16, *(void**)&fire, 0, true);
                    m_fireValue = fire;
                    if (m_usingMountedWeapon && m_mountedWeapon != m_lastWeapon) {
                        static float osc = 0.0f;
                        static float osc_add = 1.0f;
                        static float max_osc = 1.0f;
                        static float osc_dist = 0.006f;
                        CMatrix matrix;
                        CVector3 offset;
                        if (!m_396_10) {
                            m_weaponNode->GetTMLocalToWorld(matrix);
                            offset.Scale(matrix.GetFront(), osc_dist * osc);
                            offset.Add(matrix.GetPos());
                            matrix.SetPos(offset);
                            m_weaponNode->SetTMLocalToWorld(matrix);
                        } else {
                            osc_dist = 0.02f;
                            offset.Scale(m_mountMatrix.GetFront(), osc_dist * osc);
                            offset.Add(m_mountMatrix.GetPos());
                            m_mountMatrix.SetPos(offset);
                        }
                        osc = osc_add * dt + osc;
                        if (osc >= max_osc || osc <= -max_osc) {
                            osc = osc_add * max_osc;
                            osc_add = -osc_add;
                        }
                    }
                }
            } else if (!fire && m_fireValue) {
                BSObjectTriggerEvent(m_script, 148, *(void**)&fire, 0, true);
                if (m_usingMountedWeapon)
                    m_mountedWeapon->Shoot((EWeaponShootType)1, 0.0f, -1.0f);
                else
                    m_weapons[m_currentWeapon]->Shoot((EWeaponShootType)1, 0);
            } else {
                if (m_usingMountedWeapon)
                    m_mountedWeapon->Shoot((EWeaponShootType)1, 0.0f, -1.0f);
                else
                    m_weapons[m_currentWeapon]->Shoot((EWeaponShootType)1, 0);
            }
        }
        if (!m_397_80) {
            if (g_inputMgr.GetValue(m_controller, 21))
                BSObjectTriggerEvent(m_script, 20, 0, 0, false);
            if (g_inputMgr.GetValue(m_controller, 15))
                BSObjectTriggerEvent(m_script, 17, 0, 0, false);
            if (g_inputMgr.GetValue(m_controller, 19))
                BSObjectTriggerEvent(m_script, 18, 0, 0, false);
            if (g_inputMgr.GetValue(m_controller, 20))
                BSObjectTriggerEvent(m_script, 19, 0, 0, false);
            if (g_inputMgr.GetValue(m_controller, 13)) {
                if (!g_bInMultiplayerMode) {
                    if (m_398_80 && m_useTimer < 0.0f && m_fireDelay < 0.0f)
                        TriggerActivateMessages(((PlayerScriptView*)m_script)->m_basic, false);
                } else if (g_Shell.PassPlayersMultiPlayerControllerValue(m_controller) != 2) {
                    BSObjectTriggerEvent(m_script, 18, 0, 0, false);
                }
            }
        }
        float speed = m_velocity.Length();
        if (speed > 0.07340675f) {
            if (m_speedState != 2) {
                BSObjectTriggerEvent(m_script, 130, (void*)2, 0, false);
                m_speedState = 2;
            }
        } else if (speed > 0.0091758426f) {
            if (m_speedState != 1) {
                BSObjectTriggerEvent(m_script, 130, (void*)1, 0, false);
                m_speedState = 1;
            }
        } else if (m_speedState != 0) {
            BSObjectTriggerEvent(m_script, 130, 0, 0, false);
            m_speedState = 0;
        }
    }
    m_position = m_tm.GetPos();
    m_crouchHeight = m_localCapsule.m_axis.GetEnd().d.v[2];
    m_doodad.PreUpdate();
    PlaySounds(onGround, jump, fall, dt);
    if (g_inputMgr.GetValue(m_controller, 126) && !g_bInMultiplayerMode)
        DisplayHint();
}
