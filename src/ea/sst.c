// Sound streams: stream lookup and teardown, the chunk release callback, the
// frames callback that advances the current request (finishing it, or after
// 200 passes, onto the stream's finished list) and carries surplus frames to
// the next one, and the header parser, which moves to the next request,
// converts the chunk's patch into the pending sample format and attributes,
// reports the attributes' events to the registered callbacks, releases the
// chunk, and then adopts the new format (stalling, state 2, when a playing
// stream's format changes) and starts the packet player; SNDSTRMI_parsedata
// builds a packet from a data chunk (channel pointers from the chunk's
// offsets, the chunk stashed before the first channel's data for the release
// callback) and submits it, and SNDSTRMI_isheld reports whether the current
// request is still held for prebuffering; SNDSTRMI_service, for each stream
// with queued requests, restarts a stalled player once it has drained,
// and unless the stream is held reads up to the player's free space (or 10)
// chunks, parsing data and header chunks. SNDSTREAMCHANNEL, SNDSAMPLEFORMAT,
// SNDSAMPLEATTR, SNDSAMPLEDESC, SNDLINKNODE, SNDLINKLIST, STREAMCHUNKHDR and
// TAGGEDPATCH are named by the mangled symbols; their members, the stream
// records and sndgs/sndss are inferred views. SNDSTRMI_create takes a free
// stream slot, lays the stream record, its request nodes, the packet
// player's memory and the stream buffer out in the caller's memory, creates
// the packet player and (unless an external stream handle is given) the
// stream, and registers the service client with the first stream; the
// packet pointer passing through the 'created' local is an inferred form
// that reproduces the original's register use. Then queuing a request on a
// stream (a file or a memory block given to the stream layer, or an external
// stream handle, taken from the stream's free request list with a new serial
// number in the upper bits of its handle), and the public SNDSTRM_create,
// SNDSTRM_destroy, SNDSTRM_queuefile and SNDSTRM_purge; stream lookup goes
// through an inline range check of the stream index. SNDPLAYOPTS is named by
// the mangled symbols. Where the two halves of the file read a member with
// different types or volatility (the packet player, the serial word, the
// external flag), the stream record has an anonymous union of both views.
extern "C" char sndgs[];
extern "C" void* sndss[];

struct TAGGEDPATCH;

/* inferred members */
struct SNDPLAYOPTS {
    unsigned char unknown00;
    unsigned char unknown01;
    unsigned char unknown02;
    unsigned char unknown03;
    unsigned char unknown04;
    unsigned char unknown05;
    unsigned short unknown06;
    unsigned short unknown08;
    short unknown0a;
    unsigned short unknown0c;
    unsigned short unknown0e;
    unsigned short unknown10;
    unsigned short unknown12;
    unsigned short unknown14;
    unsigned short unknown16;
};
struct STREAMCHUNKHDR;

/* inferred members */
struct SNDSAMPLEFORMAT {
    unsigned short rate;
    unsigned char channels;
    unsigned char type;
};

/* inferred members */
struct SNDSAMPLEATTR {
    short unknown00;
    unsigned char unknown02;
    unsigned char unknown03;
    unsigned char unknown04;
    unsigned char unknown05;
    unsigned char unknown06;
    unsigned char unknown07;
    unsigned short unknown08;
    unsigned short unknown0a;
    int unknown0c[2];
    void* buffer[4];
    int size[4];
    unsigned int event[4];
    int eventData[4];
};

/* inferred: filled in by the patch conversion, starting with the sample's
   frame count */
struct SNDSAMPLEDESC {
    unsigned int frames;
    unsigned char unknown04[16];
};

void SNDI_patchtohdr(void*, TAGGEDPATCH*, SNDSAMPLEFORMAT*, SNDSAMPLEATTR*, SNDSAMPLEDESC*);

extern "C" {
int SNDSTRM_destroy(int);
void STREAM_release(int, unsigned char*);
int memcmp(const void*, const void*, unsigned long);
int SNDPKTPLAY_start(int, SNDSAMPLEFORMAT*, SNDSAMPLEATTR*, SNDPLAYOPTS*);
void SNDCTRL_filteradd(int, void*);
void* STREAM_gettable(int);
int STREAM_state(int);
void SNDPKTPLAY_submit(int, void*);
int SNDPKTPLAY_framesoutstanding(int);
int SNDPKTPLAY_submitspace(int);
void SNDPKTPLAY_stop(int);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
unsigned char* STREAM_get(int);
void* memset(void*, int, unsigned long);
int SNDPKTPLAY_overhead(int);
int SNDPKTPLAY_create(void (*)(void*, void*), void (*)(int, int, void*), void*, void*, int);
void SNDPKTPLAY_destroy(int);
void* STREAM_create(int, int, int, void*, int);
int STREAM_buffersize(int);
void SNDSTRM_setgreedylevel(int, int);
int SNDSTRM_purge(int);
}

