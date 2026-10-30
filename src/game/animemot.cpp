// animemot.cpp: copying an animation file's meta-data into the dynamic
// anim-copy block (header, frame table, animation list, emotion animations
// with their compressed 'SKEL' data, the state list and each state definition
// with its transition and animation lists and data blobs), all through one
// bump allocator that counts the allocations, rounds sizes up to 4 bytes and
// grows the block by 100000 bytes; fetching a frame from ARAM into the frame
// cache; uploading the frame data to ARAM and rebasing the frame table; the
// ARAM transfer callbacks; the state/animation queries; and relocating a
// loaded file (byte-swapping it and turning its offsets into pointers, with a
// warning for a file from another eMotion version). Globals, functions and
// AnimFileFormat_t are named by the symbols; the record layouts are inferred
// from the code. Notes on the forms: a blob's size is 'length + 4' computed
// from a short local at each use; list entries are copied by small inlines
// (the state-animation one takes the source first); the upload length is
// built as 'finish - start' plus the lead. The byte-swap helpers are reached
// through one or two typed wrappers per field, which is what sets the order of
// their stack temporaries; the header swap EndianSwap(AnimFileFormat_t&) is an
// inline too large to inline, so it is emitted (weak) at the end of the file.
typedef struct ARQRequest ARQRequest;
typedef void (*ARQCallback)(unsigned long);

extern "C" {
unsigned long OSGetTick(void);
void DCFlushRange(void*, unsigned long);
void VIWaitForRetrace(void);
void ARQPostRequest(ARQRequest*, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long,
                    unsigned long, ARQCallback);
unsigned long ARAM_alloc(void*, const char*, unsigned long, int);
int printf(const char*, ...);
int AnimStGetCallbackIndexFromID(int);
}

void DebugMsg(const char*, ...);
void* DWI_alloc(const char*, int, int);
extern "C" void* MEM_resize(void*, unsigned int);
extern "C" unsigned int MEM_size(void*);
extern "C" int MEM_totalunused(int);

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

struct AnimBlob_t {
    unsigned short type;
    unsigned short length;
};
struct AnimTransition_t {
    unsigned short state;
    AnimBlob_t* blob;
};
struct AnimTransList_t {
    unsigned int count;
    AnimTransition_t entries[1];
};
struct AnimStateAnim_t {
    unsigned short values[5];
    AnimBlob_t* blob;
};
struct AnimStateAnimList_t {
    unsigned int count;
    AnimStateAnim_t entries[1];
};
struct AnimEmotStateDef_t {
    unsigned short id;
    unsigned short flags;
    int callback;
    AnimTransList_t* transitions;
    AnimBlob_t* blob;
    AnimStateAnimList_t* anims;
};

struct AnimEmotStateList_t {
    int count;
    AnimEmotStateDef_t states[1];
};

