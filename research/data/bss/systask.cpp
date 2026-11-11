// systask.cpp: its zero-initialised globals (0x3c8 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct SNDAEMSVIEW { unsigned char data[0x44]; }; // view: size from the original
struct SNDPPSVIEW { unsigned char data[0x384]; }; // view: size from the original

SNDAEMSVIEW sndaems;
SNDPPSVIEW sndpps;
