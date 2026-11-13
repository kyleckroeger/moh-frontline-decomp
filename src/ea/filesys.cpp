// EA's file system, the functions at the start of the file: default memory
// callbacks that abort with a message, initialisation (installing the default
// callbacks when none are set, allocating the device's memory through the
// callback, or taking caller-supplied memory), the memory overhead for a
// number of handles and operations, and shutdown. The function and global names
// come from the symbols; the options and device layouts are inferred views,
// and the parameters' meanings (handles, buffer size, operations) are inferred
// from the defaults, the overhead formula and the 48-byte operation records.
// FILESYS_opstatus returns a valid operation's status (or -3), and
// FILESYS_callbackop installs an operation's completion callback, calling it
// at once (with the device's callback depth raised) when the operation has
// already finished. Operation handles carry a serial in the high half and the
// record slot in the low byte (an inferred union); the status is volatile
// (it is reloaded around the depth update) and the validity check is an
// inferred inline helper that takes the handle by value; so is opptr, which
// finds an operation's record from the handle's memory.
// Then the operation queue: FILESYS_priorityop re-sorts a queued operation by
// its new priority, FILESYS_cancelop flags the active operation (stopping a
// read) or unlinks a queued one and reports -1 to its callback,
// FILESYS_waitop runs or yields to the file thread until the operation has a
// status, and FILESYS_completeop returns the result for the operation's kind
// (the handle, the status, a position or count, or 1; a write extends the
// handle's size) and frees it. FILESYS_exists, FILESYS_open and FILESYS_close
// reserve an operation (and a handle, named by the request) and queue it;
// closing a handle that another record still refers to is reported.
// FILESYS_read (clamped to the handle's size) and FILESYS_size queue reads
// and size queries on an int handle. iFILE_addbigreadcallback finishes a big
// file's header read: it grows the header buffer to the size the header
// reports and reads the rest (FILESYS_read and FILESYS_callbackop are inlined
// here, FILESYS_completeop is not), or links the loaded file on the device
// and runs the pending add operation. iFILE_addbigopencallback starts that
// header read (up to 2048 bytes) once the big file is open, or fails the
// pending add operation with error 4 (its priority read goes straight to the
// operation table). FILESYS_addbig adds a big file: it reserves an add
// operation and runs it at once when a linked big file has the name,
// otherwise it allocates the big-file record and its 2048-byte header buffer
// and opens the file with iFILE_addbigopencallback (which, with
// FILESYS_callbackop, is inlined).
// FILEOPERATION_def and HANDLE_def are named by the mangled helper symbols;
// their members are inferred from offsets (the handle's name ends at +260,
// where its size is kept). This unit is built with GC 1.3: GC 1.3.2 inlines
// FILESYS_completeop into the callback, which the original calls.
//
// The end of the file: removing a big file, running a function atomically, the command queue and
// the record allocators. iFILE_delbigclosecallback completes the close and
// runs the pending remove operation. FILESYS_delbig queues a remove
// operation that fails (error 1, status -2) while an open handle still
// refers to the big file, and with error 1 when the file is not linked on
// the device; it unlinks and frees the file's header buffer and record and
// closes its handle, the close's callback running the remove. FILESYS_atomic
// calls a function with the device's third word set to the first argument,
// then runs the queue. iFILE_ExecCommand queues an operation by priority and,
// when the device is idle, runs the first one the priority limit allows:
// opening (a plain file, or an entry of a linked big file named
// "big|entry" or "|entry"), checking existence, closing, reading (through
// the big file's handle at the entry's offset), writing, querying the size
// or deleting. iFILE_CommandCompleteCallback sets the active operation's
// status, calls its callback and runs the queue again. reserveop (with a
// serial that skips 0), reservehandle, freeop and freehandle take records
// under the mutex and spin when none is free; iFILE_perror does nothing.
// FILESYS_close and FILESYS_callbackop are inlined into FILESYS_delbig.
// FileMutex, gFileDevice and the record helpers are file-static.
struct OSMutex {
    unsigned char unknown00[24];
};

