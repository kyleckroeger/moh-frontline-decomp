// pathxSND.c: EA's path (track) player, the whole file in one unit, compiled as C++ ('-lang
// c++') with '-inline auto', exceptions on (every function has an exception record), lmw/stmw
// on and read-only strings (PATHX_createtrack's strings sit in .rodata and .sdata2; no other
// function has a string), in image order: PATHX_stop, pause, volume, modifyhold, queuefile;
// endservice, initservice, status, statusall, timeremaining; readyfornewrequest, getvolume,
// destroytrack; createtrack; overheadtrack, milliseconds; the sync task and the fade.
// pathService, pathTimer, pathabortmsg, pathprintf, pathxinfo and SNDPLAYOPTS are named by the
// symbols; the track record (one view: the parts' names for each word, anonymous unions where
// two parts name a word differently), the queue, node and status records, the play options'
// members and the parameter meanings are inferred.
struct SNDPLAYOPTS {
    signed char volume;
    unsigned char byte1;
    unsigned char byte2;
    unsigned char byte3;
    unsigned char byte4;
    unsigned char byte5;
    unsigned short short6;
    unsigned short short8;
    short shortA;
    unsigned short savedPitch;
    unsigned short shortE;
    unsigned short short10;
    unsigned short short12;
    unsigned short short14;
    unsigned short short16;
};

/* inferred: a queued request (its tag - the request's length for a sound bank's status -,
   handle and a value) */
struct PathxQueueView {
    int tag;
    int handle;
    int value08;
};

struct PathxNodeView {
    int unknown00;
    int event;
    int unknown08;
};

/* inferred: a track record: its stream or sound bank, patch, volumes, pitch, name, memory,
   play options, positions and states, and its queue, sound or path nodes */
struct PathxTrackView {
    int stream;
    union {
        int value04;
        int patch;
    };
    union {
        int requestedVolume;
        int value08;
    };
    unsigned short pitch;
    signed char isSound;
    char name[9];
    void* bankData;
    void* memory;
    int memorySize;
    SNDPLAYOPTS opts;
    int position3c;
    int position40;
    int played;
    int total;
    int ready;
    union {
        int state50;
        int value50;
    };
    int currentVolume;
    int pending;
    int error5c;
    union {
        int queueCount;
        int count;
    };
    union {
        PathxQueueView* queue;
        int* sound;
        PathxNodeView* nodes;
    };
};

struct PathxFormatView {
    unsigned short rate;
    unsigned short unknown02;
};

struct PathxRequestStatusView {
    int state;
    int value04;
    int value08;
    int value0c;
};

struct PathxDescView {
    unsigned int samples;
    int unknown04[4];
};

struct PathxAttrView {
    int unknown00[21];
};

extern PathxTrackView* pathxinfo[24];
extern "C" int SNDSTRM_modifyhold(int, int);
extern "C" void PATHX_statusall(int);
extern "C" void SNDSYS_entercritical(void);
extern "C" void SNDSYS_leavecritical(void);
extern "C" void SNDvol(int, int);
extern "C" void SNDSTRM_vol(int, int);
extern "C" int SNDBANK_play(int, int, signed char*);
extern "C" void SNDSTRM_purge(int);
extern "C" int SNDSTRM_queuemem(int, void*, void*, int);
extern "C" int SNDSTRM_queuefile(int, void*, int, int);
extern "C" void SNDSTRM_pitchmult(int, int);
extern "C" void SNDpitchmult(int, int);
extern "C" void SNDstop(int);
extern void (*pathService)(void);
extern void (*pathTimer)(void);
extern void (*pathabortmsg)(const char*, ...);
extern int (*pathprintf)(const char*, ...);
int PATHX_isynctask(int, int);
extern "C" void SNDSYS_add100hzclient(void (*)(void));
extern "C" void SNDSYS_remove100hzclient(void (*)(void));
extern "C" void SYNCTASK_add(int (*)(int, int), int, int);
extern "C" void SYNCTASK_del(int (*)(int, int));
extern "C" void REAL_abortmessage(const char*, ...);
extern "C" int printf(const char*, ...);
extern "C" int SNDSTRM_status(int, int*);
extern "C" void SNDSTRM_requeststatus(int, PathxRequestStatusView*);
extern "C" int SNDSTRM_getprogvol(int);
extern "C" int SNDBANK_patchinfo(int, int, PathxFormatView*, PathxAttrView*, PathxDescView*);
extern "C" int SNDtimeremaining(int);
extern "C" int SNDCTRL_getprogvol(int);
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
extern "C" int SNDSTRM_overhead(int, int);
extern "C" int TIMER_gettick(void);
extern "C" int TIMERhz;
extern "C" int SNDautovol(int handle, int time, int volume);
extern "C" int SNDSTRM_autovol(int stream, int time, int volume);

