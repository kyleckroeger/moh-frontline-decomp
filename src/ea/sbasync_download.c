/* A fragment of the asynchronous sound-bank loader, sbasync.c (0x80159d08):
   SNDBANKI_asyncissuedownloads walks the ring of four reads from the next
   one to download and, for each completed read (state 2) while the
   download window allows, issues the download of the rest of its chunk to
   the bank's destination (state 3), or retires the read when nothing is
   left to download. SNDBANKI_asyncserver, the loader's 100 Hz client,
   completes reads, copies the header, retires finished downloads, issues
   more downloads and reads, and when everything is read and downloaded
   closes the file, removes itself, stores the bank in its slot and resolves
   the bank's patches against its destination. The load and read-record views follow sbasync.c and are
   inferred from offsets (the two counters at +0x3c/+0x3e are shared with
   the header copy); SNDPLATFORM_download and sndbas are named by the
   symbols. The rest of the file is in other units. */
int SNDPLATFORM_download(int, void*, void*, int);
int SNDPLATFORM_downloadcomplete(int);
void SNDBANKI_asynccompletereads(void);
void SNDBANKI_asyncxferhdr(void);
void SNDBANKI_asyncissuereads(void);
void SNDBANKI_asyncserver(void);
struct TAGGEDPATCH;
void SNDBANKI_asyncresolvepatch(int, TAGGEDPATCH*, char*, int*);
extern "C" {
int FILESYS_close(int, int, void*);
int FILESYS_opstatus(int);
int FILESYS_completeop(int);
void SNDSYS_remove100hzclient(void (*)(void));
}

struct SndBankReadView {
    int op;
    int download;
    char* buffer;
    signed char state;
    unsigned char unknown0d[3];
};

struct SndBankLoadView {
    unsigned char unknown00[8];
    int closeOp;
    char* header;
    unsigned char unknown10[4];
    int fileOffset;
    int file;
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
    int unknown08;
};

extern SndBankAsyncView sndbas;

/* Inferred: a loaded bank's header (patch count at +6, a word at +12, then
   self-relative patch offsets from +20, word 5). */
struct SndBankHeaderView {
    unsigned char unknown00[6];
    unsigned short patches;
    unsigned char unknown08[4];
    int unknown0c;
    unsigned char unknown10[4];
    unsigned int offsets[1];
};

/* Inferred: a bank slot (12 bytes): the header, its destination and a busy flag. */
struct SndBankSlotView {
    SndBankHeaderView* header;
    char* dest;
    unsigned char loading;
    unsigned char unknown09[3];
};

/* Inferred: the sound globals' bank slots at +0x1d8. */
struct SndGlobalsView {
    unsigned char unknown000[0x1d8];
    SndBankSlotView* banks;
    unsigned char unknown1dc[0x238 - 0x1dc];
};

extern SndGlobalsView sndgs;

void SNDBANKI_asyncissuedownloads(void) {
    SndBankReadView* read;
    int i;
    int size;

    if (sndbas.load->remaining <= 0)
        return;
    i = sndbas.load->downloadBase % 4;
    while (sndbas.load->downloaded < sndbas.load->downloadBase + 4) {
        SndBankLoadView* load = sndbas.load;
        read = &load->reads[i];
        if (read->state != 2)
            return;
        if (load->remaining <= 0) {
            read->state = 0;
            return;
        }
        if (load->remaining > 0 && load->partial < load->chunkSize) {
            size = load->chunkSize - load->partial;
            if (size > load->remaining)
                size = load->remaining;
            read->download = SNDPLATFORM_download(0x200, read->buffer + load->partial, load->dest + load->destOffset, size);
            read->state = 3;
            sndbas.load->destOffset += size;
            sndbas.load->remaining -= size;
        }
        if (read->state == 3) {
            sndbas.load->partial = 0;
            sndbas.load->downloaded++;
        }
        i++;
        if (i >= 4)
            i = 0;
    }
}

static inline void retiredownloads(void) {
    SndBankReadView* read;
    int i = sndbas.load->downloadBase % 4;

    while (sndbas.load->downloaded > sndbas.load->downloadBase) {
        read = &sndbas.load->reads[i];
        if (!SNDPLATFORM_downloadcomplete(read->download))
            break;
        if (read->download <= 0)
            break;
        read->state = 0;
        i++;
        sndbas.load->downloadBase++;
        if (i >= 4)
            i = 0;
    }
}

/* Inferred: the end of the load (stores the bank in its slot and resolves
   its patches). Its own locals keep 'resolved = i' a register copy. */
static inline void finishload(void) {
    int resolved;
    SndBankSlotView* slot;
    int i;
    SndBankHeaderView* header;
    unsigned int* p;

    resolved = i = 0;
    slot = &sndgs.banks[sndbas.load->bank];
    slot->header = (SndBankHeaderView*)sndbas.load->header;
    header = slot->header;
    slot->dest = sndbas.load->dest;
    if (header->unknown0c) {
        resolved = i; /* stored again */
        /* the patch offsets (words 5..) are walked with a word pointer from the header */
        for (p = (unsigned int*)header; i < header->patches; p++, i++) {
            if (p[5])
                SNDBANKI_asyncresolvepatch(0x200, (TAGGEDPATCH*)((char*)&p[5] + p[5]), slot->dest, &resolved);
        }
    }
    slot->loading = 0;
}

void SNDBANKI_asyncserver(void) {
    SndBankHeaderView* header;
    int i;
    SndBankReadView* read;
    int resolved;
    SndBankSlotView* slot;

    SNDBANKI_asynccompletereads();
    SNDBANKI_asyncxferhdr();
    retiredownloads();
    SNDBANKI_asyncissuedownloads();
    SNDBANKI_asyncissuereads();
    if (sndbas.load->headerRemaining <= 0 && sndbas.load->remaining <= 0) {
        if (!sndbas.load->closeOp)
            sndbas.load->closeOp = FILESYS_close(sndbas.load->file, sndbas.priority, 0);
        if (sndbas.load->downloaded == sndbas.load->downloadBase && FILESYS_opstatus(sndbas.load->closeOp) == 1) {
            FILESYS_completeop(sndbas.load->closeOp);
            SNDSYS_remove100hzclient(SNDBANKI_asyncserver);
            finishload();
            sndbas.load = 0;
        }
    }
}