struct AnimFileFormat_t {
    int field00;
    int field04;
    unsigned short field08;
    unsigned short field0A;
    unsigned short numFrames;
    unsigned short field0E;
    AnimEmotStateList_t* states;
    AnimEmotAnimList_t* animations;
    unsigned int* frames;
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

AnimEmotStateList_t* CopyStateList(AnimFileFormat_t*);
AnimEmotAnimList_t* CopyAnimList(AnimFileFormat_t*);
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

inline unsigned int* CopyFrameTable(AnimFileFormat_t* file) {
    unsigned int* frames = (unsigned int*)AnimCopyAlloc((file->numFrames + 1) * sizeof(unsigned int));
    for (int i = 0; i < file->numFrames; i++)
        frames[i] = file->frames[i];
    return frames;
}

AnimFileFormat_t* CopyAnimFileFormatStructureToDynMem(AnimFileFormat_t* file) {
    int before = MEM_totalunused(0);
    DebugMsg("Memory Free before: %d\n", before);
    g_anim_copy_memory_sz = (int)(0.2 * (int)MEM_size(file));
    g_anim_copy_memory = (char*)DWI_alloc("Animation meta-data", g_anim_copy_memory_sz, 1024);
    g_anim_copy_memory_cur = 0;
    AnimFileFormat_t* copy = (AnimFileFormat_t*)AnimCopyAlloc(sizeof(AnimFileFormat_t));
    copy->field00 = file->field00;
    copy->field04 = file->field04;
    copy->field08 = file->field08;
    copy->field0A = file->field0A;
    copy->numFrames = file->numFrames;
    copy->states = CopyStateList(file);
    copy->animations = CopyAnimList(file);
    unsigned int* frames = CopyFrameTable(file);
    DebugMsg("Frame Data start %x end %x size %d\n", frames[0], file->frames, (int)file->frames - frames[0]);
    copy->frames = frames;
    g_anim_copy_memory_sz = MEM_size(MEM_resize(g_anim_copy_memory, g_anim_copy_memory_cur));
    DebugMsg("Allocated %d Bytes for Anim File copy (%d allocations)\n", g_amount_allocated, g_num_allocations);
    int after = MEM_totalunused(0);
    DebugMsg("Memory Free after: %d (diff %d)\n", after, before - after);
    return copy;
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

extern "C" void* memcpy(void*, const void*, unsigned long);
inline AnimBlob_t* CopyBlob(AnimBlob_t* from) {
    if (from) {
        unsigned short len = from->length;
        AnimBlob_t* p = (AnimBlob_t*)AnimCopyAlloc(len + 4);
        memcpy(p, from, len + 4);
        return p;
    }
    return 0;
}

inline void CopyTransEntry(AnimTransition_t* to, AnimTransition_t* from) {
    to->state = from->state;
    to->blob = CopyBlob(from->blob);
}
inline AnimTransList_t* CopyTransList(AnimTransList_t* from) {
    AnimTransList_t* to = (AnimTransList_t*)AnimCopyAlloc((from->count - 1) * sizeof(AnimTransition_t) + sizeof(AnimTransList_t));
    to->count = from->count;
    for (unsigned int i = 0; i < to->count; i++)
        CopyTransEntry(&to->entries[i], &from->entries[i]);
    return to;
}

inline void CopyStateAnim(AnimStateAnim_t* from, AnimStateAnim_t* to) {
    to->values[0] = from->values[0];
    to->values[1] = from->values[1];
    to->values[2] = from->values[2];
    to->values[3] = from->values[3];
    to->values[4] = from->values[4];
    to->blob = CopyBlob(from->blob);
}
inline AnimStateAnimList_t* CopyStateAnimList(AnimStateAnimList_t* from) {
    AnimStateAnimList_t* to = (AnimStateAnimList_t*)AnimCopyAlloc((from->count - 1) * sizeof(AnimStateAnim_t) + sizeof(AnimStateAnimList_t));
    to->count = from->count;
    for (unsigned int i = 0; i < to->count; i++)
        CopyStateAnim(&from->entries[i], &to->entries[i]);
    return to;
}

void CopyStateDef(AnimEmotStateDef_t* dst, AnimEmotStateDef_t* src) {
    dst->id = src->id;
    dst->flags = src->flags;
    dst->callback = src->callback;
    dst->transitions = CopyTransList(src->transitions);
    dst->blob = CopyBlob(src->blob);
    dst->anims = CopyStateAnimList(src->anims);
}


void DMAFromARAMCallback(unsigned long);

struct FrameFile_t {
    unsigned char field00[24];
    unsigned long* frames;
};

extern unsigned char g_frame_cache[0xc800];
extern int g_frame_cache_used;
extern volatile bool g_bDMAToMRAM;
extern ARQRequest g_fromARAM_request; // file-local in the original

static inline unsigned char* DMAFrame(unsigned long src, int length) {
    unsigned char* cache;
    int skip;
    unsigned long aligned;
    int size;
    aligned = src & ~31;
    skip = src - aligned;
    length += skip;
    size = (length + 31) & ~31;
    if (size > 0xc800 - g_frame_cache_used)
        g_frame_cache_used = 0;
    cache = g_frame_cache + g_frame_cache_used;
    OSGetTick();
    DCFlushRange(cache, size);
    g_bDMAToMRAM = true;
    ARQPostRequest(&g_fromARAM_request, 0, 1, 1, aligned, (unsigned long)cache, size, DMAFromARAMCallback);
    while (g_bDMAToMRAM) {
    }
    cache += skip;
    g_frame_cache_used += size;
    return cache;
}

extern "C" unsigned char* AnimFileGetFrame(FrameFile_t* file, int frame) {
    unsigned long start = file->frames[frame];
    return DMAFrame(start, file->frames[frame + 1] - start);
}


extern void* g_ARAM_poolman;
extern ARQRequest g_toARAM_request;
extern unsigned long end;
extern volatile bool g_bDMAToARAM;
void DMAToARAMCallback(unsigned long);

void UploadFramesToARAM(AnimFileFormat_t* file, unsigned char* start, unsigned char* finish) {
    unsigned char* aligned;
    unsigned long size;
    int lead;
    unsigned long length;
    unsigned long aram;
    unsigned long begin;
    aligned = (unsigned char*)((unsigned long)start & ~31);
    size = finish - start;
    lead = start - aligned;
    length = finish - start;
    length += lead;
    length = (length + 31) & ~31;
    aram = ARAM_alloc(g_ARAM_poolman, "Animation", length, 0);
    begin = OSGetTick();
    DCFlushRange(aligned, length);
    g_bDMAToARAM = true;
    ARQPostRequest(&g_toARAM_request, 0, 0, 1, (unsigned long)aligned, aram, length, DMAToARAMCallback);
    while (g_bDMAToARAM)
        VIWaitForRetrace();
    DebugMsg("Transfered %d bytes of animation frame data to ARAM in %f seconds\n", length,
             (float)(end - begin) / (float)(*(unsigned long*)0x800000F8 / 4));
    for (int i = 0; i < file->numFrames; i++)
        file->frames[i] = aram + (file->frames[i] - (unsigned long)start) + lead;
    file->frames[file->numFrames] = aram + size + lead;
}


void DMAToARAMCallback(unsigned long) {
    end = OSGetTick();
    g_bDMAToARAM = false;
}

void DMAFromARAMCallback(unsigned long) {
    end = OSGetTick();
    g_bDMAToMRAM = false;
}

struct AnimFileAnimItem_t {
    unsigned short id;
    unsigned short pad;
    int data;
};

struct AnimFileAnimList_t {
    unsigned int count;
    AnimFileAnimItem_t items[1];
};

struct AnimFileState_t {
    unsigned short id;
    unsigned short pad;
    int callbackid;
    AnimFileAnimList_t* anims;
    void* info;
    void* transitions;
};

struct AnimFileStateTable_t {
    unsigned int count;
    AnimFileState_t states[1];
};

struct AnimFileView_t {
    unsigned char field00[8];
    unsigned short objecttype;
    unsigned short filenum;
    unsigned char field0c[4];
    AnimFileStateTable_t* states;
    void** animations;
};

struct AnimFileTransEntry_t {
    AnimFileView_t* file;
    unsigned short filenum;
    unsigned short pad;
    void* transitions;
};

struct AnimFileTransList_t {
    unsigned short count;
    unsigned short max;
    unsigned char field04[4];
    AnimFileTransEntry_t entries[1];
};

struct AnimFileAnimEntry_t {
    AnimFileView_t* file;
    unsigned short id;
    unsigned short pad;
    int data;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    unsigned short max;
    AnimFileAnimEntry_t entries[1];
};

extern "C" {

void* AnimFileGetStateInfo(AnimFileView_t* file, unsigned int id) {
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id)
            return file->states->states[i].info;
    }
    return 0;
}

void AnimFileGetStateTransList(AnimFileView_t* file, unsigned int id, AnimFileTransList_t* list) {
    if (list->count >= list->max)
        return;
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id) {
            void* transitions = file->states->states[i].transitions;
            if (!transitions)
                return;
            if (!*(unsigned int*)transitions)
                return;
            list->entries[list->count].file = file;
            list->entries[list->count].filenum = file->filenum;
            list->entries[list->count].transitions = file->states->states[i].transitions;
            list->count++;
            return;
        }
    }
}

