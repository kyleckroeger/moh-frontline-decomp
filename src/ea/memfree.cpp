// MEM_free, the whole file: frees a block, merging it with free neighbours on
// both sides, and when the block before it is in use moves the freed block's
// header down to just after that block's (aligned) data, then returns it to
// the class's free list. MEMBLOCK and MEMCLASS are named by the mangled
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
}
int MEM_tailsize(const char*, int);
void FREE_add(MEMCLASS*, MEMBLOCK*);
void FREE_remove(MEMBLOCK*);

extern "C" int MEM_free(void* address) {
    if (address) {
        MEMBLOCK* block = (MEMBLOCK*)((char*)address - 16);
        MEMCLASS* mc = memclass[block->flags & 0x3F];
        MEMBLOCK* next;
        MEMBLOCK* prev;
        if (mc->locked)
            MUTEX_lock(mc->mutex);
        prev = block->prev;
        next = block->next;
        if (prev->flags & MB_FREE) {
            FREE_remove(prev);
            block = prev;
            prev = prev->prev;
            block->next = next;
            prev->next = block;
            next->prev = block;
        }
        if (next->flags & MB_FREE) {
            FREE_remove(next);
            next = next->next;
            block->prev = prev;
            block->next = next;
            next->prev = block;
        }
        if (!(prev->flags & (MB_FREE | MB_FIXED))) {
            int tail = MEM_tailsize(0, prev->flags);
            int size = prev->size + tail < 8 ? 8 : prev->size + tail;
            int align;
            MEMBLOCK* moved;
            align = mc->field28 - 1;
            size += 16;
            size += align;
            moved = (MEMBLOCK*)((char*)prev + (size & ~align));
            if (block != moved) {
                unsigned short flags = block->flags;
                block = moved;
                moved->flags = flags;
                moved->next = next;
                moved->prev = prev;
                prev->next = moved;
                next->prev = moved;
            }
        }
        FREE_add(mc, block);
        if (mc->locked)
            MUTEX_unlock(mc->mutex);
    }
    return 1;
}
