// The ARAM sample-memory manager: set-up, pool bounds (in 32-byte units) and
// teardown, then the allocator: SNDARAM_alloc takes the first free range
// (before, between or after the sorted allocated ranges, clipped to the pool)
// that fits, inserting its record, and returns its byte address (0 when
// full); SNDARAM_free removes the record of a block (its address in 32-byte
// units) and moves the later records down. sndaram's members, the range record
// and the clipping helper are inferred and are not original.
/* inferred: an allocated range (start and length, in 32-byte units) */
struct SNDARAMBLOCKVIEW {
    unsigned int start;
    unsigned int length;
};

struct SNDARAMVIEW {
    unsigned short count;
    unsigned short max;
    unsigned int start;
    unsigned int end;
    SNDARAMBLOCKVIEW* table;
};

extern "C" {
extern SNDARAMVIEW sndaram;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);

void SNDARAM_init(int blocks) {
    SNDSYS_entercritical();
    sndaram.table = (SNDARAMBLOCKVIEW*)SNDMEMI_allocz(blocks * 8);
    SNDSYS_leavecritical();
    sndaram.count = 0;
    sndaram.max = blocks;
}

void SNDARAM_setpool(unsigned int base, int size) {
    sndaram.end = (base + size) & ~4;
    if (base < 32)
        base = 32;
    sndaram.start = base;
    if (base & 31)
        sndaram.start = (base + 31) & ~31;
    sndaram.start >>= 5;
    sndaram.end >>= 5;
}

void SNDARAM_restore(void) {
    if (sndaram.table) {
        SNDSYS_entercritical();
        SNDMEMI_free(sndaram.table);
        SNDSYS_leavecritical();
        sndaram.table = 0;
    }
}

/* inferred inline helper: clip a free range to the pool */
static inline void clipfree(unsigned int& start, int& length) {
    if (start < sndaram.start) {
        unsigned int skip = sndaram.start - start;
        start += skip;
        length -= skip;
    }
    if (start + length > sndaram.end)
        length = sndaram.end - start;
}

int SNDARAM_alloc(int size) {
    int i = 0;
    unsigned int start;
    int length;
    int blocks;
    int j;

    if (sndaram.count >= sndaram.max)
        return 0;
    if (sndaram.count == 0) {
        start = sndaram.start;
        length = sndaram.end - start;
        clipfree(start, length);
        if ((size + 31) >> 5 > length)
            return 0;
    } else {
        blocks = (size + 31) >> 5;
        for (i = 0; i < sndaram.count; i++) {
            if (i == 0) {
                start = sndaram.start;
                length = sndaram.table[i].start - start;
            } else {
                SNDARAMBLOCKVIEW* prev = &sndaram.table[i - 1];
                start = prev->start + prev->length;
                length = sndaram.table[i].start - start;
            }
            clipfree(start, length);
            if (blocks <= length) {
                for (j = sndaram.count; j > i; j--)
                    sndaram.table[j] = sndaram.table[j - 1];
                goto found;
            }
        }
        {
            SNDARAMBLOCKVIEW* prev = &sndaram.table[i - 1];
            start = prev->start + prev->length;
        }
        length = sndaram.end - start;
        clipfree(start, length);
        if (blocks > length)
            return 0;
    }
found:
    {
        SNDARAMBLOCKVIEW* block = &sndaram.table[i];
        block->start = start;
        block->length = (size + 31) >> 5;
    }
    sndaram.count++;
    return start << 5;
}

void SNDARAM_free(unsigned int address) {
    SNDARAMBLOCKVIEW* record = sndaram.table;
    unsigned int block = address >> 5;
    int i;

    for (i = 0; i < sndaram.count; record++, i++) {
        if (record->start == block) {
            sndaram.count--;
            for (; i < sndaram.count; i++)
                sndaram.table[i] = sndaram.table[i + 1];
            return;
        }
    }
}
