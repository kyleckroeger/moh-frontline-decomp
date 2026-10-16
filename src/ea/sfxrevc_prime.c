/* A fragment of the reverb set-up file (sfxrevc.c, 0x80167ffc):
   SNDMIXI_initfx2 copies a version-2 effect file's header, finds the
   highest output channel count it has filters for (from the mixer's
   output count down), walks past the filter modules (each sized by its
   type through filtermodsizetable) and entries of the lower channel counts
   to find the module and connection lists of that count, and then runs the
   effect set-up; findprime returns the first prime at or above a delay
   length (rate * milliseconds / 1000, at least 3), testing divisors up to
   the root found by a search. The header and mixer views are inferred; the
   effect globals belong to sdfx.c. */
struct MIXFXFILTHEADER;

/* inferred view of the fx header: a filter count and an entry count per
   output channel count */
struct MIXFXHEADERVIEW {
    unsigned char unknown00[8];
    int filtercount[6];
    int entrycount[6];
};

struct SNDMIXVIEW {
    unsigned char unknown000[5];
    unsigned char outputs;
    unsigned char unknown006[506];
};

extern "C" {
extern SNDMIXVIEW sndmix;
void SNDI_memcpy(void*, const void*, int);
}
int SNDI_getb(void*, int);
void SNDMIXI_initfx(int);

extern MIXFXHEADERVIEW filehead;
extern int filtermodsizetable[];
extern int lastoutputchannels;
extern int validstructoutputs;
extern unsigned char* modlist;
extern unsigned char* conlist;
extern int SNDMIXfxv2inited;

void SNDMIXI_initfx2(int which, MIXFXFILTHEADER* header) {
    int count;
    int j;
    unsigned char* p;
    int i;
    int last;

    SNDMIXfxv2inited = 1;
    SNDI_memcpy(&filehead, header, 56);
    p = (unsigned char*)header + 56;
    last = sndmix.outputs - 1;
    if (SNDI_getb(&filehead.filtercount[last], 4)) {
        validstructoutputs = sndmix.outputs;
    } else {
        while (!SNDI_getb(&filehead.filtercount[last], 4))
            last--;
        validstructoutputs = last + 1;
    }
    lastoutputchannels = validstructoutputs;
    for (i = 0; i < last; i++) {
        count = SNDI_getb(&filehead.filtercount[i], 4);
        for (j = 0; j < count; j++)
            p += filtermodsizetable[*p];
        p += SNDI_getb(&filehead.entrycount[i], 4) * 12;
    }
    modlist = p;
    count = SNDI_getb(&filehead.filtercount[last], 4);
    for (j = 0; j < count; j++)
        p += filtermodsizetable[*p];
    conlist = p;
    SNDMIXI_initfx(which);
}

int findprime(int rate, int ms) {
    int n = ms * rate / 1000;
    int root;
    int i;
    int limit;
    int product;

    if (n < 3)
        n = 3;
restart:
    i = 0;
    do {
        root = i;
        i++;
        product = i * root;
        if (product == n) {
            i--;
            goto found;
        }
    } while (product <= n);
    i -= 2;
found:
    limit = i + 1;
    for (i = 2; i <= limit; i++) {
        if (n % i == 0) {
            n++;
            goto restart;
        }
        if (i == limit)
            return n;
    }
    goto restart;
}
