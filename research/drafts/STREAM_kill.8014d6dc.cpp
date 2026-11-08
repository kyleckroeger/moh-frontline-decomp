// A fragment of EA's stream.cpp (0x8014cde0): STREAM_setgreedylevel (stores
// the level and switches the greedy state when the buffered count's side of
// the level changes), STREAM_setgreedystate (stores the greedy state and,
// when it is set while the stream is reading, raises the file operation's
// priority), STREAM_taphandle (the 16-byte tap entry for a 1-based tap
// number, or null), and STREAM_queuefile and STREAM_queuemem, which take a
// request from the free list (giving it a new id in the upper bits of its
// index), fill it in (a file name, or a memory block whose size is found by
// walking its chunks to the end chunk when it is not given), append it to
// the queue, and start an idle stream; they return the request handle.
// STREAM_cancelrequest frees a request still queued, or marks a running one
// cancelled (state 4) and then, tap by tap, frees its chunks (when the tap's
// first chunk lies before the request's data, by walking the request's
// chunks and freeing those of the tap's type, returning their bytes to the
// buffered count; when it lies inside, by getting and releasing chunks until
// the tap leaves the request). The file name is this project's; the original
// record is stream.cpp (stream_cb.cpp, stream_prio.cpp and stream.cpp hold
// other parts of it). The handle check is the same inferred inline as in
// stream.cpp; STREAMHEADERtag and REQUESTSTRUCTtag are named by the mangled
// symbols, the request counter by its local symbol, and the header, request
// and handle layouts and the helpers are inferred. STREAM_kill, next in the
// file, is drafted in scratch/game/stream_kill_wip.cpp (the original keeps
// the header pointer in a stack slot).
// EA's streaming layer. Every public function validates the handle through
// the same inline check (non-null handle whose header carries the stream
// magic). STREAMHEADERtag is named by the mangled symbols; the header and
// handle layouts, the magic's meaning and the inline helper are inferred.
/* a stream request: named by the mangled symbols, members inferred */
struct REQUESTSTRUCTtag {
    int index;
    int state;
    REQUESTSTRUCTtag* prev;
    REQUESTSTRUCTtag* next;
    int type;
    char filename[256];
    unsigned char* source;
    int size;
    int id;
    unsigned char* start;
};

struct STREAMHEADERtag {
    int magic;
    unsigned char mutex[28];
    REQUESTSTRUCTtag* requests;
    int requestCount;
    unsigned char unknown28[8];
    unsigned char* taps;
    int tapCount;
    int bufferStart;
    unsigned char* chunkStart;
    int bufferEnd;
    int state;
    int priority;
    int priorityClass;
    int greedyLevel;
    int greedyState;
    int buffered;
    unsigned char* read;
    unsigned char* volatile requestStart;
    unsigned char unknown64[4];
    REQUESTSTRUCTtag* volatile first;
    REQUESTSTRUCTtag* volatile current;
    REQUESTSTRUCTtag* last;
    REQUESTSTRUCTtag* free;
    unsigned char unknown78[264];
    int fileOp;
};

struct STREAMHANDLEVIEW {
    STREAMHEADERtag* header;
    int number;
    int size;
    unsigned char* first;
};

extern "C" {
int FILESYS_priorityop(int, int);
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
char* strncpy(char*, const char*, int);
unsigned char* STREAM_get(int);
void STREAM_release(int, unsigned char*);
}

// File-local in the original (defined elsewhere in the file).
void startnextrequest(STREAMHEADERtag*, int);
void freerequest(STREAMHEADERtag*, REQUESTSTRUCTtag*);
extern int requestidcounter;

#define STREAM_GET32(p) (((p)[3] << 24) | ((p)[2] << 16) | ((p)[1] << 8) | (p)[0])

static inline int validhandle(int handle, STREAMHANDLEVIEW** out, void** header) {
    int error;
    if (handle == 0) {
        error = 1;
    } else if (((STREAMHANDLEVIEW*)handle)->header->magic != 0x4D525453) {
        error = 1;
    } else {
        *out = (STREAMHANDLEVIEW*)handle;
        *(STREAMHEADERtag**)header = ((STREAMHANDLEVIEW*)handle)->header;
        error = 0;
    }
    return error;
}

extern "C" void STREAM_setgreedystate(int handle, int state);

extern "C" void STREAM_setpriority(int handle, int priority, int priorityClass);