void AnimFileGetStateAnimList(AnimFileView_t* file, unsigned int id, AnimFileStateAnimList_t* list) {
    if (list->count >= list->max)
        return;
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id) {
            for (unsigned int j = 0; j < file->states->states[i].anims->count; j++) {
                if (list->count >= list->max)
                    return;
                list->entries[list->count].file = file;
                list->entries[list->count].id = file->states->states[i].anims->items[j].id;
                list->entries[list->count].data = file->states->states[i].anims->items[j].data;
                list->count++;
            }
            return;
        }
    }
}

int AnimFileGetStateCallbackID(AnimFileView_t* file, unsigned int id) {
    for (unsigned int i = 0; i < file->states->count; i++) {
        if (file->states->states[i].id == id)
            return file->states->states[i].callbackid;
    }
    return -1;
}

void* AnimFileGetAnimation(AnimFileView_t* file, int index) {
    return file->animations[index + 1];
}

unsigned short AnimFileGetFileNum(AnimFileView_t* file) {
    return file->filenum;
}

unsigned short AnimFileGetObjectType(AnimFileView_t* file) {
    return file->objecttype;
}

}

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned int& value) { ChangeEndian(*reinterpret_cast<int*>(&value)); }
inline void ChangeEndian(unsigned short& value) { ChangeEndian(*reinterpret_cast<short*>(&value)); }
inline void ChangeEndian(long& value) { ChangeEndian(*reinterpret_cast<unsigned int*>(&value)); }
template <class T> inline void ChangeEndian(T*& value) { ChangeEndian(*reinterpret_cast<unsigned int*>(&value)); }
inline void SwapShort(unsigned short& value) { ChangeEndian(value); }



