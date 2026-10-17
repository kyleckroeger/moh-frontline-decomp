// A fragment of sound.cpp (0x800eb4b8): PlayReloadSound begins event 6 for
// a reload (sound id + 101) at an object with a script object: the azimuth
// from GetAzimuth's inline helper (sound_fail.cpp; no distance asked for)
// and the distance, three times 256 per unit, from the player's position
// (the object's position is reduced in place); both 0 without a script
// object. The event is begun through the same inferred inline as
// AEMS_BeginEvent (sound_fail.cpp). Then PlayAnimTriggeredSound looks up an
// animation's sound event in the sound table and, when there is one, plays
// it as a scripted sound at the object's position, and PlayEnemyMovement
// (outside multiplayer) begins event 4 for an enemy's movement sound: sound
// id + movement - 51713, with 100 and 101 mapped to 23 and 27 and other
// values from 100 up dropped, the object's value at +9424, the azimuth (the
// closest player's in multiplayer, a branch the early return makes dead) and
// the distance as in PlayReloadSound; the azimuth is read into a local before
// the distance, which places its load as in the original. The classes,
// functions, g_soundMapSingleton and g_bInMultiplayerMode are named by the
// symbols; the sound parameters (16 bytes), the object's position and
// script-object slots (+64, +96), its value at +9424, the event views (32
// bytes here) and the helpers are inferred. The rest of the file is in other
// units.
#include <math.h>

extern "C" {
double atan2(double, double);
extern int (*SNDAEMS_beginevent)(int*);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3& operator-=(const CVector3& o) {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }
    float LengthSq() const { return x * x + y * y + z * z; }
    float Length() const { return sqrtf(LengthSq()); }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

struct BSObject;

class CSoundTable {
public:
    struct SSoundParms {
        unsigned char unknown00[16];
    };

    bool GetSoundEvent(int, SSoundParms*) const;

    unsigned char unknown00[4];
};

extern CSoundTable g_soundMapSingleton;

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void FastInverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class ISceneNode {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void* GetScriptObject() const;
};

class CPlayerObject : public ISceneNode {
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;
    CPlayerObject* FindClosestPlayer(CVector3);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CAnimObject : public ISceneNode {
public:
    unsigned char unknown004[9420];
    short m_unknown24d0;
};

extern bool g_bInMultiplayerMode;

extern float g_sfxVolume;
void msg(char*, ...);
void err(char*, ...);

inline int BeginEvent(int* event) {
    if (event[0] == 7 && event[1] == 3 && event[2] < 3)
        event[9] = (float)event[9] * g_sfxVolume;
    SNDSYS_entercritical();
    int result = SNDAEMS_beginevent(event);
    SNDSYS_leavecritical();
    if (result < 0 && result != -8 && result != -9) {
        err("SNDAEMS_beginevent number %d failed %d", event[0], result);
        result = -1;
    } else {
        msg("SNDAEMS_beginevent number %d handle=%d\n", event[0], result);
    }
    return result;
}

inline void CalcDistanceAndAzimuth(unsigned short* distance, unsigned short* azimuth, CVector3 source,
                                   const CPlayerObject* player) {
    CVector3 listener;
    player->GetPosition(listener);
    CVector3 delta;
    delta.x = source.x - listener.x;
    delta.y = source.y - listener.y;
    delta.z = source.z - listener.z;
    if (distance)
        *distance = 256.0f * sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    if (azimuth) {
        CMatrix toWorld;
        player->GetTMLocalToWorld(toWorld);
        CMatrix toLocal;
        toLocal.FastInverse(toWorld);
        toLocal.TransformVector(delta, delta);
        delta.z = 0.0f;
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z > 0.0001f) {
            float angle = (float)atan2(delta.x, delta.y);
            *azimuth = 65536.0f * (0.15915494f * angle);
        } else {
            *azimuth = 0;
        }
    }
}

void PlayScriptedSound(ISceneNode*, CVector3, const CSoundTable::SSoundParms&, int, BSObject*);

void PlayReloadSound(long id, CAnimObject* object) {
    int azimuth;
    int distance;
    if (!object->GetScriptObject()) {
        azimuth = 0;
        distance = 0;
    } else {
        CPlayerObject* player = g_scene.GetPlayer(0);
        CVector3 playerPosition;
        player->GetPosition(playerPosition);
        CVector3 position;
        object->GetPosition(position);
        unsigned short az = 0;
        CalcDistanceAndAzimuth(0, &az, position, player);
        azimuth = az;
        position.x -= playerPosition.x;
        position.y -= playerPosition.y;
        position.z -= playerPosition.z;
        distance = (unsigned short)(3.0f * (256.0f * position.Length()));
    }
    int event[4];
    event[0] = 6;
    event[1] = id + 101;
    event[2] = (unsigned short)azimuth;
    event[3] = (unsigned short)distance;
    BeginEvent(event);
}

void PlayAnimTriggeredSound(long id, CAnimObject* object) {
    CVector3 position;
    object->GetPosition(position);
    CSoundTable::SSoundParms parms;
    if (g_soundMapSingleton.GetSoundEvent(id, &parms))
        PlayScriptedSound(object, position, parms, -1, 0);
}

void PlayEnemyMovement(unsigned short movement, long id, CAnimObject* object) {
    int event[8];
    if (g_bInMultiplayerMode)
        return;
    CVector3 position;
    object->GetPosition(position);
    CPlayerObject* player;
    if (g_bInMultiplayerMode)
        player = g_scene.FindClosestPlayer(position);
    else
        player = g_scene.GetPlayer(0);
    CVector3 playerPosition;
    player->GetPosition(playerPosition);
    unsigned short azimuth = 0;
    CalcDistanceAndAzimuth(0, &azimuth, position, player);
    int az = azimuth;
    position -= playerPosition;
    int distance = 3.0f * (256.0f * position.Length());
    int sound = movement + (id - 51713);
    if (sound >= 100) {
        switch (sound) {
        case 100:
            sound = 23;
            break;
        case 101:
            sound = 27;
            break;
        default:
            return;
        }
    }
    event[0] = 4;
    event[1] = sound;
    event[2] = object->m_unknown24d0;
    event[3] = 2;
    event[4] = az;
    event[5] = distance;
    event[6] = 1;
    BeginEvent(event);
}
