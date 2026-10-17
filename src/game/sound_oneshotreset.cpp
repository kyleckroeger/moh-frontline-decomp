// A fragment of sound.cpp (0x800ec834): ResetOneShotBank waits (two vsyncs
// at a time, updating the REAL streams and polling the asynchronous load)
// while the one-shot bank is loading, then, when it is loaded, removes it and
// clears its id, handle, load state and memory offset. AEMS_Update polls the
// one-shot load the same way, then, while the ambient track plays, sends its
// position relative to the player (event 12: azimuth and distance, both 0
// when the source is on the player) through the same inferred inline helper
// as GetAzimuth in sound_fail.cpp, here with both results (distance 256 per
// unit; MSL inline sqrtf), and reports a failed update. SoundSync, the only
// caller, does not inline AEMS_Update in the original although the compiler
// would inline this body, so the original body is larger to the inliner
// than its code shows; keeping SoundSync in another unit leaves the call. The functions and
// globals are named by the symbols; the load-state values (1 loading,
// 2 loaded), the CScreen view (sized by g_screen) and the bank function
// pointer's type (as in sound_init.cpp) are inferred. The bank id and handle
// are file-local in the original. The rest of the file is in other units.
void DebugMsg(const char*, ...);
void UpdateREAL();
extern "C" {
int SNDAEMS_asyncloadmodulebankdone(void);
double atan2(double, double);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
extern int (*SNDAEMS_updateevent)(int, int*);
}
void err(char*, ...);
extern int (*SNDAEMS_removemodulebank)(int);

class CScreen {
public:
    unsigned long GetVSyncs();

    unsigned char unknown000[0x110];
};

extern CScreen g_screen;

static int g_AEMSOneShotBankID = -1;
static int g_AEMSOneShotBankHandle = -1;
extern int g_AEMSOneShotMemOffset;
extern int g_AEMSOneShotLoadState;

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
// File-local in the original; declared extern so this fragment can refer to them.
extern int g_hAEMSAmbientTrack;
extern bool g_ambientSourceOnPlayer;
extern CVector3 g_ambientSourcePos;
extern int gAEMSAmbientTrackEvent[5];

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

void ResetOneShotBank() {
    while (g_AEMSOneShotLoadState == 1) {
        DebugMsg("Waiting for one shot sound bank to finish loading...\n");
        int start = g_screen.GetVSyncs();
        while ((int)g_screen.GetVSyncs() - start < 2) {
        }
        UpdateREAL();
        if (g_AEMSOneShotLoadState == 1 && SNDAEMS_asyncloadmodulebankdone() > 0)
            g_AEMSOneShotLoadState = 2;
    }
    if (g_AEMSOneShotLoadState == 2) {
        SNDAEMS_removemodulebank(g_AEMSOneShotBankHandle);
        g_AEMSOneShotBankID = -1;
        g_AEMSOneShotBankHandle = -1;
        g_AEMSOneShotLoadState = 0;
        g_AEMSOneShotMemOffset = 0;
    }
}

void AEMS_Update() {
    if (g_AEMSOneShotLoadState == 1 && SNDAEMS_asyncloadmodulebankdone() > 0)
        g_AEMSOneShotLoadState = 2;
    if (g_hAEMSAmbientTrack == -1)
        return;
    unsigned short azimuth = 0;
    unsigned short distance = 0;
    if (!g_ambientSourceOnPlayer)
        CalcDistanceAndAzimuth(&distance, &azimuth, g_ambientSourcePos, g_scene.GetPlayer(0));
    gAEMSAmbientTrackEvent[0] = 12;
    gAEMSAmbientTrackEvent[1] = azimuth;
    gAEMSAmbientTrackEvent[4] = distance;
    SNDSYS_entercritical();
    int result = SNDAEMS_updateevent(g_hAEMSAmbientTrack, gAEMSAmbientTrackEvent);
    SNDSYS_leavecritical();
    if (result < 0)
        err("AmbientTrack_Update(%d, %d):  SNDAEMS_updateevent() FAILED (error == %d)\n", azimuth, distance, result);
}