extern "C" void STREAM_setgreedylevel(int handle, int level) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, (void**)&header) == 0) {
        int old = header->greedyLevel;
        int now;
        int was;
        header->greedyLevel = level;
        now = header->buffered < level;
        was = header->buffered < old;
        if (was != now)
            STREAM_setgreedystate(handle, now);
    }
}

extern "C" void STREAM_setgreedystate(int handle, int state) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, (void**)&header) == 0) {
        header->greedyState = state;
        if (state && header->state == 1)
            FILESYS_priorityop(header->fileOp, header->priorityClass);
    }
}

extern "C" void* STREAM_taphandle(int handle, int tap) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, (void**)&header) != 0)
        return 0;
    if (tap < 1 || tap > header->tapCount)
        return 0;
    return header->taps + (tap - 1) * 16;
}

extern "C" void* STREAM_gettable(int handle);

extern "C" int STREAM_state(int handle);

extern "C" int STREAM_isendofstream(int handle);

extern "C" int STREAM_buffersize(int handle);

/* inferred: takes a request from the free list and gives it a new id in the
   upper bits of its index */
static inline REQUESTSTRUCTtag* allocrequest(STREAMHEADERtag* header) {
    REQUESTSTRUCTtag* request;

    MUTEX_lock(header->mutex);
    if (header->free == 0) {
        request = 0;
    } else {
        request = header->free;
        header->free = request->next;
        if ((requestidcounter += 256) == 0)
            requestidcounter = 256;
        request->index = (request->index & 0xFF) | requestidcounter;
    }
    MUTEX_unlock(header->mutex);
    return request;
}

/* inferred: appends a request to the queue */
static inline void addrequest(STREAMHEADERtag* header, REQUESTSTRUCTtag* request) {
    MUTEX_lock(header->mutex);
    if (header->last == 0) {
        request->prev = 0;
        header->first = request;
        header->current = request;
        header->last = request;
    } else {
        request->prev = header->last;
        header->last->next = request;
        header->last = request;
    }
    MUTEX_unlock(header->mutex);
}

/* inferred: marks an idle stream as reading; true when it was idle */
static inline int startstream(STREAMHEADERtag* header) {
    int state;

    MUTEX_lock(header->mutex);
    state = header->state;
    if (state == 0)
        header->state = 1;
    MUTEX_unlock(header->mutex);
    return state == 0;
}

extern "C" int STREAM_queuefile(int handle, const char* name, int size, int id) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    REQUESTSTRUCTtag* request;

    if (validhandle(handle, &stream, (void**)&header) != 0)
        return 0;
    request = allocrequest(header);
    if (request == 0)
        return 0;
    request->type = 0;
    strncpy(request->filename, name, 254);
    request->size = size;
    request->id = id;
    request->state = 1;
    request->next = 0;
    addrequest(header, request);
    if (startstream(header)) {
        if (header->greedyState)
            startnextrequest(header, header->priorityClass);
        else
            startnextrequest(header, header->priority);
    }
    return request->index;
}

extern "C" int STREAM_queuemem(int handle, unsigned char* data, int size, int id) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    REQUESTSTRUCTtag* request;

    if (validhandle(handle, &stream, (void**)&header) != 0)
        return 0;
    request = allocrequest(header);
    if (request == 0)
        return 0;
    if (size == 0) {
        unsigned char* chunk = data;

        while (STREAM_GET32(chunk) != id) {
            size += STREAM_GET32(chunk + 4);
            chunk += STREAM_GET32(chunk + 4);
        }
        size += STREAM_GET32(chunk + 4);
    }
    request->type = 1;
    request->source = data;
    request->size = size;
    request->id = id;
    request->state = 1;
    request->next = 0;
    addrequest(header, request);
    if (startstream(header))
        startnextrequest(header, 0);
    return request->index;
}

/* inferred: the request a handle names, if it is in use */
static inline REQUESTSTRUCTtag* findrequest(STREAMHEADERtag* header, int handle) {
    int index = handle & 0xFF;
    REQUESTSTRUCTtag* request;

    if (index >= header->requestCount)
        return 0;
    request = &header->requests[index];
    if (handle != request->index)
        return 0;
    if (request->state == 0)
        return 0;
    return request;
}

/* inferred: stores a little-endian word */
static inline void put32(unsigned char* p, unsigned int value) {
    p[0] = value;
    p[1] = value >> 8;
    p[2] = value >> 16;
    p[3] = value >> 24;
}

/* inferred: returns bytes to the buffer, entering greedy mode (and raising
   a running read's priority) when the count drops below the level */
