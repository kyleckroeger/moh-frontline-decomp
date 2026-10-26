// Copying an animation file's meta-data into the dynamic anim-copy block:
// the animation list, one emotion animation (header bytes, compressed data,
// frame table), the compressed data ('SKEL' record with its two tables) and
// the state list. Every copy takes its memory from one bump allocator that
// counts the allocations, rounds the size up to 4 bytes and grows the block
// by 100000 bytes when it runs out. The globals are named by the symbols; the
// record layouts are inferred from the code. The rest of the file is not part
// of this unit.
void DebugMsg(const char*, ...);
void* DWI_alloc(const char*, int, int);
extern "C" void* MEM_resize(void*, unsigned int);
extern "C" unsigned int MEM_size(void*);

extern unsigned int g_amount_allocated;
extern unsigned int g_num_allocations;
extern char* g_anim_copy_memory;
extern unsigned int g_anim_copy_memory_sz;
extern unsigned int g_anim_copy_memory_cur;

// Inferred views.
struct AnimEmotAnimation_t {
    unsigned char header[4];
    unsigned char* data;
    unsigned short frames[1];
};

struct AnimEmotAnimList_t {
    unsigned int count;
    AnimEmotAnimation_t* items[1];
};

struct AnimEmotStateDef_t {
    unsigned char data[20];
};

struct AnimEmotStateList_t {
    int count;
    AnimEmotStateDef_t states[1];
};

struct AnimFileFormat_t {
    unsigned char field00[16];
    AnimEmotStateList_t* states;
    AnimEmotAnimList_t* animations;
};

struct AnimCompEntry_t {
    short a;
    short b;
    short c;
};

struct AnimCompTable_t {
    unsigned short count;
    AnimCompEntry_t entries[1];
};

struct AnimCompPair_t {
    unsigned char a;
    unsigned char b;
};

struct AnimCompPairTable_t {
    unsigned short count;
    AnimCompPair_t entries[1];
};

struct AnimCompressedData_t {
    int magic;
    AnimCompTable_t* table;
    AnimCompPairTable_t* pairs;
};

AnimEmotAnimation_t* CopyEmotAnimation(AnimEmotAnimation_t*, AnimFileFormat_t*, int);
unsigned char* CopyCompressedData(unsigned char*);
void CopyStateDef(AnimEmotStateDef_t*, AnimEmotStateDef_t*);

// The shared allocator (inlined at every copy).
inline void* AnimCopyAlloc(unsigned int size) {
    g_amount_allocated += size;
    g_num_allocations++;
    unsigned int aligned = size;
    if (size & 3) {
        aligned = (~size & 3) + size;
        aligned += 1;
    }
    if (g_anim_copy_memory_cur + aligned > g_anim_copy_memory_sz) {
        DebugMsg("Growing animation data array\n");
        if (!g_anim_copy_memory) {
            g_anim_copy_memory_sz += 100000;
            g_anim_copy_memory = (char*)DWI_alloc("Animation meta-data", 100000, 1024);
        } else {
            g_anim_copy_memory_sz += 100000;
            g_anim_copy_memory_sz = MEM_size(MEM_resize(g_anim_copy_memory, g_anim_copy_memory_sz));
        }
    }
    void* p = g_anim_copy_memory + g_anim_copy_memory_cur;
    g_anim_copy_memory_cur += aligned;
    return p;
}

AnimEmotAnimList_t* CopyAnimList(AnimFileFormat_t* file) {
    unsigned int size = (file->animations->count - 1) * 4 + 8;
    AnimEmotAnimList_t* list = (AnimEmotAnimList_t*)AnimCopyAlloc(size);
    list->count = file->animations->count;
    for (unsigned int i = 0; i < list->count; i++)
        list->items[i] = CopyEmotAnimation(file->animations->items[i], file, i);
    return list;
}

// The frame count comes from the gap between the frame table and the
// compressed data, or (without data) from the gap to the next animation.
AnimEmotAnimation_t* CopyEmotAnimation(AnimEmotAnimation_t* anim, AnimFileFormat_t* file, int index) {
    unsigned int bytes;
    int count;
    if (anim->data) {
        bytes = (char*)anim->data - (char*)anim->frames;
        count = bytes / 2;
    } else {
        AnimEmotAnimList_t* list = file->animations;
        if (index < list->count - 1)
            bytes = (char*)list->items[index + 1] - (char*)list->items[index];
        else
            bytes = (char*)list - (char*)list->items[index];
        bytes -= 10;
        count = bytes / 2;
    }
    unsigned int size = (count - 1) * sizeof(short) + sizeof(AnimEmotAnimation_t);
    AnimEmotAnimation_t* copy = (AnimEmotAnimation_t*)AnimCopyAlloc(size);
    copy->header[0] = anim->header[0];
    copy->header[1] = anim->header[1];
    copy->header[2] = anim->header[2];
    copy->header[3] = anim->header[3];
    copy->data = CopyCompressedData(anim->data);
    for (int i = 0; i < count; i++)
        copy->frames[i] = anim->frames[i];
    return copy;
}

inline AnimCompTable_t* CopyCompTable(AnimCompTable_t* from) {
    AnimCompTable_t* to = (AnimCompTable_t*)AnimCopyAlloc((from->count - 1) * sizeof(AnimCompEntry_t) + sizeof(AnimCompTable_t));
    to->count = from->count;
    for (int i = 0; i < from->count; i++)
        to->entries[i] = from->entries[i];
    return to;
}

inline AnimCompPairTable_t* CopyCompPairTable(AnimCompPairTable_t* from) {
    AnimCompPairTable_t* to = (AnimCompPairTable_t*)AnimCopyAlloc((from->count - 1) * sizeof(AnimCompPair_t) + sizeof(AnimCompPairTable_t));
    to->count = from->count;
    for (int i = 0; i < to->count; i++)
        to->entries[i] = from->entries[i];
    return to;
}

unsigned char* CopyCompressedData(unsigned char* data) {
    if (!data)
        return 0;
    AnimCompressedData_t* src = (AnimCompressedData_t*)data;
    if (src->magic == 'SKEL') {
        AnimCompressedData_t* copy = (AnimCompressedData_t*)AnimCopyAlloc(sizeof(AnimCompressedData_t));
        copy->magic = src->magic;
        copy->table = CopyCompTable(src->table);
        copy->pairs = CopyCompPairTable(src->pairs);
        return (unsigned char*)copy;
    }
    return 0;
}

AnimEmotStateList_t* CopyStateList(AnimFileFormat_t* file) {
    unsigned int size = (file->states->count - 1) * sizeof(AnimEmotStateDef_t) + sizeof(AnimEmotStateList_t);
    AnimEmotStateList_t* list = (AnimEmotStateList_t*)AnimCopyAlloc(size);
    list->count = file->states->count;
    for (int i = 0; i < file->states->count; i++)
        CopyStateDef(&list->states[i], &file->states->states[i]);
    return list;
}