// SNDSTRMI_calcdatarate: a format's data rate in bytes per second, the
// sample rate times the channels times the codec's bits per 256 samples
// (16-bit PCM, type 16, is simply 8 bytes per channel). The locals are
// declared in the order channels, rate, bits but assigned rate, bits,
// channels (register order).
int SNDSTRMI_calcdatarate(SNDSAMPLEFORMAT* format) {
    int channels;
    int rate;
    int bits;

    rate = format->rate;
    bits = 0;
    channels = format->channels;
    rate *= channels;
    if (format->type == 10)
        bits = 136;
    else if (format->type == 4)
        bits = 51;
    else if (format->type == 7)
        bits = 512;
    else if (format->type == 16)
        return channels * 8;
    return rate * bits >> 8;
}

int SNDSTRMI_destroyall(void) {
    for (int i = 0; i < *(unsigned char*)(sndgs + 71); i++)
        SNDSTRM_destroy(i);
    return 0;
}

void* SNDSTRMI_getstreamptr(int handle) {
    if (handle >= *(unsigned char*)(sndgs + 71) || handle < 0)
        return 0;
    return sndss[handle];
}

void SNDSTRMI_releasecallback(void* data, void*) {
    unsigned char* chunk = *(unsigned char**)((char*)data - 4);

    STREAM_release(*(int*)sndss[(unsigned char)*(int*)chunk], chunk);
}

/* inferred: a queued stream request, linked by its first fields (the frame
   count left and the hold time are volatile: the original rereads them
   after comparing them) */
struct SNDLINKNODE {
    SNDLINKNODE* next;
    unsigned char unknown04[4];
    int request;
    int handle;
    unsigned int rate;
    unsigned int done;
    unsigned int total;
    volatile unsigned int left;
    volatile int hold;
    unsigned char started;
};

/* inferred: a list of request nodes */
struct SNDLINKLIST {
    SNDLINKNODE* head;
    SNDLINKNODE* tail;
    int count;
};

/* the stream record: named by the mangled symbols, members inferred */
struct SNDSTREAMCHANNEL {
    int handle;
    int voice;
    union {
        volatile int player;
        int packetPlayer;
    };
    union {
        int unknown0c;
        volatile int serial;
    };
    signed char state;
    union {
        unsigned char external;
        signed char externalStream;
    };
    unsigned char unknown012[2];
    SNDSAMPLEFORMAT format;
    SNDSAMPLEFORMAT newFormat;
    SNDSAMPLEATTR attr;
    SNDSAMPLEATTR newAttr;
    SNDPLAYOPTS opts;
    unsigned char filter[16];
    void* filterSet;
    unsigned char unknown0f0[4];
    SNDLINKLIST active;
    SNDLINKLIST finished;
    SNDLINKNODE* current;
};

SNDLINKNODE* SNDSTRMI_getrequestptr(int);
void SNDLINKI_remove(SNDLINKLIST*, SNDLINKNODE*);
void SNDLINKI_push(SNDLINKLIST*, SNDLINKNODE*);
void SNDLINKI_init(SNDLINKLIST*);
void iSNDserveraddclient(void (*)(void));

/* inferred: moves a request to the stream's finished list */
static inline void finishrequest(SNDLINKNODE* request) {
    SNDSTREAMCHANNEL* stream = (SNDSTREAMCHANNEL*)sndss[request->handle & 0xFF];
    SNDLINKNODE* node = SNDSTRMI_getrequestptr(request->handle);

    SNDLINKI_remove(&stream->active, node);
    SNDLINKI_push(&stream->finished, node);
    if (stream->current == node)
        stream->current = 0;
}

void SNDSTRMI_framescallback(int, int frames, void* data) {
    SNDSTREAMCHANNEL* stream = (SNDSTREAMCHANNEL*)data;
    int extra = 0;
    int count = 0;

    for (;;) {
        SNDLINKNODE* request = stream->active.head;

        if (++count > 200) {
            finishrequest(request);
            return;
        }
        if (frames > request->left) {
            extra = frames - request->left;
            frames -= extra;
        }
        request->done += frames;
        request->left -= frames;
        if (request->done >= request->total)
            finishrequest(request);
        if (!extra)
            return;
        frames = extra;
        extra = 0;
    }
}

