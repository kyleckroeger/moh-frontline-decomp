/* EA's AEMS bank player file: SNDAEMSI_updateplayerbank starts the
   player's sound from its bank on a rising play input (with play settings
   from the clamped pitch, volume, position, effect, filter, time and, from
   bank version 9, dry inputs), stops it on a zero input, and while playing
   reports the time remaining and the current position in milliseconds
   (64-bit arithmetic for large frame counts) and applies the inputs that
   changed to the voice; SNDAEMS_linkbank installs it in sndaems. sndaems,
   sndgs, AEMSCOMPDYNAMICPLAYER, SINT64 and the functions are named by their
   symbols; the player, definition, play settings, voice and sndgs views
   (fields named by use) and the clamp helper are inferred, following
   saemsstr.c. */
struct AEMSCOMPDYNAMICPLAYER {
    void* def;
    unsigned char unknown04[8];
    int* in;
    int out;
    int unknown14;
    int unknown18;
    int handle;
    unsigned char unknown20[12];
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

struct SNDPLAYOPTS {
    unsigned char volume;
    unsigned char unknown01[3];
    unsigned char dry;
    unsigned char fx;
    short unknown06;
    short azimuth;
    short elevation;
    unsigned short pitch;
    unsigned short time;
    short unknown10;
    short unknown12;
    unsigned short lowpass;
    unsigned short highpass;
};

struct SINT64 {
    unsigned int hi;
    unsigned int lo;
};

struct AEMSPLAYEROUTPUTSVIEW {
    unsigned char unknown00[12];
    short timeUsed;
    unsigned char unknown0e[6];
    short frameUsed;
};

struct AEMSPLAYERTABLEVIEW {
    int* sounds;
    short count;
};

struct AEMSPLAYERBANKVIEW {
    unsigned char unknown00[2];
    unsigned char version;
    unsigned char unknown03[45];
    int* bank;
};

struct AEMSPLAYERDEFVIEW {
    unsigned char unknown00[8];
    AEMSPLAYEROUTPUTSVIEW* outputs;
    AEMSPLAYERTABLEVIEW* table;
    AEMSPLAYERBANKVIEW* bank;
};

struct SNDVOICEVIEW {
    unsigned char unknown00[24];
    short azimuth;
    short elevation;
    unsigned short rate;
    unsigned char unknown1e[22];
    int volume;
    unsigned char unknown38[13];
    unsigned char volumeLevel;
    unsigned char unknown46[15];
    unsigned char fxLevel;
    unsigned char unknown56[8];
    short time;
    unsigned char unknown60[30];
    short pitch;
};

struct SNDGSVIEW {
    unsigned char unknown000[361];
    signed char masterVolume;
    unsigned char unknown16a[106];
    SNDVOICEVIEW* voices;
};

extern "C" SNDGSVIEW sndgs;

extern "C" {
int SNDplaysetdef(SNDPLAYOPTS*);
int SNDBANK_play(int, int, SNDPLAYOPTS*);
void SNDstop(int);
int SNDtimeremaining(int);
void SNDCTRL_drylevel(int, int);
}
int SNDVOICEI_get(int);
int SNDPLATFORM_getcurframe(int);
SINT64 iSNDmulu64(unsigned int, unsigned int);
int iSNDdivu64(SINT64, int);
void SNDPLATFORM_setpitch(int);
void SNDPLATFORM_setvol(int);
void SNDPLATFORM_set3dpos(int);
void SNDI_calcfxlevel(int, int);
void SNDPLATFORM_setfxlevel(int, int);
void SNDPLATFORM_lowpass(int, int);
void SNDPLATFORM_highpass(int, int);
void SNDPLATFORM_timemult(int, int);

/* inferred: an inline clamp helper */
static inline int AEMS_CLAMP(int x, int lo, int hi) {
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}

void SNDAEMSI_updateplayerbank(AEMSCOMPDYNAMICPLAYER* player) {
    AEMSPLAYERDEFVIEW* def = (AEMSPLAYERDEFVIEW*)player->def;

    if (player->playing != player->in[7]) {
        if (player->in[7] == 1) {
            if (player->playing == 0) {
                SNDPLAYOPTS opts;
                int index = player->in[6];

                if (index >= def->table->count)
                    index = def->table->count - 1;
                player->sound = def->table->sounds[index];
                SNDplaysetdef(&opts);
                player->pitch = AEMS_CLAMP(player->in[0], 0, 0xFFFF);
                opts.pitch = player->pitch;
                player->volume = AEMS_CLAMP(player->in[2], 0, 32767);
                opts.volume = player->volume >> 8;
                player->azimuth = player->in[3];
                opts.azimuth = player->azimuth;
                player->elevation = AEMS_CLAMP(player->in[4], -16384, 16383);
                opts.elevation = player->elevation;
                player->fx = AEMS_CLAMP(player->in[5], 0, 32767);
                opts.fx = player->fx >> 8;
                player->lowpass = AEMS_CLAMP(player->in[8], 0, 0xFFFF);
                opts.lowpass = player->lowpass;
                player->highpass = AEMS_CLAMP(player->in[9], 0, 0xFFFF);
                opts.highpass = player->highpass;
                player->time = AEMS_CLAMP(player->in[1], 0, 0xFFFF);
                opts.time = player->time;
                if (def->bank->version >= 9) {
                    player->dry = AEMS_CLAMP(player->in[10], 0, 32767);
                    opts.dry = player->dry >> 8;
                }
                player->handle = SNDBANK_play(*def->bank->bank, player->sound & 0x3FFF, &opts);
                if (player->handle >= 0) {
                    player->out = 1;
                    player->unknown14 = 0x7FFFFFFF;
                    player->unknown18 = 0;
                } else {
                    player->out = 0;
                    player->unknown14 = 0;
                    player->unknown18 = 0;
                }
            }
        } else if (player->in[7] == 2) {
            if (player->playing != 1)
                player->in[7] = 0;
        } else if (player->in[7] == 0) {
            SNDstop(player->handle);
            player->handle = -1;
            player->out = 0;
            player->unknown14 = 0;
            player->unknown18 = 0;
        }
        player->playing = player->in[7];
    }
    if (player->playing == 1) {
        SNDVOICEVIEW* voice;
        int index = SNDVOICEI_get(player->handle);

        if (index < 0) {
            player->out = 0;
            player->unknown14 = 0;
            player->unknown18 = 0;
        } else {
            voice = &sndgs.voices[index];

            if (def->outputs->timeUsed > 0) {
                player->unknown14 = SNDtimeremaining(player->handle) * 10;
                if (player->unknown14 < 0)
                    player->unknown14 = 0x7FFFFFFF;
            }
            if (def->outputs->frameUsed > 0) {
                player->unknown18 = SNDPLATFORM_getcurframe(index);
                if (player->unknown18 > 2000000)
                    player->unknown18 = iSNDdivu64(iSNDmulu64(player->unknown18, 1000), voice->rate);
                else if (player->unknown18 >= 0)
                    player->unknown18 = player->unknown18 * 1000 / voice->rate;
                else
                    player->unknown18 = 0;
            }
            if (player->in[0] != player->pitch) {
                player->pitch = AEMS_CLAMP(player->in[0], 0, 0xFFFF);
                voice->pitch = player->pitch;
                SNDPLATFORM_setpitch(index);
            }
            if (player->in[2] != player->volume) {
                player->volume = AEMS_CLAMP(player->in[2], 0, 32767);
                voice->volume = player->volume << 8;
                voice->volumeLevel = player->volume * sndgs.masterVolume / 32767;
                SNDPLATFORM_setvol(index);
            }
            if (player->in[3] != player->azimuth || player->in[4] != player->elevation) {
                player->azimuth = player->in[3];
                player->elevation = AEMS_CLAMP(player->in[4], -16384, 16383);
                voice->azimuth = player->azimuth;
                voice->elevation = player->elevation;
                SNDPLATFORM_set3dpos(index);
            }
            if (player->in[5] != player->fx) {
                player->fx = AEMS_CLAMP(player->in[5], 0, 32767);
                voice->fxLevel = player->fx >> 8;
                SNDI_calcfxlevel(0, index);
                SNDPLATFORM_setfxlevel(index, 0);
            }
            if (player->in[8] != player->lowpass) {
                player->lowpass = AEMS_CLAMP(player->in[8], 0, 0xFFFF);
                SNDPLATFORM_lowpass(index, player->lowpass);
            }
            if (player->in[9] != player->highpass) {
                player->highpass = AEMS_CLAMP(player->in[9], 0, 0xFFFF);
                SNDPLATFORM_highpass(index, player->highpass);
            }
            if (player->in[1] != player->time) {
                player->time = AEMS_CLAMP(player->in[1], 0, 0xFFFF);
                voice->time = player->time;
                SNDPLATFORM_timemult(index, player->time);
            }
            if (def->bank->version >= 9 && player->in[10] != player->dry) {
                player->dry = AEMS_CLAMP(player->in[10], 0, 32767);
                SNDCTRL_drylevel(player->handle, (signed char)(player->dry >> 8));
            }
        }
    }
}

struct SNDAEMSVIEW {
    unsigned char unknown00[32];
    void (*updateplayerbank)(AEMSCOMPDYNAMICPLAYER*);
    unsigned char unknown24[32];
};

extern SNDAEMSVIEW sndaems;

void SNDAEMS_linkbank() {
    sndaems.updateplayerbank = SNDAEMSI_updateplayerbank;
}
