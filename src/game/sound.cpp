#include <math.h>

struct SNDUSERDATACBINFO;
struct SSndStrmRequestStatus;
struct SSndStreamStatus;
enum ESoundMode {};

/* Inferred: the stream play settings (filled by SNDplaysetdef; the volume
   byte and a short at +8 are set for the music track) and the sound system
   options (output mode byte at +72, flags at +82). */
struct SoundPlayOptsView {
    signed char volume;
    unsigned char unknown01[7];
    short m_unknown08;
    unsigned char unknown0a[54];
};

struct SoundSysOptsView {
    unsigned char unknown00[32];
    int m_unknown20;
    unsigned char unknown24[4];
    short m_unknown28;
    unsigned char unknown2a[21];
    unsigned char m_unknown3f;
    unsigned char unknown40[8];
    unsigned char mode;
    unsigned char unknown49[4];
    unsigned char m_unknown4d;
    unsigned char unknown4e[2];
    unsigned short m_unknown50;
    unsigned short flags;
    unsigned char unknown54[124];
};

extern "C" {
unsigned long strlen(const char*);
int sprintf(char*, const char*, ...);
int printf(const char*, ...);
int rand(void);
double atan2(double, double);
int PATH_fade(unsigned int, int, int);
int PATH_event(unsigned int, short, int);
int PATH_latency(unsigned int, int);
int PATH_control(unsigned int, int);
void PATH_pause(unsigned int, signed char);
void PATH_stop(unsigned int);
int PATH_volume(unsigned int, int);
int PATH_jump(unsigned int, int, int);
void* PATH_destroy(int);
int PATH_addmapfile(void*);
int PATH_overheadtrack(int, int);
int PATH_createtrack(unsigned int, int, char*, int, SoundPlayOptsView*, int, int, void*, int);
extern int (*SNDAEMS_beginevent)(int*);
extern int (*SNDAEMS_endevent)(int, int*);
extern int (*SNDAEMS_setglobal)(int, int);
extern int (*SNDAEMS_updateevent)(int, int*);
extern int (*SNDAEMS_removemodulebank)(int);
extern int (*SNDAEMS_removesystem)(void);
extern int (*SNDAEMS_asyncloadmodulebank)(char*, int, char*, int, void*, int, void* (*)(int));
extern int (*SNDAEMS_addsystem)(void*);
extern int (*SNDAEMS_addmodulebank)(void*, char*, int, void (*)(void*, int, int));
int SNDAEMS_asyncloadmodulebankdone(void);
void SNDAEMS_streaminit(void*, int, int);
void SNDAEMS_streamrestore(void);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
void SNDSYS_getopts(SoundSysOptsView*);
void SNDSYS_setopts(SoundSysOptsView*);
void SNDSYS_vectortoreal(void);
int SNDSYSI_init(void*, int, int);
void SNDSYS_adduserdataclient(void (*)(SNDUSERDATACBINFO*));
void SNDSYS_removeuserdataclient(void (*)(SNDUSERDATACBINFO*));
void SNDSYS_restore(void);
int SNDSTRM_pitchmult(int, unsigned int);
int SNDSTRM_autovol(int, int, int);
int SNDSTRM_getprogvol(int);
int SNDSTRM_requeststatus(int, SSndStrmRequestStatus*);
int SNDSTRM_status(int, SSndStreamStatus*);
int SNDSTRM_vol(int, int);
int SNDSTRM_purge(int);
int SNDSTRM_queuefile(int, int, const char*, int);
void SNDplaysetdef(SoundPlayOptsView*);
int SNDSTRM_overhead(int, int);
int SNDSTRM_create(SoundPlayOptsView*, int, int, void*, int);
void SNDSTRM_destroy(int);
int SNDmastervol(int);
void MEM_free(void*);
}

