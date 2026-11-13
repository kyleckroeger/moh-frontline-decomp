// EA's file device (filedev.cpp). initfiledev starts the read thread (8 KB
// stack, priority 2) with its 32-message queue and killfiledev suspends it.
// openfile opens a host file through MSL (handle type 3) when the mode's bit
// 0 is clear or the name starts with "hd:\\", "hd:/" or "hd:" (compared
// case-insensitively), converting backslashes; otherwise it opens the disc
// file by path (type 2). MWIO_openfile accepts modes 0, 1 and 6 and aborts on
// the others (it is called, not inlined: the file is compiled without
// automatic inlining, and with signed char). closefile closes either kind and
// reports success.
// The asynchronous disc read path: StartNonAlignedAyncRead sets up a read of
// any alignment by recording the destination and file positions with their
// 32-byte aligned bounds, then queues the first DVD transfer: a short read
// (under 32 bytes) or a misaligned head goes through the aligned bounce
// buffer, an aligned destination is read in place up to its aligned end, and
// anything else is read in chunks of at most 16 KB through the bounce buffer.
// AyncDVDRead runs on the read thread after each completed transfer: it
// copies what the last transfer brought (the head, the in-place run, a chunk,
// or a whole short read, which ends the request), advances the positions and
// queues the next transfer the same way, or ends the request. Each transfer
// signals the read thread through AyncDVDCallback. ReadFileThreadFunc is the
// read thread: it waits for a request, reads a host file through MSL or runs
// the disc read to completion, waits until the drive is idle and reports
// completion.
// Last, the request side: readfile queues a read to the read thread, writes
// and size queries go through the MSL FILE functions for host files, and the
// cached size (the DVD file's length) is used for disc files.
// DVDFileInfo and HANDLE_def are named by the mangled symbols; their
// members, the read request record and its members are inferred (the
// destination fields are integers: the original compares them signed). The
// request words written by the requesting thread are volatile, and the read
// thread reads the handle's type through a volatile pointer.
namespace std {
struct _FILE;
typedef _FILE FILE;
}
using std::FILE;
struct OSThread {
    unsigned char unknown000[784];
};
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
int OSCreateThread(OSThread*, void* (*)(void*), void*, void*, unsigned long, long, unsigned short);
void OSInitMessageQueue(OSMessageQueue*, OSMessage*, long);
long OSResumeThread(OSThread*);
long OSSuspendThread(OSThread*);
int OSSendMessage(OSMessageQueue*, OSMessage, int);
FILE* fopen(const char*, const char*);
int fclose(FILE*);
int fseek(FILE*, long, int);
long ftell(FILE*);
unsigned long fwrite(const void*, unsigned long, unsigned long, FILE*);
void REAL_abortmessage(const char*, ...);
long DVDConvertPathToEntrynum(const char*);
int DVDFastOpen(long, DVDFileInfo*);
int DVDClose(DVDFileInfo*);
int DVDReadAsyncPrio(DVDFileInfo*, void*, long, long, void (*)(long, DVDFileInfo*), long);
long DVDGetDriveStatus();
void DCInvalidateRange(void*, unsigned long);
void MEM_copy(void*, const void*, int);
int OSReceiveMessage(OSMessageQueue*, OSMessage*, int);
unsigned long fread(void*, unsigned long, unsigned long, FILE*);
void THREAD_yield(int);
extern unsigned char __lower_map[];
}

static inline int tolower(int c) {
    return c == -1 ? -1 : (int)__lower_map[(unsigned char)c];
}

/* inferred: an open file (type 3 host file through MSL, else a disc file);
   the read thread loads the type as volatile, like the request words below
   (its loads keep their order in the original) */
struct HANDLE_def {
    int type;
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
    char temp[32768] __attribute__((aligned(32)));
};

static OSThread ReadFileThread;
static char ReadFileThreadStack[8192];
OSMessageQueue ReadFileThreadMsgQ;
OSMessage ReadFileThreadMsgData[32];
static READREQUEST gCurRead;

