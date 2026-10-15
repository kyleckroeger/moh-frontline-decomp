// A fragment of player.cpp (0x8009c6fc): CPlayerObject::DropMPDumpObject
// drops the current weapon's dump object (type and flag from the weapon's
// dump info) at the player's position with an identity rotation;
// SetMPShotLocation, when the hit counts and the player's stats counter is
// positive, maps the hit location (0-20) to a shot-location group and records
// it in the shell menu for the shooter and this player; GetPlayerName returns
// the name buffer. SetHideWeapon triggers script event 275 when a hidden
// weapon is shown and 274 when a shown one is hidden, then records the state
// (a bool bit field, bit 0x80 of +919). IsPlayerCrouching: the player is
// crouching when the flag at bit 0x80 of +918 is set or its height (+664) is
// below 0.66, or in multiplayer when the height less 0.7 is below 0.24000002
// (the constant's exact value; how the original wrote it is not known). The
// classes, functions and g_Shell (its size is its symbol's) are named by the
// symbols; members are inferred from offsets, the views (and their
// placeholder virtual slots: AsPlayerObject +156 and TriggerScriptEvent +100
// are named by the player's vtable) are not original, nor are the location
// group numbers, the event numbers or the quaternion layout (scalar first, as
// in quaternion.cpp). IsPlayerJumping returns the jump flag. PlaySounds
// sends the player's AEMS sound events: the start event, landing events
// (heavy above 4.0, light above 0.25, with a land timer that stops them),
// and footsteps paced by a timer that shortens with the speed (the speed
// scaled to 0..1); it also makes AI noise. DuckDown (only while the flag at
// bit 0x08 of +919 is set) and PeekUp set the height speed (+676) toward a
// target height blended by the fraction between the standing height 1.1 and
// the crouched height (0.7 in multiplayer, else 0). The event views, flag
// and field names are inferred, and the MSL-style inline sqrtf is as in
// bsbifunc_targetsearch.cpp. The rest of the file is not part of this unit.
extern bool g_bInMultiplayerMode;

void AEMS_SendEvent(void*);
void MakeNoise(class ISceneNode*, int, float, int);

struct PlayerSoundEvent {
    int type;
    int event;
    int handle;
    int unknown0c;
    int unknown10;
    int unknown14;
    int unknown18;
};

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

class CShellMenu;
enum EShotLocations {};

class CShellMenu {
public:
    void Add_MPShotLocation(int, int, int, EShotLocations);

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;

class CPlayerObject;

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CQuaternion {
public:
    CQuaternion(float ax, float ay, float az, float aw) : w(aw), x(ax), y(ay), z(az) {}

    float w;
    float x;
    float y;
    float z;
};

void CreateDumpObject(const CVector3&, const CQuaternion&, int, int);

struct WeaponDumpInfoView {
    unsigned char unknown000[640];
    int* type;
    short flagBase;
    short flagOffset;
};

class CWeapon {
public:
    unsigned char unknown0000[12516];
    WeaponDumpInfoView* m_dumpInfo;
};

// A scene-node view: AsPlayerObject is the vtable's slot +156 (named by the
// player's vtable); the slots before it are placeholders.
class ShotNodeView {
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
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual CPlayerObject* AsPlayerObject();
};

class ShotOwnerView {
    unsigned char unknown00[12];

public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual ShotNodeView* unknown010();
};

class ShotSourceView {
public:
    unsigned char unknown00[12];
    ShotOwnerView* owner;
};

struct BS_STRUCT_HitByBulletInfo_struct {
    ShotSourceView* source;
    int hit;
    unsigned char unknown08[8];
    unsigned int location;
};

class ShotCountView {
    unsigned char unknown00[12];

public:
    virtual void unknown008();
    virtual int* unknown00c();
};

class ShotStatsView {
public:
    unsigned char unknown00[12];
    ShotCountView* counter;
};

class CPlayerObject {
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
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void TriggerScriptEvent(int, void*, bool);
    void DropMPDumpObject();
    void SetMPShotLocation(BS_STRUCT_HitByBulletInfo_struct*);
    char* GetPlayerName();
    void SetHideWeapon(bool);
    bool IsPlayerCrouching() const;
    bool IsPlayerJumping() const;
    void PlaySounds(bool, bool, float, float);
    void DuckDown(float);
    void PeekUp(float);

