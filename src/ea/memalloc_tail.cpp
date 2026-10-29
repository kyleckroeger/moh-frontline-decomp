// A fragment of EA's memalloc.cpp (0x8014f4f8): MEM_tailsize, the tail size of
// the memory class selected by the low six flag bits (the MEMCLASS word at
// +0x30), and the file-local MEM_allocaligna, the allocator behind
// MEM_allocalign/MEM_alloc/MEM_allocz (unit memalloc): it finds a free block
// (the largest with flag 0x200, from the top with 0x100), aligns the result
// (alignment from the class, 0x400 selects its second alignment), returns a
// large enough remainder to the free list and retries while the allocation
// failure callback asks for it. The file name is this project's; the original
// record is memalloc.cpp. MEMCLASS is the inferred view of memclass.cpp; the
// file's globals are extern.
// REAL memory allocation from a memory class. The block found on the free list
// is aligned (from the bottom, or from the top with flag 0x100) and any large
// enough remainder goes back on the free list as a new block.
//
// MEMBLOCK and MEMCLASS are named by the mangled symbols; their members are
// inferred from offsets (see memclass.cpp) and their names are not original.
struct MEMBLOCK {
    unsigned short magic;
    unsigned short flags;
    int size;
    MEMBLOCK* next;
    MEMBLOCK* prev;
};

struct MEMCLASS {
    char name[8];
    MEMBLOCK* low;
    MEMBLOCK* high;
    MEMBLOCK free;
    MEMBLOCK* field20;
    MEMBLOCK* field24;
    int field28;
    int alignment;
    int field30;
    int flags;
    unsigned char locked;
    int mutex[8];
};

extern MEMCLASS* memclass[64];

extern "C" {
extern int mb_default;
}
int (*MEM_allocfailcallback)(int, int, int);
extern "C" {
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
}
int MEM_initblock(MEMBLOCK*, const char*, int, int, int, MEMBLOCK*, MEMBLOCK*);
MEMBLOCK* FREE_find(MEMCLASS*, int, int);
MEMBLOCK* FREE_findlargest(MEMCLASS*, int, int);
void FREE_remove(MEMBLOCK*);
void FREE_add(MEMCLASS*, MEMBLOCK*);

int MEM_tailsize(const char* name, int flags) {
    return memclass[flags & 0x3f]->field30;
}

static void* MEM_allocaligna(const char* name, int size, int align, int offset, int flags, bool clear) {
    MEMBLOCK* block;
    int needed;
    char* result = 0;
    int total;
    int classalign;
    int alignmask;
    MEMBLOCK* header;
    int retry;
    MEMCLASS* cls;
    int blocksize;
    MEMBLOCK* rest;
    MEMBLOCK* next;
    MEMBLOCK* prev;

    if (!flags)
        flags = mb_default;
    cls = memclass[flags & 0x3f];
    if (size < 0)
        return 0;
    do {
        retry = 0;
        if (cls->locked)
            MUTEX_lock(cls->mutex);
        classalign = cls->field28 - 1;
        if (align == 0)
            alignmask = (flags & 0x400 ? cls->alignment : cls->field28) - 1;
        else
            alignmask = align - 1;
        needed = size + MEM_tailsize(name, flags) < 8 ? 8 : size + MEM_tailsize(name, flags);
        total = needed + 16 + alignmask;
        if (flags & 0x200)
            block = FREE_findlargest(cls, total, flags & 0x100);
        else
            block = FREE_find(cls, total, flags & 0x100);
        if (block) {
            blocksize = block->size;
            rest = 0;
            FREE_remove(block);
            next = block->next;
            prev = block->prev;
            if (flags & 0x100) {
                result = (char*)(((unsigned int)block + blocksize - needed + offset) & ~alignmask) - offset;
                rest = block;
            } else {
                result = (char*)(((unsigned int)block + 16 + alignmask + offset) & ~alignmask) - offset;
            }
            header = (MEMBLOCK*)(result - 16);
            header->next = next;
            header->prev = prev;
            if (blocksize - total > classalign + 48) {
                if (flags & 0x100) {
                    rest->next->prev = header;
                    header->next = rest->next;
                    header->prev = rest;
                    MEM_initblock(rest, 0, 0, 0, 0, rest->prev, header);
                    FREE_add(cls, rest);
                } else {
                    rest = (MEMBLOCK*)((unsigned int)(result + needed + classalign) & ~classalign);
                    header->next->prev = rest;
                    header->prev->next = header;
                    MEM_initblock(rest, 0, 0, 0, 0, header, header->next);
                    FREE_add(cls, rest);
                    header->next = rest;
                }
            } else {
                header->next->prev = header;
                header->prev->next = header;
            }
            MEM_initblock(header, 0, size, cls->field30, flags, header->prev, header->next);
        } else if (MEM_allocfailcallback) {
            retry = MEM_allocfailcallback(0, size, flags);
        }
        if (cls->locked)
            MUTEX_unlock(cls->mutex);
    } while (retry == 1);
    return result;
}

extern "C" void* MEM_allocalign(const char* name, int size, int align, int offset, int flags);

extern "C" void* MEM_alloc(const char* name, int size, int flags);

extern "C" void* MEM_allocz(const char* name, int size, int flags);


