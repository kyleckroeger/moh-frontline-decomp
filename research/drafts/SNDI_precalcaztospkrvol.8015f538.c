/* best WIP (late2c, 2026-11-13): 4 real lines (was 79), scored with config/GMFE69/satospkr.json (the
   EA file's flags; the index named ShellMenu_ctor.json, same score). Keys, all value-creation order:
   - 'int azimuth = i * 256;' in the loop body (a strength-reduced IV created after the row pointer and
     the speaker count) instead of an accumulator 'azimuth += 256': outer-loop registers exact (79 -> 59).
   - third branch indexes 'az[k]', 'az[k + 1]' and 'sndaztospkr[i][k]' (no walking az/level pointers):
     the az pointer stays a separate value from its 'lhz 120(rN)' load, speaker count in r31 (59 -> 20).
   - first branch through an inline helper given the two differences as ARGUMENTS ('pan', args evaluate
     right to left: before first): exact (20 -> 7).
   - second branch through 'pan2' called with (after, before) and the slots swapped, storing *q first: left
     only the sum's register (orig r30, ours r31 = the second quotient's) and 'mulli after*127' scheduled
     after the first divw (4). The orig's sum interferes with both quotients. 480 spellings of pan2 (body
     order, operand orders, call/local forms; scratch/loop3/late2c/pcgen11.py) and plain-code grids with
     const qualifiers, separate sum locals and a sum parameter (pcgen6/9/10) stay at 4-7.
   Two helpers that differ only in store order are not landable as they stand: find one form for both. */
/* SNDI_aztospkrvol, the last function of the azimuth-to-speaker file: it
   looks up the precalculated per-speaker levels for an azimuth (its high
   byte selects one of 256 rows) and scales each into a 16-bit volume for
   every output speaker. sndaztospkr and sndgs are named by their symbols;
   the table's row layout and the view of sndgs (the speaker count at +72)
   are inferred. */
struct SNDGSVIEW {
    unsigned char unknown000[72];
    unsigned char speakers;
    unsigned char unknown049[47];
    unsigned short azimuths[4][4];
    unsigned char unknown098[416];
};

extern signed char sndaztospkr[256][4];
extern SNDGSVIEW sndgs;

static inline signed char shape(int v) {
    return v + (v - v * v / 127);
}

static inline void pan(signed char* p, signed char* q, int x, int y) {
    *p = shape(x * 127 / (y + x));
    *q = shape(y * 127 / (y + x));
}

static inline void pan2(signed char* p, signed char* q, int x, int y) {
    int a = x * 127 / (x + y);
    int b = y * 127 / (x + y);
    *q = shape(b);
    *p = shape(a);
}

void SNDI_precalcaztospkrvol() {
    int i;

    for (i = 0; i < 256; i++) {
        int azimuth = i * 256;
        if (sndgs.speakers == 1) {
            sndaztospkr[i][0] = 127;
        } else {
            int k;
            unsigned short* az;

            for (k = 0; k < sndgs.speakers; k++)
                sndaztospkr[i][k] = 0;
            az = sndgs.azimuths[sndgs.speakers - 1];
            if (azimuth <= az[0]) {
                pan(&sndaztospkr[i][0], &sndaztospkr[i][sndgs.speakers - 1], azimuth + (0x10000 - az[sndgs.speakers - 1]), az[0] - azimuth);
            } else if (azimuth >= az[sndgs.speakers - 1]) {
                pan2(&sndaztospkr[i][sndgs.speakers - 1], &sndaztospkr[i][0], 0x10000 - azimuth + az[0], azimuth - az[sndgs.speakers - 1]);
            } else {
                for (k = 0; k < sndgs.speakers; k++) {
                    if (azimuth >= az[k] && azimuth <= az[k + 1]) {
                        int before = azimuth - az[k];
                        int after = az[k + 1] - azimuth;
                        int const a = after * 127 / (before + after);
                        int b = before * 127 / (before + after);
        
                        sndaztospkr[i][k] = shape(a);
                        sndaztospkr[i][k + 1] = shape(b);
                    }
                }
            }
        }
    }
}

void SNDI_aztospkrvol(int azimuth, short* volumes) {
    signed char* levels = sndaztospkr[(azimuth >> 8) & 0xFF];
    int i = 0;
    while (i < sndgs.speakers) {
        int level = *levels++;
        i++;
        *volumes++ = level * 258;
    }
}