    unsigned char unknown004[172];
    CVector3 m_position;
    unsigned char unknown0c0[400];
    ShotStatsView* m_stats;
    unsigned char unknown254[4];
    float m_velocityX;
    float m_velocityY;
    float m_velocityZ;
    unsigned char unknown264[52];
    float m_height;
    unsigned char unknown29c[8];
    float m_heightSpeed;
    unsigned char unknown2a8[236];
    unsigned char m_flags394 : 7;
    bool m_flag394 : 1;
    unsigned char m_flags395a : 4;
    bool m_flag395 : 1;
    unsigned char m_flags395b : 3;
    unsigned char m_flag396 : 1;
    unsigned char m_flags396 : 7;
    bool m_weaponHidden : 1;
    unsigned char m_flags397a : 2;
    bool m_flag397 : 1;
    bool m_flag397b : 1;
    unsigned char m_flags397b : 3;
    unsigned char unknown398[4];
    float m_footstepTimer;
    unsigned char unknown3a0[188];
    int m_currentWeapon;
    CWeapon* m_weapons[188];
    short m_soundHandle;
    short m_landTimer;
    unsigned char unknown754[24];
    int m_playerIndex;
    unsigned char unknown770[2858];
    char m_name[16];
    unsigned char unknown12aa[10];
    bool m_jumping;
};

void CPlayerObject::DropMPDumpObject() {
    CWeapon* weapon = m_weapons[m_currentWeapon];
    if (weapon) {
        WeaponDumpInfoView* info = weapon->m_dumpInfo;
        int type = *info->type;
        int flag = info->flagBase + info->flagOffset;
        CQuaternion rotation(0.0f, 0.0f, 0.0f, 1.0f);
        CVector3 position = m_position;
        CreateDumpObject(position, rotation, type, flag);
    }
}

void CPlayerObject::SetMPShotLocation(BS_STRUCT_HitByBulletInfo_struct* info) {
    CPlayerObject* shooter = info->source->owner->unknown010()->AsPlayerObject();
    if (info->hit && *m_stats->counter->unknown00c() > 0) {
        int location;
        switch (info->location) {
        case 3:
        case 4:
        case 5:
            location = 1;
            break;
        case 6:
        case 7:
        case 8:
            location = 2;
            break;
        case 13:
        case 14:
        case 15:
            location = 4;
            break;
        case 16:
        case 17:
        case 18:
            location = 5;
            break;
        case 19:
        case 20:
            location = 6;
            break;
        case 0:
        case 1:
            location = 0;
            break;
        case 2:
        case 9:
        case 10:
        case 11:
        case 12:
            location = 3;
            break;
        default:
            location = 3;
            break;
        }
        g_Shell.Add_MPShotLocation(shooter->m_playerIndex, m_playerIndex, 1, (EShotLocations)location);
    }
}

char* CPlayerObject::GetPlayerName() {
    return m_name;
}

void CPlayerObject::SetHideWeapon(bool hide) {
    if (m_weaponHidden && !hide)
        TriggerScriptEvent(275, 0, false);
    else if (!m_weaponHidden && hide)
        TriggerScriptEvent(274, 0, false);
    m_weaponHidden = hide;
}

bool CPlayerObject::IsPlayerCrouching() const {
    if (g_bInMultiplayerMode) {
        bool crouching = false;
        if (m_flag396 || m_height - 0.7f < 0.24000002f)
            crouching = true;
        return crouching;
    }
    bool crouching = false;
    if (m_flag396 || m_height < 0.66f)
        crouching = true;
    return crouching;
}

bool CPlayerObject::IsPlayerJumping() const {
    return m_jumping;
}

void CPlayerObject::PlaySounds(bool moving, bool start, float landSpeed, float dt) {
    if (m_soundHandle == 0)
        m_soundHandle = 1;
    if (start) {
        PlayerSoundEvent event;
        event.type = 4;
        event.event = 0;
        event.handle = m_soundHandle;
        event.unknown0c = 1;
        event.unknown10 = 0;
        event.unknown14 = 0;
        event.unknown18 = 0;
        AEMS_SendEvent(&event);
    }
    if (landSpeed > 0.25f) {
        if (landSpeed > 4.0f) {
            m_landTimer = 10;
            PlayerSoundEvent event;
            event.type = 4;
            event.handle = m_soundHandle;
            event.unknown0c = 1;
            event.unknown10 = 0;
            event.unknown14 = 0;
            event.unknown18 = 0;
            event.event = 18;
            AEMS_SendEvent(&event);
            AEMS_SendEvent(&event);
            event.event = 9;
            AEMS_SendEvent(&event);
            event.event = 20;
            AEMS_SendEvent(&event);
        } else {
            m_landTimer = 1;
        }
        MakeNoise((ISceneNode*)this, 51, 0.5f, 1);
    }
    if (m_landTimer) {
        m_landTimer -= (int)(0.5f + dt);
        if (m_landTimer <= 0) {
            PlayerSoundEvent event;
            event.type = 4;
            event.handle = m_soundHandle;
            event.unknown0c = 1;
            event.unknown10 = 0;
            event.unknown14 = 0;
            event.unknown18 = 0;
            event.event = 9;
            AEMS_SendEvent(&event);
            event.event = 20;
            AEMS_SendEvent(&event);
            m_landTimer = 0;
        }
    }
    float speed = m_velocityX * m_velocityX + m_velocityY * m_velocityY + m_velocityZ * m_velocityZ;
    if (m_flag395 || speed < 1e-05f) {
        if (m_footstepTimer > 0.0f && (moving || m_flag394)) {
            PlayerSoundEvent event;
            event.type = 4;
            event.event = 4;
            event.handle = m_flag394 ? 6 : m_soundHandle;
            event.unknown0c = 1;
            event.unknown10 = 0;
            event.unknown14 = 0;
            event.unknown18 = 0;
            AEMS_SendEvent(&event);
            MakeNoise((ISceneNode*)this, 51, 0.0f, 1);
        }
        m_flag397 = true;
        m_footstepTimer = 0.0f;
    } else {
        float scale = sqrtf(speed) / 0.08074742f;
        if (scale > 1.0f)
            scale = 1.0f;
        else if (scale < 0.0f)
            scale = 0.0f;
        if (m_footstepTimer > 0.0f)
            m_footstepTimer -= 0.016683351f * dt;
        if (m_footstepTimer <= 0.0f) {
            if (m_flag397 || m_flag394)
                m_footstepTimer = 0.37f;
            else
                m_footstepTimer = 1.32f - scale;
            if (moving || m_flag394) {
                PlayerSoundEvent event;
                event.type = 4;
                event.event = (int)(4.0f * scale + 4.5f);
                event.handle = m_flag394 ? 6 : m_soundHandle;
                event.unknown0c = 1;
                event.unknown10 = 0;
                event.unknown14 = 0;
                event.unknown18 = 0;
                AEMS_SendEvent(&event);
                m_flag397 = false;
                MakeNoise((ISceneNode*)this, 51, 2.0f * speed / 0.00841961f, 1);
            }
        }
    }
}

void CPlayerObject::DuckDown(float t) {
    if (!m_flag397b)
        return;
    float low = g_bInMultiplayerMode ? 0.7f : 0.0f;
    float high = 1.1f;
    float target = low * t + high * (1.0f - t);
    m_heightSpeed = 0.04170838f * (target - m_height) / 0.1f;
}

void CPlayerObject::PeekUp(float t) {
    float low = g_bInMultiplayerMode ? 0.7f : 0.0f;
    float high = 1.1f;
    float target = high * t + low * (1.0f - t);
    m_heightSpeed = 0.04170838f * (target - m_height) / 0.1f;
}
