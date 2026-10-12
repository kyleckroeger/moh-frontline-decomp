// PATHX_readyfornewrequest and PATHX_getvolume (pathxSND.c, 0x800e93ac):
// refresh the track states when a request is pending, then report whether
// the track can take a new request, or its volume; PATHX_destroytrack
// destroys the track's stream, or removes its sound bank and frees the bank
// data, and clears every track slot that points at it (through an inline
// helper, inferred: the original's slot loop allocates like one). The track
// record view is inferred (see pathxSND.c).
struct PathxTrackView {
    int stream;
    unsigned char unknown04[10];
    signed char isSound;
    unsigned char unknown0f[9];
    void* bankData;
    unsigned char unknown1c[48];
    int ready;
    unsigned char unknown50[4];
    int currentVolume;
    int pending;
    unsigned char unknown5c[8];
    int* sound;
};

extern PathxTrackView* pathxinfo[];

extern "C" int PATHX_statusall(int);
extern "C" void SNDSTRM_destroy(int);
extern "C" void SNDbankremove(int);
extern "C" void MEM_free(void*);

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

static inline void ClearTrack(PathxTrackView* info) {
    int i;
    for (i = 0; i < 24; i++) {
        if (pathxinfo[i] == info)
            pathxinfo[i] = 0;
    }
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