extern "C" {
void OSInitMutex(OSMutex*);
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
bool THREAD_iscurrent(int);
void SYNCTASK_run(int);
void THREAD_yield(int);
char* strncpy(char*, const char*, unsigned long);
int strncmp(const char*, const char*, unsigned long);
int BIG_typeofheader(void*);
int BIG_sizeofheader(void*);
void MEM_copy(void*, const void*, int);
int FILESYS_completeop(int);
void MEM_fill(void*, int, int);
char* strchr(const char*, int);
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
void* BIG_locateentryz(void*, const char*, int, int*, int*);
void REAL_abortmessage(const char*, ...);
int FILESYS_overhead(int, int, int);
int FILESYS_initadr(int, int, int, void*);
}

struct BIGFILEVIEW;

struct HANDLE_def {
    int unknown000;
    unsigned char unknown004;
    char name[255];
    int unknown104;
    BIGFILEVIEW* big;
    int unknown10c;
    unsigned char unknown110[52];
};

struct FILEOPERATION_def;

void initfiledev();
void stopreadfile(HANDLE_def*);
static void freehandle(HANDLE_def*);
static void freeop(FILEOPERATION_def*);
void iFILE_addbigreadcallback(int, int, void*);
static FILEOPERATION_def* reserveop();
static HANDLE_def* reservehandle();
static void iFILE_perror(FILEOPERATION_def*);
static void iFILE_ExecCommand(FILEOPERATION_def*);
void iFILE_CommandCompleteCallback(int);
int openfile(const char*, unsigned int, HANDLE_def*);
int closefile(HANDLE_def*);
int getfilesize(HANDLE_def*);
void readfile(HANDLE_def*, void*, int, int);
void writefile(HANDLE_def*, void*, int, int);
void deletefile(const char*);
void killfiledev();

struct FILESYSOPTSVIEW {
    int unknown00;
    void* (*malloc)(const char*, int, int);
    int (*mfree)(void*);
};

/* inferred: an operation handle (serial in the high half, slot in the low
   byte) and the 48-byte operation record it names */
union FILEOPID {
    int value;
    struct {
        unsigned short serial;
        unsigned char unknown2;
        unsigned char slot;
    } parts;
};

typedef void (*FILEOPCALLBACK)(int, int, void*);

struct FILEOPERATION_def {
    FILEOPID id;
    int cancelled;
    volatile int status;
    int error;
    int priority;
    void* callbackData;
    int unknown18;
    int unknown1c;
    void* buffer;
    HANDLE_def* handle;
    FILEOPCALLBACK callback;
    FILEOPERATION_def* next;
};

/* inferred: a big file being added (its header buffer, the bytes read so
   far, the allocation flags, its handle and the pending add operation),
   linked on the device once loaded */
struct BIGFILEVIEW {
    void* buffer;
    int size;
    int memflags;
    HANDLE_def* handle;
    FILEOPERATION_def* op;
    BIGFILEVIEW* next;
};

struct FILEDEVICEVIEW {
    int operations;
    int handles;
    int unknown08;
    int unknown0c;
    int callbackDepth;
    FILEOPERATION_def* active;
    union {
        char* memory;
        FILEOPERATION_def* ops;
    };
    union {
        char* handleMemory;
        HANDLE_def* handleList;
    };
    FILEOPERATION_def* queue;
    BIGFILEVIEW* links;
};

extern FILESYSOPTSVIEW gFileSysOpts;

static OSMutex FileMutex;
static FILEDEVICEVIEW gFileDevice;

static void* DefaultFILE_malloc(const char*, int, int) {
    REAL_abortmessage("DefaultFILE_malloc - must set memory callbacks with FILESYS_setopts.\n");
    return 0;
}

static int DefaultFILE_mfree(void*) {
    REAL_abortmessage("DefaultFILE_mfree - must set memory callbacks with FILESYS_setopts.\n");
    return 0;
}