extern "C" int PATHX_stop(int track) {
    PathxTrackView* info = pathxinfo[track];

    SNDSYS_entercritical();
    if (info->isSound)
        SNDstop(info->queue->handle);
    else
        SNDSTRM_purge(info->stream);
    SNDSYS_leavecritical();
    info->pending = 1;
    return 0;
}

extern "C" int PATHX_pause(int track, char pause) {
    unsigned short pitch;
    PathxTrackView* info = pathxinfo[track];

    if (pause) {
        pitch = 0;
        info->pitch = info->opts.savedPitch;
    } else {
        pitch = info->pitch;
    }
    SNDSYS_entercritical();
    if (info->isSound)
        SNDpitchmult(info->queue->handle, pitch);
    else
        SNDSTRM_pitchmult(info->stream, pitch);
    SNDSYS_leavecritical();
    info->pending = 1;
    return 0;
}

extern "C" int PATHX_volume(int track, int volume) {
    int result = 0;
    PathxTrackView* info = pathxinfo[track];
    int requested;
    int current;

    if (info->pending)
        PATHX_statusall(0);
    requested = info->requestedVolume;
    current = info->currentVolume;
    if (requested < 0 || requested == current) {
        info->requestedVolume = -1;
        if (current != volume && volume >= 0 && current >= 0) {
            SNDSYS_entercritical();
            if (info->isSound)
                SNDvol(info->queue->handle, volume);
            else
                SNDSTRM_vol(info->stream, volume);
            SNDSYS_leavecritical();
            info->currentVolume = volume;
        }
    } else {
        result = -8;
    }
    return result;
}

extern "C" void PATHX_modifyhold(int track, int hold) {
    PathxTrackView* info = pathxinfo[track];

    if (!info->isSound)
        SNDSTRM_modifyhold(info->queue->handle, hold);
}

extern "C" int PATHX_queuefile(int track, void* file, int offset, int size, int tag, signed char volume, int start,
                               unsigned int purge) {
    int result;
    PathxTrackView* info = pathxinfo[track];
    int i;

    if (volume >= 0) {
        info->opts.volume = volume;
        PATHX_volume(track, volume);
    }
    if (info->isSound) {
        if (start < 0)
            start = 0;
        info->value04 = size + start - 1;
        SNDSYS_entercritical();
        result = SNDBANK_play(info->stream, info->value04, &info->opts.volume);
        SNDSYS_leavecritical();
        info->queue->handle = result;
        info->queue->tag = tag;
        info->pending = 1;
    } else {
        if (purge) {
            SNDSTRM_purge(info->stream);
            for (i = info->queueCount - 1; i > 0; i--) {
                info->queue[i - 1].handle = -1;
                info->queue[i - 1].tag = -1;
            }
        }
        if (info->memory && size * 4 < info->memorySize)
            result = SNDSTRM_queuemem(info->stream, file, info->memory, size * 4);
        else
            result = SNDSTRM_queuefile(info->stream, file, offset, size * 4);
        if (result >= 0) {
            for (i = info->queueCount - 1; i > 0; i--)
                info->queue[i] = info->queue[i - 1];
            info->queue->handle = result;
            info->queue->tag = tag;
        }
        info->pending = 1;
    }
    return result;
}

static inline void ClearTrack(PathxTrackView* info) {
    int i;
    for (i = 0; i < 24; i++) {
        if (pathxinfo[i] == info)
            pathxinfo[i] = 0;
    }
}

extern "C" void PATHX_endservice(void) {
    if (pathService) {
        SNDSYS_remove100hzclient(pathTimer);
        SYNCTASK_del(PATHX_isynctask);
        pathService = 0;
        pathTimer = 0;
    }
}

extern "C" void PATHX_initservice(void (*service)(void), void (*timer)(void)) {
    int i;

    for (i = 0; i < 24; i++)
        pathxinfo[i] = 0;
    pathService = service;
    pathTimer = timer;
    SNDSYS_add100hzclient(timer);
    SYNCTASK_add(PATHX_isynctask, 5, 100);
    pathabortmsg = REAL_abortmessage;
    pathprintf = printf;
}

extern "C" int PATHX_status(int track, int* position1, int* position2) {
    PathxTrackView* info = pathxinfo[track];

    if (!info)
        return -8;
    if (info->pending)
        PATHX_statusall(0);
    *position1 = info->position3c;
    *position2 = info->position40;
    if (info->error5c)
        return info->error5c;
    return info->state50;
}

