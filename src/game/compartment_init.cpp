// A fragment of compartment.cpp (0x800761c8): CCompartment::Init fixes up a
// loaded compartment, as partial source. Loaded files store offsets, which
// offsetPtr turns into pointers. The file's .bss block is laid out below with
// views that have no constructors; this unit does not construct the file's
// globals. The Shape, palette and CPT record views and their member names are
// inferred from this function's accesses.
extern "C" {
void OSReport(const char*, ...);
void DCFlushRange(void*, unsigned long);
enum GXTexFmt { GX_TF_RGBA8 = 6, GX_TF_CMPR = 14 };
enum GXCITexFmt { GX_TF_C4 = 8, GX_TF_C8 = 9 };
enum GXTexWrapMode { GX_CLAMP, GX_REPEAT };
enum GXTlutFmt { GX_TL_RGB5A3 = 2 };
void GXInitTexObj(void*, void*, unsigned short, unsigned short, GXTexFmt, GXTexWrapMode, GXTexWrapMode, unsigned char);
void GXInitTexObjCI(void*, void*, unsigned short, unsigned short, GXCITexFmt, GXTexWrapMode, GXTexWrapMode, unsigned char, unsigned long);
void GXInitTlutObj(void*, void*, GXTlutFmt, unsigned short);
}
inline void* operator new(unsigned long, void* p) { return p; }
template <class T> void offsetPtr(T*&, int);

struct ShapeFile {
    unsigned char unknown00[0x14];
    int m_shapeOffset;
};
struct ShapeHeader {
    unsigned int m_type : 8;
    int m_paletteOffset : 24;
    short m_width;
    short m_height;
    unsigned char unknown08[8];
    unsigned char m_pixels[1];
};
struct PaletteHeader {
    unsigned char m_type;
    unsigned char unknown01[3];
    short m_count;
    unsigned char unknown06[10];
    unsigned char m_colors[1];
};

class CDmaPacket;
class CDmaTag;

// CRenderBin's non-polymorphic base, so the virtual table pointer is at +0x1c (see
// renderlist_finish.cpp, which reads it there).
struct CRenderBinData {
    CRenderBinData() : m_prev(0), m_next(0), m_head(0), m_tail(0), m_field10(0), m_priority(3) {}
    class CRenderBin* m_prev;
    class CRenderBin* m_next;
    class CRenderBin* m_head;
    class CRenderBin* m_tail;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    CRenderBin(int priority) { m_priority = priority; }
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};
class CPTMaterial : public CRenderBin {
public:
    CPTMaterial() {}
    virtual ~CPTMaterial();
};
class CCPTMatBin : public CPTMaterial {
public:
    CCPTMatBin(int base) { Load(base); }
    virtual ~CCPTMatBin();
    void Load(int base);
    unsigned char unknown20[8];
    unsigned char m_texture[0x20]; // +0x28
    unsigned char m_tlut[0xc];     // +0x48
    int m_hasPalette;   // +0x54
    unsigned char unknown58[8];
    ShapeFile* m_shape; // +0x60
    int m_flags;        // +0x64: 8 wraps the texture, 2 redirects to a global bin
    int m_field68;
    CCPTMatBin* m_global; // +0x6c: an index, until it's fixed up
};

// The shape's palette record, when its type is known (as in staticmesh_init.cpp and
// texture_load.cpp).
static inline PaletteHeader* GetPalette(ShapeHeader* header) {
    int offset = header->m_paletteOffset;
    if (!offset)
        return 0;
    PaletteHeader* palette = (PaletteHeader*)((char*)header + offset);
    if (palette->m_type == 0x20 || palette->m_type == 0x21 || palette->m_type == 0x30 || palette->m_type == 0x31 || palette->m_type == 0x32)
        return palette;
    return 0;
}

