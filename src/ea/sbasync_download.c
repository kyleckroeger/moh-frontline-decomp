/* A fragment of the asynchronous sound-bank loader, sbasync.c (0x80159d08):
   SNDBANKI_asyncissuedownloads walks the ring of four reads from the next
   one to download and, for each completed read (state 2) while the
   download window allows, issues the download of the rest of its chunk to
   the bank's destination (state 3), or retires the read when nothing is
   left to download. The load and read-record views follow sbasync.c and are
   inferred from offsets (the two counters at +0x3c/+0x3e are shared with
   the header copy); SNDPLATFORM_download and sndbas are named by the
   symbols. The rest of the file is in other units. */
int SNDPLATFORM_download(int, void*, void*, int);

struct SndBankReadView {
    int op;
    int download;
    char* buffer;
    signed char state;
    unsigned char unknown0d[3];
};

struct SndBankLoadView {
    unsigned char unknown00[12];
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
    unsigned char unknown36[2];
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
