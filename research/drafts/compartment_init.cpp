// compartment.cpp: fixes up a loaded compartment and creates the file's globals, as
// partial source. Loaded files store offsets, which offsetPtr turns into pointers.
extern "C" {
void OSReport(const char*, ...);
void DCFlushRange(void*, unsigned long);
void GXInitTexObj(void*, void*, unsigned short, unsigned short, int, int, int, unsigned char);
void GXInitTexObjCI(void*, void*, unsigned short, unsigned short, int, int, int, unsigned char, unsigned long);
void GXInitTlutObj(void*, void*, int, unsigned short);
}
inline void* operator new(unsigned long, void* p) { return p; }
template <class T> void offsetPtr(T*&, int);

struct ShapeFile {
    unsigned char unknown00[0x14];
    int m_shapeOffset;
};
struct ShapeHeader {
    unsigned int m_typeAndPalette; // type in the top byte, palette offset below
    unsigned short m_width;
    unsigned short m_height;
    unsigned char unknown08[8];
    unsigned char m_pixels[1];
};
struct PaletteHeader {
    unsigned char m_type;
    unsigned char unknown01[3];
    unsigned short m_count;
    unsigned char unknown06[10];
    unsigned char m_colors[1];
};

class CRenderBin {
public:
    CRenderBin(int priority = 3)
        : m_prev(0), m_next(0), m_head(0), m_tail(0), m_field10(0), m_priority(priority) {}
    virtual ~CRenderBin();
    CRenderBin* m_prev;
    CRenderBin* m_next;
    CRenderBin* m_head;
    CRenderBin* m_tail;
    int m_field10;
    int m_priority;
};
class CPTMaterial : public CRenderBin {
public:
    CPTMaterial() {}
    virtual ~CPTMaterial();
};
class CCPTMatBin : public CPTMaterial {
public:
    CCPTMatBin() {}
    virtual ~CCPTMatBin();
    void Load(int base);
    unsigned char m_texture[0x20]; // +0x28
    unsigned char m_tlut[0xc];     // +0x48
    int m_hasPalette;   // +0x54
    unsigned char unknown58[8];
    ShapeFile* m_shape; // +0x60
    int m_flags;        // +0x64: 8 wraps the texture, 2 redirects to a global bin
    int m_field68;
    CCPTMatBin* m_global; // +0x6c: an index, until it's fixed up
};

void CCPTMatBin::Load(int base) {
    offsetPtr(m_shape, base);
    if (!m_shape)
        return;
    ShapeHeader* header = (ShapeHeader*)((char*)m_shape + m_shape->m_shapeOffset);
    if (((unsigned int)header & 0x10) != 0x10)
        return;
    int paletteOffset = (int)(header->m_typeAndPalette << 8) >> 8;
    PaletteHeader* palette = 0;
    if (paletteOffset) {
        palette = (PaletteHeader*)((char*)header + paletteOffset);
        switch (palette->m_type) {
        case 0x20:
        case 0x21:
        case 0x30:
        case 0x31:
        case 0x32:
            break;
        default:
            palette = 0;
            break;
        }
    }
    m_hasPalette = palette != 0;
    unsigned char type = *(unsigned char*)header;
    int wrap = (m_flags >> 3) & 1;
    switch (type) {
    case 0x16:
    case 0x1e:
        GXInitTexObj(m_texture, header->m_pixels, header->m_width, header->m_height,
                     type == 0x1e ? 14 : 6, wrap, wrap, 0);
        break;
    case 0x18:
    case 0x19:
        GXInitTexObjCI(m_texture, header->m_pixels, header->m_width, header->m_height,
                       type == 0x18 ? 8 : 9, wrap, wrap, 0, 0);
        GXInitTlutObj(m_tlut, palette->m_colors, 2, palette->m_count);
        break;
    default:
        OSReport("Unknown shape type\n");
        break;
    }
}

struct gcVertex;
struct CPTDisplayList {
    unsigned short m_flags;
    unsigned short m_count;
    float* m_positions;
    unsigned long* m_colors;
    signed char* m_normals;
    short* m_uvs;
    unsigned char* m_commands;
};
struct CPTNode;
struct CPTChunk {
    CPTNode* m_node;
    unsigned char unknown04[4];
    CCPTMatBin* m_bin;
    CPTDisplayList* m_list;
    unsigned char unknown10[4];
};
struct CPTTexSwap {
    CPTNode* m_node;
    unsigned char unknown04[4];
    CCPTMatBin* m_bin;
    CPTDisplayList* m_list;
    unsigned char unknown10[4];
    CCPTMatBin* m_alternate[5]; // +0x14: count in the low bits of +0x2b
    unsigned char m_field28;
    unsigned char m_cached;     // +0x29
    unsigned char m_field2a;
    unsigned char m_flags;      // +0x2b
};
struct CPTNode {
    unsigned char unknown00[0x40];
    CPTNode* m_left;
    CPTNode* m_right;
    CPTChunk* m_chunks;
    unsigned char unknown4c[4];
    CPTTexSwap* m_swaps;
    int m_numSwaps;
    unsigned char unknown58[0x18];
};
struct CPTData {
    unsigned char unknown00[8];
    CCPTMatBin* m_simple;
    int m_numSimple;
    CCPTMatBin* m_alpha;
    int m_numAlpha;
    CPTChunk* m_chunks;
    int m_numChunks;
    CPTNode* m_nodes;
    int m_numNodes;
    CPTTexSwap** m_swaps;
    int m_numSwaps;
};

