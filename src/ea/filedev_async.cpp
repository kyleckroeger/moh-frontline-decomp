// The asynchronous disc read path of EA's file device (filedev.cpp):
// StartNonAlignedAyncRead sets up a read of any alignment by recording the
// destination and file positions with their 32-byte aligned bounds, then
// queues the first DVD transfer: a short read (under 32 bytes) or a
// misaligned head goes through the aligned bounce buffer, an aligned
// destination is read in place up to its aligned end, and anything else is
// read in chunks of at most 16 KB through the bounce buffer. AyncDVDRead runs
// on the read thread after each completed transfer: it copies what the last
// transfer brought (the head, the in-place run, a chunk, or a whole short
// read, which ends the request), advances the positions and queues the next
// transfer the same way, or ends the request. Each transfer signals the read
// thread through AyncDVDCallback. ReadFileThreadFunc is the read thread: it
// waits for a request, reads a host file through MSL or runs the disc read
// to completion, waits until the drive is idle and reports completion. The
// request words written by the requesting thread are volatile. DVDFileInfo
// and HANDLE_def are named by the mangled symbols; their members, the read
// request record and its members are inferred (the destination fields are
// integers: the original compares them signed).

namespace std {
struct _FILE;
typedef _FILE FILE;
}
using std::FILE;

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
int OSReceiveMessage(OSMessageQueue*, OSMessage*, int);
int fseek(FILE*, long, int);
unsigned long fread(void*, unsigned long, unsigned long, FILE*);
long DVDGetDriveStatus();
void THREAD_yield(int);
int OSSendMessage(OSMessageQueue*, OSMessage, int);
int DVDReadAsyncPrio(DVDFileInfo*, void*, long, long, void (*)(long, DVDFileInfo*), long);
void DCInvalidateRange(void*, unsigned long);
void MEM_copy(void*, const void*, int);
}

/* inferred: an open file (type 3 host file through MSL, else a disc file);
   the type is volatile like the request words below (the read thread's
   loads of them keep their order in the original) */
struct HANDLE_def {
    volatile int type;
    char field04[260];
    union {
        FILE* file;
        DVDFileInfo dvd;
    };
};

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
    // written by the requesting thread
    volatile int requestOffset;
    volatile int requestSize;
    char field4c[20];
    char temp[32768];
};

extern OSMessageQueue ReadFileThreadMsgQ;
// File-local in the original; declared without static here because this
// fragment does not define it (the manifest lists it as a local external).
extern READREQUEST gCurRead;

static void AyncDVDCallback(long, DVDFileInfo*);
void iFILE_CommandCompleteCallback(int);

static void AyncDVDRead(DVDFileInfo* info) {
    long length;
    READREQUEST* read;

    if (gCurRead.state == 2) {
        MEM_copy((void*)gCurRead.dest, gCurRead.temp + gCurRead.misalign, gCurRead.headLength);
        gCurRead.offset += gCurRead.headLength;
        gCurRead.dest += gCurRead.headLength;
    } else if (gCurRead.state == 3) {
        DCInvalidateRange((void*)gCurRead.dest, gCurRead.destEndAligned - gCurRead.dest);
        length = gCurRead.destEndAligned - gCurRead.dest;
        gCurRead.offset += length;
        gCurRead.dest += length;
    } else if (gCurRead.state == 4) {
        length = 16384;
        if (gCurRead.destEnd - gCurRead.dest < 16384)
            length = gCurRead.destEnd - gCurRead.dest;
        MEM_copy((void*)gCurRead.dest, gCurRead.temp, length);
        gCurRead.offset += length;
        gCurRead.dest += length;
    } else if (gCurRead.state == 1) {
        MEM_copy((void*)gCurRead.dest, gCurRead.temp + gCurRead.misalign, gCurRead.size);
        gCurRead.state = 1;
        gCurRead.state = 5;
        AyncDVDCallback(0, 0);
        return;
    }
    // The in-place test reads the request through a local pointer (the chunk
    // test then reloads the destination, as in the original).
    read = &gCurRead;
    if (read->dest == read->destAlignedUp && read->dest < read->destEndAligned) {
        read->state = 3;
        gCurRead.queued = DVDReadAsyncPrio(info, (void*)read->dest, read->destEndAligned - read->dest,
                                           read->offset, AyncDVDCallback, 2);
    } else if (gCurRead.destEnd > read->dest) {
        length = 16384;
        if (((-read->dest + gCurRead.destEnd + 31) & ~31) < 16384)
            length = (-read->dest + gCurRead.destEnd + 31) & ~31;
        gCurRead.state = 4;
        gCurRead.queued += DVDReadAsyncPrio(info, gCurRead.temp, length, gCurRead.offset, AyncDVDCallback, 2);
    } else {
        gCurRead.state = 5;
        AyncDVDCallback(0, 0);
    }
}

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

static void* ReadFileThreadFunc(void*) {
    OSMessage message;

    for (;;) {
        HANDLE_def* handle;
        void* buffer;
        int offset;
        int size;
        long status;

        OSReceiveMessage(&ReadFileThreadMsgQ, &message, 1);
        handle = gCurRead.handle;
        buffer = gCurRead.buffer;
        offset = gCurRead.requestOffset;
        size = gCurRead.requestSize;
        if (handle->type == 3) {
            fseek(handle->file, offset, 0);
            fread(buffer, size, 1, handle->file);
        } else {
            StartNonAlignedAyncRead(&handle->dvd, buffer, offset, size);
            OSReceiveMessage(&ReadFileThreadMsgQ, &message, 1);
            while (gCurRead.state != 5) {
                AyncDVDRead(&handle->dvd);
                OSReceiveMessage(&ReadFileThreadMsgQ, &message, 1);
            }
        }
        for (;;) {
            status = DVDGetDriveStatus();
            if (status == 8 || (unsigned long)status <= 1)
                break;
            THREAD_yield(100);
        }
        iFILE_CommandCompleteCallback(1);
    }
}
