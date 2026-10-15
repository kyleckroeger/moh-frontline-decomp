// A fragment of sound.cpp (0x800ec6f4): aems_BankAlloc allocates an AEMS
// sound bank of the requested size (16-byte aligned, from the "AEMS Bank"
// pool) and records it in the bank table at the current bank count. The
// function (file-local in the original) and the globals are named by the
// symbols; the first and third parameters (unused here) and the table's
// element type are inferred. AEMSOneShotMalloc hands out the next block of
// the one-shot sound memory (the offset advanced by the size; the memory is
// file-local in the original and declared extern here, with its symbol's
// size; the element types are inferred). LoadOneShotSound loads the one-shot
// sound bank for a level and stage unless it is loaded already: the bank name
// is formatted from them, a loaded bank is removed first, and the bank is then
// loaded asynchronously into the one-shot memory through the stream buffer
// (the bank id, handle and stream buffer are file-local in the
// original and declared extern here; the parameter meanings are inferred).
// The rest of the file is in other units.
void* DWI_allocalign(const char*, int, int, int);

extern int gAEMSBankCount;
extern void* gAEMSBank[];

static void aems_BankAlloc(void*, int size, int) {
    gAEMSBank[gAEMSBankCount] = DWI_allocalign("AEMS Bank", size, 16, 1024);
}

extern char g_AEMSOneShotMem[0x70800]; // file-local in the original
extern int g_AEMSOneShotMemOffset;

void* AEMSOneShotMalloc(int size) {
    void* block = &g_AEMSOneShotMem[g_AEMSOneShotMemOffset];
    g_AEMSOneShotMemOffset += size;
    return block;
}

extern "C" int sprintf(char*, const char*, ...);
extern int (*SNDAEMS_removemodulebank)(int);
extern int (*SNDAEMS_asyncloadmodulebank)(char*, int, char*, int, void*, int, void* (*)(int));

extern int g_AEMSOneShotBankID;              // file-local in the original
extern int g_AEMSOneShotBankHandle;          // file-local in the original
extern int g_AEMSOneShotLoadState;
extern char g_AEMSOneShotStreamBuf[0xc800]; // file-local in the original

void LoadOneShotSound(int id, int level, int stage) {
    char name[64];
    if (g_AEMSOneShotBankID == id)
        return;
    sprintf(name, "data\\%d\\%d_%d\\%d%dload%d.abk", level, level, stage, level, stage, id);
    if (g_AEMSOneShotLoadState == 2) {
        SNDAEMS_removemodulebank(g_AEMSOneShotBankHandle);
        g_AEMSOneShotBankID = -1;
        g_AEMSOneShotBankHandle = -1;
        g_AEMSOneShotLoadState = 0;
        g_AEMSOneShotMemOffset = 0;
    }
    g_AEMSOneShotBankHandle =
        SNDAEMS_asyncloadmodulebank(name, 0, 0, 0, g_AEMSOneShotStreamBuf, 0xc800, AEMSOneShotMalloc);
    g_AEMSOneShotLoadState = 1;
    g_AEMSOneShotBankID = id;
    g_AEMSOneShotMemOffset = 0;
}
