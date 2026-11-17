// settings: config/GMFE69/sound.json
// replaces: strlen
// Test harness: strlen that reports a pointer past main memory, with its caller.
extern "C" void OSReport(const char*, ...);
static int s_reported;
extern "C" unsigned long strlen(const char* s) {
    register unsigned long lr;
    asm { mflr lr }
    if ((unsigned long)s >= 0x84000000 && s_reported < 20) {
        s_reported++;
        OSReport("BADSTRLEN %08x lr %08x\n", (unsigned long)s, lr);
    }
    const char* p = s;
    while (*p)
        p++;
    return p - s;
}