/* inferred: the notification passed to the registered callbacks */
struct SNDSTREAMEVENT {
    int type;
    int event;
    int data;
    int unknown0c;
    int handle;
};


int SNDSTRMI_parseheader(int index, STREAMCHUNKHDR* chunk) {
    SNDSTREAMCHANNEL* stream = (SNDSTREAMCHANNEL*)sndss[index];
    SNDLINKNODE* request;
    SNDSAMPLEDESC desc;
    SNDSTREAMEVENT event;
    int i;
    int j;

    if (!stream->current)
        stream->current = stream->active.head;
    else
        stream->current = stream->current->next;
    request = stream->current;
    SNDI_patchtohdr(0, (TAGGEDPATCH*)((char*)chunk + 8), &stream->newFormat, &stream->newAttr, &desc);
    request->total = desc.frames;
    request->started = 0;
    i = 0;
    while (stream->newAttr.event[i]) {
        event.type = 3;
        event.event = stream->newAttr.event[i];
        event.data = stream->newAttr.eventData[i];
        event.handle = request->handle;
        stream->newAttr.event[i] = 0;
        stream->newAttr.eventData[i] = 0;
        i++;
        for (j = 0; j < *(signed char*)(sndgs + 366); j++)
            (*(void (**)(SNDSTREAMEVENT*))(sndgs + 424 + j * 4))(&event);
    }
    STREAM_release(stream->handle, (unsigned char*)chunk);
    request->rate = SNDSTRMI_calcdatarate(&stream->newFormat);
    if (memcmp(&stream->format, &stream->newFormat, sizeof(SNDSAMPLEFORMAT)) != 0
        || memcmp(&stream->attr, &stream->newAttr, sizeof(SNDSAMPLEATTR)) != 0 || stream->newAttr.buffer[0]) {
        if (stream->format.rate) {
            stream->state = 2;
            return 0;
        }
        stream->format = stream->newFormat;
        stream->attr = stream->newAttr;
        stream->newAttr.buffer[0] = 0;
    }
    if (stream->state != 1) {
        stream->voice = SNDPKTPLAY_start(stream->player, &stream->format, &stream->attr, &stream->opts);
        if (stream->filterSet)
            SNDCTRL_filteradd(stream->voice, stream->filter);
        stream->state = 1;
        stream->state = 1;
    }
    return 0;
}

/* inferred: a packet for the packet player: a frame count, a flag marking a
   request's first packet, and the channels' sample data */
struct SNDSTREAMPACKET {
    int unknown00;
    unsigned int frames : 31;
    unsigned int first : 1;
    int unknown08;
    unsigned char* data[4];
};

void SNDSTRMI_parsedata(SNDSTREAMCHANNEL* stream, STREAMCHUNKHDR* chunk) {
    SNDSTREAMPACKET packet;
    SNDLINKNODE* request;
    int i;
    int* offsets = (int*)((char*)chunk + 12);
    unsigned char* base;

    packet.frames = ((int*)chunk)[2];
    base = (unsigned char*)offsets + stream->format.channels * 4;
    for (i = 0; i < stream->format.channels; i++)
        packet.data[i] = base + offsets[i];
    request = stream->current;
    if (!packet.frames) {
        STREAM_release(stream->handle, (unsigned char*)chunk);
        return;
    }
    ((STREAMCHUNKHDR**)packet.data[0])[-1] = chunk;
    *(int*)chunk = request->handle;
    request->left += packet.frames;
    packet.first = request->started;
    SNDPKTPLAY_submit(stream->player, &packet);
    request->started = 1;
}

int SNDSTRMI_isheld(SNDSTREAMCHANNEL* stream) {
    SNDLINKNODE* request = stream->current;
    unsigned int buffered;

    if (!request)
        return 0;
    if (!request->rate)
        return 0;
    if (request->hold < 0)
        return 1;
    if (request->hold) {
        buffered = (unsigned int)STREAM_gettable(stream->handle);
        if (buffered > 4000000)
            buffered = 4000000;
        if (buffered * 1000 / request->rate >= request->hold) {
            request->hold = 0;
            return 0;
        }
        if (STREAM_state(stream->handle) == 2) {
            request->hold = 0;
            return 0;
        }
        if (stream->finished.count <= 0 && STREAM_state(stream->handle) == 0) {
            request->hold = 0;
            return 0;
        }
        return 1;
    }
    return 0;
}

/* inferred: hands a chunk to the data or header parser (or releases an
   unknown one); false when a header was parsed */
