// resize.cpp, the whole file: MEM_resize shrinks or grows a block in place (a size of -1
// means as large as possible), absorbing a free block after it, moving the
// block's tail data to the new end, and splitting off the rest as a free
// block when it is large enough. MEMBLOCK and MEMCLASS are named by the mangled
// symbols; their members are inferred from offsets as in memclass.cpp, and
// the names are not original.
struct MEMBLOCK {
    unsigned short magic;
    unsigned short flags;
    int size;
    MEMBLOCK* next;
    MEMBLOCK* prev;
    MEMBLOCK* freenext;
    MEMBLOCK* freeprev;
};

struct MEMCLASS {
    char name[8];
    MEMBLOCK* low;
    MEMBLOCK* high;
    MEMBLOCK free;
    int field28;
    int alignment;
    int field30;
    int flags;
    unsigned char locked;
    int mutex[8];
};

#define MEM_ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

static const int MB_FREE = 0x4000;
static const int MB_FIXED = 0x8000;

extern MEMCLASS* memclass[64];

extern "C" {
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
void MEM_move(void*, const void*, int);
}
int MEM_tailsize(const char*, int);
int MEM_initblock(MEMBLOCK*, const char*, int, int, int, MEMBLOCK*, MEMBLOCK*);
void FREE_add(MEMCLASS*, MEMBLOCK*);
void FREE_remove(MEMBLOCK*);

extern "C" void* MEM_resize(void* address, int size) {
    int align;
    int newsize;
    int flags;
    MEMBLOCK* block;
    MEMBLOCK* next;
    int space;
    int tail;
    MEMBLOCK* rest;
    MEMCLASS* mc;
    char* p;
    p = (char*)address;
    block = (MEMBLOCK*)(p - 16);
    flags = block->flags;
    mc = memclass[flags & 0x3F];
    if (mc->locked)
        MUTEX_lock(mc->mutex);
    next = block->next;
    if (next->flags & MB_FREE) {
        FREE_remove(next);
        next = next->next;
        block->next = next;
        next->prev = block;
    }
    newsize = size;
    if (size < 8) {
        if (size == -1)
            newsize = 0x40000000;
        else if (size >= 0)
            newsize = 8;
    }
    tail = MEM_tailsize(0, flags);
    space = (char*)next - (char*)block - 16;
    align = mc->field28 - 1;
    if (newsize + tail > space) {
        newsize = space - tail;
        size = newsize;
    }
    MEM_move(p + newsize, p + block->size, tail);
    block->size = size;
    rest = (MEMBLOCK*)(((int)p + newsize + tail + align) & ~align);
    if (((char*)next - (char*)rest) / 16 > 48) {
        MEM_initblock(rest, 0, 0, 0, 0, block, next);
        FREE_add(mc, rest);
        next->prev = rest;
        block->next = rest;
    }
    if (mc->locked)
        MUTEX_unlock(mc->mutex);
    return p;
}
