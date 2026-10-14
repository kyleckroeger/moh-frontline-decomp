// EA's asynchronous file layer, the whole file: the completion callbacks of
// the file-system operations (a whole-file load opens, sizes and allocates a
// buffer when asked, reads in 32 KB steps and closes; a read reads in 32 KB
// steps; a finished request reports to its callback or is freed when it was
// cancelled before being released), initialising (allocating the
// request table, chaining its requests into the free list and creating the
// mutex), shutting down (cancel every request, wait until none has a file
// operation running, free the table and destroy the mutex), queueing a
// whole-file load or a read (take a request from the free list, give it a
// fresh id, start the file-system operation and its completion callback),
// reporting whether a request is still busy, releasing a finished request
// (waiting for its operation, reporting its byte counts) and cancelling one
// (cancelling its operation, freeing it when it never started). The function
// and static names come from the symbols; the 48-byte request layout, the
// options view and the inline helpers are inferred; the operation handle is
// volatile (completion callbacks clear it, and the target reloads it). The
// file's small statics and mutex are defined here in their original order.
// Compiled at GC 1.3: only it matches ASYNCFILE_init's free-list stores
// (1.3.2 schedules them differently); a working profile, not proof of the
// original release.
extern "C" {
int FILESYS_waitop(int);
int FILESYS_cancelop(int);
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
void MUTEX_destroy(void*);
void MUTEX_create(void*);
bool THREAD_iscurrent(int);
void SYNCTASK_run(int);
void THREAD_yield(int);
int FILESYS_open(const char*, int, int, void*);
int FILESYS_read(int, void*, int, int, int, void*);
void FILESYS_callbackop(int, void (*)(int, int, void*));
int ASYNCFILE_cancel(int);
int FILESYS_completeop(int);
int FILESYS_close(int, int, void*);
int FILESYS_size(int, int, void*);
}


struct FILESYSOPTSVIEW {
    int unknown00;
    void* (*malloc)(const char*, int, int);
    int (*mfree)(void*);
};

extern FILESYSOPTSVIEW gFileSysOpts;

struct ASYNCREQUESTVIEW {
    int id;
    ASYNCREQUESTVIEW* next;
    int done;
    int released;
    int cancelled;
    void* allocated;
    int unknown18;
    volatile int op;
    int file;
    void* buffer;
    int size;
    int position;
};

static int numrequests;
static ASYNCREQUESTVIEW* request;
static ASYNCREQUESTVIEW* freequeue[2];
static int requestidcounter;
static unsigned char mutex[28];

static inline ASYNCREQUESTVIEW* allocrequest() {
    ASYNCREQUESTVIEW* r;
    MUTEX_lock(mutex);
    if (freequeue[0] == 0) {
        r = 0;
    } else {
        r = freequeue[0];
        freequeue[0] = r->next;
    }
    MUTEX_unlock(mutex);
    return r;
}

static inline void newrequestid(ASYNCREQUESTVIEW* r) {
    requestidcounter += 256;
    if (requestidcounter == 0)
        requestidcounter = 256;
    r->id = (r->id & 0xFF) | requestidcounter;
}

static inline void freerequest(ASYNCREQUESTVIEW* r) {
    if (r->cancelled && (unsigned int)r->allocated > 1)
        gFileSysOpts.mfree(r->allocated);
    r->id &= 0xFF;
    r->op = 0;
    MUTEX_lock(mutex);
    if (freequeue[0] == 0)
        freequeue[0] = r;
    else
        freequeue[1]->next = r;
    freequeue[1] = r;
    r->next = 0;
    MUTEX_unlock(mutex);
}

/* Inferred: a finished request either reports to its callback or, when it
   was cancelled before being released, goes back to the free list. */