extern "C" void PATHX_statusall(int flag) {
    int j;
    int i;
    int error;
    int position2;
    int position1;
    int remaining;
    PathxTrackView* info;
    PathxFormatView format;
    int status[3];
    PathxRequestStatusView request;
    PathxDescView desc;
    PathxAttrView attr;

    SNDSYS_entercritical();
    for (i = 0; i < 24; i++) {
        info = pathxinfo[i];
        if (!info)
            continue;
        if (flag) {
            info->error5c = 0;
            info->pending = 1;
            continue;
        }
        error = -8;
        info->ready = 1;
        position1 = 0;
        position2 = 0;
        if (info->stream >= 0) {
            if (info->isSound) {
                if (info->patch >= 0 && info->queue->handle >= 0) {
                    error = SNDBANK_patchinfo(info->stream, info->patch, &format, &attr, &desc);
                    if (error >= 0) {
                        remaining = SNDtimeremaining(info->queue->handle);
                        if (remaining < 0) {
                            position2 = 0;
                            info->queue->handle = -1;
                            goto done;
                        }
                        position2 = remaining * 10;
                        position1 = (int)(1000.0f * ((float)desc.samples / (float)format.rate)) - position2;
                    }
                    info->queue->value08 = position2;
                    info->played = info->queue->tag;
                    info->currentVolume = SNDCTRL_getprogvol(info->queue->handle);
                    info->state50 = info->queue->handle;
                }
            } else {
                error = SNDSTRM_status(info->stream, status);
                info->played = 0;
                if (status[0] >= info->queueCount)
                    info->ready = 0;
                for (j = 0; j < info->queueCount; j++) {
                    info->queue[j].value08 = 0;
                    if (info->queue[j].handle < 0)
                        continue;
                    SNDSTRM_requeststatus(info->queue[j].handle, &request);
                    switch (request.state) {
                    case 1:
                        info->queue[j].value08 = info->queue[j].tag;
                        info->total += info->queue[j].tag;
                        info->played += request.value0c;
                        info->ready = 0;
                        info->state50 = info->queue[j].handle;
                        break;
                    case 0:
                    case 2:
                        position1 = request.value04;
                        position2 = request.value08;
                        if (!position1 && !position2)
                            position1 = position2 = info->queue[j].tag;
                        info->queue[j].value08 = position2;
                        info->total += position2;
                        info->played += request.value0c;
                        if (request.value0c < request.value08)
                            info->ready = 0;
                        info->state50 = info->queue[j].handle;
                        break;
                    case 3:
                    default:
                        info->queue[j].handle = -1;
                        break;
                    }
                }
                info->currentVolume = SNDSTRM_getprogvol(info->stream);
            }
        }
    done:
        info->position3c = position1;
        info->position40 = position2;
        info->error5c = error;
        info->pending = 0;
    }
    SNDSYS_leavecritical();
}

extern "C" int PATHX_timeremaining(int track, int index) {
    PathxTrackView* info = pathxinfo[track];

    if (info && info->pending)
        PATHX_statusall(0);
    if (index < 0)
        return info->total;
    if (index < info->queueCount && info->queue[index].handle >= 0)
        return info->queue[index].value08;
    return -8;
}

extern "C" int PATHX_readyfornewrequest(int track) {
    if (pathxinfo[track] && pathxinfo[track]->pending)
        PATHX_statusall(0);
    return pathxinfo[track]->ready;
}

extern "C" int PATHX_getvolume(int track) {
    if (pathxinfo[track] && pathxinfo[track]->pending)
        PATHX_statusall(0);
    return pathxinfo[track]->currentVolume;
}

extern "C" void PATHX_destroytrack(int track) {
    PathxTrackView* info = pathxinfo[track];
    if (info->isSound) {
        SNDbankremove(info->stream);
        if (info->bankData)
            MEM_free(info->bankData);
    } else {
        SNDSTRM_destroy(info->stream);
    }
    ClearTrack(info);
}

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

// inferred: a track's memory is the record and its path nodes, then the stream's
static inline int TrackOverhead(int nodes) {
    return nodes * sizeof(PathxNodeView) + sizeof(PathxTrackView);
}

extern "C" int PATHX_overheadtrack(int nodes, int streams) {
    int size = TrackOverhead(nodes);

    if (streams > 0)
        size += SNDSTRM_overhead(nodes, streams);
    size += 16 - size % 16;
    return size;
}

extern "C" int PATHX_milliseconds(void) {
    return 1000.0f * ((float)TIMER_gettick() / (float)TIMERhz);
}

int PATHX_isynctask(int, int) {
    if (pathService)
        pathService();
    return 0;
}

int PATHX_ifade(int track, int volume, int time, char immediate) {
    int result = 0;
    PathxTrackView* info = pathxinfo[track];

    if (!immediate) {
        SNDSYS_entercritical();
        info->requestedVolume = volume;
        if (info->isSound)
            result = SNDautovol(info->sound[1], time / 10, volume);
        else
            result = SNDSTRM_autovol(info->stream, time, volume);
        SNDSYS_leavecritical();
    }
    return result;
}