extern "C" int FILESYS_init(int handles, int bufferSize, int operations) {
    if (!gFileSysOpts.malloc) {
        gFileSysOpts.malloc = DefaultFILE_malloc;
        gFileSysOpts.mfree = DefaultFILE_mfree;
    }
    if (gFileDevice.operations == 0) {
        void* memory = gFileSysOpts.malloc("File Sys", FILESYS_overhead(handles, bufferSize, operations), 0);
        return FILESYS_initadr(handles, bufferSize, operations, memory);
    }
    return 0;
}

extern "C" int FILESYS_initadr(int handles, int bufferSize, int operations, void* memory) {
    if (!handles)
        handles = 24;
    if (!bufferSize)
        bufferSize = 2048;
    if (!operations)
        operations = 10;
    if (!gFileSysOpts.malloc) {
        gFileSysOpts.malloc = DefaultFILE_malloc;
        gFileSysOpts.mfree = DefaultFILE_mfree;
    }
    if (gFileDevice.operations == 0) {
        gFileDevice.operations = operations;
        gFileDevice.handles = handles;
        gFileDevice.unknown08 = 255;
        gFileDevice.memory = (char*)memory;
        OSInitMutex(&FileMutex);
        MEM_fill(gFileDevice.memory, 0, FILESYS_overhead(handles, bufferSize, operations));
        gFileDevice.handleMemory = gFileDevice.memory + gFileDevice.operations * 48;
        initfiledev();
        return 1;
    }
    return 0;
}

extern "C" int FILESYS_overhead(int handles, int, int operations) {
    if (!handles)
        handles = 24;
    if (!operations)
        operations = 10;
    return operations * 48 + handles * 324;
}

extern "C" void FILESYS_restore() {
    if (gFileDevice.operations) {
        killfiledev();
        gFileSysOpts.mfree(gFileDevice.memory);
        gFileDevice.operations = 0;
    }
}

static inline FILEOPERATION_def* getop(int op) {
    FILEOPID id;

    id.value = op;
    return &gFileDevice.ops[id.parts.slot];
}

static inline FILEOPERATION_def* opptr(FILEOPID* id) {
    return &gFileDevice.ops[id->parts.slot];
}

static inline bool validop(FILEOPID id) {
    bool valid = false;

    if (id.value && id.parts.serial == gFileDevice.ops[id.parts.slot].id.parts.serial)
        valid = true;
    return valid;
}

extern "C" int FILESYS_opstatus(int op) {
    if (validop(*(FILEOPID*)&op))
        return gFileDevice.ops[((FILEOPID*)&op)->parts.slot].status;
    return -3;
}

extern "C" void FILESYS_callbackop(int op, FILEOPCALLBACK callback) {
    FILEOPERATION_def* record = getop(op);

    record->callback = callback;
    if (record->status) {
        gFileDevice.callbackDepth++;
        callback(op, record->status, record->callbackData);
        gFileDevice.callbackDepth--;
    }
}

extern "C" void FILESYS_priorityop(int op, int priority) {
    FILEOPERATION_def* record = getop(op);
    FILEOPERATION_def* prev;
    FILEOPERATION_def* entry;
    int old;

    OSLockMutex(&FileMutex);
    old = record->priority;
    record->priority = priority;
    if (gFileDevice.unknown0c >= 2 && record != gFileDevice.active && record->status == 0 && old != priority) {
        prev = 0;
        for (entry = gFileDevice.queue; entry && entry != record; entry = entry->next)
            prev = entry;
        if (!entry) {
            OSUnlockMutex(&FileMutex);
            return;
        }
        if (prev)
            prev->next = record->next;
        else
            gFileDevice.queue = record->next;
        record->next = gFileDevice.queue;
        prev = 0;
        while (record->next && record->next->priority <= record->priority) {
            prev = record->next;
            record->next = record->next->next;
        }
        if (prev)
            prev->next = record;
        else
            gFileDevice.queue = record;
    }
    OSUnlockMutex(&FileMutex);
}

