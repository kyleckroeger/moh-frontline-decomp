// A fragment of sound.cpp (0x800ec6f4): aems_BankAlloc allocates an AEMS
// sound bank of the requested size (16-byte aligned, from the "AEMS Bank"
// pool) and records it in the bank table at the current bank count. The
// function (file-local in the original) and the globals are named by the
// symbols; the first and third parameters (unused here) and the table's
// element type are inferred. AEMSOneShotMalloc hands out the next block of
// the one-shot sound memory (the offset advanced by the size; the memory is
// file-local in the original and declared extern here, with its symbol's
// size; the element types are inferred). The rest of the file is in other units.
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
