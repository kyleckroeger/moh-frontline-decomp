// A fragment of sound.cpp (0x800ea76c); the file name is this project's.
// PlayBulletWhiz begins event 5 for a bullet passing the player: the azimuth
// of the shot's start, the azimuth of the point where it passes (the start
// moved along the normalized direction by twice the distance times the
// cosine between the direction and the line to the player) and the distance
// to the player; it returns the event's result. PlayBackgroundImpactSound
// begins event 3 for a bullet's impact (the given sound) unless the bullet is
// nearly still (squared speed under 0.01) or is a thrown bullet past its
// third impact (its count at +508 raised first): kind 1 for a projectile, 5
// for a thrown bullet of type 5 (+164), else 2, with the azimuth (the closest
// player's in multiplayer) and the distance, 256 per unit, from the player's
// position (the bullet's position reduced in place). Then PlayImpactSound
// starts an impact sound (event 3 with the two sound values) at a position:
// unless it is flagged as on the player, the azimuth and the distance, 256
// per unit rounded, from the player's position. Every azimuth comes from
// GetAzimuth's inline helper (sound_fail.cpp) and every event is begun
// through the same inferred inline as AEMS_BeginEvent. The functions,
// globals and CBullet's virtuals are named by the symbols; the thrown-bullet
// view, the event views (16 bytes for the whiz), the parameter meanings, the
// vector helpers and the helpers are inferred. The rest of the file is in
// other units.
extern "C" {
double atan2(double, double);
extern int (*SNDAEMS_beginevent)(int*);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
}
void msg(char*, ...);
void err(char*, ...);
// File-local in the original; declared extern so this fragment can refer to it.
extern float g_sfxVolume;

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
        return y;
    }
    return x;
}

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    float LengthSquared() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    float Length() const { return sqrtf(LengthSquared()); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }
    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    void FastInverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;
    float m[4][4];
} __attribute__((aligned(16)));

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
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual int GetLocalBoundingVolume(int) const;
    virtual int GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
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

class CProjectileBullet;

/* Inferred view: the thrown bullet's type (+164) and impact count (+508). */
class CThrownBullet {
public:
    unsigned char unknown000[164];
    int m_type;
    unsigned char unknown0a8[340];
    int m_impacts;
};

class CBullet : public ISceneNode {
public:
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
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
    virtual void vfe8();
    virtual void vfec();
    virtual void vff0();
    virtual void vff4();
    virtual void vff8();
    virtual void vffc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual void vf130();
    virtual void vf134();
    virtual void* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
};

extern bool g_bInMultiplayerMode;

/* Inferred inline helper, as in sound_fail.cpp (where GetAzimuth passes no
   distance). Its locals are placed from the bottom of the frame. */
inline void CalcDistanceAndAzimuth(unsigned short* distance, unsigned short* azimuth, CVector3 source,
                                   const CPlayerObject* player) {
    CVector3 listener;
    player->GetPosition(listener);
    CVector3 delta;
    delta.d.v[0] = source.d.v[0] - listener.d.v[0];
    delta.d.v[1] = source.d.v[1] - listener.d.v[1];
    delta.d.v[2] = source.d.v[2] - listener.d.v[2];
    if (distance)
        *distance = 256.0f * sqrtf(delta.d.v[0] * delta.d.v[0] + delta.d.v[1] * delta.d.v[1] + delta.d.v[2] * delta.d.v[2]);
    if (azimuth) {
        CMatrix toWorld;
        player->GetTMLocalToWorld(toWorld);
        CMatrix toLocal;
        toLocal.FastInverse(toWorld);
        toLocal.TransformVector(delta, delta);
        delta.d.v[2] = 0.0f;
        if (delta.d.v[0] * delta.d.v[0] + delta.d.v[1] * delta.d.v[1] + delta.d.v[2] * delta.d.v[2] > 0.0001f) {
            float angle = (float)atan2(delta.d.v[0], delta.d.v[1]);
            *azimuth = 65536.0f * (0.15915494f * angle);
        } else {
            *azimuth = 0;
        }
    }
}