static void* ReadFileThreadFunc(void*);
void iFILE_CommandCompleteCallback(int);

void initfiledev() {
    OSCreateThread(&ReadFileThread, ReadFileThreadFunc, 0, ReadFileThreadStack + sizeof(ReadFileThreadStack),
                   sizeof(ReadFileThreadStack), 2, 1);
    OSInitMessageQueue(&ReadFileThreadMsgQ, ReadFileThreadMsgData, 32);
    OSResumeThread(&ReadFileThread);
}

void killfiledev() {
    OSSuspendThread(&ReadFileThread);
}

static int MWIO_openfile(const char* name, unsigned int mode, FILE** file) {
    switch (mode & 7) {
    default:
        REAL_abortmessage("openfile - ILLEGAL OPEN MODE (%d).\n", mode);
        *file = 0;
        return 0;
    case 0:
    case 1:
    case 6:
        *file = fopen(name, "a+");
        return *file != 0;
    }
}

static inline int prefixlength(const char* name, const char* prefix) {
    int j = 0;

    while (prefix[j]) {
        if (prefix[j] != tolower(name[j]))
            return 0;
        j++;
    }
    return j;
}

static inline void copypath(char* dst, const char* src) {
    int k;

    for (k = 0; src[k]; k++) {
        if (src[k] == '\\')
            dst[k] = '/';
        else
            dst[k] = src[k];
    }
    dst[k] = src[k];
}

int openfile(const char* name, unsigned int mode, HANDLE_def* handle) {
    int length = 0;
    const char* prefixes[3] = {"hd:\\", "hd:/", "hd:"};
    int host = (mode & 1) == 0;
    DVDFileInfo* info = &handle->dvd;
    char path[256];
    int result;
    int i;

    if (!host) {
        for (i = 0; i < 3; i++) {
            length = prefixlength(name, prefixes[i]);
            if (length) {
                host = 1;
                break;
            }
        }
    }
    if (host) {
        copypath(path, name + length);
        result = MWIO_openfile(path, mode, &handle->file);
        if (!result)
            fclose(handle->file);
        else
            handle->type = 3;
        return result;
    } else {
        long entry;
        int k;

        for (k = 0; name[k]; k++) {
            if (name[k] == '\\')
                path[k] = '/';
            else
                path[k] = name[k];
        }
        path[k] = name[k];
        entry = DVDConvertPathToEntrynum(path);
        if (entry == -1) {
            result = 0;
        } else {
            result = DVDFastOpen(entry, info);
            if (!result)
                DVDClose(&handle->dvd);
        }
        if (result)
            handle->type = 2;
        return result;
    }
}

int closefile(HANDLE_def* handle) {
    int result = 0;

    if (handle->type == 3)
        result = fclose(handle->file);
    else if (handle->type == 2)
        result = DVDClose(&handle->dvd);
    return result == 0;
}

static void AyncDVDCallback(long, DVDFileInfo*);

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
        if (*(volatile int*)&handle->type == 3) {
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

int readfile(HANDLE_def* handle, void* buffer, int offset, int size) {
    gCurRead.requestOffset = offset;
    gCurRead.handle = handle;
    gCurRead.buffer = buffer;
    gCurRead.requestSize = size;
    OSSendMessage(&ReadFileThreadMsgQ, 0, 1);
    return 1;
}

int writefile(HANDLE_def* handle, void* buffer, int offset, int size) {
    if (handle->type == 3) {
        fseek(handle->file, offset, 0);
        fwrite(buffer, size, 1, handle->file);
        return 1;
    }
    return -1;
}

int getfilesize(HANDLE_def* handle) {
    if (handle->type == 3) {
        long position = ftell(handle->file);
        fseek(handle->file, 0, 2);
        long size = ftell(handle->file);
        fseek(handle->file, position, 0);
        return size;
    }
    if (handle->type == 2)
        return handle->dvd.length;
    return 0;
}

void stopreadfile(HANDLE_def*) {
}

int deletefile(const char*) {
    return -1;
}
