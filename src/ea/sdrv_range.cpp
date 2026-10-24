/* A fragment of the sound driver (snddrv.c, 0x8016b0f0): the voice range a
   render mode may use (mode bit 0x200: none below the first driver voice;
   bit 4: from the first driver voice to the voice count), resolving a
   timbre's sample data asynchronously (the data offset is taken from the
   header when none is given, and the sample pointer becomes the buffer plus
   the remaining offset), and SNDPLATFORM_playtimbre, which starts a timbre on
   a voice: through the mixer (each channel's mixer voice initialised with
   its sample data, length and loop, the voice's pitch, position, effect,
   time and filters set, then played) or, for DSP voices, through AX (ADPCM
   or 16-bit PCM addresses in nibbles or samples with the loop, or a silent
   loop, ADPCM coefficients and loop context per channel; -9 when no AX voice
   is free). sndgs, snddrv and the AX types are named by their symbols; the
   views of sndgs, snddrv, the voice record and the patch header's members
   are inferred. */
extern "C" {
void* memset(void*, int, unsigned long);
void SNDI_memcpy(void*, const void*, int);
}

typedef struct _AXVPB AXVPB;

struct AXPBADDR {
    unsigned short loopFlag;
    unsigned short format;
    unsigned short loopAddressHi;
    unsigned short loopAddressLo;
    unsigned short endAddressHi;
    unsigned short endAddressLo;
    unsigned short currentAddressHi;
    unsigned short currentAddressLo;
};

struct AXPBADPCM {
    unsigned short a[8][2];
    unsigned short gain;
    unsigned short pred_scale;
    unsigned short yn1;
    unsigned short yn2;
};

struct AXPBADPCMLOOP {
    unsigned short loop_pred_scale;
    unsigned short loop_yn1;
    unsigned short loop_yn2;
};

extern "C" {
void AXSetVoiceSrcType(AXVPB*, unsigned long);
void AXSetVoiceAdpcm(AXVPB*, AXPBADPCM*);
void AXSetVoiceAdpcmLoop(AXVPB*, AXPBADPCMLOOP*);
void AXSetVoiceAddr(AXVPB*, AXPBADDR*);
void AXSetVoiceState(AXVPB*, unsigned short);
}

/* inferred: the patch header's sample fields */
struct SNDIPATCHHEADER {
    unsigned char unknown00[3];
    signed char channels;
    unsigned char unknown04[15];
    unsigned char mixmode;
    unsigned char unknown14[66];
    unsigned short rate;
    unsigned int length;
    unsigned int loopStart;
    int loopEnd;
    unsigned char** data;
    int offset;
    int offsets[4];
    int extra[8];
    unsigned char* coefs[4];
    unsigned short* loopContext[4];
};

struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short voices[6];
    unsigned int length;
    unsigned char unknown14[8];
    unsigned short rate;
    unsigned char format;
    unsigned char channels;
    unsigned short flags;
    unsigned char unknown22[94];
};

struct SNDGSVIEW {
    unsigned char unknown000[51];
    unsigned char firstVoice;
    unsigned char unknown034[316];
    short voiceCount;
    unsigned char unknown172[98];
    SNDVOICEVIEW* voices;
};

struct SNDDRVVOICEVIEW {
    unsigned char unknown00[8];
    unsigned int offset;
    unsigned char unknown0c[8];
    unsigned char active;
    unsigned char unknown15[23];
    AXVPB* ax;
};

struct SNDDRVVIEW {
    SNDDRVVOICEVIEW voices[77];
    unsigned char unknown0e70[4];
    unsigned int silence;
    unsigned int silenceAdpcm;
};

extern "C" SNDGSVIEW sndgs;
extern SNDDRVVIEW snddrv;

void SNDPLATFORM_getvoicerange(int mode, int* low, int* high) {
    if (mode & 0x200) {
        *low = 0;
        *high = sndgs.firstVoice;
    } else if (mode & 4) {
        *low = sndgs.firstVoice;
        *high = sndgs.voiceCount;
    }
}

int SNDPLATFORM_asyncresolvetimbre(SNDIPATCHHEADER* header, char* buffer, int* offset) {
    if (*offset == 0)
        *offset = header->offset;
    *header->data = (unsigned char*)(buffer + (header->offset - *offset));
    return 0;
}

int SNDDRV_allocateaxvoice(int);
void SNDPLATFORM_set3dpos(int);
void SNDPLATFORM_setvol(int);
void SNDPLATFORM_setpitch(int);
void SNDPLATFORM_setfxlevel(int, int);
void SNDPLATFORM_timemult(int, int);
void SNDPLATFORM_lowpass(int, int);
void SNDPLATFORM_highpass(int, int);
extern "C" {
void MIX_playinit(int, int, int, void*, int, int, int, unsigned int, unsigned int, int, int, int, int);
void MIX_play(int);
}

