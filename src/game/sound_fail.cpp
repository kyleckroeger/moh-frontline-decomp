// A fragment of sound.cpp (0x800ebc90); the file name is this project's.
// GetAzimuth gives the direction of a position as seen by the player: the
// offset from the player's position turned into the player's frame (the
// inverse of its local-to-world matrix), flattened, and its angle as a 16-bit
// fraction of a turn (0 when the offset is under 0.01 long). The body is an
// inferred inline helper writing through a result pointer: its locals take the
// original's stack slots (an inline's locals are placed in declaration order
// from the bottom) and the result stays in a stack short; the same code is
// inlined into PlayImpactSound and AEMS_Update. The player's virtual slots
// follow the ISceneNode views of the other units. Then the
// ambient track: AmbientTrack_SetLocation places its source (or attaches it to
// the player when given no position), AmbientTrack_Volume and _Select update
// the track's AEMS event (event 12: volume scaled to 32767, track number) and
// AmbientTrack_Start begins it. AEMS_SetObject records the sound object;
// AEMS_EndEvent, _UpdateEvent, _BeginEvent and _SendEvent wrap the AEMS hooks
// in the sound system's critical section and report through the file's msg and
// err hooks (begin and send are the same code, an inferred inline that first
// scales an effects event's volume by the effects volume). AEMS_SetVolume
// records the effects volume and passes it to AEMS as global 0. AEMS_Pause
// starts the pause event (event 13) when pausing and ends it when resuming and
// returns whether it is playing. Then the file's failure reporting hook, fail:
// a variadic function whose body is empty in this build. The functions, the
// file's statics and the AEMS hooks are named by the symbols; the event views,
// the event block size, AEMS_Pause's return type, the AEMS hook prototypes,
// the position view (8-aligned, copied as double pairs) and the parameter
// names are inferred. The messages are .rodata pool entries and the constants
// .sdata2 pool entries. The functions around these are not part of this unit.
extern "C" {
double atan2(double, double);
extern int (*SNDAEMS_beginevent)(int*);
extern int (*SNDAEMS_endevent)(int, int*);
extern int (*SNDAEMS_setglobal)(int, int);
extern int (*SNDAEMS_updateevent)(int, int*);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
}

class CSoldierObject;

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

// File-local in the original; declared extern so this fragment can refer to them.
extern float g_sfxVolume;
extern int g_hAEMSAmbientTrack;
extern bool g_ambientSourceOnPlayer;
extern CVector3 g_ambientSourcePos;
extern int gAEMSAmbientTrackEvent[5];
extern CSoldierObject* g_objSound;
void msg(char*, ...);
void err(char*, ...);

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

/* Inferred inline helper (see the header). */
inline void CalcAzimuth(unsigned short* result, CVector3 source, const CPlayerObject* player) {
    CVector3 listener;
    player->GetPosition(listener);
    CVector3 delta;
    delta.d.v[0] = source.d.v[0] - listener.d.v[0];
    delta.d.v[1] = source.d.v[1] - listener.d.v[1];
    delta.d.v[2] = source.d.v[2] - listener.d.v[2];
    CMatrix toWorld;
    player->GetTMLocalToWorld(toWorld);
    CMatrix toLocal;
    toLocal.FastInverse(toWorld);
    toLocal.TransformVector(delta, delta);
    delta.d.v[2] = 0.0f;
    if (delta.d.v[0] * delta.d.v[0] + delta.d.v[1] * delta.d.v[1] + delta.d.v[2] * delta.d.v[2] > 0.0001f) {
        float angle = (float)atan2(delta.d.v[0], delta.d.v[1]);
        *result = 65536.0f * (0.15915494f * angle);
    } else {
        *result = 0;
    }
}

unsigned short GetAzimuth(CVector3 position, const CPlayerObject* player) {
    unsigned short azimuth = 0;
    unsigned short* result = &azimuth;
    CalcAzimuth(result, position, player);
    return *result;
}

void AmbientTrack_SetLocation(const CVector3* position) {
    if (g_hAEMSAmbientTrack == -1)
        return;
    g_ambientSourceOnPlayer = position == 0;
    if (!g_ambientSourceOnPlayer)
        g_ambientSourcePos = *position;
}