void* DWI_alloc(const char*, int, int);
void* DWI_allocalign(const char*, int, int, int);
void DebugMsg(const char*, ...);
void UpdateREAL();
void* TLT_LoadFileFromLevelBigFile(const char*, int*);
void* TLT_LoadFileFromShellBigFile(const char*, int*);
void* TLT_LoadFileNormal(const char*, int*, int);
void TLT_CloseFile(void*);

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) {
        d.v[0] = ax;
        d.v[1] = ay;
        d.v[2] = az;
    }
    CVector3& operator-=(const CVector3& o) {
        d.v[0] -= o.d.v[0];
        d.v[1] -= o.d.v[1];
        d.v[2] -= o.d.v[2];
        return *this;
    }
    float LengthSquared() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    float LengthSq() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
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

struct BSObject;

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

class CSoundTable {
public:
    struct SSoundParms {
        unsigned char unknown00[16];
    };

    bool GetSoundEvent(int, SSoundParms*) const;
    void Init(int, int);

    unsigned char unknown00[4];
};

extern CSoundTable g_soundMapSingleton;

class CSoldierObject {
public:
    void SetFaceAnimationState(unsigned short, long);
};

extern CSoldierObject* g_objSound;

/* inferred: the shell's subtitle flag (+536) and multiplayer music track (+5176) */
class CShellMenu {
public:
    unsigned int Get_LanguageSubtitleIndex();

    unsigned char unknown0000[536];
    bool m_subtitles : 1;
    unsigned char unknown0219[4639];
    int m_musicTrack;
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

class CScreen {
public:
    unsigned long GetVSyncs();

    unsigned char unknown000[0x110];
};

extern CScreen g_screen;

/* inferred: the 'SMUL' chunk header (40 bytes) followed by its strings */
struct SOUND_MULTISUBTITLEDATA {
    unsigned int id;
    unsigned int size;
    unsigned int count;
    unsigned int optional;
    unsigned int color;
    unsigned int offsets[5];
};

struct SNDUSERDATACBINFO {
    int type;
    char* data;
    char* base;
    long cookie;
};

/* inferred: the level resource's sound record (system, banks, music names) */
struct SoundResourceView {
    unsigned char unknown00[4];
    char* system;
    unsigned char unknown08[4];
    char* bank0c;
    unsigned char unknown10[4];
    char* bank14;
    unsigned char unknown18[4];
    char* bank1c;
    unsigned char unknown20[4];
    char* bank24;
    unsigned char unknown28[4];
    char* bank2c;
    unsigned char unknown30[4];
    char* bank34;
    unsigned char unknown38[4];
    char* bankName3c;
    unsigned char unknown40[4];
    char* musicMap;
    unsigned char unknown48[4];
    char* music;
    unsigned char unknown50[4];
    char* bank54;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    SoundResourceView* m_data;
};

enum TLTResourceID {};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);

class CSoundSysLock {
public:
    ~CSoundSysLock();
    CSoundSysLock();
};

class CSoundStream {
public:
    void Unpause();
    void Pause();
    void FadeVolume(int, int);
    int GetVolume() const;
    int GetRequestStatus(int, SSndStrmRequestStatus&) const;
    int GetStatus(SSndStreamStatus&) const;
    void SetVolume(int);
    void PurgeQueue();
    int QueueFile(const char*, int, int);
    ~CSoundStream();
    CSoundStream(int, int);

    int m_stream;
    void* m_overhead;
    SoundPlayOptsView m_play;
};

void EndianSwap(SOUND_MULTISUBTITLEDATA&);
void DisplaySubtitle(const char*, float, CColor);
void PlayScriptedSound(ISceneNode*, CVector3, const CSoundTable::SSoundParms&, int, BSObject*);
void AEMS_Update();
void* AEMSOneShotMalloc(int);
static void aems_Init(char*, char*, char*, char*, bool);