extern "C" void FILESYS_cancelop(int op) {
    int cancelled = 0;

    OSLockMutex(&FileMutex);
    if (validop(*(FILEOPID*)&op) && ((FILEOPID*)&op)->parts.unknown2 != 3 && ((FILEOPID*)&op)->parts.unknown2 != 10) {
        FILEOPERATION_def* record = opptr((FILEOPID*)&op);
        FILEOPERATION_def* prev;
        FILEOPERATION_def* entry;

        if (gFileDevice.active == record) {
            record->cancelled = 1;
            cancelled = 1;
        } else {
            if (record->status != 1) {
                prev = 0;
                for (entry = gFileDevice.queue; entry && entry != record; entry = entry->next)
                    prev = entry;
                if (!entry) {
                    OSUnlockMutex(&FileMutex);
                    return;
                }
                if (prev)
                    prev->next = record->next;
                else
                    gFileDevice.queue = record->next;
                cancelled = 2;
                gFileDevice.unknown0c--;
            }
            record->status = -1;
        }
        if (cancelled == 1 && record->id.parts.unknown2 == 4)
            stopreadfile(record->handle);
        else if (cancelled == 2 && record->callback)
            record->callback(record->id.value, -1, record->callbackData);
    }
    OSUnlockMutex(&FileMutex);
}

extern "C" int FILESYS_waitop(int op) {
    FILEOPERATION_def* record = opptr((FILEOPID*)&op);

    if (!validop(*(FILEOPID*)&op))
        return -3;
    do {
        if (THREAD_iscurrent(0))
            SYNCTASK_run(0);
        else
            THREAD_yield(10);
        if (!validop(*(FILEOPID*)&op))
            return -3;
    } while (record->status == 0);
    return record->status;
}

extern "C" int FILESYS_completeop(int op) {
    FILEOPERATION_def* record = getop(op);
    int result = 0;

    switch (record->status) {
    case 1:
        switch (record->id.parts.unknown2) {
        case 2:
        case 9:
            result = (int)record->handle;
            break;
        case 3:
        case 7:
        case 10:
            result = record->status;
            break;
        case 6:
        case 8:
            result = record->unknown18;
            break;
        case 4:
            result = record->unknown1c;
            break;
        case 5: {
            int end;

            result = record->unknown1c;
            end = record->unknown1c + record->unknown18;
            if (end > record->handle->unknown104)
                record->handle->unknown104 = end;
            break;
        }
        case 11:
            result = 1;
            break;
        }
        break;
    case -2:
    case -1:
        if (record->id.parts.unknown2 == 2 || record->id.parts.unknown2 == 9)
            freehandle(record->handle);
        break;
    }
    freeop(record);
    return result;
}

extern "C" int FILESYS_exists(const char* name, int priority, void* callbackData) {
    FILEOPERATION_def* op = reserveop();

    op->id.parts.unknown2 = 8;
    op->callbackData = callbackData;
    op->unknown18 = 1;
    op->priority = priority;
    if (!(op->handle = reservehandle())) {
        op->error = 2;
        iFILE_perror(op);
    }
    strncpy(op->handle->name, name, 255);
    iFILE_ExecCommand(op);
    return op->id.value;
}

extern "C" int FILESYS_open(const char* name, int mode, int priority, void* callbackData) {
    FILEOPERATION_def* op = reserveop();

    op->id.parts.unknown2 = 2;
    op->callbackData = callbackData;
    op->unknown18 = mode;
    op->priority = priority;
    if (!(op->handle = reservehandle())) {
        op->error = 2;
        iFILE_perror(op);
    }
    strncpy(op->handle->name, name, 255);
    iFILE_ExecCommand(op);
    return op->id.value;
}

extern "C" int FILESYS_close(HANDLE_def* handle, int priority, void* callbackData) {
    FILEOPERATION_def* op = reserveop();
    BIGFILEVIEW* link = gFileDevice.links;

    op->id.parts.unknown2 = 3;
    op->priority = priority;
    op->callbackData = callbackData;
    op->handle = handle;
    for (; link; link = link->next) {
        if (link->handle == handle) {
            op->error = 3;
            iFILE_perror(op);
            break;
        }
    }
    iFILE_ExecCommand(op);
    return op->id.value;
}

