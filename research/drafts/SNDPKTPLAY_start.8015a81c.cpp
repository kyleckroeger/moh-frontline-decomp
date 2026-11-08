// A fragment of the packet player (0x8015ab48): submitting a packet (copied
// into the next free slot with a sequence number, its channels' data counted
// as queued), the free packet slots and outstanding frames, stopping a player
// (stopping its voice, flushing the deferred callbacks and freeing the
// channel buffers) and destroying it, fetching a channel's next packet (frame
// count and first flag out, the primary channel also releasing the oldest
// delivered packet and moving its frames from queued to pending), returning
// played frames, and running the deferred frames-done and release callbacks.
// SNDPKTPLAY_start before it is not reconstructed. Player records and sndpps
// are inferred views; the sequence number, queued counts, packet count and
// released-packet index are volatile (the original rereads them, or loads
// them in source order).
/* inferred: a packet as queued in a player */
struct SNDPKTPLAYPACKET {
    int sequence;
    int frames : 31;
    int first : 1;
    void* data[4];
};

/* inferred: a packet as submitted (the stream layer's packet) */
struct SNDPKTSUBMIT {
    int unknown00;
    unsigned int frames : 31;
    unsigned int first : 1;
    int unknown08;
    void* data[4];
};

struct SNDPKTPLAYER {
    int handle;
    volatile int sequence;
    short read[4];
    volatile short queued[4];
    volatile short maxpackets;
    signed char primary;
    signed char active;
    volatile short released;
    volatile short write;
    unsigned int frames;
    unsigned int pending;
    void* memory;
    void (*release)(void*, void*);
    void (*framesdone)(int, int, void*);
    void* data;
    unsigned short rate;
    unsigned char channels;
    unsigned char type;
    void* buffers[4];
    SNDPKTPLAYPACKET packets[1];
};

/* inferred: a deferred callback (type 0: frames done, 1: packet released) */
struct SNDPKTCALLBACK {
    unsigned short type;
    unsigned short player;
    int data;
};

struct SNDPPSVIEW {
    int count;
    SNDPKTCALLBACK callbacks[96];
    SNDPKTPLAYER* players[1];
};

extern "C" {
extern SNDPPSVIEW sndpps;
extern char sndgs[];
void SNDstop(int);
}
void SNDPKTPLAYI_flushcallbackdata(void);
void SNDMEMI_free(void*);
void SNDPLATFORM_packetplaydestroy(int);

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
    signed char unknown06;
    unsigned char unknown07;
    unsigned short unknown08;
    unsigned short unknown0a;
    unsigned short unknown0c[4];
    void* buffer[4];
};

/* inferred members */
struct SNDPLAYOPTS {
    signed char unknown00;
    unsigned char unknown01;
    unsigned char unknown02[2];
    unsigned char unknown04;
    unsigned char unknown05;
    unsigned char unknown06[2];
    unsigned short unknown08;
    unsigned char unknown0a[2];
    unsigned short unknown0c;
    unsigned short unknown0e;
    unsigned char unknown10[2];
    unsigned short unknown12;
    unsigned short unknown14;
    unsigned short unknown16;
};

/* inferred: the header SNDI_validrendermode checks */
struct SNDIPATCHHEADER {
    unsigned char unknown00[3];
    unsigned char channels;
    unsigned char unknown04[20];
    unsigned short unknown18;
    unsigned char unknown1a[162];
};

/* inferred: a voice record */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short voices[4];
    unsigned char unknown0c[2];
    short unknown0e;
    int unknown10;
    int unknown14;
    unsigned short unknown18;
    unsigned char unknown1a[2];
    unsigned short rate;
    unsigned char type;
    unsigned char channels;
    short mode;
    unsigned char unknown22;
    unsigned char unknown23;
    unsigned char unknown24[8];
    int unknown2c;
    unsigned char unknown30[4];
    int unknown34;
    int unknown38;
    int unknown3c;
    int unknown40;
    unsigned char unknown44;
    unsigned char unknown45;
    short unknown46;
    unsigned short unknown48[4];
    unsigned char unknown50;
    unsigned char unknown51;
    unsigned char unknown52;
    unsigned char unknown53;
    unsigned char unknown54;
    unsigned char unknown55;
    unsigned char unknown56[2];
    unsigned char unknown58;
    unsigned char unknown59[7];
    int unknown60;
    int unknown64;
    int unknown68;
    int unknown6c;
    int unknown70;
    unsigned char unknown74[2];
    short unknown76;
    short unknown78;
    short unknown7a;
    unsigned short unknown7c;
    unsigned char unknown7e[2];
};