inline void CCPTMatBin::Load(int base) {
    offsetPtr(m_shape, base);
    if (!m_shape)
        return;
    ShapeHeader* header = (ShapeHeader*)((char*)m_shape + m_shape->m_shapeOffset);
    if (((int)header & 0x10) != 0x10)
        return;
    PaletteHeader* palette = GetPalette(header);
    m_hasPalette = palette != 0;
    unsigned int type = header->m_type;
    switch (type) {
    case 0x18:
    case 0x19: {
        GXCITexFmt format = type == 0x18 ? GX_TF_C4 : GX_TF_C8;
        GXInitTexObjCI(m_texture, header->m_pixels, header->m_width, header->m_height,
                       format, (m_flags & 8) ? GX_REPEAT : GX_CLAMP, (m_flags & 8) ? GX_REPEAT : GX_CLAMP, 0, 0);
        GXInitTlutObj(m_tlut, palette->m_colors, GX_TL_RGB5A3, palette->m_count);
        break;
    }
    case 0x16:
    case 0x1e: {
        GXTexFmt format = type == 0x1e ? GX_TF_CMPR : GX_TF_RGBA8;
        GXInitTexObj(m_texture, header->m_pixels, header->m_width, header->m_height,
                     format, (m_flags & 8) ? GX_REPEAT : GX_CLAMP, (m_flags & 8) ? GX_REPEAT : GX_CLAMP, 0);
        break;
    }
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
    unsigned char m_unknown2b : 2; // +0x2b
    unsigned char m_enabled : 1;
    unsigned char m_unknown2b3 : 2;
    unsigned char m_numAlternates : 3;
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
static inline void WriteCP(char* p, char reg, unsigned long v) {
    p[0] = 8;
    p[1] = reg;
    *(unsigned long*)(p + 2) = v;
}
static inline void WriteVCD(CPTDisplayList* list, char* p) {
    WriteCP(p + 0, -96, (unsigned long)list->m_positions & 0x3fffffff);
    WriteCP(p + 6, -80, 12);
    WriteCP(p + 12, -95, (unsigned long)list->m_normals & 0x3fffffff);
    WriteCP(p + 18, -79, 3);
    WriteCP(p + 24, -94, (unsigned long)list->m_colors & 0x3fffffff);
    WriteCP(p + 30, -78, 4);
    WriteCP(p + 36, -92, (unsigned long)list->m_uvs & 0x3fffffff);
    WriteCP(p + 42, -76, 8);
}
static inline void FinishDL(CPTDisplayList* list, char* p, int stride) {
    p[48] = 0;
    { unsigned short v = list->m_count; p[49] = 0x98; *(unsigned short*)(p + 50) = v; }
    int size = list->m_count * stride + 52;
    if (size & 0x1f)
        size += 32 - (size & 0x1f);
    list->m_count = size;
}
// The texture-swap loop's copy of the writer (the original repeats the block).
static inline void WriteSwapVCD(CPTDisplayList* list, char* p) {
    WriteCP(p + 0, -96, (unsigned long)list->m_positions & 0x3fffffff);
    WriteCP(p + 6, -80, 12);
    {
        // Same command as in WriteVCD, but the address is masked in a pointer local.
        signed char* normals = list->m_normals;
        normals = (signed char*)((unsigned long)normals & 0x3fffffff);
        WriteCP(p + 12, -95, (unsigned long)normals);
    }
    WriteCP(p + 18, -79, 3);
    WriteCP(p + 24, -94, (unsigned long)list->m_colors & 0x3fffffff);
    WriteCP(p + 30, -78, 4);
    WriteCP(p + 36, -92, (unsigned long)list->m_uvs & 0x3fffffff);
    WriteCP(p + 42, -76, 8);
}
static inline void BuildSwapDL(CPTDisplayList* list, int stride) {
    char* p = (char*)list->m_commands;
    WriteSwapVCD(list, p);
    FinishDL(list, p, stride);
}
static inline void BuildDL(CPTDisplayList* list, int stride) {
    char* p = (char*)list->m_commands;
    WriteVCD(list, p);
    FinishDL(list, p, stride);
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
    CUcodeRenderBin(int priority) : CRenderBin(priority) {}
    virtual ~CUcodeRenderBin();
};
class CCptSimpleBin : public CUcodeRenderBin {
public:
    CCptSimpleBin(int priority) : CUcodeRenderBin(priority), m_field20(0) {}
    virtual ~CCptSimpleBin();
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
    bool m_field20;
    unsigned char unknown21[15];
};
class CCptShadowCleanUpBin : public CRenderBin {
public:
    CCptShadowCleanUpBin() : CRenderBin(3) {}
    virtual ~CCptShadowCleanUpBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
    unsigned char unknown[0xc];
};
class CCptShadowBin : public CUcodeRenderBin {
public:
    CCptShadowBin(CCptShadowCleanUpBin* cleanUp) : CUcodeRenderBin(5), m_field20(0) { m_head = cleanUp; }
    virtual ~CCptShadowBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
    int m_field20;
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

/* view (inferred): the file's .bss block in original order up to the
   texture-swap cache, with exact sizes; the objects are viewed without
   constructors so that no static initialiser is compiled (the matrices are
   CMatrix, the GameCube one 3x4, g_CamPos a CVector3), and each object with a
   destructor follows its destructor-chain record (@537..@542) */
struct MatrixView {
    float m[16];
} __attribute__((aligned(16)));
struct CamPosView {
    float v[4];
};
struct DestructorChainView {
    unsigned char unknown00[12];
};
struct CompartmentView {
    double unknown00[11];
    CPTData* m_data; // +0x58
    unsigned char unknown5c[0xc];
};
struct SimpleBinView {
    CRenderBin* m_prev;
    CRenderBin* m_next;
    CRenderBin* m_head;
    unsigned char unknown0c[0x18];
};
struct ShadowCleanUpBinView {
    unsigned char unknown00[0x20];
};
struct TextureSwapCacheView {
    TextureSwapEntry m_entries[32][512];
    unsigned char m_enabled[32];
};
static MatrixView g_ModelToScreenMatrix;
static MatrixView g_ModelToClipMatrix;
static MatrixView g_ClipToScreenMatrix;
static float g_WorldToCameraGCMatrix[3][4];
static CamPosView g_CamPos;
static DestructorChainView s_globalCptChain;
static CompartmentView g_GlobalCpt;
static DestructorChainView s_simpleBinChain;
static SimpleBinView g_CptSimpleBin;
static DestructorChainView s_simpleAlphaBinChain;
static SimpleBinView g_CptSimpleAlphaBin;
static DestructorChainView s_shadowBinChain;
static SimpleBinView g_CptShadowBin;
static DestructorChainView s_shadowCleanUpBinChain;
static ShadowCleanUpBinView g_CptShadowCleanUpBin;
static DestructorChainView s_textureSwapCacheChain;
static TextureSwapCacheView g_TextureSwapCache;

// Links a compartment's bins in front of a global list.
static inline void LinkBins(SimpleBinView& list, CCPTMatBin*& bins, int& count) {
    if (bins) {
        CRenderBin* last = &bins[count - 1];
        CRenderBin* old = list.m_head;
        list.m_head = bins;
        last->m_next = old;
        if (old)
            old->m_prev = last;
    }
}

void CCompartment::Init() {
    int base;
    CCPTMatBin* previous;
    CCPTMatBin* bin;
    int i;
    int n;
    CPTChunk* chunk;
    CPTNode* node;
    CPTTexSwap* swap;
    int j;
    unsigned int k;
    base = (int)m_data;
    offsetPtr(m_data->m_simple, base);
    offsetPtr(m_data->m_alpha, base);
    offsetPtr(m_data->m_chunks, base);
    offsetPtr(m_data->m_nodes, base);
    offsetPtr(m_data->m_swaps, base);

    previous = 0;
    bin = m_data->m_simple;
    for (i = 0; i < m_data->m_numSimple; i++, bin++) {
        bin = new (bin) CCPTMatBin(base);
        if (previous)
            previous->m_next = bin;
        bin->m_prev = previous;
        previous = bin;
        if (bin->m_flags & 2)
            bin->m_global = g_GlobalCpt.m_data->m_simple + (int)bin->m_global;
    }
    previous = 0;
    bin = m_data->m_alpha;
    for (n = 0; n < m_data->m_numAlpha; n++, bin++) {
        bin = new (bin) CCPTMatBin(base);
        if (previous)
            previous->m_next = bin;
        bin->m_prev = previous;
        previous = bin;
        if (bin->m_flags & 2)
            bin->m_global = g_GlobalCpt.m_data->m_alpha + (int)bin->m_global;
    }

    chunk = m_data->m_chunks;
    for (i = 0; i < m_data->m_numChunks; i++, chunk++) {
        offsetPtr(chunk->m_node, base);
        offsetPtr(chunk->m_bin, base);
        offsetPtr((void*&)chunk->m_list, base);
        CPTDisplayList* list = chunk->m_list;
        if (list) {
            offsetPtr(list->m_positions, base);
            offsetPtr(list->m_normals, base);
            offsetPtr(list->m_uvs, base);
            offsetPtr(list->m_colors, base);
            offsetPtr((gcVertex*&)list->m_commands, base);
            int stride;
            switch (list->m_flags & 1) {
            default:
                stride = 4;
                break;
            case 1:
                stride = 8;
                break;
            }
            BuildDL(list, stride);
            DCFlushRange(list->m_commands, 52);
        }
        if (chunk->m_bin->m_flags & 2)
            chunk->m_bin = chunk->m_bin->m_global;
    }

    node = m_data->m_nodes;
    for (i = 0; i < m_data->m_numNodes; i++, node++) {
        offsetPtr(node->m_left, base);
        offsetPtr(node->m_right, base);
        offsetPtr(node->m_chunks, base);
        offsetPtr(node->m_swaps, base);
        swap = node->m_swaps;
        for (j = 0; j < node->m_numSwaps; j++, swap++) {
            offsetPtr(swap->m_node, base);
            offsetPtr((void*&)swap->m_list, base);
            offsetPtr(swap->m_bin, base);
            CPTDisplayList* list = swap->m_list;
            if (list) {
                offsetPtr(list->m_positions, base);
                offsetPtr(list->m_normals, base);
                offsetPtr(list->m_uvs, base);
                offsetPtr(list->m_colors, base);
                offsetPtr((gcVertex*&)list->m_commands, base);
                int stride;
                switch (list->m_flags & 1) {
                default:
                    stride = 4;
                    break;
                case 1:
                    stride = 8;
                    break;
                }
                BuildSwapDL(list, stride);
                DCFlushRange(list->m_commands, 52);
            }
            if (swap->m_bin->m_flags & 2)
                swap->m_bin = swap->m_bin->m_global;
            for (k = 0; k < swap->m_numAlternates; k++) {
                offsetPtr(swap->m_alternate[k], base);
                if (swap->m_alternate[k]->m_flags & 2)
                    swap->m_alternate[k] = swap->m_alternate[k]->m_global;
            }
        }
    }

    if (m_data->m_simple) {
        CRenderBin* last = &m_data->m_simple[m_data->m_numSimple - 1];
        CRenderBin* old = g_CptSimpleBin.m_head;
        g_CptSimpleBin.m_head = m_data->m_simple;
        last->m_next = old;
        if (old)
            old->m_prev = last;
    }
    if (m_data->m_alpha) {
        CRenderBin* last = &m_data->m_alpha[m_data->m_numAlpha - 1];
        CRenderBin* old = g_CptSimpleAlphaBin.m_head;
        g_CptSimpleAlphaBin.m_head = m_data->m_alpha;
        last->m_next = old;
        if (old)
            old->m_prev = last;
    }
    for (int n = 0; n < m_data->m_numSwaps;  n++)
        offsetPtr(m_data->m_swaps[n], base);

    // Restore cached texture swap state.
    if (m_field50 >= 0 && g_TextureSwapCache.m_enabled[m_field50]) {
        for (int m = 0; m < m_data->m_numSwaps;  m++) {
            CPTTexSwap* swap = m_data->m_swaps[m];
            swap->m_cached = g_TextureSwapCache.m_entries[m_field50][m].m_active;
            swap->m_enabled = g_TextureSwapCache.m_entries[m_field50][m].m_active != 0;
        }
    }
}
