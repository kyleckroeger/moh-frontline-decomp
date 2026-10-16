/* EA's sound memory manager (smemman.c): the pool is laid out in caller
   memory with a table of (offset, size) allocation records at its end,
   indexed downward (the count kept as a negative number). SNDMEMI_init lays
   the pool out (data 16-byte aligned after a 24-byte header, the free and
   lowest-free sizes set), SNDMEMI_restore asks for the high-water mark,
   SNDMEMI_allocz takes the first gap between records large enough for the
   16-byte-rounded size (moving the lower records down to make room for its
   record) and SNDMEMI_free removes a block's record. The pool and record
   views, the inline record test and sndgs's view are inferred. */
struct SNDMEMRECORDVIEW {
    unsigned int offset;
    int size;
};

/* inferred: the sound memory pool header */
struct SNDMEMPOOLVIEW {
    char* base;
    SNDMEMRECORDVIEW* records;
    int size;
    unsigned int free;
    int lowest;
    int count;
};

struct SNDGSVIEW {
    unsigned char unknown000[516];
    SNDMEMPOOLVIEW* pool;
};

extern SNDGSVIEW sndgs;

extern "C" int SNDMEM_gethighwater();

void SNDMEMI_init(void* memory, int size) {
    sndgs.pool = (SNDMEMPOOLVIEW*)memory;
    ((SNDMEMPOOLVIEW*)memory)->size = size;
    sndgs.pool->records = (SNDMEMRECORDVIEW*)((char*)memory + size - 8);
    sndgs.pool->base = (char*)memory + 24;
    sndgs.pool->base += 15;
    sndgs.pool->base = (char*)((int)sndgs.pool->base & ~15);
    sndgs.pool->free = size - 47;
    sndgs.pool->lowest = size - 15;
}

void SNDMEMI_restore() {
    SNDMEM_gethighwater();
}

void* SNDMEMI_allocz(int size) {
    int i = 0;
    int start;
    int gap;

    if (sndgs.pool->count == 0) {
        start = 0;
        gap = sndgs.pool->free;
        if ((unsigned int)(start + gap) > sndgs.pool->free)
            gap = sndgs.pool->free - start;
        if (((size + 15) & ~15) <= gap)
            goto found;
        goto fail;
    } else {
        for (i = 0; i > sndgs.pool->count; i--) {
            SNDMEMRECORDVIEW* records = sndgs.pool->records;

            if (i == 0) {
                gap = records[i].offset;
                start = 0;
            } else {
                SNDMEMRECORDVIEW* prev = &records[i + 1];

                start = prev->offset + prev->size;
                gap = records[i].offset - start;
            }
            if ((unsigned int)(start + gap) > sndgs.pool->free)
                gap = sndgs.pool->free - start;
            if (((size + 15) & ~15) <= gap) {
                int j;

                for (j = sndgs.pool->count; j < i; j++)
                    sndgs.pool->records[j] = sndgs.pool->records[j + 1];
                goto found;
            }
        }
        {
            SNDMEMRECORDVIEW* prev = &sndgs.pool->records[i + 1];

            start = prev->offset + prev->size;
        }
        gap = sndgs.pool->free - start;
        if ((unsigned int)(start + gap) > sndgs.pool->free)
            gap = sndgs.pool->free - start;
        if (((size + 15) & ~15) > gap)
            goto fail;
    }
found:
    {
        int remaining;
        void* result;
        SNDMEMRECORDVIEW* record;

        record = &sndgs.pool->records[i];
        record->offset = start;
        record->size = (size + 15) & ~15;
        sndgs.pool->count--;
        sndgs.pool->free -= 8;
        remaining = sndgs.pool->free - (start + ((size + 15) & ~15));
        result = sndgs.pool->base + start;
        if (remaining < sndgs.pool->lowest)
            sndgs.pool->lowest = remaining;
        return result;
    }
fail:
    return 0;
}

/* inferred: whether a record holds an offset */
static inline int recordat(SNDMEMRECORDVIEW* record, unsigned int offset) {
    return record->offset == offset;
}

void SNDMEMI_free(void* memory) {
    SNDMEMPOOLVIEW* pool = sndgs.pool;
    unsigned int offset = (char*)memory - pool->base;
    int i;

    for (i = 0; i > pool->count; i--) {
        if (recordat(&pool->records[i], offset)) {
            pool->count++;
            sndgs.pool->free += 8;
            for (; i > sndgs.pool->count; i--)
                sndgs.pool->records[i] = sndgs.pool->records[i - 1];
            return;
        }
    }
}
