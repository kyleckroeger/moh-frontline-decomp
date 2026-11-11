// srandom.c: its zero-initialised globals (0xb54 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct REVERBVIEW { unsigned char data[0x18c]; }; // view: size from the original
struct MIXFXHEADERVIEW { unsigned char data[0x38]; }; // view: size from the original

char sndmix[512];
char sndfx[1924];
REVERBVIEW sndfb;
int sndfxnummodules[3];
MIXFXHEADERVIEW filehead;