static inline void subbuffered(STREAMHEADERtag* header, int size) {
    int before;
    int after;

    MUTEX_lock(header->mutex);
    before = header->buffered;
    after = before - size;
    header->buffered = after;
    MUTEX_unlock(header->mutex);
    if (before >= header->greedyLevel && after < header->greedyLevel) {
        header->greedyState = 1;
        if (header->state == 1)
            FILESYS_priorityop(header->fileOp, header->priorityClass);
    }
}

/* inferred: whether a buffer address lies in [start, end) of the ring */
static inline int inrange(unsigned char* p, unsigned char* start, unsigned char* end) {
    if (start <= end)
        return p >= start && p < end;
    return p >= start || p < end;
}

extern "C" void STREAM_cancelrequest(int handle, int requestHandle) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    STREAMHANDLEVIEW* tap;
    unsigned char* chunk;
    unsigned char* read = 0;
    unsigned char* start = 0;
    unsigned char* end = 0;
    int size;
    int type;
    int i;
    int done;
    REQUESTSTRUCTtag* request;
    int word;

    if (validhandle(handle, &stream, (void**)&header) != 0)
        return;
    MUTEX_lock(header->mutex);
    request = findrequest(header, requestHandle);
    if (!request || request->state == 4) {
        done = 1;
    } else if (request->state == 1) {
        freerequest(header, request);
        done = 1;
    } else {
        request->state = 4;
        read = header->read;
        if (request == header->first)
            start = read;
        else
            start = request->start;
        if (!request->next || request->next->state == 1)
            end = header->requestStart;
        else
            end = request->next->start;
        done = 0;
    }
    MUTEX_unlock(header->mutex);
    if (done)
        return;
    for (i = 0; i < header->tapCount; i++) {
        tap = &((STREAMHANDLEVIEW*)header->taps)[i];
        if (tap->size <= 0)
            continue;
        if (inrange(tap->first, read, start)) {
            type = tap->number << 24;
            chunk = start;
            while (chunk != end) {
                if (STREAM_GET32(chunk) == -1) {
                    chunk = header->chunkStart;
                    continue;
                }
                word = STREAM_GET32(chunk + 4);
                size = word & 0xFFFFFF;
                if (type == (word & ~0xFFFFFF)) {
                    MUTEX_lock(header->mutex);
                    tap->size -= size;
                    MUTEX_unlock(header->mutex);
                    subbuffered(header, size);
                    chunk[0] = 0xFE;
                    chunk[1] = 0xFF;
                    chunk[2] = 0xFF;
                    chunk[3] = 0xFF;
                    put32(chunk + 4, size);
                }
                chunk += size;
            }
        } else if (inrange(tap->first, start, end)) {
            do {
                STREAM_release((int)tap, STREAM_get((int)tap));
            } while (tap->size > 0 && inrange(tap->first, start, end));
        }
    }
}

extern "C" void STREAM_kill(int handle) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    REQUESTSTRUCTtag* request;
    unsigned char* chunk;
    int size = 0;
    int i;

    if (validhandle(handle, &stream, (void**)&header) != 0)
        return;
    request = header->last;
    if (!request)
        return;
    while (request->state == 1 || request->state == 2) {
        STREAM_cancelrequest(handle, request->index);
        request = header->last;
    }
    while (header->first != header->current)
        freerequest(header, header->first);
    header->current->state = 4;
    for (i = 0; i < header->tapCount; i++)
        ((STREAMHANDLEVIEW*)header->taps)[i].size = 0;
    subbuffered(header, header->buffered);
    chunk = header->read;
    while (chunk != header->requestStart) {
        if (STREAM_GET32(chunk) == -1) {
            chunk = header->chunkStart;
            continue;
        }
        size = STREAM_GET32(chunk + 4) & 0xFFFFFF;
        chunk[0] = 0xFE;
        chunk[1] = 0xFF;
        chunk[2] = 0xFF;
        chunk[3] = 0xFF;
        put32(chunk + 4, size);
        chunk += size;
    }
    if (header->state == 2) {
        if (chunk == header->chunkStart) {
            header->chunkStart = header->requestStart = (unsigned char*)header->bufferStart;
        } else {
            int pad = 32 - ((unsigned int)header->requestStart & 31);

            if (pad == 32)
                pad = 0;
            chunk = header->requestStart - size;
            size += pad;
            put32(chunk + 4, size);
            header->requestStart += pad;
        }
        header->state = 0;
    }
}