static inline void completerequest(ASYNCREQUESTVIEW* r) {
    void (*callback)(int);
    int cancelled;
    int released;
    MUTEX_lock(mutex);
    callback = (void (*)(int))r->unknown18;
    cancelled = r->cancelled;
    released = r->released;
    r->op = 0;
    MUTEX_unlock(mutex);
    if (cancelled) {
        if (!released)
            freerequest(r);
    } else if (callback) {
        callback(r->id);
    }
}

static void loadfileclosecallback(int, int, void* data) {
    ASYNCREQUESTVIEW* r = (ASYNCREQUESTVIEW*)data;
    FILESYS_completeop(r->op);
    completerequest(r);
}

static void loadfilereadcallback(int, int, void* data) {
    ASYNCREQUESTVIEW* r = (ASYNCREQUESTVIEW*)data;
    int n = FILESYS_completeop(r->op);
    r->done += n;
    r->position += n;
    if (n < 0x8000 || r->cancelled) {
        r->op = FILESYS_close(r->file, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, loadfileclosecallback);
    } else {
        r->buffer = (char*)r->buffer + n;
        r->size -= n;
        n = r->size;
        if (n > 0x8000)
            n = 0x8000;
        r->op = FILESYS_read(r->file, r->buffer, r->position, n, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, loadfilereadcallback);
    }
}

static void loadfilesizecallback(int, int, void* data) {
    ASYNCREQUESTVIEW* r = (ASYNCREQUESTVIEW*)data;
    int size = FILESYS_completeop(r->op);
    if (r->cancelled) {
        r->op = FILESYS_close(r->file, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, loadfileclosecallback);
    } else {
        int flags = r->size;
        r->size = 0x7fffffff;
        r->allocated = gFileSysOpts.malloc("ASYNCFILEBUF", size, flags);
        r->position = (int)r->allocated;
        r->op = FILESYS_read(r->file, r->buffer, r->position, 0x8000, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, loadfilereadcallback);
    }
}

static void loadfileopencallback(int, int, void* data) {
    ASYNCREQUESTVIEW* r = (ASYNCREQUESTVIEW*)data;
    r->file = FILESYS_completeop(r->op);
    if (!r->file) {
        completerequest(r);
    } else if (r->cancelled) {
        r->op = FILESYS_close(r->file, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, loadfileclosecallback);
    } else if (!r->allocated) {
        int n = r->size;
        if (n > 0x8000)
            n = 0x8000;
        r->op = FILESYS_read(r->file, r->buffer, r->position, n, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, loadfilereadcallback);
    } else {
        r->op = FILESYS_size(r->file, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, loadfilesizecallback);
    }
}

static void readfilereadcallback(int, int, void* data) {
    ASYNCREQUESTVIEW* r = (ASYNCREQUESTVIEW*)data;
    int n = FILESYS_completeop(r->op);
    r->done += n;
    r->position += n;
    if (n < 0x8000 || r->cancelled) {
        completerequest(r);
    } else {
        r->buffer = (char*)r->buffer + n;
        r->size -= n;
        n = r->size;
        if (n > 0x8000)
            n = 0x8000;
        r->op = FILESYS_read(r->file, r->buffer, r->position, n, 99, r);
        if (r->op)
            FILESYS_callbackop(r->op, readfilereadcallback);
    }
}

extern "C" void ASYNCFILE_init(int count, int flags) {
    int i;
    if (request == 0 && count <= 256) {
        numrequests = count;
        request = (ASYNCREQUESTVIEW*)gFileSysOpts.malloc("ASYNCFILE", count * sizeof(ASYNCREQUESTVIEW), flags);
        freequeue[0] = request;
        freequeue[1] = request + (count - 1);
        MUTEX_create(mutex);
        for (i = 0; i < count; i++) {
            ASYNCREQUESTVIEW* r = &request[i];
            r->id = i;
            r->op = 0;
            r->next = &request[i + 1];
        }
        request[count - 1].next = 0;
    }
}

