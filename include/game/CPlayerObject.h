// CPlayerObject: the player (movement, weapons, camera, interface), shared by every file that
// uses it (PortReadiness step 3.3).
//
// Evidence:
// - __vt__13CPlayerObject (0x80187cfc): IMovingSceneNode's 71 slots, then one new slot,
//   SetScript (+0x124). The overrides are declared below; the slot names are the original's
//   (GetScriptObject at +0x60, which some views called GetInventory; SetScript, which some
//   called SetScriptObject).
// - Weak in the original, so inline here with the matched bodies: Destroy, both AsPlayerObject,
//   GetWorldLinearVelocity, GetAIDoodad const (player.cpp), GetAIDoodad (bsbifunc_level.cpp),
//   SetCollisionId (bsbifunc_playercoll.cpp), GetScriptObject (objcreate_playerscript.cpp).
//   Every other member function is out of line (player*.cpp units).
// - The table is emitted with the destructor, the first virtual declared here; player_dtor.cpp,
//   which defines it, keeps its own view so that its fragment doesn't emit the table.
// - 0x12d0 bytes (mainloop.cpp: `new CPlayerObject`). Field offsets: the matched views and the
//   layout lint (research/layout/classes.tsv); names are the most common matched ones.
//
// Inferred, not in any mangled name: the return types (the matched views'), every field name
// and scalar type, and the unknown ranges. Layout only: embedded objects with no shared header
// (the AI doodad, the volumes, the contact list, the hearing volume, the script object, the
// user interface) are byte ranges named by what they hold.
#ifndef GAME_CPLAYEROBJECT_H
#define GAME_CPLAYEROBJECT_H

#include "game/CCamera.h"

class CAIDoodad;
class CAISplinePath;
class CHierObject;
class CPlayerWeaponObject;
class CScreen;
class CSoldierObject;
class CSprite;
class CWeapon;
struct MPPlayerData;
struct BS_STRUCT_HitByBulletInfo_struct;
enum EWeaponTypes {};

