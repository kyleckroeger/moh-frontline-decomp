// A fragment of the mixer effects (smixfx.c, 0x80160d54): SNDMIXI_modlapifxadd
// runs the effects for a block: it counts the call, starts the source filter
// when the header lists filters for the current output layout, then runs each
// output's effect head once (on the dry and wet work buffers) and adds the wet
// result to that output and to every output the copy table lists for it,
// marking them done, and finally clears the effect buffer for the block.
// SNDMIXI_fxadd runs the reverb modules of the three effect stages over a
// block (the first stage in place on the effect buffer, the later stages
// chaining through the second and third effect buffers), splitting the block
// at each module's two update periods, then adds the stage outputs to the
// mixer outputs (odd outputs take the second stage's buffer when it has
// modules, the others the third stage's buffer when it has modules, else the
// effect buffer; output 5 gets nothing) and clears the effect buffer; the
// module position is an index into sndfb. sndfx's layout, the mixer, header,
// head and module views and the copy table's shape are inferred (the mixer as
// in smixer_create.c). The rest of the file is not part of this unit.
extern "C" void* memset(void*, int, unsigned long);
extern "C" float sndfx[];

// File-local in the original; declared extern so this fragment can refer to it.
extern int FXfilterindex;

/* inferred: a float-to-integer stage (FT24_32STATE) as a filter record */
struct FT24_32STATE {
    unsigned char unknown00[24];
    unsigned short priority;
    unsigned char unknown1a[2];
};

/* inferred: the software mixer's state, as far as it is used here */
struct SNDMIXVIEW {
    unsigned int value00;
    unsigned char channels;
    unsigned char outputs;
    unsigned char value06;
    unsigned char value07;
    unsigned int value08;
    unsigned char unknown00c[3];
    signed char value0f;
    unsigned char unknown010[272];
    void* work[2];
    int* alignedWork[2];
    void* mix[4];
    int* alignedMix[4];
    unsigned char unknown150[20];
    void* filters[4];
    unsigned char unknown174[16];
    FT24_32STATE ft[4];
    void* channelRecords;
    void (*scalemix)(int, float, int*, float*);
};


/* inferred: an effect head, processed through its first slot */
struct MIXFXHEADVIEW {
    void (*process)(MIXFXHEADVIEW*, int, int*, int*, int);
};

/* inferred view of the fx header: a filter count per output channel count */
struct MIXFXHEADERVIEW {
    unsigned char unknown00[8];
    int filtercount[1];
};

struct SRCSTATE;

int SNDI_getb(void*, int);
void SFILTER_initSOURCE(SRCSTATE*, void*);

extern "C" SNDMIXVIEW sndmix;
extern MIXFXHEADERVIEW* pheaderinfo;
extern int validstructoutputs;
extern SRCSTATE** pfilterlist;
extern MIXFXHEADVIEW** phead;
extern signed char tablecopyto[][6][6][6];

extern "C" void SNDMIXI_modlapifxadd(int count) {
    int done[6] = {0, 0, 0, 0, 0, 0};
    (*(int*)(sndfx + 480))++;
    int* dry = sndmix.alignedWork[0];
    int* wet = sndmix.alignedWork[1];
    int filters = SNDI_getb(&pheaderinfo->filtercount[validstructoutputs - 1], 4);
    if (filters)
        SFILTER_initSOURCE(pfilterlist[FXfilterindex], sndfx);
    if (filters) {
        for (int i = 0; i < sndmix.outputs; i++) {
            if (done[i] == 0 && phead[i + 1]) {
                phead[i + 1]->process(phead[i + 1], count, dry, wet, i + 1);
                sndmix.scalemix(count, 1.0f, wet, (float*)sndmix.alignedMix[i]);
                done[i] = 1;
                int j;
                signed char* copyto = tablecopyto[validstructoutputs - 1][sndmix.outputs - 1][i];
                for (j = 0; j < sndmix.outputs; copyto++, j++) {
                    if (*copyto >= 0) {
                        sndmix.scalemix(count, 1.0f, wet, (float*)sndmix.alignedMix[*copyto]);
                        done[*copyto] = 1;
                    }
                }
            }
        }
    }
    memset(sndfx, 0, count * 4);
}

/* inferred: a reverb module (36 bytes) with two countdowns to its next
   update and their reload period */
struct REVERBVIEW {
    int countA;
    int countB;
    int period;
    unsigned char unknown0c[24];
};

extern "C" {
extern REVERBVIEW sndfb[];
extern int sndfxnummodules[3];
void MIXI_reverbblock(REVERBVIEW*, int, float*, float*);
}

extern "C" void SNDMIXI_fxadd(int count) {
    float* in;
    float* out;
    int n;
    int left;
    int module;
    REVERBVIEW* reverb;
    int stage;
    int i;

    module = 0;
    (*(int*)(sndfx + 480))++;
    in = sndfx;
    out = sndfx;
    for (stage = 0; stage < 3; stage++) {
        for (i = 0; i < sndfxnummodules[stage]; module++, i++) {
            reverb = &sndfb[module];
            if (stage == 1) {
                if (i == 0)
                    out = sndfx + 160;
                else if (i == 1)
                    in = sndfx + 160;
            } else if (stage == 2) {
                if (i == 0) {
                    out = sndfx + 320;
                    in = sndfx;
                } else if (i == 1) {
                    in = sndfx + 320;
                }
            }
            for (left = count; left > 0;) {
                n = left;
                if (reverb->countA <= 0)
                    reverb->countA = reverb->period;
                if (reverb->countB <= 0)
                    reverb->countB = reverb->period;
                if (reverb->countA < n)
                    n = reverb->countA;
                if (reverb->countB < n)
                    n = reverb->countB;
                MIXI_reverbblock(reverb, n, in + (count - left), out + (count - left));
                left -= n;
                reverb->countA -= n;
                reverb->countB -= n;
            }
        }
    }
    for (i = 0; i < sndmix.outputs; i++) {
        if (i == 5)
            continue;
        if ((i & 1) && sndfxnummodules[1])
            sndmix.scalemix(count, 1.0f, (int*)(sndfx + 320), (float*)sndmix.alignedMix[i]);
        else if (sndfxnummodules[2])
            sndmix.scalemix(count, 1.0f, (int*)(sndfx + 160), (float*)sndmix.alignedMix[i]);
        else
            sndmix.scalemix(count, 1.0f, (int*)sndfx, (float*)sndmix.alignedMix[i]);
    }
    memset(sndfx, 0, count * 4);
}