extern "C" void ASYNCFILE_restore() {
    int busy;
    int i;
    if (request) {
        for (i = 0; i < numrequests; i++)
            ASYNCFILE_cancel(request[i].id);
        do {
            busy = 0;
            for (i = 0; i < numrequests; i++) {
                if (request[i].op != 0)
                    busy = 1;
            }
            if (busy) {
                if (THREAD_iscurrent(0))
                    SYNCTASK_run(0);
                THREAD_yield(0);
            }
        } while (busy);
        gFileSysOpts.mfree(request);
        MUTEX_destroy(mutex);
        request = 0;
    }
}

extern "C" int ASYNCFILE_load(const char* name, int size) {
    ASYNCREQUESTVIEW* r = allocrequest();
    if (r == 0)
        return 0;
    newrequestid(r);
    r->done = 0;
    r->released = 0;
    r->cancelled = 0;
    r->allocated = (void*)1;
    r->unknown18 = 0;
    r->buffer = 0;
    r->size = size;
    r->position = 0;
    r->op = FILESYS_open(name, 1, 100, r);
    if (r->op == 0)
        return 0;
    FILESYS_callbackop(r->op, loadfileopencallback);
    return r->id;
}

extern "C" int ASYNCFILE_read(int file, void* buffer, int position, int size) {
    ASYNCREQUESTVIEW* r = allocrequest();
    if (r == 0)
        return 0;
    newrequestid(r);
    r->done = 0;
    r->released = 0;
    r->cancelled = 0;
    r->allocated = 0;
    r->unknown18 = 0;
    r->file = file;
    r->buffer = buffer;
    r->size = size;
    r->position = position;
    if (size > 0x8000)
        size = 0x8000;
    r->op = FILESYS_read(file, buffer, position, size, 100, r);
    if (r->op == 0)
        return 0;
    FILESYS_callbackop(r->op, readfilereadcallback);
    return r->id;
}

static inline ASYNCREQUESTVIEW* findrequest(int id) {
    int index = id & 0xFF;
    ASYNCREQUESTVIEW* r;
    if (id < 256 || index >= numrequests)
        return 0;
    r = &request[index];
    if (r->id != id)
        return 0;
    return r;
}

extern "C" int ASYNCFILE_getstatus(int id) {
    ASYNCREQUESTVIEW* r;
    int status;
    MUTEX_lock(mutex);
    r = findrequest(id);
    if (r == 0)
        status = -1;
    else
        status = r->op == 0;
    MUTEX_unlock(mutex);
    return status;
}

extern "C" int ASYNCFILE_release(int id, int* remaining, int* done) {
    ASYNCREQUESTVIEW* r;
    int cancelled = 0;
    MUTEX_lock(mutex);
    r = findrequest(id);
    if (r) {
        cancelled = r->cancelled;
        if (!cancelled)
            r->released = 1;
    }
    MUTEX_unlock(mutex);
    if (r == 0 || cancelled)
        return -1;
    while (r->op != 0)
        FILESYS_waitop(r->op);
    if (r->cancelled) {
        if (remaining)
            *remaining = 0;
        if (done)
            *done = 0;
        freerequest(r);
        return -1;
    }
    if (remaining)
        *remaining = r->position - r->done;
    if (done)
        *done = r->done;
    freerequest(r);
    return 1;
}

extern "C" int ASYNCFILE_cancel(int id) {
    ASYNCREQUESTVIEW* r;
    int op = 0;
    int cancelled = 0;
    int released = 0;
    MUTEX_lock(mutex);
    r = findrequest(id);
    if (r) {
        op = r->op;
        cancelled = r->cancelled;
        released = r->released;
        if (op != 0 || released == 0)
            r->cancelled = 1;
    }
    MUTEX_unlock(mutex);
    if (r == 0 || cancelled != 0 || r->cancelled == 0)
        return -1;
    if (op != 0)
        FILESYS_cancelop(op);
    if (op == 0 && released == 0)
        freerequest(r);
    return 1;
}
