// A fragment of pathxSND.c (0x800e9574): PATHX_createtrack takes a free
// track slot, lays the track record, its path nodes (events cleared) and,
// 16-byte aligned, the stream buffer out in the caller's memory, copies the
// play settings (or the defaults) and the name, and creates the track's
// stream or, for a sound track, adds its sound bank (from the given data or
// loaded from the file through an inferred inline helper; a bank that only
// needs its header keeps a copied header). The track record view and the
// inline slot/initialisation helpers are inferred (see pathxSND.c); the
// memory and size parameters serve as the layout cursor and the remaining
// size. Built with -use_lmw_stmw on like the other pathx fragments and with
// read-only strings (they sit in .rodata/.sdata2).
/* inferred: the play settings (24 bytes) */
struct SNDPLAYOPTS {
    unsigned char byte0;
    unsigned char byte1;
    unsigned char byte2;
    unsigned char byte3;
    unsigned char byte4;
    unsigned char byte5;
    unsigned short short6;
    unsigned short short8;
    short shortA;
    unsigned short shortC;
    unsigned short shortE;
    unsigned short short10;
    unsigned short short12;
    unsigned short short14;
    unsigned short short16;
};

/* inferred: a track's path node (12 bytes) */
struct PathxNodeView {
    int unknown00;
    int event;
    int unknown08;
};

struct PathxTrackView {
    int stream;
    int value04;
    int value08;
    unsigned char unknown0c[2];
    signed char isSound;
    char name[9];
    void* bankData;
    unsigned char unknown1c[8];
    SNDPLAYOPTS opts;
    unsigned char unknown3c[16];
    int ready;
    int value50;
    int currentVolume;
    int pending;
    unsigned char unknown5c[4];
    int count;
    union {
        int* sound;
        PathxNodeView* nodes;
    };
};

extern PathxTrackView* pathxinfo[];

extern "C" int PATHX_statusall(int);
extern "C" void SNDSTRM_destroy(int);
extern "C" void SNDbankremove(int);
extern "C" void MEM_free(void*);
extern "C" void* MEM_allocz(const char*, int, int);
extern "C" int FILE_size(const char*);
extern "C" bool FILESYS_opensync(const char*, int, int, int*);
extern "C" int FILESYS_readsync(int, int, void*, int, int);
extern "C" int FILESYS_closesync(int, int);
extern "C" int SNDbankadd(int*, void*);
extern "C" int SNDbankheadersize(int);
extern "C" int SNDbankheadercopy(void*, int);
extern "C" int SNDplaysetdef(SNDPLAYOPTS*);
extern "C" int SNDSTRM_create(SNDPLAYOPTS*, int, int, void*, int);
extern "C" int sprintf(char*, const char*, ...);

static inline int FindFreeTrack() {
    int i;
    for (i = 0; i < 24; i++) {
        if (!pathxinfo[i])
            return i;
    }
    return -1;
}

static inline void InitTrack(PathxTrackView* info) {
    for (char* p = (char*)info; p < (char*)info + sizeof(PathxTrackView); p++)
        *p = 0;
    info->stream = -1;
    info->value04 = -1;
    info->count = -1;
    info->value50 = -1;
    info->value08 = -1;
    info->pending = 1;
}

static inline void* LoadBankFile(const char* filename) {
    int size = FILE_size(filename);
    void* buffer;
    int handle;
    if (!size) {
        buffer = 0;
    } else {
        buffer = MEM_allocz("PATHXbnk", size, 0);
        FILESYS_opensync(filename, 1, 0, &handle);
        FILESYS_readsync(handle, 0, buffer, size, 0);
        FILESYS_closesync(handle, 0);
    }
    return buffer;
}
extern "C" int PATHX_createtrack(const char* filename, const char* name, SNDPLAYOPTS* opts, int count, int streamSize, char* memory, int memorySize, void* bankData) {
    PathxTrackView* sound;
    int track;
    int i;
    PathxTrackView* info;
    int bank;
    int result;
    track = FindFreeTrack();
    if (track < 0)
        return -13;
    pathxinfo[track] = (PathxTrackView*)memory;
    memory += sizeof(PathxTrackView);
    InitTrack(pathxinfo[track]);
    info = pathxinfo[track];
    info->nodes = (PathxNodeView*)memory;
    memory += count * sizeof(PathxNodeView);
    memory += 16 - (memory - (char*)info) % 16;
    memorySize -= memory - (char*)info;
    for (i = 0; i < count; i++)
        info->nodes[i].event = -1;
    if (opts)
        info->opts = *opts;
    else
        SNDplaysetdef(&info->opts);
    sprintf(info->name, "%s", name);
    if (streamSize > 0) {
        info->stream = SNDSTRM_create(&info->opts, count, streamSize, memory, memorySize);
    } else {
        info->isSound = 1;
        bank = -1;
        sound = pathxinfo[track];
        if (bankData) {
            sound->bankData = bankData;
        } else {
            sound->bankData = LoadBankFile(filename);
        }
        if (!sound->bankData) {
            result = -39;
        } else {
            if (SNDbankadd(&bank, sound->bankData) == 7) {
                int hsize = SNDbankheadersize(bank);
                void* header = MEM_allocz("PATHXbkh", hsize, 0);
                if (header) {
                    SNDbankheadercopy(header, bank);
                    MEM_free(sound->bankData);
                    sound->bankData = header;
                }
            }
            result = bank;
        }
        info->stream = result;
    }
    if (info->stream < 0)
        return info->stream;
    info->count = count;
    return track;
}
