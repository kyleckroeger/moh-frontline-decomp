/* EA's AEMS module-bank install (saemsamb.c): SNDAEMSI_addmodulebank takes
   the first free slot of sndaems's 16 bank pointers, registers the bank's
   patch and MIDI sound banks with SNDbankadd (failing with its result), asks
   the caller's allocator for the final block (smaller when one sound bank
   reports 7, so only its header is kept), copies the module header and the
   sound bank headers when the block moved, resolves the bank, records the
   sound bank handles and installs it, returning the slot. The global names
   come from the symbols; MODULEBANK is named by the mangled symbol, its
   members (and sndaems's) follow saemsmbf.c and are inferred, and the local
   names are not original. Compiled as C++ like the rest of the sound
   library. */
struct AEMSBANKINFOVIEW {
    int soundBank;
    int soundBank2;
};

struct MODULEBANK {
    unsigned char unknown00[4];
    int size;
    unsigned char unknown08[20];
    unsigned int patchOffset;
    int moduleSize;
    unsigned int midiOffset;
    int soundBankSize;
    unsigned char unknown2c[4];
    AEMSBANKINFOVIEW* info;
};

struct SNDAEMSVIEW {
    unsigned char unknown00[12];
    MODULEBANK** banks;
    unsigned char unknown10[24];
    unsigned char loading;
    unsigned char unknown29[27];
};

extern SNDAEMSVIEW sndaems;

extern "C" {
int SNDbankadd(int*, void*);
int SNDbankheadersize(int);
void SNDbankheadercopy(void*, int);
void SNDI_memcpy(void*, const void*, int);
}
void SNDAEMSI_resolvemodulebank(MODULEBANK*, char*, int);

int SNDAEMSI_addmodulebank(void* data, char* name, int flag, void* (*realloc)(void*, int, int)) {
    MODULEBANK* bank = (MODULEBANK*)data;
    int patchBank = -1;
    int midiBank = -1;
    int slot = 0;
    int patchResult = 0;
    int midiResult = 0;
    int size = bank->size;
    int headerSize = bank->size - (bank->moduleSize + bank->soundBankSize);
    int newSize;
    MODULEBANK* copy;

    for (slot = 0; slot < 16; slot++) {
        if (sndaems.banks[slot] == 0)
            break;
    }

    if (bank->patchOffset) {
        patchResult = SNDbankadd(&patchBank, (void*)(bank->patchOffset + (unsigned int)data));
        if (patchResult < 0)
            return patchResult;
    }
    if (bank->midiOffset) {
        midiResult = SNDbankadd(&midiBank, (void*)(bank->midiOffset + (unsigned int)data));
        if (midiResult < 0)
            return midiResult;
    }

    if (patchResult != 0 && midiResult != 0)
        newSize = size;
    else if (patchResult == 7)
        newSize = headerSize + SNDbankheadersize(patchBank);
    else if (midiResult == 7)
        newSize = headerSize + SNDbankheadersize(midiBank);
    else
        newSize = size;

    copy = (MODULEBANK*)realloc(bank, newSize, size);
    if (copy != bank) {
        SNDI_memcpy(copy, bank, headerSize);
        bank = copy;
        copy = (MODULEBANK*)((char*)copy + headerSize);
        if (bank->patchOffset)
            SNDbankheadercopy(copy, patchBank);
        if (bank->midiOffset && midiResult >= 0) {
            int patchHeaderSize = SNDbankheadersize(midiBank);
            SNDbankheadercopy((char*)copy + patchHeaderSize, midiBank);
        }
    }

    SNDAEMSI_resolvemodulebank(bank, name, flag);
    bank->info->soundBank = patchBank;
    bank->info->soundBank2 = midiBank;
    sndaems.banks[slot] = bank;
    return slot;
}
