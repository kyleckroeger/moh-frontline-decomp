// A fragment of the mixer effects (smixfx.c, 0x80160d54): SNDMIXI_modlapifxadd
// runs the effects for a block: it counts the call, starts the source filter
// when the header lists filters for the current output layout, then runs each
// output's effect head once (on the dry and wet work buffers) and adds the wet
// result to that output and to every output the copy table lists for it,
// marking them done, and finally clears the effect buffer for the block.
// sndfx's layout, the mixer, header and head views and the copy table's shape
// are inferred (the mixer as in smixer_create.c). The rest of the file is not
// part of this unit.
extern "C" void* memset(void*, int, unsigned long);
extern "C" char sndfx[];

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
    (*(int*)(sndfx + 1920))++;
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