void AmbientTrack_Volume(float volume) {
    if (g_hAEMSAmbientTrack == -1)
        return;
    gAEMSAmbientTrackEvent[0] = 12;
    gAEMSAmbientTrackEvent[3] = (unsigned short)(32767.0f * volume);
    SNDSYS_entercritical();
    int result = SNDAEMS_updateevent(g_hAEMSAmbientTrack, gAEMSAmbientTrackEvent);
    SNDSYS_leavecritical();
    if (result < 0)
        err("AmbientTrack_Volume(%d):  SNDAEMS_updateevent() FAILED (error == %d)\n", volume, result);
}

void AmbientTrack_Select(int track) {
    if (g_hAEMSAmbientTrack == -1)
        return;
    gAEMSAmbientTrackEvent[0] = 12;
    gAEMSAmbientTrackEvent[2] = track;
    SNDSYS_entercritical();
    int result = SNDAEMS_updateevent(g_hAEMSAmbientTrack, gAEMSAmbientTrackEvent);
    SNDSYS_leavecritical();
    if (result < 0)
        err("AmbientTrack_Select(%d):  SNDAEMS_updateevent() FAILED (error == %d)\n", track, result);
}

void AmbientTrack_Start() {
    gAEMSAmbientTrackEvent[0] = 12;
    gAEMSAmbientTrackEvent[1] = 0;
    gAEMSAmbientTrackEvent[2] = 0;
    gAEMSAmbientTrackEvent[3] = 32767;
    gAEMSAmbientTrackEvent[4] = 0;
    SNDSYS_entercritical();
    g_hAEMSAmbientTrack = SNDAEMS_beginevent(gAEMSAmbientTrackEvent);
    SNDSYS_leavecritical();
    if (g_hAEMSAmbientTrack < 0) {
        err("AmbientTrack_Start():  SNDAEMS_beginevent() FAILED!!! (error == %d)\n", g_hAEMSAmbientTrack);
        g_hAEMSAmbientTrack = -1;
    }
}

void AEMS_SetObject(CSoldierObject* object) {
    if (object)
        g_objSound = object;
}

int AEMS_EndEvent(void* event, int handle) {
    if (handle == -1)
        return -1;
    SNDSYS_entercritical();
    int result = SNDAEMS_endevent(handle, (int*)event);
    SNDSYS_leavecritical();
    if (result < 0) {
        err("SNDAEMS_endeevent number %d failed %d", *(int*)event, handle);
        return -1;
    }
    msg("SNDAEMS_endeevent number %d handle=%d\n", *(int*)event, handle);
    return 0;
}

int AEMS_UpdateEvent(void* event, int handle) {
    if (handle == -1)
        return -1;
    SNDSYS_entercritical();
    int result = SNDAEMS_updateevent(handle, (int*)event);
    SNDSYS_leavecritical();
    if (result < 0 && result != -8 && result != -9)
        err("SNDAEMS_updateevent number %d failed %d", *(int*)event, handle);
    else
        msg("SNDAEMS_updateevent number %d handle=%d\n", *(int*)event, handle);
    return result;
}

int AEMS_BeginEvent(void* event) {
    return BeginEvent((int*)event);
}

int AEMS_SendEvent(void* event) {
    return BeginEvent((int*)event);
}

void AEMS_SetVolume(float volume) {
    g_sfxVolume = volume;
    SNDSYS_entercritical();
    int result = SNDAEMS_setglobal(0, (int)(32767.0f * g_sfxVolume));
    SNDSYS_leavecritical();
    if (result < 0)
        err("SNDAEMS_setglobal failed %d", result);
}

bool AEMS_Pause(bool pause) {
    static int handle = -1;
    int event[4];
    event[0] = 13;
    SNDSYS_entercritical();
    if (pause) {
        handle = SNDAEMS_beginevent(event);
    } else if (handle != -1) {
        handle = SNDAEMS_endevent(handle, event);
    }
    SNDSYS_leavecritical();
    return !(handle < 0);
}

static void fail(char*, ...) {
}