static inline int parsechunk(int index, STREAMCHUNKHDR* chunk) {
    SNDSTREAMCHANNEL* stream = (SNDSTREAMCHANNEL*)sndss[index];

    if (*(int*)chunk == 0x5343446C) {
        SNDSTRMI_parsedata(stream, chunk);
    } else if (*(int*)chunk == 0x5343486C) {
        SNDSTRMI_parseheader(index, chunk);
        return 0;
    } else {
        STREAM_release(stream->handle, (unsigned char*)chunk);
    }
    return 1;
}

void SNDSTRMI_service(void) {
    SNDSTREAMCHANNEL* stream;
    int i;
    int more;
    int count;
    STREAMCHUNKHDR* chunk;

    SNDSYS_entercritical();
    for (i = 0; i < *(unsigned char*)(sndgs + 71); i++) {
        stream = (SNDSTREAMCHANNEL*)sndss[i];
        if (!stream || !stream->active.count)
            continue;
        if (stream->state == 2) {
            if (SNDPKTPLAY_framesoutstanding(stream->player) > 0)
                continue;
            stream->format = stream->newFormat;
            stream->attr = stream->newAttr;
            stream->newAttr.buffer[0] = 0;
            SNDPKTPLAY_stop(stream->player);
            stream->voice = SNDPKTPLAY_start(stream->player, &stream->format, &stream->attr, &stream->opts);
            if (stream->filterSet)
                SNDCTRL_filteradd(stream->voice, stream->filter);
            stream->state = 1;
        }
        if (SNDSTRMI_isheld(stream))
            continue;
        if (stream->state == 1) {
            count = SNDPKTPLAY_submitspace(stream->player);
            if (!count)
                continue;
        } else {
            count = 10;
        }
        more = 0;
        do {
            count--;
            chunk = (STREAMCHUNKHDR*)STREAM_get(stream->handle);
            if (chunk)
                more = parsechunk(i, chunk);
        } while (more && count > 0);
    }
    SNDSYS_leavecritical();
}

int SNDSTRMI_create(SNDPLAYOPTS* opts, int requests, int players, void* memory, int size, int handle,
                    int external) {
    int count;
    int i;
    unsigned char* buffer;
    SNDSTREAMCHANNEL* stream;
    unsigned char* packets;
    int index;
    int overhead;
    void* created;

    for (i = 0; i < *(unsigned char*)(sndgs + 71); i++) {
        if (!sndss[i]) {
            index = i;
            goto found;
        }
    }
    return -9;
found:
    stream = (SNDSTREAMCHANNEL*)memory;
    size -= sizeof(SNDSTREAMCHANNEL);
    memory = (unsigned char*)memory + sizeof(SNDSTREAMCHANNEL);
    memset(stream, 0, sizeof(SNDSTREAMCHANNEL));
    created = (unsigned char*)memory + requests * sizeof(SNDLINKNODE);
    size -= requests * sizeof(SNDLINKNODE);
    overhead = SNDPKTPLAY_overhead(players);
    packets = (unsigned char*)created;
    buffer = (unsigned char*)created + overhead;
    size -= overhead;
    SNDLINKI_init(&stream->active);
    SNDLINKI_init(&stream->finished);
    for (i = 0; i < requests; i++) {
        SNDLINKI_push(&stream->finished, (SNDLINKNODE*)memory);
        memory = (unsigned char*)memory + sizeof(SNDLINKNODE);
    }
    stream->player = SNDPKTPLAY_create(SNDSTRMI_releasecallback, SNDSTRMI_framescallback, stream, packets,
                                       SNDPKTPLAY_overhead(players));
    if (stream->player < 0)
        return stream->player;
    if (external) {
        stream->handle = handle;
        stream->external = 1;
    } else {
        created = STREAM_create(requests + 2, 1, 1, buffer, size);
        if (!created) {
            SNDPKTPLAY_destroy(stream->player);
            return -9;
        }
        stream->handle = (int)created;
        stream->external = 0;
        SNDSTRM_setgreedylevel(stream->handle, STREAM_buffersize(stream->handle) / 3);
    }
    stream->unknown0c = count = 0;
    stream->voice = -1;
    stream->opts = *opts;
    for (i = 0; i < *(unsigned char*)(sndgs + 71); i++) {
        if (sndss[i])
            count++;
    }
    if (!count) {
        iSNDserveraddclient(SNDSTRMI_service);
        *(void (**)(void))(sndgs + 464) = (void (*)(void))SNDSTRMI_destroyall;
    }
    sndss[index] = stream;
    stream->newAttr.buffer[0] = 0;
    SNDSTRM_purge(index);
    return index;
}

// SNDSTRMI_queue and the stream functions after it are not reconstructed.

