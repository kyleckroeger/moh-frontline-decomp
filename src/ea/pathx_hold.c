// A fragment of pathxSND.c (0x800e88a8): PATHX_pause (pauses a track by
// setting its pitch to 0, restoring the saved pitch first, or resumes it at
// its pitch; through the sound or the stream, then marks a refresh pending),
// PATHX_volume (applies a track's
// volume to its sound or stream unless a different volume change is still
// pending, -8), PATHX_modifyhold (passes a hold change to a streamed track's
// stream; sound-bank tracks are left alone) and PATHX_queuefile (sets the
// track volume, then either plays the sound-bank entry or queues the file
// data on the stream - from memory when it fits the track's buffer - after an
// optional purge, recording the request at the head of the track's queue).
// The track record and queue views (the pitch fields included) and the
// parameter meanings are inferred
// (see pathxSND.c). This fragment is built with -use_lmw_stmw on, as
// PATHX_queuefile's stmw shows.
/* inferred: a queued request (tag, then the stream or sound handle) */
struct PathxQueueView {
    int tag;
    int handle;
    int value08;
};

struct PathxTrackView {
    int stream;
    int value04;
    int requestedVolume;
    unsigned short pitch;
    signed char isSound;
    unsigned char unknown0f[13];
    void* memory;
    int memorySize;
    signed char volume;
    unsigned char unknown25[11];
    unsigned short savedPitch;
    unsigned char unknown32[26];
    int ready;
    unsigned char unknown50[4];
    int currentVolume;
    int pending;
    unsigned char unknown5c[4];
    int queueCount;
    PathxQueueView* queue;
};

extern PathxTrackView* pathxinfo[];

extern "C" {
int SNDSTRM_modifyhold(int, int);
void PATHX_statusall(int);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
void SNDvol(int, int);
void SNDSTRM_vol(int, int);
int SNDBANK_play(int, int, signed char*);
void SNDSTRM_purge(int);
int SNDSTRM_queuemem(int, void*, void*, int);
int SNDSTRM_queuefile(int, void*, int, int);
void SNDSTRM_pitchmult(int, int);
void SNDpitchmult(int, int);
}

extern "C" int PATHX_pause(int track, char pause) {
    unsigned short pitch;
    PathxTrackView* info = pathxinfo[track];

    if (pause) {
        pitch = 0;
        info->pitch = info->savedPitch;
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
        info->volume = volume;
        PATHX_volume(track, volume);
    }
    if (info->isSound) {
        if (start < 0)
            start = 0;
        info->value04 = size + start - 1;
        SNDSYS_entercritical();
        result = SNDBANK_play(info->stream, info->value04, &info->volume);
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