int SNDPLATFORM_playtimbre(SNDIPATCHHEADER* header, void* data, int index, int time, int unused, int lowpass, int highpass) {
    SNDVOICEVIEW* voice;
    SNDDRVVOICEVIEW* drv;
    unsigned int current;
    unsigned int loopAddress;
    unsigned int endAddress;
    unsigned int step;
    unsigned int offset;
    unsigned int start;
    unsigned int end;
    int i;

    voice = &sndgs.voices[index];
    drv = &snddrv.voices[index];
    voice->rate = header->rate;
    voice->length = header->length;
    step = 0;
    offset = 0;
    voice->channels = header->channels;
    if (voice->flags & 0x200) {
        AXPBADPCMLOOP loop;
        AXPBADDR addr;
        AXPBADPCM adpcm;

        memset(&loop, 0, sizeof(loop));
        if (header->loopEnd >= 0) {
            addr.loopFlag = 1;
            end = header->loopEnd;
            start = header->loopStart;
        } else {
            addr.loopFlag = 0;
            if (voice->format == 18)
                start = snddrv.silenceAdpcm;
            else
                start = snddrv.silence;
            end = header->length - 1;
        }
        if (voice->format == 18) {
            addr.format = 0;
            adpcm.gain = 0;
            adpcm.yn1 = 0;
            adpcm.yn2 = 0;
            step = ((header->length + 13) / 14 * 14 << 3) / 7;
            offset = (unsigned int)*header->data << 1;
            start = start * 8 / 7;
            end = end * 8 / 7;
        } else if (voice->format == 7) {
            addr.format = 10;
            adpcm.gain = 0x800;
            step = header->length;
            offset = (unsigned int)*header->data >> 1;
        }
        drv->offset = offset;
        if (!SNDDRV_allocateaxvoice(index))
            return -9;
        SNDPLATFORM_set3dpos(index);
        SNDPLATFORM_setvol(index);
        SNDPLATFORM_setpitch(index);
        SNDPLATFORM_setfxlevel(voice->voices[0], 0);
        for (i = 0; i < header->channels; i++) {
            if (voice->format == 18) {
                SNDI_memcpy(&adpcm, header->coefs[i], 32);
                adpcm.pred_scale = header->coefs[i][32];
                if (header->loopEnd > 0) {
                    loop.loop_pred_scale = *(unsigned char*)&header->loopContext[i][2];
                    loop.loop_yn1 = header->loopContext[i][0];
                    loop.loop_yn2 = header->loopContext[i][1];
                }
            }
            current = offset + step * i;
            loopAddress = header->loopEnd > 0 ? current + start : start;
            endAddress = current + end;
            endAddress -= 2;
            if (voice->format == 18) {
                current += 2;
                if ((loopAddress & 15) == 0)
                    loopAddress += 2;
                else if ((loopAddress & 15) == 1)
                    loopAddress += 1;
                if ((endAddress & 15) == 0)
                    endAddress += 2;
                else if ((endAddress & 15) == 1)
                    endAddress += 1;
            }
            AXSetVoiceSrcType(snddrv.voices[voice->voices[i]].ax, 1);
            AXSetVoiceAdpcm(snddrv.voices[voice->voices[i]].ax, &adpcm);
            AXSetVoiceAdpcmLoop(snddrv.voices[voice->voices[i]].ax, &loop);
            addr.endAddressHi = endAddress >> 16;
            addr.endAddressLo = endAddress;
            addr.currentAddressHi = current >> 16;
            addr.currentAddressLo = current;
            addr.loopAddressHi = loopAddress >> 16;
            addr.loopAddressLo = loopAddress;
            AXSetVoiceAddr(snddrv.voices[voice->voices[i]].ax, &addr);
            AXSetVoiceState(snddrv.voices[voice->voices[i]].ax, 1);
            snddrv.voices[voice->voices[i]].active = 1;
        }
    } else {
        for (i = 0; i < voice->channels; i++)
            MIX_playinit(voice->voices[i] - sndgs.firstVoice, header->mixmode, 1, (char*)data + (&header->offset)[i], -1,
                         header->extra[i], voice->channels, voice->length, header->loopStart, header->loopEnd, 0, 0, i);
        SNDPLATFORM_setpitch(voice->voices[0]);
        SNDPLATFORM_set3dpos(voice->voices[0]);
        SNDPLATFORM_setfxlevel(voice->voices[0], 0);
        SNDPLATFORM_timemult(voice->voices[0], time);
        SNDPLATFORM_lowpass(voice->voices[0], lowpass);
        SNDPLATFORM_highpass(voice->voices[0], highpass);
        for (i = 0; i < voice->channels; i++)
            MIX_play(voice->voices[i] - sndgs.firstVoice);
    }
    return 0;
}
