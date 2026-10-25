/* A fragment of the asynchronous sound-bank loader, sbasync.c (0x80159d08):
   SNDBANKI_asyncissuedownloads walks the ring of four reads from the next
   one to download and, for each completed read (state 2) while the
   download window allows, issues the download of the rest of its chunk to
   the bank's destination (state 3), or retires the read when nothing is
   left to download. SNDBANKI_asyncserver, the loader's 100 Hz client,
   completes reads, copies the header, retires finished downloads, issues
   more downloads and reads, and when everything is read and downloaded
   closes the file, removes itself, stores the bank in its slot and resolves
   the bank's patches against its destination. SNDBANKI_asyncservice, the
   loader's server client while the file opens and the header is read,
   finishes the open (or fails the load), then, once the header read is
   complete, sizes the load from the file header, allocates the header copy
   and the download buffer, retires finished downloads and hands over to
   SNDBANKI_asyncserver. The load and read-record views follow sbasync.c and are
   inferred from offsets (the two counters at +0x3c/+0x3e are shared with
   the header copy); SNDPLATFORM_download and sndbas are named by the
   symbols. The rest of the file is in other units. */
int SNDPLATFORM_download(int, void*, void*, int);
int SNDPLATFORM_downloadcomplete(int);
void SNDBANKI_asynccompletereads(void);
void SNDBANKI_asyncxferhdr(void);
void SNDBANKI_asyncissuereads(void);
void SNDBANKI_asyncserver(void);
void SNDBANKI_asyncservice(void);
void SNDBANKI_asyncissuedownloads(void);
void* SNDPLATFORM_memalloc(int, int);
void iSNDserverremoveclient(void (*)(void));
struct TAGGEDPATCH;
void SNDBANKI_asyncresolvepatch(int, TAGGEDPATCH*, char*, int*);
extern "C" {
int FILESYS_close(int, int, void*);
int FILESYS_opstatus(int);
int FILESYS_completeop(int);
void SNDSYS_remove100hzclient(void (*)(void));
void SNDSYS_add100hzclient(void (*)(void));
}

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
    int openOp;
    int closeOp;
    char* header;
    void* (*alloc)(int);
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
    int error;
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

/* Inferred: the bank file's header (header and data sizes at +8 and +12). */
struct SndBankFileHeaderView {
    unsigned char unknown00[8];
    int headerSize;
    int dataSize;
};

void SNDBANKI_asyncservice(void) {
    SndBankReadView* read;
    int status;

    if (sndbas.load->openOp) {
        status = FILESYS_opstatus(sndbas.load->openOp);
        if (status == 1) {
            sndbas.load->file = FILESYS_completeop(sndbas.load->openOp);
            sndbas.load->openOp = 0;
            SNDBANKI_asyncissuereads();
        } else {
            if (status == 0)
                return;
            FILESYS_completeop(sndbas.load->openOp);
            sndbas.error = -19;
            if (sndbas.load->active)
                iSNDserverremoveclient(SNDBANKI_asyncservice);
            sndgs.banks[sndbas.load->bank].header = 0;
            sndgs.banks[sndbas.load->bank].loading = 0;
            sndbas.load = 0;
            return;
        }
    }
    read = &sndbas.load->reads[0];
    if (FILESYS_opstatus(read->op) == 1) {
        FILESYS_completeop(read->op);
        read->state = 2;
        sndbas.load->completed++;
        iSNDserverremoveclient(SNDBANKI_asyncservice);
        sndbas.load->active = 0;
        sndbas.load->header = read->buffer;
        sndbas.load->headerRemaining = ((SndBankFileHeaderView*)sndbas.load->header)->headerSize;
        sndbas.load->size = ((SndBankFileHeaderView*)sndbas.load->header)->headerSize;
        sndbas.load->remaining = ((SndBankFileHeaderView*)sndbas.load->header)->dataSize;
        sndbas.load->size += ((SndBankFileHeaderView*)sndbas.load->header)->dataSize;
        sndbas.load->header = (char*)sndbas.load->alloc(((SndBankFileHeaderView*)sndbas.load->header)->headerSize);
        if (sndbas.load->remaining)
            sndbas.load->dest = (char*)SNDPLATFORM_memalloc(0x200, sndbas.load->remaining + 0x40);
        else
            sndbas.load->dest = 0;
        SNDBANKI_asynccompletereads();
        SNDBANKI_asyncxferhdr();
        retiredownloads();
        SNDBANKI_asyncissuedownloads();
        SNDBANKI_asyncissuereads();
        SNDSYS_add100hzclient(SNDBANKI_asyncserver);
    }
}