// Writes the vertex-array setup at the front of a chunk's display list. Each
// command is a 6-byte load (0x08, register, value): array base then stride.
static inline void BuildDisplayList(CPTDisplayList* list, int base) {
    offsetPtr(list->m_positions, base);
    offsetPtr(list->m_normals, base);
    offsetPtr(list->m_uvs, base);
    offsetPtr(list->m_colors, base);
    offsetPtr((gcVertex*&)list->m_commands, base);
    short stride = (list->m_flags & 1) == 1 ? 8 : 4;
    unsigned char* p = list->m_commands;
#define LOAD(off, reg, value) p[off] = 8; p[off + 1] = reg; *(unsigned int*)(p + off + 2) = value
    LOAD(0, 0xa0, (unsigned int)list->m_positions & 0x3fffffff);
    LOAD(6, 0xb0, 12);
    LOAD(12, 0xa1, (unsigned int)list->m_normals & 0x3fffffff);
    LOAD(18, 0xb1, 3);
    LOAD(24, 0xa2, (unsigned int)list->m_colors & 0x3fffffff);
    LOAD(30, 0xb2, 4);
    LOAD(36, 0xa4, (unsigned int)list->m_uvs & 0x3fffffff);
    LOAD(42, 0xb4, 8);
#undef LOAD
    p[48] = 0;
    p[49] = 0x98;
    *(unsigned short*)(p + 50) = list->m_count;
    unsigned short size = list->m_count * stride + 52;
    if (size & 0x1f)
        size += 32 - (size & 0x1f);
    list->m_count = size;
    DCFlushRange(list->m_commands, 52);
}

class CDB;
class CCDBObject {
public:
    CCDBObject(CDB*);
    unsigned char unknown[0x28];
};
class IDestructible {
public:
    IDestructible() {}
    virtual ~IDestructible();
};
class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};
class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};
class ISceneNode : public IObserver {
public:
    ISceneNode() : m_field04(0), m_field08(0), m_field0c(0), m_field10(0), m_field14(0), m_field18(0), m_field1c(0), m_field20(0) {}
    virtual ~ISceneNode();
    int m_field04, m_field08, m_field0c, m_field10, m_field14, m_field18, m_field1c, m_field20;
};
class CCompartment : public ISceneNode {
public:
    CCompartment() : m_db(0), m_field50(-1), m_field54(0), m_data(0), m_field5c(0), m_field60(true), m_field61(false) {}
    virtual ~CCompartment();
    void Init();
    int m_pad24;
    CCDBObject m_db;
    int m_field50;      // +0x50: texture swap slot (Init), or -1
    int m_field54;
    CPTData* m_data;
    int m_field5c;
    bool m_field60;
    bool m_field61;
};

