// A fragment of sound.cpp (0x800eb17c); the file name is this project's.
// PlayImpactSound starts an impact sound (event 3 with the two sound values)
// at a position: unless it is flagged as on the player, the azimuth comes
// from GetAzimuth's inline helper (sound_fail.cpp) and the distance, 256 per
// unit rounded, from the player's position; the event is begun through the
// same inferred inline as AEMS_BeginEvent. The functions and globals are
// named by the symbols; the event view, the parameter meanings and the
// helpers are inferred. The rest of the file is in other units.
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
    unsigned char unknown000[328];
};

extern CScene g_scene;

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