SNDLINKNODE* SNDLINKI_pop(SNDLINKLIST*);
void SNDLINKI_push(SNDLINKLIST*, SNDLINKNODE*);
void SNDLINKI_pushtail(SNDLINKLIST*, SNDLINKNODE*);
void SNDMEMI_free(void*);
void iSNDserverremoveclient(void (*)(void));
void SNDSTRMI_service(void);
int SNDSTRMI_create(SNDPLAYOPTS*, int, int, void*, int, int, int);

extern "C" {
void* memset(void*, int, unsigned long);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
int STREAM_queuefile(int, const char*, int, int);
int STREAM_queuemem(int, void*, int, int);
void STREAM_destroy(int);
void STREAM_kill(int);
void SNDPKTPLAY_stop(int);
void SNDPKTPLAY_destroy(int);
int SNDSTRM_purge(int);
}

/* inferred: the stream an index names, or null */
static inline SNDSTREAMCHANNEL* streamptr(int index) {
    if (index >= *(unsigned char*)(sndgs + 71) || index < 0)
        return 0;
    return (SNDSTREAMCHANNEL*)sndss[index];
}

/* inferred: the number of streams in use */
static inline int streamcount(void) {
    int count = 0;
    int i;

    for (i = 0; i < *(unsigned char*)(sndgs + 71); i++) {
        if (sndss[i])
            count++;
    }
    return count;
}

int SNDSTRMI_queue(int index, int user, char* data, int size, int type) {
    SNDSTREAMCHANNEL* stream = streamptr(index);
    SNDLINKNODE* node;
    int handle;
    int result;

    if (!stream)
        return -8;
    if (!stream->finished.count)
        return -13;
    if (type == 0)
        handle = STREAM_queuefile(stream->handle, data, size, 0x6C454353);
    else if (type == 1)
        handle = STREAM_queuemem(stream->handle, data, 0, 0x6C454353);
    else
        handle = size;
    if (!handle)
        return -1;
    SNDSYS_entercritical();
    node = SNDLINKI_pop(&stream->finished);
    memset(node, 0, sizeof(SNDLINKNODE));
    SNDLINKI_pushtail(&stream->active, node);
    node->request = handle;
    stream->serial += 256;
    if (stream->serial < 0)
        stream->serial = 0;
    node->handle = stream->serial | index;
    node->hold = user;
    result = node->handle;
    SNDSYS_leavecritical();
    return result;
}

extern "C" int SNDSTRM_create(SNDPLAYOPTS* opts, int requests, int players, void* memory, int size) {
    return SNDSTRMI_create(opts, requests, players, memory, size, 0, 0);
}

extern "C" int SNDSTRM_destroy(int index) {
    SNDSTREAMCHANNEL* stream = streamptr(index);

    if (!stream)
        return -8;
    SNDSTRM_purge(index);
    if (streamcount() == 1) {
        iSNDserverremoveclient(SNDSTRMI_service);
        *(void**)(sndgs + 464) = 0;
    }
    SNDPKTPLAY_destroy(stream->packetPlayer);
    {
        int handle = stream->handle;
        int external = stream->externalStream;

        sndss[index] = 0;
        if (!external)
            STREAM_destroy(handle);
    }
    return 0;
}

extern "C" int SNDSTRM_queuefile(int index, int user, char* name, int size) {
    return SNDSTRMI_queue(index, user, name, size, 0);
}

extern "C" int SNDSTRM_purge(int index) {
    SNDSTREAMCHANNEL* stream;
    SNDLINKNODE* node;
    int i;

    SNDSYS_entercritical();
    stream = streamptr(index);
    if (!stream) {
        SNDSYS_leavecritical();
        return -8;
    }
    if (stream->voice >= 0)
        SNDPKTPLAY_stop(stream->packetPlayer);
    stream->voice = -1;
    if (!stream->externalStream)
        STREAM_kill(stream->handle);
    if (stream->newAttr.buffer[0]) {
        for (i = 0; i < stream->newFormat.channels; i++)
            SNDMEMI_free(stream->newAttr.buffer[i]);
    }
    do {
        node = SNDLINKI_pop(&stream->active);
        if (node)
            SNDLINKI_push(&stream->finished, node);
    } while (node);
    stream->current = 0;
    stream->state = 0;
    memset(&stream->format, 0, sizeof(SNDSAMPLEFORMAT));
    memset(&stream->newFormat, 0, sizeof(SNDSAMPLEFORMAT));
    memset(&stream->attr, 0, sizeof(SNDSAMPLEATTR));
    memset(&stream->newAttr, 0, sizeof(SNDSAMPLEATTR));
    SNDSYS_leavecritical();
    return 0;
}
