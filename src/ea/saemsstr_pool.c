/* A fragment of EA's AEMS streaming file saemsstr.c (0x80157790):
   SNDAEMS_streamrestore removes the stream task and destroys every stream
   and its status record; SNDAEMSI_streamgethandle gives a player a free
   stream, or takes the lowest priority one when the request outranks it;
   SNDAEMSI_streamdying detaches a stream's player; and
   SNDAEMSI_streamupdatestatus copies the stream's request status (and the
   queued request's) into the player's outputs, releasing the stream when its
   request has finished; SNDAEMSI_updateplayerstream starts a stream request
   for the player's sound on a rising play input (or releases the stream on
   stop) and applies the clamped pitch, volume, position, effect, filter,
   time and dry inputs; SNDAEMSI_streamsystemtask queues each pending request
   (and its loop section) on its stream outside the critical section, checking
   that the player still owns the stream afterwards; and
   SNDAEMSI_streamupdateattributes applies the inputs that changed.
   SNDAEMS_streaminit, first, links the stream player, replaces any earlier
   pool and divides the caller's memory between the streams (4-byte aligned
   chunks, each stream created on its chunk), returning a chunk's size less
   the stream overhead. AEMSSTREAMSTATUS and AEMSCOMPDYNAMICPLAYER are named by the mangled
   symbols; their members and the stream pool view are inferred. */
struct AEMSSTREAMREQUESTVIEW;

struct AEMSCOMPDYNAMICPLAYER {
    void* def;
    unsigned char unknown04[8];
    int* in;
    int out;
    int unknown14;
    int unknown18;
    int handle;
    int queued;
    int stream;
    AEMSSTREAMREQUESTVIEW* request;
    unsigned short sound;
    unsigned char playing;
    unsigned char unknown2f;
    unsigned short pitch;
    unsigned short time;
    unsigned short azimuth;
    short elevation;
    unsigned short volume;
    unsigned short fx;
    unsigned short lowpass;
    unsigned short highpass;
    unsigned short dry;
};

/* inferred: a stream definition in a module bank (id, priority, flags,
   start and loop offsets) and the bank fields the player reads */
struct AEMSSTREAMDEFVIEW {
    unsigned short id;
    unsigned char priority;
    unsigned char flags;
    int start;
    int loop;
};

struct AEMSSTREAMFILEVIEW {
    unsigned char unknown00[8];
    int file;
    int base;
};

struct AEMSPLAYERBANKVIEW {
    unsigned char unknown00[2];
    unsigned char version;
    unsigned char unknown03[45];
    AEMSSTREAMFILEVIEW* file;
    AEMSSTREAMDEFVIEW* streams;
};

struct AEMSPLAYERTABLEVIEW {
    int* sounds;
    short count;
};

struct AEMSPLAYERDEFVIEW {
    unsigned char unknown00[12];
    AEMSPLAYERTABLEVIEW* table;
    AEMSPLAYERBANKVIEW* bank;
};

/* inferred: the request a stream status record carries (player, stream id,
   file, start and loop offsets, state) */
struct AEMSSTREAMREQUESTVIEW {
    AEMSCOMPDYNAMICPLAYER* player;
    int id;
    int file;
    int start;
    int loop;
    unsigned char state;
};

/* inferred: a stream request's status as SNDSTRM_requeststatus reports it */
struct STREAMREQUESTVIEW {
    int state;
    int unknown04;
    int unknown08;
    int unknown0c;
};

struct AEMSSTREAMSTATUS {
    char* buffer;
    int handle;
    int priority;
    AEMSCOMPDYNAMICPLAYER* player;
    AEMSSTREAMREQUESTVIEW request;
};

/* inferred: the stream pool */
struct AEMSSTREAMPOOLVIEW {
    char* memory;
    int size;
    int count;
    int rate;
    AEMSSTREAMSTATUS** statuses;
};

struct SNDAEMSVIEW {
    unsigned char unknown00[16];
    AEMSSTREAMPOOLVIEW* streams;
};

/* inferred: the default play settings SNDplaysetdef fills (its size is
   bounded by SNDAEMS_streaminit's frame) */
struct PLAYOPTSVIEW {
    unsigned char unknown00[24];
};

extern SNDAEMSVIEW sndaems;