static void msg(char*, ...);
static void err(char*, ...);
static void fail(char*, ...);
static void _userdata_client(SNDUSERDATACBINFO*);
static void aems_BankAlloc(void*, int, int);
static int BeginEvent(int*);
static void CalcDistanceAndAzimuth(unsigned short*, unsigned short*, CVector3, const CPlayerObject*);
static void LoadMusicTrack(char*, char*);
static void UnloadAEMSModules();
static void UnloadMusic();

extern float g_sfxVolume;
extern float g_musicVolume;
extern int gMusicHandle;
extern int gMusicProjectID;
extern int gMusicBufSize;
extern void* gMusicMap;
extern void* gMusicBuf;
extern SoundPlayOptsView gMusicOpts;
extern int g_hAEMSAmbientTrack;
extern bool g_ambientSourceOnPlayer;
extern int gAEMSAmbientTrackEvent[5];
extern int gAEMSbHandle[8];
extern void* gAEMSBank[8];
extern int gAEMSBankCount;
extern void* gAEMSsystem;
extern bool gAEMSStreamUsed;
extern char gAEMSStreamBuf[0x32000];
extern char g_AEMSOneShotMem[0x70800];
extern int g_AEMSOneShotMemOffset;
extern int g_AEMSOneShotLoadState;
extern char g_AEMSOneShotStreamBuf[0xc800];
extern bool SoundHasBeenInitialized;
extern unsigned char SoundBankHeapStart[];
extern int SoundBankHeapSize;

static int g_AEMSOneShotBankID = -1;
static int g_AEMSOneShotBankHandle = -1;
static CColor _subtitle_rgb[3] = {CColor(255, 255, 255, 128), CColor(255, 255, 0, 128), CColor(0, 255, 0, 128)};
static CVector3 g_ambientSourcePos(0.0f, 0.0f, 0.0f);

void EndianSwap(SOUND_MULTISUBTITLEDATA& data) {
    ChangeEndian(data.id);
    ChangeEndian(data.size);
    ChangeEndian(data.count);
    ChangeEndian(data.optional);
    ChangeEndian(data.color);
    ChangeEndian(data.offsets[0]);
    ChangeEndian(data.offsets[1]);
    ChangeEndian(data.offsets[2]);
    ChangeEndian(data.offsets[3]);
    ChangeEndian(data.offsets[4]);
}

int SoundInit(ESoundMode mode) {
    SoundSysOptsView opts;
    int flags;
    if (SoundHasBeenInitialized)
        return -7;
    SNDSYS_vectortoreal();
    SNDSYS_getopts(&opts);
    opts.m_unknown28 = 8;
    opts.m_unknown20 = 512;
    opts.mode = 2;
    opts.m_unknown3f = 0;
    opts.m_unknown4d = 2;
    if (mode == 2) {
        opts.mode = 3;
        flags = 32;
    } else if (mode == 1) {
        opts.mode = 2;
        flags = 32;
    } else {
        opts.mode = 1;
        flags = 32;
    }
    opts.m_unknown50 = flags | 512;
    opts.flags = flags | 4;
    SNDSYS_setopts(&opts);
    if (SNDSYSI_init(SoundBankHeapStart, SoundBankHeapSize, 0x72b00))
        return -1;
    SNDSYS_adduserdataclient(_userdata_client);
    for (int i = 0; i < 8; i++)
        gAEMSbHandle[i] = -1;
    SoundHasBeenInitialized = true;
    return 0;
}

int SoundClose() {
    if (!SoundHasBeenInitialized)
        return -8;
    UnloadAEMSModules();
    UnloadMusic();
    SNDSYS_removeuserdataclient(_userdata_client);
    SNDSYS_restore();
    SoundHasBeenInitialized = false;
    return 0;
}

