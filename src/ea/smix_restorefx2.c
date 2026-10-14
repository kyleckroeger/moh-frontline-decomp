/* SNDMIXI_restorefx2 (0x80167a98): tears down the version-2 mixer effects: each
   loaded filter (as many as the effect header lists for the last output
   channel count) is restored through its own callback and freed, then the
   filter list, the six head entries and the head block are cleared and freed
   and the effect lists and the initialised flag are reset. Its file record is
   not known (the file map only guesses smixc.c), so this unit claims none. The
   filter, header and list views are inferred.
*/
struct MIXFXFILTERVIEW {
    unsigned char unknown00[4];
    void (*restore)(MIXFXFILTERVIEW*);
};

int SNDI_getb(void*, int);
void SNDMEMI_free(void*);

/* inferred view of the fx header: a filter count per output channel count */
struct MIXFXHEADERVIEW {
    unsigned char unknown00[8];
    int filtercount[1];
};

extern MIXFXHEADERVIEW* pheaderinfo;
extern int lastoutputchannels;
extern MIXFXFILTERVIEW** pfilterlist;
extern void** phead;
extern void* modlist;
extern void* conlist;
extern void* pconhead;
extern void* plisthead;
extern int SNDMIXfxv2inited;

void SNDMIXI_restorefx2() {
    int count = SNDI_getb(&pheaderinfo->filtercount[lastoutputchannels - 1], 4);
    if (count) {
        for (int i = 0; i < count; i++) {
            if (pfilterlist[i]->restore)
                pfilterlist[i]->restore(pfilterlist[i]);
            if (pfilterlist[i]) {
                SNDMEMI_free(pfilterlist[i]);
                pfilterlist[i] = 0;
            }
        }
    }
    if (pfilterlist) {
        SNDMEMI_free(pfilterlist);
        pfilterlist = 0;
    }
    for (int i = 0; i < 6; i++) {
        if (phead[i])
            phead[i] = 0;
    }
    if (phead) {
        SNDMEMI_free(phead);
        phead = 0;
    }
    modlist = 0;
    conlist = 0;
    pconhead = 0;
    plisthead = 0;
    SNDMIXfxv2inited = 0;
}