extern "C" {
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
void SNDSTRM_destroy(int);
void SNDSTRM_purge(int);
void SNDSTRM_requeststatus(int, STREAMREQUESTVIEW*);
void SNDSTRM_pitchmult(int, int);
void SNDSTRM_vol(int, int);
void SNDSTRM_3dpos(int, int, int);
void SNDSTRM_fxlevel(int, int, int);
void SNDSTRM_lowpass(int, int);
void SNDSTRM_highpass(int, int);
void SNDSTRM_timemult(int, int);
void SNDSTRM_drylevel(int, int);
int SNDSTRM_queuefile(int, int, int, int);
}
void SNDMEMI_free(void*);
void iSNDserverremoveclient(void (*)());
void SNDAEMSI_streamsystemtask();
void SNDAEMSI_streamdying(AEMSSTREAMSTATUS*);
int SNDAEMSI_streamgethandle(AEMSCOMPDYNAMICPLAYER*, int);
void SNDAEMSI_streamupdatestatus(AEMSCOMPDYNAMICPLAYER*);
void SNDAEMSI_streamupdateattributes(AEMSCOMPDYNAMICPLAYER*);
void SNDAEMSI_linkstream();
extern "C" int SNDAEMS_streamrestore();
void* SNDMEMI_allocz(int);
void iSNDserveraddclient(void (*)());
extern "C" {
void SNDplaysetdef(PLAYOPTSVIEW*);
int SNDSTRM_create(PLAYOPTSVIEW*, int, int, char*, int);
int SNDSTRM_overhead(int, int);
}

extern "C" int SNDAEMS_streaminit(char* memory, int size, int count) {
    PLAYOPTSVIEW opts;
    int i;
    AEMSSTREAMSTATUS* status;
    int chunk;

    SNDAEMSI_linkstream();
    if (sndaems.streams)
        SNDAEMS_streamrestore();
    SNDSYS_entercritical();
    sndaems.streams = (AEMSSTREAMPOOLVIEW*)SNDMEMI_allocz(sizeof(AEMSSTREAMPOOLVIEW));
    sndaems.streams->memory = memory;
    sndaems.streams->size = size;
    sndaems.streams->count = count;
    sndaems.streams->rate = 60;
    sndaems.streams->statuses = (AEMSSTREAMSTATUS**)SNDMEMI_allocz(count * sizeof(AEMSSTREAMSTATUS*));
    SNDplaysetdef(&opts);
    chunk = size / count;
    chunk &= 0x7ffffffc;
    for (i = 0; i < count; i++) {
        sndaems.streams->statuses[i] = (AEMSSTREAMSTATUS*)SNDMEMI_allocz(sizeof(AEMSSTREAMSTATUS));
        status = sndaems.streams->statuses[i];
        status->buffer = memory + i * chunk;
        status->handle = SNDSTRM_create(&opts, 2, 60, status->buffer, chunk);
        status->player = 0;
        status->request.state = 0;
        status->request.id = -1;
        status->request.start = -1;
        status->request.loop = -1;
        status->request.file = 0;
    }
    iSNDserveraddclient(SNDAEMSI_streamsystemtask);
    SNDSYS_leavecritical();
    chunk -= SNDSTRM_overhead(2, 60);
    return chunk;
}

extern "C" int SNDAEMS_streamrestore() {
    int i;

    if (!sndaems.streams)
        return 0;
    iSNDserverremoveclient(SNDAEMSI_streamsystemtask);
    for (i = 0; i < sndaems.streams->count; i++) {
        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[i];

        if (status->player)
            SNDAEMSI_streamdying(status);
        SNDSTRM_destroy(status->handle);
        SNDSYS_entercritical();
        SNDMEMI_free(status);
        SNDSYS_leavecritical();
    }
    SNDSYS_entercritical();
    SNDMEMI_free(sndaems.streams->statuses);
    SNDMEMI_free(sndaems.streams);
    SNDSYS_leavecritical();
    sndaems.streams = 0;
    return 0;
}

int SNDAEMSI_streamgethandle(AEMSCOMPDYNAMICPLAYER* player, int priority) {
    int lowest = 100;
    int index = -1;
    int i;

    if (!sndaems.streams)
        return -14;
    for (i = 0; i < sndaems.streams->count; i++) {
        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[i];

        if (!status->player && !status->request.state) {
            status->player = player;
            status->priority = priority;
            return status->handle;
        }
    }
    for (i = 0; i < sndaems.streams->count; i++) {
        if (sndaems.streams->statuses[i]->priority < lowest) {
            lowest = sndaems.streams->statuses[i]->priority;
            index = i;
        }
    }
    if (priority > lowest && index >= 0) {
        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[index];

        if (status->player)
            SNDAEMSI_streamdying(status);
        SNDSTRM_purge(status->handle);
        status->player = player;
        status->priority = priority;
        return status->handle;
    }
    return -1;
}

void SNDAEMSI_streamdying(AEMSSTREAMSTATUS* status) {
    SNDSYS_entercritical();
    if (!status->player) {
        SNDSYS_leavecritical();
        return;
    }
    status->request.state = 0;
    status->player->out = 0;
    status->player->unknown14 = 0;
    status->player->unknown18 = 0;
    status->player->handle = -1;
    status->player->stream = -1;
    status->player->request = 0;
    SNDSYS_leavecritical();
}