void LoadLevelSoundModules(int level, int stage) {
    LevelFileContentsStruct_* resource = TLT_FindNextResourceByType((TLTResourceID)12, 0);
    aems_Init("data\\", resource->m_data->system, 0, 0, false);
    char path[64];
    sprintf(path, "data\\%d\\%d_%d\\", level, level, stage);
    aems_Init(path, 0, resource->m_data->bank0c, 0, false);
    aems_Init(path, 0, resource->m_data->bank14, 0, false);
    aems_Init(path, 0, resource->m_data->bank1c, 0, false);
    aems_Init(path, 0, resource->m_data->bank24, 0, false);
    aems_Init(path, 0, resource->m_data->bank2c, 0, false);
    aems_Init(path, 0, resource->m_data->bank34, resource->m_data->bankName3c, false);
    aems_Init(path, 0, resource->m_data->bank54, 0, false);
    SNDSYS_entercritical();
    int result = SNDAEMS_setglobal(0, (int)(32767.0f * g_sfxVolume));
    SNDSYS_leavecritical();
    if (result < 0)
        err("SNDAEMS_setglobal failed %d", result);
    if (level != 6 || stage != 1) {
        if (g_bInMultiplayerMode) {
            int track = g_Shell.m_musicTrack;
            printf("Music track = %d \n", track);
            if (track == 0)
                track = rand() % 8 + 1;
            char map[255] = "";
            char music[255] = "";
            if (track == 1) {
                sprintf(map, "data\\1\\1_1\\1_1M.mpf");
                sprintf(music, "data\\1\\1_1\\main.mus");
            } else if (track == 4) {
                sprintf(map, "data\\3\\3_3\\3_3M.mpf");
                sprintf(music, "data\\3\\3_3\\main.mus");
            } else {
                sprintf(map, "data\\7\\7_%d\\7_%dM.mpf", track, track);
                sprintf(music, "data\\7\\7_%d\\main.mus", track);
            }
            DebugMsg("MapFile: %s \n", map);
            DebugMsg("MusicFile: %s \n", music);
            LoadMusicTrack(map, music);
        } else {
            char* map = resource->m_data->musicMap;
            char* music = resource->m_data->music;
            LoadMusicTrack(map, music);
        }
    }
    g_soundMapSingleton.Init(level, stage);
    g_AEMSOneShotLoadState = 0;
    g_AEMSOneShotMemOffset = 0;
}

void LoadShellSoundModules() {
    aems_Init("data\\sound\\", "EventSystem.aem", 0, 0, true);
    aems_Init("data\\shell\\", 0, "ShellBnk.abk", 0, true);
    SNDSYS_entercritical();
    int result = SNDAEMS_setglobal(0, (int)(32767.0f * g_sfxVolume));
    SNDSYS_leavecritical();
    if (result < 0)
        err("SNDAEMS_setglobal failed %d", result);
}

void UnloadLevelSoundModules() {
    UnloadAEMSModules();
    UnloadMusic();
}

void UnloadShellSoundModules() {
    UnloadAEMSModules();
}

void SoundSetMasterVolume(int volume) {
    if (volume < 0)
        volume = 0;
    if (volume > 127)
        volume = 127;
    SNDmastervol(volume);
}

void SoundSetVolumes(float sfx, float music) {
    g_sfxVolume = sfx;
    g_musicVolume = music;
    SNDSYS_entercritical();
    int result = SNDAEMS_setglobal(0, (int)(32767.0f * g_sfxVolume));
    SNDSYS_leavecritical();
    if (result < 0)
        err("SNDAEMS_setglobal failed %d", result);
    PATH_volume(gMusicHandle, (signed char)(100.0f * (music * g_musicVolume)));
}

void SoundGetVolumes(float& sfx, float& music) {
    sfx = g_sfxVolume;
    music = g_musicVolume;
}

void SoundSync() {
    AEMS_Update();
}

void SoundSetMode(ESoundMode mode) {
    CSoundSysLock lock;
    SoundSysOptsView opts;
    unsigned short flags;
    SNDSYS_getopts(&opts);
    if (mode == 2) {
        opts.mode = 3;
        flags = 32;
    } else if (mode == 1) {
        opts.mode = 2;
        flags = 32;
    } else {
        opts.mode = 1;
        flags = 32;
    }
    opts.flags = flags | 4;
    SNDSYS_setopts(&opts);
}

