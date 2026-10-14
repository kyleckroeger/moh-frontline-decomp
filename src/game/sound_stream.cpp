// A fragment of sound.cpp (0x800ecba8); the file name is this project's. The sound lock (its
// destructor and constructor leave and enter the sound system's critical section) and the CSoundStream wrappers over the EA stream API
// (pitch-multiplier pause, fades, volume, status, purge and file queueing),
// then the CSoundStream destructor and constructor (a stream created over a
// "SoundStream overhead" buffer of the stream overhead plus 100 KB, freed
// again when creation fails), SoundSetMode (under a sound lock, the sound system's output mode,
// 3/2/1 for modes 2/1/other, with option flags 0x24), SoundSync,
// SoundGetVolumes/SoundSetVolumes (in the critical section, the effect volume
// scaled to 32767 into the AEMS global through its hook, the music path volume as 100 * music volume
// squared), SoundSetMasterVolume (clamped to 0..127) and the shell and level
// sound-module unloading (every AEMS module bank and the AEMS system removed;
// for a level also the music paths; two inferred inlines).
// CSoundSysLock, CSoundStream, SSndStrmRequestStatus and SSndStreamStatus are
// named by the mangled symbols; the stream handle at the start of the object
// and the result types are inferred, and the status records are left
// incomplete. The rest of the file is not part of this unit.
struct SSndStrmRequestStatus;
struct SSndStreamStatus;
enum ESoundMode {};

/* Inferred: the stream play settings (filled by SNDplaysetdef) and the sound
   system options (output mode byte at +72, flags at +82). */
struct SoundPlayOptsView {
    unsigned char unknown00[64];
};

struct SoundSysOptsView {
    unsigned char unknown00[72];
    unsigned char mode;
    unsigned char unknown49[9];
    unsigned short flags;
    unsigned char unknown54[124];
};

extern "C" {
void SNDSYS_entercritical(void);
int SNDSTRM_pitchmult(int, unsigned int);
int SNDSTRM_autovol(int, int, int);
int SNDSTRM_getprogvol(int);
int SNDSTRM_requeststatus(int, SSndStrmRequestStatus*);
int SNDSTRM_status(int, SSndStreamStatus*);
int SNDSTRM_vol(int, int);
int SNDSTRM_purge(int);
int SNDSTRM_queuefile(int, int, const char*, int);
void SNDSYS_leavecritical(void);
void SNDSYS_getopts(SoundSysOptsView*);
void SNDSYS_setopts(SoundSysOptsView*);
void SNDplaysetdef(SoundPlayOptsView*);
int SNDSTRM_overhead(int, int);
int SNDSTRM_create(SoundPlayOptsView*, int, int, void*, int);
void SNDSTRM_destroy(int);
int SNDmastervol(int);
void MEM_free(void*);
extern int (*SNDAEMS_setglobal)(int, int);
extern int (*SNDAEMS_removemodulebank)(int);
extern int (*SNDAEMS_removesystem)(void);
void SNDAEMS_streamrestore(void);
void PATH_stop(int);
void* PATH_destroy(int);
int PATH_volume(unsigned int, int);
}

void* DWI_alloc(const char*, int, int);
void AEMS_Update();
void err(char*, ...);
extern float g_sfxVolume;
extern float g_musicVolume;
extern int gMusicHandle;
extern int gAEMSbHandle[8];
extern void* gAEMSBank[8];
extern int gAEMSBankCount;
extern void* gAEMSsystem;
extern bool gAEMSStreamUsed;
extern void* gMusicMap;
extern void* gMusicBuf;
void fail(char*, ...);
void TLT_CloseFile(void*);

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

CSoundSysLock::~CSoundSysLock() {
    SNDSYS_leavecritical();
}

CSoundSysLock::CSoundSysLock() {
    SNDSYS_entercritical();
}

void CSoundStream::Unpause() {
    SNDSTRM_pitchmult(m_stream, 4096);
}

void CSoundStream::Pause() {
    SNDSTRM_pitchmult(m_stream, 0);
}

void CSoundStream::FadeVolume(int time, int volume) {
    SNDSTRM_autovol(m_stream, time, volume);
}

int CSoundStream::GetVolume() const {
    return SNDSTRM_getprogvol(m_stream);
}

int CSoundStream::GetRequestStatus(int request, SSndStrmRequestStatus& status) const {
    return SNDSTRM_requeststatus(request, &status);
}

int CSoundStream::GetStatus(SSndStreamStatus& status) const {
    return SNDSTRM_status(m_stream, &status);
}

void CSoundStream::SetVolume(int volume) {
    SNDSTRM_vol(m_stream, volume);
}

void CSoundStream::PurgeQueue() {
    SNDSTRM_purge(m_stream);
}

int CSoundStream::QueueFile(const char* name, int offset, int flags) {
    return SNDSTRM_queuefile(m_stream, offset, name, flags);
}

CSoundStream::~CSoundStream() {
    if (m_stream >= 0) {
        SNDSTRM_destroy(m_stream);
        MEM_free(m_overhead);
        m_overhead = 0;
    }
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

void SoundSync() {
    AEMS_Update();
}

void SoundGetVolumes(float& sfx, float& music) {
    sfx = g_sfxVolume;
    music = g_musicVolume;
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

void SoundSetMasterVolume(int volume) {
    if (volume < 0)
        volume = 0;
    if (volume > 127)
        volume = 127;
    SNDmastervol(volume);
}

/* Inferred inline: every loaded AEMS module bank removed and freed, then the
   AEMS system removed, its file closed and any stream restored. */
inline void UnloadAEMSModules() {
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

/* Inferred inline: the music path stopped and every path destroyed (each
   freed), the music map file closed and the music state cleared. */
inline void UnloadMusic() {
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

void UnloadShellSoundModules() {
    UnloadAEMSModules();
}

void UnloadLevelSoundModules() {
    UnloadAEMSModules();
    UnloadMusic();
}