class CPlayerObject : public IMovingSceneNode {
public:
    CPlayerObject();
    virtual ~CPlayerObject();
    virtual void Destroy() { delete this; }
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
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
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId id) { m_collisionId = id; }
    virtual BSObject* GetScriptObject() const { return m_script; }
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CPlayerObject* AsPlayerObject() { return this; }
    virtual const CPlayerObject* AsPlayerObject() const { return this; }
    virtual CAIDoodad* GetAIDoodad() { return (CAIDoodad*)m_doodad; }
    virtual const CAIDoodad* GetAIDoodad() const { return (const CAIDoodad*)m_doodad; }
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
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
    virtual CVector3 GetWorldLinearVelocity() const { return m_velocity; }
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual void SetScript(BSObject*);

    static void InitClass(CScreen*, CCamera*);
    static int GetWeaponInfoByCRC(int, int*, int*);
    void Init();
    void InitAI();
    void ReInit();
    void InitMultiplayer(MPPlayerData*, bool);
    void CleanUp();
    void Respawn();
    bool AddAmmoOrWeapon(int, int, void*);
    bool AddHealth(int, int, void*);
    void BeginCrouch();
    void BeginStand();
    void StartPlayerCrouched();
    bool IsPlayerCrouching() const;
    bool IsPlayerJumping() const;
    bool CanPlayerSwitchWeapon();
    void CycleWeapon(int, int);
    void FireMyWeapon(float);
    void StartGrenadeCook();
    void DropMPDumpObject();
    void DuckDown(float);
    void PeekLeftRight();
    void PeekUp(float);
    void DoMotionShake(float, float);
    void StartCameraShake(float, float);
    void StopCameraShake(float);
    void ShakeCamera(float&, float&, float);
    void TransitionFOV(float, float);
    void StartDeathSequence();
    void UpdateDeathSequence(float);
    void MoveOnPath(CAISplinePath*, CAISplinePath*, float);
    void UpdateMovePath(float);
    void StopPath();
    void PlaySounds(bool, bool, float, float);
    void SetClimbingState(bool);
    void SetFallingDamageState(bool);
    void SetFinishedLevelData();
    void SetHideWeapon(bool);
    void SetMPShotLocation(BS_STRUCT_HitByBulletInfo_struct*);
    void SetPlayersMountedMGPositions(CMatrix*);
    void SetUsingMountedMachineGun(bool, CStaticObject*);
    bool SetWeaponEnable(EWeaponTypes, bool);
    char* GetPlayerName();
    CWeapon* GetCurrentWeapon() const;
    CPlayerWeaponObject* GetCurrentPlayerWeapon() const;
    CPlayerWeaponObject* GetWeapon(int) const;
    CPlayerWeaponObject* GetWeaponByCRC(int) const;
    CSprite* GetCurrentBulletSprite() const;
    int GetCurrentWeaponClipAmoCount() const;
    int GetCurrentWeaponReserveAmoCount() const;

    unsigned char m_doodad[0x54];          // +0x24 the player's AI doodad (a CAIDoodad)
    float m_value078;
    float m_value07c;
    CMatrix m_tm;                          // +0x80
    CMatrix m_cameraTM;                    // +0xc0
    IVolume* m_localVolume3;               // +0x100
    IVolume* m_worldVolume3;
    unsigned char unknown108[8];
    unsigned char m_boxA[0x50];            // +0x110 a CVolBox
    unsigned char m_boxB[0x50];            // +0x160 a CVolBox
    unsigned char m_localCapsule[0x50];    // +0x1b0 a CVolCapsule
    unsigned char m_worldCapsule[0x50];    // +0x200 a CVolCapsule
    BSObject* m_script;                    // +0x250
    unsigned char unknown254[4];
    CVector3 m_velocity;                   // +0x258
    CVector3 m_localVelocity;              // +0x268
    CVector3 m_force;                      // +0x278
    CVector3 m_position;                   // +0x288
    float m_crouchHeight;                  // +0x298
    float m_yaw;
    float m_pitch;
    float m_heightSpeed;
    unsigned char m_collisions[0x18];      // +0x2a8 dwi::fast_vec<CPlayerObject::CClsnInfo>
    unsigned char m_hearing[0xd0];         // +0x2c0 a CHearingVolume
    float m_value390;                      // +0x390
    // +0x394..+0x398: flag bits, most significant first (names: the matched views')
    unsigned char m_landed : 1;                 // +0x394
    unsigned char m_onFloor : 1;
    unsigned char m_onGround : 1;
    unsigned char m_collided : 1;
    unsigned char m_aiming : 1;
    unsigned char m_flag394_04 : 1;
    unsigned char m_flag394_02 : 1;
    unsigned char m_climbing : 1;
    unsigned char m_crouched : 1;                 // +0x395
    unsigned char m_crouchStarted : 1;
    unsigned char m_cameraShaking : 1;
    unsigned char m_peeking : 1;
    unsigned char m_onPath : 1;
    unsigned char m_forceApplied : 1;
    unsigned char m_flag395_02 : 1;
    unsigned char m_flag395_01 : 1;
    unsigned char m_usingMountedWeapon : 1;                 // +0x396
    unsigned char m_noShells : 1;
    unsigned char m_peekStarted : 1;
    unsigned char m_mgIsHier : 1;
    unsigned char m_mountingMG : 1;
    unsigned char m_mgHasWeapon : 1;
    unsigned char m_noExplosionPush : 1;
    unsigned char m_flag396_01 : 1;
    unsigned char m_weaponHidden : 1;                 // +0x397
    unsigned char m_dead : 1;
    unsigned char m_fallingDamage : 1;
    unsigned char m_flag397_10 : 1;
    unsigned char m_canCrouch : 1;
    unsigned char m_motionShaking : 1;
    unsigned char m_flag397_02 : 1;
    unsigned char m_wielding : 1;
    unsigned char m_triggerChecks : 1;                 // +0x398
    unsigned char m_superAim : 1;
    unsigned char m_flag398_20 : 1;
    unsigned char m_flag398_10 : 1;
    unsigned char m_flag398_08 : 1;
    unsigned char m_flag398_04 : 1;
    unsigned char m_flag398_02 : 1;
    unsigned char m_flag398_01 : 1;
    unsigned char unknown399[3];
    float m_footstepTimer;                 // +0x39c
    float m_cameraShakeStrength;
    float m_cameraShakeRate;
    float m_cameraShakeTime;
    float m_motionShakeStrength;
    float m_motionShakeTime;
    float m_motionShakeElapsed;
    float m_deathTime;                     // +0x3b8
    unsigned char unknown3bc[4];
    CVector3 m_cameraForward;              // +0x3c0
    unsigned char unknown3d0[16];
    CVector3 m_lastGroundPosition;         // +0x3e0
    CVector3 m_peekRight;                  // +0x3f0
    CAISplinePath* m_path;                 // +0x400
    CAISplinePath* m_nextPath;
    float m_pathTime;
    float m_pathRate;
    float m_fallStartZ;                    // +0x410
    float m_mgYawLimit;
    float m_mgPitchLimit;
    float m_mgYaw;
    float m_mgPitch;                       // +0x420
    unsigned char unknown424[8];
    float m_fov;                           // +0x42c
    float m_zoomFov;
    float m_targetFov;
    float m_fovSpeed;
    CSprite* m_muzzleFlash;                // +0x43c
    float m_recoilYaw;                     // +0x440
    float m_recoilPitch;
    float m_field448;
    float m_peek;
    float m_duck;                          // +0x450
    float m_fireDelay;
    float m_grenadeHoldTime;
    int m_currentWeapon;                   // +0x45c
    CPlayerWeaponObject* m_weapons[45];    // +0x460
    int m_soldierWeapons[45];              // +0x514
    CHierObject* m_mgObject;               // +0x5c8
    IMovingSceneNode* m_weaponNode;
    CStaticObject* m_lastMG;
    CWeapon* m_mgWeaponA;
    CWeapon* m_mgWeaponB;                  // +0x5d8
    CWeapon* m_lastWeapon;
    CWeapon* m_mountedWeapon;              // +0x5e0
    unsigned char unknown5e4[12];
    CMatrix m_flashMatrix;                 // +0x5f0
    CMatrix m_mountMatrix;                 // +0x630
    CMatrix m_mgSaved;                     // +0x670
    int m_mgCollisionId;                   // +0x6b0
    float m_turnScale;
    short m_savedClipSize;
    unsigned char unknown6ba[18];
    EClsnId m_collisionId;                 // +0x6cc
    CVector3 m_flashScale;                 // +0x6d0
    CVector3 m_mgDirection;                // +0x6e0
    int m_mgIndex;                         // +0x6f0
    CSprite* m_crossHair;
    unsigned char m_throwHelmet;           // +0x6f8
    unsigned char unknown6f9[7];
    CMatrix m_handMatrix;                  // +0x700
    CVector3 m_scriptPosition;             // +0x740
    short m_soundHandle;                   // +0x750
    short m_landTimer;
    float m_fireValue;
    float m_delta;                         // +0x758
    float m_fallScale;
    int m_value760;                        // +0x760
    int m_controller;
    int m_viewport;
    int m_playerIndex;                     // +0x76c
    unsigned char m_scriptObject[0x40];    // +0x770 the player's script object (a BSGO_Player)
    int m_speedState;                      // +0x7b0
    float m_recoil;
    float m_lastPitch;
    float m_recoilStep;
    float m_value7c0;                      // +0x7c0
    unsigned char m_value7c4;
    unsigned char unknown7c5[3];
    CSoldierObject* m_soldier;             // +0x7c8
    unsigned char unknown7cc[4];
    CCamera m_camera;                      // +0x7d0
    unsigned char m_ui[0x7c4];             // +0xa70 the player's UserInterface
    float m_unknown1234;                   // +0x1234
    unsigned char unknown1238[0x38];
    unsigned char m_showUI;                // +0x1270
    unsigned char unknown1271[0x17];
    int m_viewport1288;                    // +0x1288
    float m_timeRemaining;
    float m_timeFlash;                     // +0x1290
    unsigned char unknown1294[4];
    short m_value1298;
    char m_name[16];                       // +0x129a
    unsigned char unknown12aa[6];
    int m_team;                            // +0x12b0
    unsigned char m_jumping;               // +0x12b4
    unsigned char unknown12b5[3];
    int m_jumpTimer;                       // +0x12b8
    float m_startYaw;
    float m_startPitch;                    // +0x12c0
    unsigned char unknown12c4[12];
};

#endif