CSoundStream::CSoundStream(int channels, int buffers) {
    m_stream = -1;
    m_overhead = 0;
    SoundPlayOptsView* play = &m_play;
    SNDplaysetdef(play);
    int size = SNDSTRM_overhead(channels, buffers);
    m_overhead = DWI_alloc("SoundStream overhead", size + 0x19000, 1024);
    m_stream = SNDSTRM_create(play, channels, buffers, m_overhead, size + 0x19000);
    if (m_stream < 0) {
        MEM_free(m_overhead);
        m_overhead = 0;
    }
}

CSoundStream::~CSoundStream() {
    if (m_stream >= 0) {
        SNDSTRM_destroy(m_stream);
        MEM_free(m_overhead);
        m_overhead = 0;
    }
}

int CSoundStream::QueueFile(const char* name, int offset, int flags) {
    return SNDSTRM_queuefile(m_stream, offset, name, flags);
}

void CSoundStream::PurgeQueue() {
    SNDSTRM_purge(m_stream);
}

void CSoundStream::SetVolume(int volume) {
    SNDSTRM_vol(m_stream, volume);
}

int CSoundStream::GetStatus(SSndStreamStatus& status) const {
    return SNDSTRM_status(m_stream, &status);
}

int CSoundStream::GetRequestStatus(int request, SSndStrmRequestStatus& status) const {
    return SNDSTRM_requeststatus(request, &status);
}

int CSoundStream::GetVolume() const {
    return SNDSTRM_getprogvol(m_stream);
}

void CSoundStream::FadeVolume(int time, int volume) {
    SNDSTRM_autovol(m_stream, time, volume);
}

void CSoundStream::Pause() {
    SNDSTRM_pitchmult(m_stream, 0);
}

void CSoundStream::Unpause() {
    SNDSTRM_pitchmult(m_stream, 4096);
}

CSoundSysLock::CSoundSysLock() {
    SNDSYS_entercritical();
}

CSoundSysLock::~CSoundSysLock() {
    SNDSYS_leavecritical();
}