extern "C" int FILESYS_read(int handle, int position, void* buffer, int count, int priority, void* callbackData) {
    FILEOPERATION_def* op = reserveop();

    op->id.parts.unknown2 = 4;
    op->callbackData = callbackData;
    op->priority = priority;
    if (!handle) {
        op->error = 6;
        iFILE_perror(op);
    }
    op->handle = (HANDLE_def*)handle;
    if (position + count > op->handle->unknown104)
        count = op->handle->unknown104 - position;
    op->unknown1c = count;
    op->buffer = buffer;
    op->unknown18 = position;
    iFILE_ExecCommand(op);
    return op->id.value;
}

extern "C" int FILESYS_size(int handle, int priority, void* callbackData) {
    FILEOPERATION_def* op = reserveop();

    op->id.parts.unknown2 = 6;
    op->callbackData = callbackData;
    op->priority = priority;
    if (!handle) {
        op->error = 6;
        iFILE_perror(op);
    }
    op->handle = (HANDLE_def*)handle;
    iFILE_ExecCommand(op);
    return op->id.value;
}

void iFILE_addbigreadcallback(int op, int, void* data) {
    BIGFILEVIEW* big = (BIGFILEVIEW*)data;
    FILEOPERATION_def* record = opptr((FILEOPID*)&op);
    int priority = record->priority;
    HANDLE_def* handle = record->handle;
    int size;

    big->handle = handle;
    big->op->handle = handle;
    FILESYS_completeop(op);
    if (!BIG_typeofheader(big->buffer))
        gFileSysOpts.mfree(big->buffer);
    size = BIG_sizeofheader(big->buffer);
    if (size > big->size) {
        void* temp = gFileSysOpts.malloc("tmp bigfile buf", size, big->memflags ^ 256);
        void* buffer;

        MEM_copy(temp, big->buffer, big->size);
        gFileSysOpts.mfree(big->buffer);
        buffer = gFileSysOpts.malloc("bigfile buf", size, big->memflags);
        MEM_copy(buffer, temp, big->size);
        gFileSysOpts.mfree(temp);
        big->buffer = buffer;
        op = FILESYS_read((int)big->handle, big->size, (char*)big->buffer + big->size, size - big->size,
                          priority, big);
        FILESYS_callbackop(op, iFILE_addbigreadcallback);
        big->size = size;
    } else {
        big->next = gFileDevice.links;
        gFileDevice.links = big;
        iFILE_ExecCommand(big->op);
    }
}

void iFILE_addbigopencallback(int op, int status, void* data) {
    BIGFILEVIEW* big = (BIGFILEVIEW*)data;
    int priority = gFileDevice.ops[((FILEOPID*)&op)->parts.slot].priority;
    int handle = FILESYS_completeop(op);

    if (status != 1) {
        big->op->error = 4;
        iFILE_ExecCommand(big->op);
    } else {
        op = FILESYS_read(handle, 0, big->buffer, 2048, priority, big);
        FILESYS_callbackop(op, iFILE_addbigreadcallback);
    }
}

extern "C" int FILESYS_addbig(const char* name, int memflags, int priority, void* callbackData) {
    BIGFILEVIEW* link = gFileDevice.links;
    FILEOPERATION_def* op = reserveop();
    BIGFILEVIEW* big;

    op->id.parts.unknown2 = 9;
    op->callbackData = callbackData;
    op->priority = priority;
    for (; link; link = link->next) {
        if (!strncmp(name, link->handle->name, 255)) {
            op->handle = link->handle;
            iFILE_ExecCommand(op);
            return (int)op;
        }
    }
    big = (BIGFILEVIEW*)gFileSysOpts.malloc(name, sizeof(BIGFILEVIEW), memflags);
    big->op = op;
    big->size = 2048;
    big->buffer = gFileSysOpts.malloc("bigfile buf", 2048, memflags);
    big->memflags = memflags;
    FILESYS_callbackop(FILESYS_open(name, 1, priority, big), iFILE_addbigopencallback);
    return op->id.value;
}

void iFILE_delbigclosecallback(int op, int, void* data) {
    FILESYS_completeop(op);
    iFILE_ExecCommand((FILEOPERATION_def*)data);
}