int SNDI_validrendermode(int*, SNDIPATCHHEADER*);
void SNDPLATFORM_getvoicerange(int, int*, int*);
int SNDVOICEI_alloc(int, int, int*, int, int);
void SNDVOICEI_free(int);
void iSNDcalcpitch(int);
void iSNDcalcvol(int);
void SNDI_calcfxlevel(int, int);
int SNDPLATFORM_packetplay(int, int, int, int, int, int, SNDSAMPLEFORMAT*, void**);

/* inferred: the channel whose voice number is highest */
static inline int lv_c(short* voices, int count) {
    int best = -1;
    int loudest = best;
    int i;

    for (i = 0; i < count; i++) {
        if (voices[i] > best) {
            best = voices[i];
            loudest = i;
        }
    }
    return loudest;
}

static inline int loudestvoice(SNDVOICEVIEW* voice, int count) {
    int best = -1;
    int loudest = best;
    int i;

    for (i = 0; i < count; i++) {
        if (voice->voices[i] > best) {
            best = voice->voices[i];
            loudest = i;
        }
    }
    return loudest;
}

extern "C" int SNDPKTPLAY_start(int index, SNDSAMPLEFORMAT* format, SNDSAMPLEATTR* attr, SNDPLAYOPTS* opts) {
    // DECLS
    SNDIPATCHHEADER header;
    int low;
    int high;
    int mode = 0;
    int handle;
    SNDPKTPLAYER* player;
    SNDVOICEVIEW* voice;
    int render;
    int slot;
    int channels = format->channels;
    int result;
    int i;
    // END

    mode = 0;
    player = sndpps.players[index];
    header.channels = format->channels;
    header.unknown18 = attr->unknown08;
    player->rate = format->rate;
    player->channels = format->channels;
    player->type = format->type;
    player->sequence = 0;
    player->frames = 0;
    player->pending = 0;
    player->write = 0;
    player->released = 0;
    player->active = 1;
    for (i = 0; i < player->channels; i++) {
        player->queued[i] = 0;
        player->read[i] = 0;
        player->buffers[i] = attr->buffer[i];
    }
    for (;;) {
        render = SNDI_validrendermode(&mode, &header);
        if (!render)
            break;
        SNDPLATFORM_getvoicerange(render, &low, &high);
        slot = SNDVOICEI_alloc(channels, 101, &handle, low, high);
        if (slot < 0) {
            player->handle = -9;
            continue;
        }
        player->handle = handle;
        player->primary = lv_c((*(SNDVOICEVIEW**)(sndgs + 468))[slot].voices, format->channels);
        voice = &(*(SNDVOICEVIEW**)(sndgs + 468))[slot];
        voice->unknown46 = 0;
        voice->unknown18 = opts->unknown08;
        voice->unknown0e = -1;
        voice->unknown78 = attr->unknown00;
        voice->unknown7c = opts->unknown0c;
        voice->unknown2c = 0;
        voice->unknown34 = opts->unknown00 << 16;
        voice->unknown38 = 0;
        voice->unknown40 = 0x7FFFFFFF;
        voice->unknown3c = 0x7F0000;
        voice->unknown44 = attr->unknown03;
        voice->unknown58 = opts->unknown01;
        voice->unknown50 = 1;
        voice->unknown51 = 0;
        voice->unknown52 = 0;
        voice->unknown54 = attr->unknown05;
        voice->unknown53 = opts->unknown04;
        voice->unknown55 = opts->unknown05;
        voice->unknown76 = attr->unknown06 * 100;
        voice->unknown22 = 0;
        voice->unknown23 = 0;
        voice->unknown60 = 0;
        voice->unknown64 = 0;
        voice->unknown68 = 0;
        voice->unknown6c = 0;
        voice->unknown70 = 0;
        for (i = 0; i < format->channels; i++)
            voice->unknown48[i] = attr->unknown0c[i];
        voice->unknown14 = -1;
        voice->unknown10 = 0;
        voice->rate = format->rate;
        voice->type = format->type;
        voice->channels = format->channels;
        voice->mode = render;
        voice->unknown7a = 0;
        iSNDcalcpitch(slot);
        iSNDcalcvol(slot);
        SNDI_calcfxlevel(0, slot);
        result = SNDPLATFORM_packetplay(index, slot, opts->unknown0e, opts->unknown12, opts->unknown14, opts->unknown16, format, attr->buffer);
        if (result >= 0)
            break;
        for (i = 0; i < channels; i++)
            SNDVOICEI_free((*(SNDVOICEVIEW**)(sndgs + 468))[slot].voices[i]);
        player->handle = result;
    }
    return player->handle;
}