struct AnimFilePluginData_t;
void EndianSwapAnimPlugIn(AnimFilePluginData_t&, int);

inline void EndianSwap(AnimFileFormat_t& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.field08);
    ChangeEndian(value.field0A);
    ChangeEndian(value.numFrames);
    ChangeEndian(value.field0E);
    ChangeEndian(*reinterpret_cast<int*>(&value.states));
    ChangeEndian(*reinterpret_cast<int*>(&value.animations));
    ChangeEndian(*reinterpret_cast<int*>(&value.frames));
}

// Views of the file as loaded (offsets not yet relocated) used by
// AnimFileRelocate.
struct RelCompTable_t { unsigned short count; };
struct RelCompPairTable_t { unsigned short count; };
struct RelCompressedData_t {
    int magic;
    RelCompTable_t* table;
    RelCompPairTable_t* pairs;
};

struct RelEmotAnimation_t {
    unsigned char header[4];
    RelCompressedData_t* data;
    unsigned short frames[1];
};

struct RelEmotAnimList_t {
    long count;
    RelEmotAnimation_t* items[1];
};

struct RelStateAnim_t {
    unsigned short id;
    unsigned short pad;
    AnimFilePluginData_t* data;
};

struct RelStateAnimList_t {
    int count;
    RelStateAnim_t items[1];
};

struct RelTransBlob_t {
    short a, b, c, d;
    int e;
    short f, g;
};
inline void EndianSwap(RelTransBlob_t& blob) {
    ChangeEndian(*(unsigned short*)&blob.a);
    ChangeEndian(*(unsigned short*)&blob.b);
    ChangeEndian(*(unsigned short*)&blob.c);
    ChangeEndian(*(unsigned short*)&blob.d);
    ChangeEndian(*(unsigned int*)&blob.e);
    ChangeEndian(*(unsigned short*)&blob.f);
    ChangeEndian(*(unsigned short*)&blob.g);
}

struct RelTransition_t {
    unsigned short values[5];
    RelTransBlob_t* blob;
};

struct RelTransList_t {
    int count;
    RelTransition_t items[1];
};

inline void EndianSwap(RelStateAnim_t& item) {
    ChangeEndian(item.id);
    ChangeEndian(item.pad);
    ChangeEndian(*(unsigned int*)&item.data);
}
inline void EndianSwap(RelStateAnimList_t& list) {
    ChangeEndian(*(unsigned int*)&list.count);
    for (unsigned int i = 0; i < list.count; i++)
        EndianSwap(list.items[i]);
}
inline void EndianSwap(RelTransition_t& item) {
    ChangeEndian(item.values[0]);
    ChangeEndian(item.values[1]);
    ChangeEndian(item.values[2]);
    ChangeEndian(item.values[3]);
    ChangeEndian(item.values[4]);
    ChangeEndian(*(unsigned int*)&item.blob);
}
inline void EndianSwap(RelTransList_t& list) {
    ChangeEndian(*(unsigned int*)&list.count);
    for (unsigned int i = 0; i < list.count; i++)
        EndianSwap(list.items[i]);
}
struct RelEmotStateDef_t {
    unsigned short id;
    unsigned short flags;
    int callback;
    RelStateAnimList_t* anims;
    AnimFilePluginData_t* info;
    RelTransList_t* transitions;
};

struct RelEmotStateList_t {
    long count;
    RelEmotStateDef_t states[1];
};

struct RelFileFormat_t {
    int magic;
    int version;
    unsigned short objecttype;
    unsigned short filenum;
    unsigned short numFrames;
    unsigned short field0e;
    RelEmotStateList_t* states;
    RelEmotAnimList_t* animations;
    int* frames;
};

inline void EndianSwap(RelEmotAnimList_t& list) {
    ChangeEndian(*(unsigned int*)&list.count);
    for (unsigned int i = 0; i < list.count; i++)
        ChangeEndian(*(unsigned int*)&list.items[i]);
}

inline int AnimFrameCount(RelEmotAnimation_t* anim, RelFileFormat_t* file, long index) {
    unsigned int bytes;
    int count;
    if (anim->data) {
        bytes = (char*)anim->data - (char*)anim->frames;
        count = bytes / 2;
    } else {
        if (index < file->animations->count - 1U)
            bytes = (char*)file->animations->items[index + 1] - (char*)file->animations->items[index];
        else
            bytes = (char*)file->animations - (char*)file->animations->items[index];
        bytes -= 10;
        count = bytes / 2;
    }
    return count;
}