class CUcodeRenderBin : public CRenderBin {
public:
    CUcodeRenderBin(int priority) : CRenderBin(priority), m_pending(0) {}
    virtual ~CUcodeRenderBin();
    int m_pad18;
    int m_pending;
};
class CCptSimpleBin : public CUcodeRenderBin {
public:
    CCptSimpleBin(int priority) : CUcodeRenderBin(priority) {}
    virtual ~CCptSimpleBin();
    unsigned char unknown24[12];
};
class CCptShadowCleanUpBin : public CRenderBin {
public:
    CCptShadowCleanUpBin() : CRenderBin(3) {}
    virtual ~CCptShadowCleanUpBin();
    unsigned char unknown[0x10];
};
class CCptShadowBin : public CUcodeRenderBin {
public:
    CCptShadowBin(CCptShadowCleanUpBin* cleanUp) : CUcodeRenderBin(5) { m_head = cleanUp; }
    virtual ~CCptShadowBin();
    unsigned char unknown24[12];
};
struct TextureSwapEntry {
    unsigned short m_frames : 3;
    unsigned short m_active : 1;
    unsigned short m_index : 12;
};
class CTextureSwapCache {
public:
    CTextureSwapCache() {
        for (int i = 0; i < 32; i++) {
            m_enabled[i] = 0;
            for (int j = 0; j < 512; j++) {
                m_entries[i][j].m_frames = 0;
                m_entries[i][j].m_index = 0;
                m_entries[i][j].m_active = 0;
            }
        }
    }
    ~CTextureSwapCache();
    TextureSwapEntry m_entries[32][512];
    unsigned char m_enabled[32];
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

CMatrix g_ModelToClipMatrix;
CMatrix g_ClipToScreenMatrix;
CMatrix g_ModelToScreenMatrix;
CCompartment g_GlobalCpt;
CCptSimpleBin g_CptSimpleBin(2);
CCptSimpleBin g_CptSimpleAlphaBin(4);
CCptShadowCleanUpBin g_CptShadowCleanUpBin;
CCptShadowBin g_CptShadowBin(&g_CptShadowCleanUpBin);
CTextureSwapCache g_TextureSwapCache;

struct TextureCacheView {
    TextureCacheView() : m_first(0), m_second(0), m_third(0) {}
    int m_first;
    int m_second;
    int m_third;
};
TextureCacheView g_TextureCache;
struct LightPoolView {
    LightPoolView() : m_base(0), m_count(0), m_free(0), m_capacity(0), m_flags(0) {}
    unsigned char m_nodes[3072];
    void* m_base;
    int m_count;
    void* m_free;
    int m_capacity;
    int m_flags;
};
LightPoolView g_LightPool;
extern char SPR_BEGIN[];
extern char SPR_END[];
class CSPRStack {
public:
    CSPRStack() : m_begin(SPR_BEGIN), m_end(SPR_END), m_top(SPR_BEGIN) {}
    ~CSPRStack();
    char* m_begin;
    char* m_end;
    char* m_top;
};
CSPRStack g_SPRStack;

// Links a compartment's bins in front of a global list.
static inline void LinkBins(CRenderBin& list, CCPTMatBin* bins, int count) {
    if (bins) {
        CRenderBin* last = &bins[count - 1];
        CRenderBin* old = list.m_head;
        last->m_next = old;
        list.m_head = bins;
        if (old)
            old->m_prev = last;
    }
}

void CCompartment::Init() {
    int base = (int)m_data;
    offsetPtr(m_data->m_simple, base);
    offsetPtr(m_data->m_alpha, base);
    offsetPtr(m_data->m_chunks, base);
    offsetPtr(m_data->m_nodes, base);
    offsetPtr(m_data->m_swaps, base);

    CCPTMatBin* previous = 0;
    CCPTMatBin* bin = m_data->m_simple;
    int i;
    for (i = 0; i < m_data->m_numSimple; i++) {
        new (bin) CCPTMatBin;
        bin->Load(base);
        if (previous)
            previous->m_next = bin;
        bin->m_prev = previous;
        if (bin->m_flags & 2)
            bin->m_global = g_GlobalCpt.m_data->m_simple + (int)bin->m_global;
        previous = bin;
        bin++;
    }
    previous = 0;
    bin = m_data->m_alpha;
    for (i = 0; i < m_data->m_numAlpha; i++) {
        new (bin) CCPTMatBin;
        bin->Load(base);
        if (previous)
            previous->m_next = bin;
        bin->m_prev = previous;
        if (bin->m_flags & 2)
            bin->m_global = g_GlobalCpt.m_data->m_alpha + (int)bin->m_global;
        previous = bin;
        bin++;
    }

    CPTChunk* chunk = m_data->m_chunks;
    for (i = 0; i < m_data->m_numChunks; i++) {
        offsetPtr(chunk->m_node, base);
        offsetPtr(chunk->m_bin, base);
        offsetPtr((void*&)chunk->m_list, base);
        if (chunk->m_list)
            BuildDisplayList(chunk->m_list, base);
        if (chunk->m_bin->m_flags & 2)
            chunk->m_bin = chunk->m_bin->m_global;
        chunk++;
    }

    CPTNode* node = m_data->m_nodes;
    for (i = 0; i < m_data->m_numNodes; i++) {
        offsetPtr(node->m_left, base);
        offsetPtr(node->m_right, base);
        offsetPtr(node->m_chunks, base);
        offsetPtr(node->m_swaps, base);
        CPTTexSwap* swap = node->m_swaps;
        for (int j = 0; j < node->m_numSwaps; j++) {
            offsetPtr(swap->m_node, base);
            offsetPtr((void*&)swap->m_list, base);
            offsetPtr(swap->m_bin, base);
            if (swap->m_list)
                BuildDisplayList(swap->m_list, base);
            if (swap->m_bin->m_flags & 2)
                swap->m_bin = swap->m_bin->m_global;
            for (unsigned int k = 0; k < (swap->m_flags & 7); k++) {
                offsetPtr(swap->m_alternate[k], base);
                if (swap->m_alternate[k]->m_flags & 2)
                    swap->m_alternate[k] = swap->m_alternate[k]->m_global;
            }
            swap++;
        }
        node++;
    }

    LinkBins(g_CptSimpleBin, m_data->m_simple, m_data->m_numSimple);
    LinkBins(g_CptSimpleAlphaBin, m_data->m_alpha, m_data->m_numAlpha);
    for (i = 0; i < m_data->m_numSwaps; i++)
        offsetPtr(m_data->m_swaps[i], base);

    // Restore cached texture swap state.
    if (m_field50 >= 0 && g_TextureSwapCache.m_enabled[m_field50]) {
        for (i = 0; i < m_data->m_numSwaps; i++) {
            CPTTexSwap* swap = m_data->m_swaps[i];
            unsigned int active = g_TextureSwapCache.m_entries[m_field50][i].m_active;
            swap->m_cached = active;
            swap->m_flags = (swap->m_flags & 0xdf) | (active << 5);
        }
    }
}