extern "C" int SNDPKTPLAY_submit(int index, SNDPKTSUBMIT* submit) {
    SNDPKTPLAYER* player = sndpps.players[index];
    SNDPKTPLAYPACKET* packet;
    int i;
    int sequence;

    if (player->queued[0] >= player->maxpackets - 1)
        return -13;
    packet = &player->packets[player->write];
    packet->frames = submit->frames;
    packet->first = submit->first;
    packet->sequence = player->sequence;
    for (i = 0; i < player->channels; i++) {
        packet->data[i] = submit->data[i];
        player->queued[i]++;
    }
    player->frames += submit->frames;
    sequence = player->sequence;
    player->sequence++;
    player->write++;
    player->active = 1;
    if (player->write >= player->maxpackets)
        player->write = 0;
    return sequence;
}

extern "C" int SNDPKTPLAY_submitspace(int index) {
    SNDPKTPLAYER* player = sndpps.players[index];

    return player->maxpackets - player->queued[0] - 1;
}

extern "C" int SNDPKTPLAY_framesoutstanding(int index) {
    SNDPKTPLAYER* player = sndpps.players[index];

    return player->frames + player->pending;
}

extern "C" int SNDPKTPLAY_stop(int index) {
    SNDPKTPLAYER* player = sndpps.players[index];
    int i;

    SNDstop(player->handle);
    SNDPKTPLAYI_flushcallbackdata();
    player->handle = -1;
    for (i = 0; i < player->channels; i++) {
        if (player->buffers[i])
            SNDMEMI_free(player->buffers[i]);
    }
    return 0;
}

extern "C" int SNDPKTPLAY_destroy(int index) {
    SNDPLATFORM_packetplaydestroy(index);
    sndpps.players[index] = 0;
    return 0;
}

/* inferred: defers a callback until the next flush */
static inline void addcallback(int type, int index, int data) {
    SNDPKTCALLBACK* callback = &sndpps.callbacks[sndpps.count];

    callback->type = type;
    callback->player = index;
    callback->data = data;
    sndpps.count++;
}

/* inferred: releases the oldest packet not yet released */
static inline void releasepacket(SNDPKTPLAYER* player, int index) {
    SNDPKTPLAYPACKET* packet = &player->packets[player->released];

    if (player->release) {
        SNDPKTCALLBACK* callback = &sndpps.callbacks[sndpps.count];

        callback->type = 1;
        callback->player = index;
        callback->data = (int)packet->data[0];
        sndpps.count++;
    }
    player->released++;
    if (player->released >= player->maxpackets)
        player->released = 0;
}

void* SNDPKTPLAYI_get(int index, int channel, int* frames, int* first) {
    SNDPKTPLAYER* player = sndpps.players[index];
    SNDPKTPLAYPACKET* packet;

    if (player->queued[channel] == 0) {
        if (channel == player->primary && !player->active && player->read[channel] != player->released)
            releasepacket(player, index);
        return 0;
    }
    packet = &player->packets[player->read[channel]];
    *frames = packet->frames;
    *first = packet->first;
    if (channel == player->primary) {
        if (player->read[channel] != player->released)
            releasepacket(player, index);
        player->frames -= packet->frames;
        player->pending += packet->frames;
    }
    player->read[channel]++;
    if (player->read[channel] >= player->maxpackets)
        player->read[channel] = 0;
    player->queued[channel]--;
    if (player->queued[channel] == 0 && channel == player->primary)
        player->active = 0;
    if (packet->data[channel])
        return packet->data[channel];
    return (void*)-1;
}

void SNDPKTPLAYI_freeframes(int index, int channel, int frames) {
    SNDPKTPLAYER* player = sndpps.players[index];

    if (channel != player->primary)
        return;
    player->pending -= frames;
    if (player->framesdone)
        addcallback(0, index, frames);
}

void SNDPKTPLAYI_flushcallbackdata(void) {
    int i;

    for (i = 0; i < sndpps.count; i++) {
        SNDPKTPLAYER* player;
        SNDPKTCALLBACK* callback = &sndpps.callbacks[i];

        player = sndpps.players[callback->player];

        if (callback->type == 0)
            player->framesdone(callback->player, callback->data, player->data);
        else
            player->release((void*)callback->data, player->data);
    }
    sndpps.count = 0;
}