void SNDAEMSI_streamupdatestatus(AEMSCOMPDYNAMICPLAYER* player) {
    STREAMREQUESTVIEW current;
    STREAMREQUESTVIEW queued = {0};
    AEMSSTREAMSTATUS* status = 0;
    int i;

    for (i = 0; i < sndaems.streams->count; i++) {
        status = sndaems.streams->statuses[i];
        if (status->handle == player->stream)
            break;
    }
    if (i >= sndaems.streams->count || !status->player)
        return;
    if (player->handle == 0x7FFFFFFF) {
        player->out = 0;
        player->unknown14 = 0;
        player->unknown18 = 0;
        player->handle = -1;
        player->stream = -1;
        status->player = 0;
        status->request.player = 0;
        status->request.state = 0;
        player->request = 0;
        return;
    }
    if (status->request.state == 2 || status->request.state == 1)
        return;
    SNDSTRM_requeststatus(player->handle, &current);
    if (player->queued >= 0)
        SNDSTRM_requeststatus(player->queued, &queued);
    if (current.state == 3) {
        player->out = 0;
        player->unknown14 = 0;
        player->unknown18 = 0;
        player->handle = -1;
        player->queued = -1;
        status->player = 0;
        status->request.state = 0;
        player->request = 0;
        SNDSTRM_purge(player->stream);
        player->stream = -1;
    } else if (current.state == 2) {
        player->out = 1;
        if (player->request && status->request.state == 3) {
            status->request.state = 2;
        } else {
            player->unknown14 = current.unknown08;
            player->unknown18 = current.unknown04;
        }
    } else if (current.state == 0 || current.state == 1) {
        player->out = 1;
        player->unknown14 = 0x7FFFFFFF;
        if (player->request && player->queued >= 0 && queued.state == 2)
            player->unknown18 = queued.unknown04;
        else
            player->unknown18 = 0;
    }
}

/* inferred: an inline clamp helper */
static inline int AEMS_CLAMP(int x, int lo, int hi) {
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}

void SNDAEMSI_updateplayerstream(AEMSCOMPDYNAMICPLAYER* player) {
    AEMSPLAYERDEFVIEW* def = (AEMSPLAYERDEFVIEW*)player->def;
    AEMSSTREAMDEFVIEW* stream;

    if (player->playing != player->in[7]) {
        if (player->in[7] == 1) {
            if (player->playing == 0) {
                int index = player->in[6];

                if (index >= def->table->count)
                    index = def->table->count - 1;
                player->sound = def->table->sounds[index];
                stream = &def->bank->streams[player->sound & 0x3FFF];
                player->stream = SNDAEMSI_streamgethandle(player, stream->priority);
                player->handle = -1;
                if (player->stream >= 0) {
                    AEMSSTREAMREQUESTVIEW* request = 0;
                    int i;

                    for (i = 0; i < sndaems.streams->count; i++) {
                        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[i];

                        if (status->handle == player->stream) {
                            request = &status->request;
                            break;
                        }
                    }
                    request->player = player;
                    request->id = stream->id;
                    request->file = def->bank->file->file;
                    request->start = def->bank->file->base + stream->start;
                    if (stream->flags & 1) {
                        request->state = 2;
                        request->loop = def->bank->file->base + stream->loop;
                        player->request = request;
                    } else {
                        request->state = 1;
                        request->loop = -1;
                        player->request = 0;
                    }
                    player->out = 1;
                    player->unknown14 = 0x7FFFFFFF;
                    player->unknown18 = 0;
                    player->pitch = AEMS_CLAMP(player->in[0], 0, 0xFFFF);
                    SNDSTRM_pitchmult(player->stream, player->pitch);
                    player->volume = AEMS_CLAMP(player->in[2], 0, 0xFFFF);
                    SNDSTRM_vol(player->stream, player->volume >> 8);
                    player->azimuth = player->in[3];
                    player->elevation = AEMS_CLAMP(player->in[4], -16384, 16383);
                    SNDSTRM_3dpos(player->stream, player->azimuth, player->elevation);
                    player->fx = AEMS_CLAMP(player->in[5], 0, 32767);
                    SNDSTRM_fxlevel(player->stream, 0, player->fx >> 8);
                    player->lowpass = AEMS_CLAMP(player->in[8], 0, 0xFFFF);
                    SNDSTRM_lowpass(player->stream, player->lowpass);
                    player->highpass = AEMS_CLAMP(player->in[9], 0, 0xFFFF);
                    SNDSTRM_highpass(player->stream, player->highpass);
                    player->time = AEMS_CLAMP(player->in[1], 0, 0xFFFF);
                    SNDSTRM_timemult(player->stream, player->time);
                    if (def->bank->version >= 9) {
                        player->dry = AEMS_CLAMP(player->in[10], 0, 32767);
                        SNDSTRM_drylevel(player->stream, (signed char)(player->dry >> 8));
                    }
                } else {
                    AEMSSTREAMREQUESTVIEW* request;

                    player->out = 0;
                    player->unknown14 = 0;
                    player->unknown18 = 0;
                    request = player->request;

                    if (request) {
                        request->state = 0;
                        request->player = 0;
                    }
                    player->request = 0;
                }
            }
        } else if (player->in[7] == 2) {
            if (player->playing != 1)
                player->in[7] = 0;
        } else if (player->in[7] == 0) {
            if (player->stream > -1) {
                SNDSTRM_purge(player->stream);
                if (player->handle < 0)
                    player->handle = 0x7FFFFFFF;
                SNDAEMSI_streamupdatestatus(player);
            }
        }
        player->playing = player->in[7];
    }
    if (player->playing == 1 && player->stream > -1)
        SNDAEMSI_streamupdateattributes(player);
}