extern "C" int FILESYS_delbig(HANDLE_def* handle, int priority, void* callbackData) {
    BIGFILEVIEW* prev;
    BIGFILEVIEW* link = gFileDevice.links;
    FILEOPERATION_def* op;
    HANDLE_def* other;
    int i;

    prev = 0;
    op = reserveop();
    other = gFileDevice.handleList;
    op->id.parts.unknown2 = 10;
    op->callbackData = callbackData;
    op->priority = priority;
    if (handle->unknown000 == 1)
        op->error = 1;
    for (i = gFileDevice.handles; i > 0; i--, other++) {
        if (other && other->unknown000 == 1 && other->big->handle == handle) {
            op->status = -2;
            op->error = 1;
            break;
        }
    }
    for (; link && link->handle != handle; link = link->next)
        prev = link;
    if (!link)
        op->error = 1;
    if (prev)
        prev->next = link->next;
    else
        gFileDevice.links = gFileDevice.links->next;
    gFileSysOpts.mfree(link->buffer);
    gFileSysOpts.mfree(link);
    FILESYS_callbackop(FILESYS_close(handle, priority, op), iFILE_delbigclosecallback);
    return op->id.value;
}

extern "C" int FILESYS_atomic(int (*function)(int, int), int, int a, int b) {
    int result;
    int saved = gFileDevice.unknown08;

    gFileDevice.unknown08 = a;
    result = function(a, b);
    gFileDevice.unknown08 = saved;
    iFILE_ExecCommand(0);
    return result;
}

static void iFILE_ExecCommand(FILEOPERATION_def* op) {
    char bigname[256];
    char path[256];
    int size;
    int offset;

    OSLockMutex(&FileMutex);
    if (op) {
        FILEOPERATION_def* prev = 0;

        op->next = gFileDevice.queue;
        while (op->next && op->next->priority <= op->priority) {
            prev = op->next;
            op->next = op->next->next;
        }
        if (prev)
            prev->next = op;
        else
            gFileDevice.queue = op;
        gFileDevice.unknown0c++;
    }
    if (gFileDevice.active) {
        OSUnlockMutex(&FileMutex);
        return;
    }
    op = gFileDevice.queue;
    if (op && op->priority <= gFileDevice.unknown08) {
        gFileDevice.queue = op->next;
        gFileDevice.unknown0c--;
    } else {
        op = 0;
    }
    gFileDevice.active = op;
    OSUnlockMutex(&FileMutex);
    if (op && !op->cancelled) {
        switch (op->id.parts.unknown2) {
        case 2:
        case 8: {
            int flags;
            int length;
            int done;
            BIGFILEVIEW* big;

            path[0] = 0;
            if (strchr(op->handle->name, '|')) {
                if (op->handle->name[0] == '|') {
                    flags = 2;
                } else {
                    length = strchr(op->handle->name, '|') - op->handle->name;
                    bigname[0] = 0;
                    strncpy(bigname, op->handle->name, length);
                    bigname[length] = 0;
                    flags = 4;
                }
                strcpy(path, strchr(op->handle->name, '|') + 1);
            } else {
                flags = 1;
                if (op->unknown18 & 1) {
                    flags |= 2;
                    strcpy(path, op->handle->name);
                }
            }
            done = 0;
            if (flags & 1) {
                if (openfile(op->handle->name, op->unknown18, op->handle)) {
                    done = 1;
                    if (op->id.parts.unknown2 == 8)
                        closefile(op->handle);
                    else
                        op->handle->unknown104 = getfilesize(op->handle);
                }
            }
            if (!done && (flags & 6)) {
                flags &= 4;
                big = gFileDevice.links;
                while (big && !done) {
                    if (flags && strcmp(big->handle->name, bigname)) {
                        big = big->next;
                    } else {
                        if (BIG_locateentryz(big->buffer, path, 0, &offset, &size)) {
                            done = 1;
                            op->handle->big = big;
                            op->handle->unknown104 = size;
                            op->handle->unknown10c = offset;
                            op->handle->unknown000 = 1;
                        }
                        big = big->next;
                    }
                }
            }
            if (op->id.parts.unknown2 == 8) {
                freehandle(op->handle);
                op->unknown18 = done;
            }
            iFILE_CommandCompleteCallback(done);
            break;
        }
        case 3:
            op->error = 1;
            if (op->handle) {
                if (op->handle->unknown000 != 1)
                    op->error = closefile(op->handle);
                freehandle(op->handle);
                op->handle = 0;
            }
            iFILE_CommandCompleteCallback(op->error == 0);
            break;
        case 4:
            if (op->unknown1c > 0) {
                if (op->handle->unknown000 == 1)
                    readfile(op->handle->big->handle, op->buffer, op->handle->unknown10c + op->unknown18, op->unknown1c);
                else
                    readfile(op->handle, op->buffer, op->unknown18, op->unknown1c);
            } else {
                iFILE_CommandCompleteCallback(1);
            }
            break;
        case 5:
            writefile(op->handle, op->buffer, op->unknown18, op->unknown1c);
            iFILE_CommandCompleteCallback(1);
            break;
        case 6:
            op->unknown18 = op->handle->unknown104;
            iFILE_CommandCompleteCallback(1);
            break;
        case 7:
        case 9:
        case 10:
            iFILE_CommandCompleteCallback(op->error == 0);
            break;
        case 11:
            deletefile(op->handle->name);
            iFILE_CommandCompleteCallback(1);
            break;
        }
    }
}