/* Inferred inline: AEMS_BeginEvent and AEMS_SendEvent are the same code. An
   effects event (type 7, kind 3, channel below 3) has its volume scaled by the
   effects volume first. */
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

int PlayBulletWhiz(CVector3 position, CVector3 direction, CPlayerObject* player) {
    CVector3 playerPosition;
    CVector3 closest;
    int event[4];
    CVector3 start = position;
    unsigned short startAzimuth = 0;
    CalcDistanceAndAzimuth(0, &startAzimuth, start, player);
    int first = startAzimuth;
    player->GetPosition(playerPosition);
    CVector3 delta;
    delta.d.v[0] = playerPosition.d.v[0] - position.d.v[0];
    delta.d.v[1] = playerPosition.d.v[1] - position.d.v[1];
    delta.d.v[2] = playerPosition.d.v[2] - position.d.v[2];
    float distance = delta.Length();
    delta.Normalize();
    direction.Normalize();
    float along = (float)__fabs(distance * (delta.d.v[0] * direction.d.v[0] + delta.d.v[1] * direction.d.v[1] + delta.d.v[2] * direction.d.v[2]));
    along *= 2.0f;
    CVector3 step;
    step.d.v[0] = along * direction.d.v[0];
    step.d.v[1] = along * direction.d.v[1];
    step.d.v[2] = along * direction.d.v[2];
    closest.d.v[0] = position.d.v[0] + step.d.v[0];
    closest.d.v[1] = position.d.v[1] + step.d.v[1];
    closest.d.v[2] = position.d.v[2] + step.d.v[2];
    unsigned short endAzimuth = 0;
    CalcDistanceAndAzimuth(0, &endAzimuth, closest, player);
    event[0] = 5;
    event[1] = first;
    event[2] = endAzimuth;
    event[3] = distance;
    return BeginEvent(event);
}

void PlayBackgroundImpactSound(CBullet* bullet, int sound) {
    int event[6];
    CVector3 velocity;
    bullet->GetVelocity(velocity);
    if (velocity.d.v[0] * velocity.d.v[0] + velocity.d.v[1] * velocity.d.v[1] + velocity.d.v[2] * velocity.d.v[2] < 0.01f)
        return;
    CProjectileBullet* projectile = bullet->AsProjectile();
    CThrownBullet* thrown = bullet->AsThrown();
    if (thrown && ++thrown->m_impacts > 3)
        return;
    CVector3 position;
    bullet->GetPosition(position);
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
    position.d.v[0] -= playerPosition.d.v[0];
    position.d.v[1] -= playerPosition.d.v[1];
    position.d.v[2] -= playerPosition.d.v[2];
    float distance = 256.0f * sqrtf(position.d.v[0] * position.d.v[0] + position.d.v[1] * position.d.v[1] + position.d.v[2] * position.d.v[2]);
    event[0] = 3;
    event[4] = sound;
    if (projectile) {
        event[1] = 1;
    } else {
        event[1] = 2;
        if (thrown) {
            if (thrown->m_type == 5)
                event[1] = 5;
            else
                event[1] = 2;
        }
    }
    event[3] = az;
    event[2] = distance;
    BeginEvent(event);
}

void PlayImpactSound(unsigned short sound, unsigned short bank, CVector3 position, bool onPlayer) {
    int event[6];
    event[0] = 3;
    event[1] = bank;
    event[4] = sound;
    if (onPlayer) {
        event[3] = 0;
        event[2] = 0;
    } else {
        CPlayerObject* player = g_scene.GetPlayer(0);
        CVector3 listener;
        player->GetPosition(listener);
        CVector3 source = position;
        unsigned short azimuth = 0;
        CalcDistanceAndAzimuth(0, &azimuth, source, player);
        int az = azimuth;
        float dx = position.d.v[0] - listener.d.v[0];
        float dy = position.d.v[1] - listener.d.v[1];
        float dz = position.d.v[2] - listener.d.v[2];
        float distance = 256.0f * sqrtf(dx * dx + dy * dy + dz * dz);
        event[3] = az;
        event[2] = 0.5f + distance;
    }
    BeginEvent(event);
}
