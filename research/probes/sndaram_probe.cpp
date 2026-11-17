// settings: config/GMFE69/saramman.json
// replaces: SNDARAM_setpool__FUii
// Test harness (W1, pure image): SNDARAM_setpool as saramman.c has it, reporting the sound pool's
// ARAM base and size (the animation pool is ARAM 0x800000..0x1000000).
struct SNDARAMVIEW {
    unsigned short count;
    unsigned short max;
    unsigned int start;
    unsigned int end;
    void* table;
};
extern "C" {
extern SNDARAMVIEW sndaram;
void OSReport(const char*, ...);
}
void SNDARAM_setpool(unsigned int base, int size) {
    OSReport("SNDARAMPOOL base %08x size %08x\n", base, size);
    sndaram.end = (base + size) & ~4;
    if (base < 32)
        base = 32;
    sndaram.start = base;
    if (base & 31)
        sndaram.start = (base + 31) & ~31;
    sndaram.start >>= 5;
    sndaram.end >>= 5;
}
