// A fragment of sound.cpp (0x800e9ba8); the file name is this project's.
// PlayScriptedSound plays a sound described by a sound-table record: for a
// positioned sound (flag 0x10 of the record's third byte) the listener is
// the closest player in multiplayer (else the first player; the origin when
// there is none), the distance is the record's volume scale (value 3 / 256)
// times 256 per unit, at most 32767, and the azimuth comes from GetAzimuth's
// inline helper (sound_fail.cpp). A soldier node becomes the current sound
// object. The generic event memory is filled (event 7, or 14 for flag 0x80;
// the record's signed kind nibble, a random pitch between values 0 and 1,
// a random volume between the first two bytes, the azimuth and distance,
// value 2) and begun through the inferred inline of AEMS_BeginEvent; for a
// scheduled sound (flag 0x20) it is then added to the sound schedule for the
// node (or the given script object). Then the file's message and error
// reporting hooks, msg and err: variadic functions whose bodies are empty in
// this build (named by the symbols, local to the file). The record view,
// the event and schedule arguments' meanings and the helpers are inferred.
// _userdata_client after these is not part of this unit.
extern "C" {
double atan2(double, double);
extern int (*SNDAEMS_beginevent)(int*);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
}
static void msg(char*, ...);
static void err(char*, ...);
// File-local in the original; declared extern so this fragment can refer to it.
extern float g_sfxVolume;
long long MathFunRandomI64(long long, long long);

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

struct BSObject;
class CSoldierObject;

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
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual BSObject* GetScriptObject() const;
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual CSoldierObject* AsSoldierObject();
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

extern bool g_bInMultiplayerMode;
extern int g_pGenericSoundMemory[20];

extern CSoldierObject* g_objSound;

class CSoundTable {
public:
    /* inferred: a sound record's flags and values */
    struct SSoundParms {
        signed char m_volumeMin;
        signed char m_volumeMax;
        signed char m_type14 : 1;
        signed char m_flag40 : 1;
        signed char m_scheduled : 1;
        signed char m_positioned : 1;
        signed char m_kind : 4;
        short m_pitchMin;
        short m_pitchMax;
        short m_value2;
        short m_scale;
        short m_value4;
    };
};

enum EBSEventEnum {};

class SoundSchedule {
public:
    enum ESoundScheduleObjectType {};
    int AddSound(ESoundScheduleObjectType, ISceneNode*, CVector3*, void*, int, int, int, int, int, BSObject*,
                 EBSEventEnum, float, bool, int);
};

extern SoundSchedule g_SoundSchedule;

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

typedef CSoundTable::SSoundParms SoundParms;
typedef SoundSchedule::ESoundScheduleObjectType SoundObjectType;

int PlayScriptedSound(ISceneNode* node, CVector3 position, const SoundParms& parms, int event,
                      BSObject* script) {
    float scale = parms.m_scale;
    scale *= 0.00390625f;
    int azimuth = 0;
    int distance = 0;
    if (parms.m_positioned) {
        CPlayerObject* player;
        if (g_bInMultiplayerMode)
            player = g_scene.FindClosestPlayer(position);
        else
            player = g_scene.GetPlayer(0);
        CVector3 listener;
        if (player)
            player->GetPosition(listener);
        else
            listener.d.v[0] = listener.d.v[1] = listener.d.v[2] = 0.0f;
        float dx = position.d.v[0] - listener.d.v[0];
        float dy = position.d.v[1] - listener.d.v[1];
        float dz = position.d.v[2] - listener.d.v[2];
        distance = scale * (256.0f * sqrtf(dx * dx + dy * dy + dz * dz));
        if (distance > 32767)
            distance = 32767;
        CVector3 source = position;
        unsigned short az = 0;
        CalcDistanceAndAzimuth(0, &az, source, player);
        azimuth = az;
    }
    CSoldierObject* soldier = node ? node->AsSoldierObject() : 0;
    if (soldier)
        g_objSound = soldier;
    int type = 7;
    if (parms.m_type14)
        type = 14;
    int pitch = MathFunRandomI64(parms.m_pitchMin, parms.m_pitchMax);
    int* data = g_pGenericSoundMemory;
    data[0] = type;
    data[1] = parms.m_kind;
    data[2] = MathFunRandomI64(parms.m_volumeMin, parms.m_volumeMax);
    data[3] = parms.m_value4 == 0 ? pitch : 0;
    data[4] = azimuth;
    data[5] = distance;
    data[6] = 0;
    data[7] = 24999;
    data[8] = 0;
    data[9] = parms.m_value2;
    int handle = BeginEvent(data);
    if (parms.m_scheduled) {
        data[3] = pitch;
        BSObject* object;
        int kind;
        if (node) {
            object = node->GetScriptObject();
            kind = 0;
        } else {
            object = script;
            kind = 1;
        }
        g_SoundSchedule.AddSound((SoundObjectType)kind, node, &position, data, 40, handle, 4,
                                 5, -1, object, (EBSEventEnum)event, scale, data[1] == 1, parms.m_value4);
    }
    return handle;
}

static void msg(char*, ...) {
}

static void err(char*, ...) {
}