inline void SwapAnimFrames(RelEmotAnimation_t* anim, RelFileFormat_t* file, long index) {
    unsigned int bytes;
    int count;
    if (anim->data) {
        bytes = (char*)anim->data - (char*)anim->frames;
        count = bytes / 2;
    } else {
        if (index < file->animations->count - 1U)
            bytes = (char*)file->animations->items[index + 1] - (char*)file->animations->items[index];
        else
            bytes = (char*)file->animations - (char*)file->animations->items[index];
        bytes -= 10;
        count = bytes / 2;
    }
    for (int k = 0; k < count; k++)
        ChangeEndian(anim->frames[k]);
}

inline void RelocateCompressed(RelCompressedData_t* data, RelFileFormat_t* file) {
    {
        ChangeEndian(*(unsigned int*)&data->table);
        ChangeEndian(*(unsigned int*)&data->pairs);
        data->table = (RelCompTable_t*)((char*)data->table + (int)file);
        data->pairs = (RelCompPairTable_t*)((char*)data->pairs + (int)file);
        ChangeEndian(data->pairs->count);
        ChangeEndian(data->table->count);
    }
}

extern "C" void AnimFileRelocate(RelFileFormat_t* file, const char* name) {
    EndianSwap(*(AnimFileFormat_t*)file);
    if (file->magic != 'EMT\0') {
        const char* def = "no_name_specified";
        if (!name)
            name = def;
        printf("The EMT file %s was exported with an old version of eMotion\n", name);
    } else if (file->version != 1U) {
        printf("The EMT file %s was exported with an different version of eMotion\n", name);
        printf("EMT version = %d, code version = %d", file->version, 1);
    }
    file->states = (RelEmotStateList_t*)((char*)file->states + (int)file);
    file->animations = (RelEmotAnimList_t*)((char*)file->animations + (int)file);
    file->frames = (int*)((char*)file->frames + (int)file);
    int i;
    for (i = 0; i < file->numFrames; i++) {
        ChangeEndian(*(unsigned int*)&file->frames[i]);
        file->frames[i] += (int)file;
    }

    ChangeEndian(file->states->count);
    for (i = 0; i < file->states->count; i++) {
        RelEmotStateDef_t* state = &file->states->states[i];
        SwapShort(state->id);
        SwapShort(state->flags);
        ChangeEndian(*(unsigned int*)&state->callback);
        ChangeEndian(state->anims);
        ChangeEndian(state->info);
        ChangeEndian(state->transitions);
        state->callback = AnimStGetCallbackIndexFromID(state->callback);

        state->anims = (RelStateAnimList_t*)((char*)state->anims + (int)file);
        EndianSwap(*state->anims);
        int j;
        for (j = 0; j < state->anims->count; j++) {
            if (state->anims->items[j].data) {
                state->anims->items[j].data = (AnimFilePluginData_t*)((char*)state->anims->items[j].data + (int)file);
                EndianSwapAnimPlugIn(*state->anims->items[j].data, state->callback);
            }
        }
        if (state->info) {
            state->info = (AnimFilePluginData_t*)((char*)state->info + (int)file);
            EndianSwapAnimPlugIn(*state->info, state->callback);
        }

        state->transitions = (RelTransList_t*)((char*)state->transitions + (int)file);
        EndianSwap(*state->transitions);
        for (j = 0; j < state->transitions->count; j++) {
            if (state->transitions->items[j].blob) {
                state->transitions->items[j].blob = (RelTransBlob_t*)((char*)state->transitions->items[j].blob + (int)file);
                EndianSwap(*state->transitions->items[j].blob);
            }
        }
    }

    EndianSwap(*file->animations);
    for (i = 0; i < file->animations->count; i++) {
        file->animations->items[i] = (RelEmotAnimation_t*)((char*)file->animations->items[i] + (int)file);
        ChangeEndian(file->animations->items[i]->data);
        if (file->animations->items[i]->data) {
            file->animations->items[i]->data = (RelCompressedData_t*)((char*)file->animations->items[i]->data + (int)file);
            ChangeEndian(*(unsigned int*)&file->animations->items[i]->data->magic);
            SwapAnimFrames(file->animations->items[i], file, i);
            if (file->animations->items[i]->data->magic == 'SKEL')
                RelocateCompressed(file->animations->items[i]->data, file);
        }
    }
    for (i = 0; i < file->animations->count; i++) {
        if (!file->animations->items[i]->data)
            SwapAnimFrames(file->animations->items[i], file, i);
    }
}
