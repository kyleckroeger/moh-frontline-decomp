// The start of the asynchronous disc read path of EA's file device
// (filedev.cpp): StartNonAlignedAyncRead sets up a read of any alignment by
// recording the destination and file positions with their 32-byte aligned
// bounds, then queues the first DVD transfer: a short read (under 32 bytes)
// or a misaligned head goes through the aligned bounce buffer, an aligned
// destination is read in place up to its aligned end, and anything else is
// read in chunks of at most 16 KB through the bounce buffer. Each completed
// transfer signals the read thread through AyncDVDCallback. AyncDVDRead
// (before this unit) and the read thread (after it) are not part of this
// unit. DVDFileInfo is named by the mangled symbols; the read request record
// and its members are inferred (the destination fields are integers: the
// original compares them signed).

struct OSMessageQueue {
    unsigned char unknown00[32];
};
typedef void* OSMessage;

struct DVDFileInfo {
    unsigned char commandBlock[48];
    unsigned long startAddr;
    unsigned long length;
    void* callback;
};

extern "C" {
int OSSendMessage(OSMessageQueue*, OSMessage, int);
int DVDReadAsyncPrio(DVDFileInfo*, void*, long, long, void (*)(long, DVDFileInfo*), long);
}

struct HANDLE_def;

/* inferred: the read in progress (state, destination and file positions,
   their 32-byte aligned bounds, the bytes queued, the request) and the
   aligned bounce buffer */
struct READREQUEST {
    int state;
    int dest;
    int offset;
    int size;
    int destAlignedDown;
    int destAlignedUp;
    int destEnd;
    int destEndAligned;
    int offsetAligned;
    int offsetAlignedUp;
    int offsetEnd;
    int offsetEndAligned;
    int misalign;
    int headLength;
    int queued;
    HANDLE_def* handle;
    void* buffer;
    int requestOffset;
    int requestSize;
    char field4c[20];
    char temp[32768];
};

extern OSMessageQueue ReadFileThreadMsgQ;
// File-local in the original; declared without static here because this
// fragment does not define it (the manifest lists it as a local external).
extern READREQUEST gCurRead;

static void AyncDVDCallback(long, DVDFileInfo*);

static int StartNonAlignedAyncRead(DVDFileInfo* info, void* buffer, long offset, long size) {
    if (size == 0) {
        gCurRead.state = 5;
        AyncDVDCallback(0, 0);
        return 0;
    }
    gCurRead.size = size;
    gCurRead.offset = offset;
    gCurRead.dest = (int)buffer;
    gCurRead.destAlignedDown = gCurRead.dest & ~31;
    gCurRead.destAlignedUp = (gCurRead.dest + 31) & ~31;
    gCurRead.destEnd = gCurRead.dest + gCurRead.size;
    gCurRead.destEndAligned = gCurRead.destEnd & ~31;
    gCurRead.offsetAligned = gCurRead.offset & ~31;
    gCurRead.offsetAlignedUp = (gCurRead.offset + 31) & ~31;
    gCurRead.offsetEnd = gCurRead.offset + gCurRead.size;
    gCurRead.offsetEndAligned = gCurRead.offsetEnd & ~31;
    gCurRead.queued = 0;
    gCurRead.misalign = offset - gCurRead.offsetAligned;
    gCurRead.headLength = 32 - gCurRead.misalign;
    if (size < 32) {
        gCurRead.state = 1;
        gCurRead.queued = DVDReadAsyncPrio(info, gCurRead.temp, (gCurRead.offsetEnd - gCurRead.offsetAligned + 31) & ~31,
                                           gCurRead.offsetAligned, AyncDVDCallback, 2);
    } else if (gCurRead.misalign != 0) {
        gCurRead.state = 2;
        gCurRead.queued += DVDReadAsyncPrio(info, gCurRead.temp, (gCurRead.headLength + 31) & ~31,
                                            gCurRead.offsetAligned, AyncDVDCallback, 2);
    } else if ((int)buffer == gCurRead.destAlignedUp && (int)buffer < gCurRead.destEndAligned) {
        gCurRead.state = 3;
        gCurRead.queued = DVDReadAsyncPrio(info, buffer, gCurRead.destEndAligned - (int)buffer, offset,
                                           AyncDVDCallback, 2);
    } else if (gCurRead.destEnd > gCurRead.dest) {
        int length = 16384;

        if (((-gCurRead.dest + gCurRead.destEnd + 31) & ~31) < 16384)
            length = (-gCurRead.dest + gCurRead.destEnd + 31) & ~31;
        gCurRead.state = 4;
        gCurRead.queued += DVDReadAsyncPrio(info, gCurRead.temp, length, gCurRead.offset, AyncDVDCallback, 2);
    }
    return gCurRead.size;
}

static void AyncDVDCallback(long, DVDFileInfo*) {
    OSSendMessage(&ReadFileThreadMsgQ, (OSMessage)1, 1);
}