// The original AEMS_Update is not auto-inlined into SoundSync, although this body
// would be (CW inlines callees with fewer than 30 statements of their own). Its
// source most likely held debug statements compiled out of the release build;
// the empty statements below stand in for them (inferred; count = the minimum
// that keeps the call).
void AEMS_Update() {
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
    (void)0;
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

void LoadOneShotSound(int id, int level, int stage) {
    char name[64];
    if (g_AEMSOneShotBankID == id)
        return;
    sprintf(name, "data\\%d\\%d_%d\\%d%dload%d.abk", level, level, stage, level, stage, id);
    if (g_AEMSOneShotLoadState == 2) {
        SNDAEMS_removemodulebank(g_AEMSOneShotBankHandle);
        g_AEMSOneShotBankID = -1;
        g_AEMSOneShotBankHandle = -1;
        g_AEMSOneShotLoadState = 0;
        g_AEMSOneShotMemOffset = 0;
    }
    g_AEMSOneShotBankHandle =
        SNDAEMS_asyncloadmodulebank(name, 0, 0, 0, g_AEMSOneShotStreamBuf, 0xc800, AEMSOneShotMalloc);
    g_AEMSOneShotLoadState = 1;
    g_AEMSOneShotBankID = id;
    g_AEMSOneShotMemOffset = 0;
}

void* AEMSOneShotMalloc(int size) {
    void* block = &g_AEMSOneShotMem[g_AEMSOneShotMemOffset];
    g_AEMSOneShotMemOffset += size;
    return block;
}

static void aems_BankAlloc(void*, int size, int) {
    gAEMSBank[gAEMSBankCount] = DWI_allocalign("AEMS Bank", size, 16, 1024);
}

static void aems_Init(char* path, char* system, char* bank, char* name, bool shell) {
    char file[256];
    if (system) {
        if (gAEMSsystem)
            fail("Can't load any more event systems");
        sprintf(file, "%s%s", path, system);
        if (shell) {
            gAEMSsystem = TLT_LoadFileFromShellBigFile(file, 0);
            if (!gAEMSsystem)
                fail("Failure loading AEMS system '%s'", file);
        } else {
            gAEMSsystem = TLT_LoadFileFromLevelBigFile(file, 0);
            if (!gAEMSsystem)
                fail("Failure loading AEMS system '%s'", file);
        }
        int result = SNDAEMS_addsystem(gAEMSsystem);
        if (result < 0)
            fail("SNDAEMS_addsystem(%s) failure, ret = %ld", file, result);
        SNDAEMS_streaminit(gAEMSStreamBuf, 0x32000, 2);
        gAEMSStreamUsed = true;
    }
    if (bank) {
        if (gAEMSBankCount >= 8)
            fail("Can't load any more module banks");
        sprintf(file, "%s%s", path, bank);
        void* data = TLT_LoadFileNormal(file, 0, 0);
        if (!data && !(data = TLT_LoadFileNormal(bank, 0, 0)))
            fail("Failure loading AEMS bank '%s'", file);
        if (name && *name != 'd') {
            sprintf(file, "%s%s", path, name);
            name = file;
        }
        int handle = SNDAEMS_addmodulebank(data, name, 0, aems_BankAlloc);
        if (handle < 0)
            fail("SNDAEMS_addmodulebank(%s) failure, ret = %d", bank, handle);
        TLT_CloseFile(data);
        gAEMSbHandle[gAEMSBankCount++] = handle;
    }
}

/* Inferred helper: the music path stopped and every path destroyed (each
   freed), the music map file closed and the music state cleared. */
static void UnloadMusic() {
    if (gMusicHandle >= 0)
        PATH_stop(gMusicHandle);
    void* path;
    while ((path = PATH_destroy(-1)) != 0)
        MEM_free(path);
    if (gMusicMap)
        TLT_CloseFile(gMusicMap);
    gMusicMap = 0;
    gMusicBuf = 0;
    gMusicHandle = -1;
}

/* Inferred helper: every loaded AEMS module bank removed and freed, then the
   AEMS system removed, its file closed and any stream restored. */
static void UnloadAEMSModules() {
    for (int i = 0; i < 8; i++) {
        if (gAEMSbHandle[i] != -1) {
            int result = SNDAEMS_removemodulebank(gAEMSbHandle[i]);
            if (result < 0)
                fail("SNDAEMS_removemodulebank failure, ret = %d\n", result);
            if (gAEMSBank[i])
                MEM_free(gAEMSBank[i]);
            gAEMSBank[i] = 0;
            gAEMSbHandle[i] = -1;
            gAEMSBankCount--;
        }
    }
    int result = SNDAEMS_removesystem();
    if (result < 0)
        fail("SNDAEMS_removesystem failure, ret = %d\n", result);
    if (gAEMSsystem)
        TLT_CloseFile(gAEMSsystem);
    gAEMSsystem = 0;
    if (gAEMSStreamUsed)
        SNDAEMS_streamrestore();
    gAEMSStreamUsed = false;
}

static void fail(char*, ...) {
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

void AEMS_SetVolume(float volume) {
    g_sfxVolume = volume;
    SNDSYS_entercritical();
    int result = SNDAEMS_setglobal(0, (int)(32767.0f * g_sfxVolume));
    SNDSYS_leavecritical();
    if (result < 0)
        err("SNDAEMS_setglobal failed %d", result);
}

int AEMS_SendEvent(void* event) {
    return BeginEvent((int*)event);
}

int AEMS_BeginEvent(void* event) {
    return BeginEvent((int*)event);
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

void AEMS_SetObject(CSoldierObject* object) {
    if (object)
        g_objSound = object;
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

/* AmbientTrack_Stop: unreferenced, so the linker dropped it; only its message
   survives in .rodata. The body is not known beyond that message. */
static void AmbientTrack_Stop() {
    SNDSYS_entercritical();
    int result = SNDAEMS_endevent(g_hAEMSAmbientTrack, gAEMSAmbientTrackEvent);
    SNDSYS_leavecritical();
    if (result < 0)
        err("AmbientTrack_Stop():  SNDAEMS_endevent() FAILED!!! (error == %d)\n", result);
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

void AmbientTrack_SetLocation(const CVector3* position) {
    if (g_hAEMSAmbientTrack == -1)
        return;
    g_ambientSourceOnPlayer = position == 0;
    if (!g_ambientSourceOnPlayer)
        g_ambientSourcePos = *position;
}

unsigned short GetAzimuth(CVector3 position, const CPlayerObject* player) {
    unsigned short azimuth = 0;
    CalcDistanceAndAzimuth(0, &azimuth, position, player);
    return azimuth;
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

void PlayAnimTriggeredSound(long id, CAnimObject* object) {
    CVector3 position;
    object->GetPosition(position);
    CSoundTable::SSoundParms parms;
    if (g_soundMapSingleton.GetSoundEvent(id, &parms))
        PlayScriptedSound(object, position, parms, -1, 0);
}

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
        position.d.v[0] -= playerPosition.d.v[0];
        position.d.v[1] -= playerPosition.d.v[1];
        position.d.v[2] -= playerPosition.d.v[2];
        distance = (unsigned short)(3.0f * (256.0f * position.Length()));
    }
    int event[4];
    event[0] = 6;
    event[1] = id + 101;
    event[2] = (unsigned short)azimuth;
    event[3] = (unsigned short)distance;
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

/* Inferred helper: the music map loaded (from the level big file, or by name
   in multiplayer) and added to the path player, the music buffer allocated
   and the music track created on it, then stopped. */
static void LoadMusicTrack(char* map, char* music) {
    char file[256];
    sprintf(file, "%s", map);
    if (g_bInMultiplayerMode)
        gMusicMap = TLT_LoadFileNormal(map, 0, 0);
    else
        gMusicMap = TLT_LoadFileFromLevelBigFile(file, 0);
    gMusicProjectID = PATH_addmapfile(gMusicMap);
    if (gMusicProjectID < 0)
        err("PATH_addmapfile(%s) failed %d", file, gMusicProjectID);
    gMusicBufSize = PATH_overheadtrack(2, 100);
    gMusicBufSize += 0x20000;
    gMusicBuf = DWI_alloc("Music buffer", gMusicBufSize, 1024);
    if (!gMusicBuf)
        err("Can't alloc music streaming buffer of size %d", gMusicBufSize);
    SNDplaysetdef(&gMusicOpts);
    gMusicOpts.m_unknown08 = 0;
    gMusicOpts.volume = 127.0f * g_musicVolume;
    sprintf(file, "%s", music);
    gMusicHandle = PATH_createtrack(0x1000000 << gMusicProjectID | 0x10000000, 0, file, 750, &gMusicOpts, 2, 100,
                                    gMusicBuf, gMusicBufSize);
    if (gMusicHandle < 0)
        err("Failed creating music track (%d)", gMusicHandle);
    else
        msg("track created 0x%x\n", gMusicHandle);
    if (gMusicHandle >= 0)
        PATH_stop(gMusicHandle);
}

void MUSIC_Start(int level, int latency, int position) {
    if (gMusicHandle >= 0) {
        PATH_control(gMusicHandle, level < 0 ? 0 : (level > 127 ? 127 : level));
        PATH_latency(gMusicHandle, latency < 50 ? 50 : (latency > 5000 ? 5000 : latency));
        PATH_volume(gMusicHandle, (signed char)(100.0f * (g_musicVolume * g_musicVolume)));
        int result = PATH_jump(0xFFFFFFFF, position, 1);
        if (result < 0)
            err("PATH_jump Failed (%d)", result);
    }
}

void MUSIC_Stop() {
    if (gMusicHandle >= 0)
        PATH_stop(gMusicHandle);
}

bool MUSIC_Pause(bool pause) {
    if (gMusicHandle < 0)
        return false;
    PATH_pause(0xFFFFFFFF, pause != 0);
    return pause;
}

bool MUSIC_SetLevel(int level) {
    PATH_control(gMusicHandle, level < 0 ? 0 : (level > 127 ? 127 : level));
    return true;
}

bool MUSIC_SetLatency(int latency) {
    PATH_latency(gMusicHandle, latency < 50 ? 50 : (latency > 5000 ? 5000 : latency));
    return true;
}

bool MUSIC_SendEvent(int event) {
    PATH_event(gMusicHandle, (gMusicProjectID << 8) | event, 1);
    return true;
}

void MUSIC_Fade(float level, int time) {
    PATH_fade(gMusicHandle, 100.0f * (level * g_musicVolume), time);
}

static void _userdata_client(SNDUSERDATACBINFO* info) {
    char* data = info->data;
    if (data) switch (info->type) {
    case 1: {
        char* p = data;
        unsigned long id = *(unsigned long*)p;
        ChangeEndian(id);
        if (id == 'SPIL') {
            unsigned short state = *(unsigned short*)(p + 4);
            ChangeEndian(state);
            g_objSound->SetFaceAnimationState(state, info->cookie);
            return;
        }
        while ((unsigned long)(p - info->base) < (unsigned long)data) {
            unsigned long size;
            id = ((unsigned long*)p)[0];
            size = ((unsigned long*)p)[1];
            ChangeEndian(id);
            ChangeEndian(size);
            switch (id) {
            case 'NYSL': {
                unsigned long state = ((unsigned long*)p)[2];
                ChangeEndian(state);
                g_objSound->SetFaceAnimationState(state, info->cookie);
                break;
            }
            case 'TBUS': {
                unsigned long optional = ((unsigned long*)p)[2];
                unsigned long color = ((unsigned long*)p)[3];
                ChangeEndian(optional);
                ChangeEndian(color);
                if (strlen(p + 16) && (!optional || g_Shell.m_subtitles))
                    DisplaySubtitle(p + 16, 5.0f, _subtitle_rgb[color]);
                break;
            }
            case 'LUMS': {
                SOUND_MULTISUBTITLEDATA subtitles = *(SOUND_MULTISUBTITLEDATA*)p;
                EndianSwap(subtitles);
                const char* strings = p + sizeof(SOUND_MULTISUBTITLEDATA);
                SOUND_MULTISUBTITLEDATA* header = &subtitles;
                unsigned int language = g_Shell.Get_LanguageSubtitleIndex();
                if (language < header->count) {
                    if (strlen(strings + header->offsets[language]) && (!header->optional || g_Shell.m_subtitles))
                        DisplaySubtitle(strings + header->offsets[language], 5.0f, _subtitle_rgb[header->color]);
                }
                return;
            }
            default:
                return;
            }
            p += size;
        }
    }
    }
}

/* Inferred helper: AEMS_BeginEvent and AEMS_SendEvent are the same code. An
   effects event (type 7, kind 3, channel below 3) has its volume scaled by the
   effects volume first. */
static int BeginEvent(int* event) {
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

/* Inferred helper: the listener-relative distance (256 * length) and/or
   azimuth (65536ths of a turn) of a source; inlined at every use. */
static void CalcDistanceAndAzimuth(unsigned short* distance, unsigned short* azimuth, CVector3 source,
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

static void err(char*, ...) {
}

static void msg(char*, ...) {
}