void SNDAEMSI_streamsystemtask() {
    int i;

    if (!sndaems.streams)
        return;
    SNDSYS_entercritical();
    for (i = 0; i < sndaems.streams->count; i++) {
        AEMSSTREAMSTATUS* status = sndaems.streams->statuses[i];

        if (status->request.state != 0 && status->request.state != 3) {
            AEMSCOMPDYNAMICPLAYER* player = status->request.player;
            int handle;
            int queued;

            status->player = player;
            queued = player->handle;
            SNDSYS_leavecritical();
            handle = SNDSTRM_queuefile(player->stream, status->request.id, status->request.file, status->request.start);
            SNDSYS_entercritical();
            if (player != status->player)
                continue;
            if (status->request.loop > -1) {
                queued = handle;
                SNDSYS_leavecritical();
                handle = SNDSTRM_queuefile(player->stream, status->request.id, status->request.file, status->request.loop);
                SNDSYS_entercritical();
                if (player != status->player)
                    continue;
                status->request.start = status->request.loop;
                status->request.loop = -1;
            }
            player->handle = handle;
            player->queued = queued;
            if (status->request.state == 2)
                status->request.state = 3;
            else
                status->request.state = 0;
            player->out = 1;
            player->unknown14 = 0x7FFFFFFF;
            player->unknown18 = 0;
        }
    }
    SNDSYS_leavecritical();
}

void SNDAEMSI_streamupdateattributes(AEMSCOMPDYNAMICPLAYER* player) {
    AEMSPLAYERDEFVIEW* def = (AEMSPLAYERDEFVIEW*)player->def;

    SNDAEMSI_streamupdatestatus(player);
    if (player->stream == -1)
        return;
    if (player->in[0] != player->pitch) {
        player->pitch = AEMS_CLAMP(player->in[0], 0, 0xFFFF);
        SNDSTRM_pitchmult(player->stream, player->pitch);
    }
    if (player->in[2] != player->volume) {
        player->volume = AEMS_CLAMP(player->in[2], 0, 32767);
        SNDSTRM_vol(player->stream, player->volume >> 8);
    }
    if (player->in[3] != player->azimuth || player->in[4] != player->elevation) {
        player->azimuth = player->in[3];
        player->elevation = AEMS_CLAMP(player->in[4], -16384, 16384);
        SNDSTRM_3dpos(player->stream, player->azimuth, player->elevation);
    }
    if (player->in[5] != player->fx) {
        player->fx = AEMS_CLAMP(player->in[5], 0, 32767);
        SNDSTRM_fxlevel(player->stream, 0, player->fx >> 8);
    }
    if (player->in[8] != player->lowpass) {
        player->lowpass = AEMS_CLAMP(player->in[8], 0, 0xFFFF);
        SNDSTRM_lowpass(player->stream, player->lowpass);
    }
    if (player->in[9] != player->highpass) {
        player->highpass = AEMS_CLAMP(player->in[9], 0, 0xFFFF);
        SNDSTRM_highpass(player->stream, player->highpass);
    }
    if (player->in[1] != player->time) {
        player->time = AEMS_CLAMP(player->in[1], 0, 0xFFFF);
        SNDSTRM_timemult(player->stream, player->time);
    }
    if (def->bank->version >= 9 && player->in[10] != player->dry) {
        player->dry = AEMS_CLAMP(player->in[10], 0, 32767);
        SNDSTRM_drylevel(player->stream, (signed char)(player->dry >> 8));
    }
}