void iFILE_CommandCompleteCallback(int ok) {
    FILEOPERATION_def* op = gFileDevice.active;

    if (op) {
        if (op->cancelled)
            op->status = -1;
        else
            op->status = ok ? 1 : -2;
        gFileDevice.active = 0;
        if (op->callback) {
            gFileDevice.callbackDepth++;
            op->callback(op->id.value, op->status, op->callbackData);
            gFileDevice.callbackDepth--;
        }
        if (gFileDevice.callbackDepth == 0)
            iFILE_ExecCommand(0);
    }
}

static void iFILE_perror(FILEOPERATION_def*) {
}

static FILEOPERATION_def* reserveop() {
    static unsigned short opcount;
    static char init;
    int i;

    if (!init) {
        opcount = 0;
        init = 1;
    }
    OSLockMutex(&FileMutex);
    for (i = 0; i < gFileDevice.operations; i++) {
        if (gFileDevice.ops[i].id.parts.unknown2 == 0) {
            gFileDevice.ops[i].id.parts.unknown2 = 1;
            gFileDevice.ops[i].id.parts.slot = i;
            if (++opcount == 0)
                opcount = 1;
            gFileDevice.ops[i].id.parts.serial = opcount;
            break;
        }
    }
    OSUnlockMutex(&FileMutex);
    if (i == gFileDevice.operations)
        for (;;)
            ;
    return &gFileDevice.ops[i];
}

static void freeop(FILEOPERATION_def* op) {
    OSLockMutex(&FileMutex);
    MEM_fill(op, 0, sizeof(FILEOPERATION_def));
    OSUnlockMutex(&FileMutex);
}

static HANDLE_def* reservehandle() {
    int i;

    OSLockMutex(&FileMutex);
    for (i = 0; i < gFileDevice.handles; i++) {
        if (gFileDevice.handleList[i].unknown004 == 0) {
            gFileDevice.handleList[i].unknown004 = 1;
            break;
        }
    }
    OSUnlockMutex(&FileMutex);
    if (i == gFileDevice.handles)
        for (;;)
            ;
    return &gFileDevice.handleList[i];
}

static void freehandle(HANDLE_def* handle) {
    OSLockMutex(&FileMutex);
    MEM_fill(handle, 0, sizeof(HANDLE_def));
    OSUnlockMutex(&FileMutex);
}

extern "C" void FILESYS_setmemcallbacks(void* (*malloc)(const char*, int, int), int (*mfree)(void*)) {
    gFileSysOpts.malloc = malloc;
    gFileSysOpts.mfree = mfree;
}
