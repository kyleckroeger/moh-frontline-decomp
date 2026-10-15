/* The end of the asynchronous sound-bank loader (0x8015a370; the code map
   gives no file record for these, though they follow sbasync.c's functions):
   SNDBANK_asyncload lays a load out in the caller's memory (the load record,
   then four 64-byte-aligned read buffers of the chunk size, a quarter of the
   memory less 0x1c0 rounded down to 64 bytes and at most 0x6000), opens the
   file, claims the first free bank slot, adds the loader's service as a
   server client and runs it once, returning the bank slot (or the error
   when the load already ended); SNDBANK_asyncdone reports whether the load
   has finished - 0 while it is in progress, else 1 or the load's error code.
   sndbas and sndgs are named by their symbols; their views follow sbasync.c
   and are inferred. */
struct SndBankReadView {
    int op;
    int download;
    char* buffer;
    signed char state;
    unsigned char unknown0d[3];
};

struct SndBankLoadView {
    unsigned char active;
    unsigned char unknown01[3];
    int file;
    int closeOp;
    char* header;
    int unknown10;
    int fileOffset;
    unsigned char unknown18[4];
    int partial;
    int size;
    int headerRemaining;
    char* dest;
    int destOffset;
    int remaining;
    unsigned short chunkSize;
    unsigned short bank;
    unsigned short issued;
    unsigned short completed;
    unsigned short downloaded;
    unsigned short downloadBase;
    SndBankReadView reads[4];
};

struct SndBankAsyncView {
    SndBankLoadView* load;
    int priority;
    int error;
};

extern SndBankAsyncView sndbas;

/* Inferred: a bank slot (12 bytes): the header, its destination and a busy flag. */
struct SndBankSlotView {
    void* header;
    char* dest;
    unsigned char loading;
    unsigned char unknown09[3];
};

/* Inferred: the sound globals' bank count at +0x28 and slots at +0x1d8. */
struct SndGlobalsView {
    unsigned char unknown000[0x28];
    unsigned short bankCount;
    unsigned char unknown02a[0x1d8 - 0x2a];
    SndBankSlotView* banks;
    unsigned char unknown1dc[0x238 - 0x1dc];
};

extern SndGlobalsView sndgs;

extern "C" int FILESYS_open(const char*, int, int, int);
void iSNDserveraddclient(void (*)(void));
void SNDBANKI_asyncservice(void);

extern "C" int SNDBANK_asyncload(const char* name, int offset, void* memory, int size, int value) {
    SndBankSlotView* slot;
    SndBankLoadView* load;
    short index;
    int count;

    sndbas.error = 0;
    sndbas.load = (SndBankLoadView*)memory;
    ((SndBankLoadView*)memory)->unknown10 = value;
    sndbas.load->fileOffset = offset;
    sndbas.load->partial = 0;
    sndbas.load->size = 0;
    sndbas.load->headerRemaining = 0;
    sndbas.load->destOffset = 0;
    sndbas.load->header = 0;
    sndbas.load->issued = 0;
    sndbas.load->completed = 0;
    sndbas.load->downloaded = 0;
    sndbas.load->downloadBase = 0;
    sndbas.load->closeOp = 0;
    sndbas.load->chunkSize = (unsigned int)(size - 0x1c0) >> 2;
    if (sndbas.load->chunkSize > 0x6000)
        sndbas.load->chunkSize = 0x6000;
    sndbas.load->chunkSize &= ~0x3f;
    sndbas.load->file = FILESYS_open(name, 1, sndbas.priority, 0);
    index = 0;
    load = sndbas.load;
    load->reads[0].download = -1;
    load->reads[0].state = 0;
    load->reads[0].buffer = (char*)memory + 0xc0;
    load->reads[0].buffer += 0x40;
    load->reads[0].buffer = (char*)((unsigned int)load->reads[0].buffer & ~0x3f);
    load = sndbas.load;
    load->reads[1].download = -1;
    load->reads[1].state = 0;
    load->reads[1].buffer = (char*)memory + (sndbas.load->chunkSize + 0x40) * 1 + 0xc0;
    load->reads[1].buffer += 0x40;
    load->reads[1].buffer = (char*)((unsigned int)load->reads[1].buffer & ~0x3f);
    load = sndbas.load;
    load->reads[2].download = -1;
    load->reads[2].state = 0;
    load->reads[2].buffer = (char*)memory + (sndbas.load->chunkSize + 0x40) * 2 + 0xc0;
    load->reads[2].buffer += 0x40;
    load->reads[2].buffer = (char*)((unsigned int)load->reads[2].buffer & ~0x3f);
    load = sndbas.load;
    load->reads[3].download = -1;
    load->reads[3].state = 0;
    load->reads[3].buffer = (char*)memory + (sndbas.load->chunkSize + 0x40) * 3 + 0xc0;
    load->reads[3].buffer += 0x40;
    load->reads[3].buffer = (char*)((unsigned int)load->reads[3].buffer & ~0x3f);
    count = sndgs.bankCount;
    slot = sndgs.banks;
    for (; count > 0; count--, slot++, index++) {
        if (!slot->header) {
            slot->loading = 1;
            slot->header = (void*)1;
            sndbas.load->bank = index;
            break;
        }
    }
    iSNDserveraddclient(SNDBANKI_asyncservice);
    sndbas.load->active = 1;
    SNDBANKI_asyncservice();
    if (!sndbas.load)
        return sndbas.error;
    return sndbas.load->bank;
}

extern "C" int SNDBANK_asyncdone(void) {
    int result;
    if (!sndbas.load) {
        result = 1;
        if (sndbas.error)
            result = sndbas.error;
        return result;
    }
    return 0;
}
