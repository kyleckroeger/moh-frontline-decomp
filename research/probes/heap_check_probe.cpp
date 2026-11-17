// settings: config/GMFE69/memalloc.json
// replaces: MEM_allocaligna__FPCciiiib
// replaces: MEM_tailsize__FPCci
// replaces: MEM_allocalign
// replaces: MEM_alloc
// replaces: MEM_allocz
// Test harness: memalloc.cpp with a check of every memory class's block chain at each allocation;
// reports the first link outside main memory with the allocation's name and caller.
// EA's memalloc.cpp, the whole file: MEM_tailsize, the tail size of
// the memory class selected by the low six flag bits (the MEMCLASS word at
// +0x30), and the file-local MEM_allocaligna, the allocator behind
// MEM_allocalign/MEM_alloc/MEM_allocz: it finds a free block
// (the largest with flag 0x200, from the top with 0x100), aligns the result
// (alignment from the class, 0x400 selects its second alignment), returns a
// large enough remainder to the free list and retries while the allocation
// failure callback asks for it. Then the entry points MEM_allocalign,
// MEM_alloc and MEM_allocz forward to it, the latter two with no alignment;
// the final flag is true except in MEM_allocz (its meaning is not
// established). MEMCLASS is the inferred view of memclass.cpp; the parameter
// names are inferred.
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
extern int (*MEM_allocfailcallback)(int, int, int);
extern "C" {
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
}
int MEM_initblock(MEMBLOCK*, const char*, int, int, int, MEMBLOCK*, MEMBLOCK*);
MEMBLOCK* FREE_find(MEMCLASS*, int, int);
MEMBLOCK* FREE_findlargest(MEMCLASS*, int, int);
void FREE_remove(MEMBLOCK*);
void FREE_add(MEMCLASS*, MEMBLOCK*);


extern "C" void OSReport(const char*, ...);
static int s_bad, s_calls;
static unsigned long s_lastlr;
static const char* s_lastname;
static inline bool BadPtr(void* p) { return (unsigned long)p < 0x80000000 || (unsigned long)p >= 0x84000000; }
static void CheckHeap(const char* name, unsigned long lr) {
    s_calls++;
    if (s_bad)
        return;
    for (int c = 0; c < 64; c++) {
        MEMCLASS* cls = memclass[c];
        if (!cls || BadPtr(cls))
            continue;
        MEMBLOCK* b = cls->low;
        for (int n = 0; n < 100000 && b && b != cls->high; n++) {
            if (BadPtr(b) || BadPtr(b->next)) {
                s_bad = 1;
                OSReport("HEAPBAD call %d class %d block %08x next %08x size %d; now %s lr %08x; previous %s lr %08x\n",
                         s_calls, c, (unsigned long)b, BadPtr(b) ? 0 : (unsigned long)b->next, BadPtr(b) ? 0 : b->size,
                         name ? name : "?", lr, s_lastname ? s_lastname : "?", s_lastlr);
                return;
            }
            b = b->next;
        }
    }
    if (s_calls > 3072)
        OSReport("ALLOCCALL %d %s lr %08x\n", s_calls, name ? name : "?", lr);
    s_lastname = name;
    s_lastlr = lr;
}

int MEM_tailsize(const char* name, int flags) {
    return memclass[flags & 0x3f]->field30;
}

void* MEM_allocaligna(const char* name, int size, int align, int offset, int flags, bool clear) {
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

extern "C" {
void* MEM_allocalign(const char* name, int size, int align, int offset, int flags) {
    register unsigned long lr;
    asm { mflr lr }
    CheckHeap(name, lr);
    return MEM_allocaligna(name, size, align, offset, flags, true);
}

void* MEM_alloc(const char* name, int size, int flags) {
    register unsigned long lr;
    asm { mflr lr }
    CheckHeap(name, lr);
    return MEM_allocaligna(name, size, 0, 0, flags, true);
}

void* MEM_allocz(const char* name, int size, int flags) {
    register unsigned long lr;
    asm { mflr lr }
    CheckHeap(name, lr);
    return MEM_allocaligna(name, size, 0, 0, flags, false);
}
}
