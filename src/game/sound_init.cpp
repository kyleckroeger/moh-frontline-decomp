// A fragment of sound.cpp (0x800ed984); the file name is this project's.
// SoundClose (when initialised: every AEMS module bank, the AEMS system and
// the music paths unloaded as in UnloadLevelSoundModules, the user-data
// client removed and the sound system restored; -8 when not initialised) and
// SoundInit (when not yet initialised: the sound system options set - the
// output mode 3/2/1 for modes 2/1/other (2 by default) and fixed buffer and
// flag values -,
// the sound system started on the sound bank heap, the user-data client added
// and the eight AEMS bank handles cleared; -1 when the start fails, -7 when
// already initialised). The functions and globals are named by the symbols;
// the options layout, the unloading inlines and the result values' meanings
// are inferred. LoadLevelSoundModules before it is not part of this unit.
struct SSndStrmRequestStatus;
struct SSndStreamStatus;
enum ESoundMode {};

/* Inferred: the stream play settings (filled by SNDplaysetdef) and the sound
   system options (output mode byte at +72, flags at +82). */
struct SoundPlayOptsView {
    unsigned char unknown00[64];
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
void SNDSYS_vectortoreal(void);
int SNDSYSI_init(void*, int, int);
void SNDSYS_adduserdataclient(void (*)(struct SNDUSERDATACBINFO*));
void SNDSYS_removeuserdataclient(void (*)(struct SNDUSERDATACBINFO*));
void SNDSYS_restore(void);
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
void _userdata_client(SNDUSERDATACBINFO*);
extern bool SoundHasBeenInitialized;
extern unsigned char SoundBankHeapStart[];
extern int SoundBankHeapSize;


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
