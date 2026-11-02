// A fragment of staticmesh.cpp (0x800f25bc): CStaticMesh::Init rebases the mesh
// file's pointers, constructs its materials in place (each loads its shape's
// texture object, and palette for indexed formats), rebases the chunks and
// writes each display list's vertex array header, rebases the nodes and links
// the material lists into the two simple mesh bins. The classes, globals and
// functions are named by the symbols; the record layouts and the member and
// inline names are inferred. The original's register use needs the writer split
// into inline steps and the declaration order below.
extern "C" {
void OSReport(const char*, ...);
void DCFlushRange(void*, unsigned long);
struct GXTexObj { unsigned char data[32]; };
struct GXTlutObj { unsigned char data[12]; };
void GXInitTexObj(GXTexObj*, void*, unsigned short, unsigned short, int, int, int, unsigned char);
void GXInitTexObjCI(GXTexObj*, void*, unsigned short, unsigned short, int, int, int, unsigned char, unsigned long);
void GXInitTlutObj(GXTlutObj*, void*, int, unsigned short);
}

struct Shape {
    unsigned int format : 8;
    int paletteOffset : 24;
    short width;
    short height;
    unsigned char unknown08[8];
    unsigned char data[1];
};

struct ShapeFile {
    unsigned char unknown00[20];
    int shapeOffset;
};

struct MSHVertex_GC;
struct MSHAttachNode;
struct MSHNode;
class CMSHMatBin;

template <class T> void offsetPtr(T*&, int);

static inline unsigned char* GetPalette(Shape* shape) {
    if (!shape->paletteOffset)
        return 0;
    unsigned char* palette = (unsigned char*)shape + shape->paletteOffset;
    if ((unsigned)*palette == ' ' || (unsigned)*palette == '!' || (unsigned char)(*palette - '0') <= 1 || (unsigned)*palette == '2')
        return palette;
    return 0;
}

struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}
    CMSHMatBin* data00;
    CMSHMatBin* m_next;
    CMSHMatBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
};

class MSHMaterial : public CRenderBin {
public:
    virtual ~MSHMaterial();
};

class CMSHMatBin : public MSHMaterial {
public:
    CMSHMatBin(int base);
    virtual ~CMSHMatBin();
    unsigned char unknown20[8];
    GXTexObj m_texObj;
    GXTlutObj m_tlutObj;
    int m_hasPalette;
    unsigned char unknown58[8];
    ShapeFile* m_shape;
    unsigned int m_flags;
    unsigned char unknown68[8];
};

inline void* operator new(unsigned long, void* p) { return p; }


static inline int IsWrapped(unsigned int f) { unsigned int x = f & 8; return (-x | x) >> 31; }
inline CMSHMatBin::CMSHMatBin(int base) {
    offsetPtr(m_shape, base);
    if (m_shape) {
        Shape* shape = (Shape*)((char*)m_shape + m_shape->shapeOffset);
        if (((int)shape & 0x10) == 16) {
            unsigned char* palette = GetPalette(shape);
            m_hasPalette = palette != 0;
            switch (shape->format) {
            case 24:
            case 25: {
                int format = 9;
                if (shape->format == 24)
                    format = 8;
                GXInitTexObjCI(&m_texObj, shape->data, shape->width, shape->height, format, IsWrapped(m_flags), (bool)((m_flags >> 3) & 1), 0, 0);
                GXInitTlutObj(&m_tlutObj, palette + 16, 2, *(short*)(palette + 4));
                break;
            }
            case 22:
            case 30: {
                int format = 6;
                if (shape->format == 30)
                    format = 14;
                unsigned int flags = m_flags;
GXInitTexObj(&m_texObj, shape->data, shape->width, shape->height, format, IsWrapped(flags), (bool)((flags >> 3) & 1), 0);
                break;
            }
            default:
                OSReport("Unknown shape type\n");
            }
        }
    }
}

struct MSHDisplayList {
    unsigned int flags;
    unsigned char unknown04[8];
    unsigned int count;
    float* position;
    unsigned long* index;
    unsigned char* color;
    float* texcoord;
    MSHVertex_GC* commands;
};

struct MSHChunk {
    MSHNode* node;
    CMSHMatBin* material;
    void* list;
    unsigned char unknown0c[20];
};

struct MSHNode {
    MSHChunk* chunk;
    unsigned char unknown04[4];
    MSHNode* first;
    MSHNode* second;
    unsigned char unknown10[80];
};

struct StaticMesh {
    unsigned char unknown00[8];
    CMSHMatBin* materials;
    int materialCount;
    CMSHMatBin* alphaMaterials;
    int alphaMaterialCount;
    MSHChunk* chunks;
    int chunkCount;
    MSHNode* nodes;
    int nodeCount;
    MSHAttachNode* attachNodes;
};

struct CRenderBinList {
    unsigned char unknown00[8];
    CMSHMatBin* m_children;

    void AddChildren(CMSHMatBin* first, int count) {
        CMSHMatBin* old = m_children;
        m_children = first;
        first[count - 1].m_next = old;
    }
};

extern CRenderBinList g_MshSimpleBin;
extern CRenderBinList g_MshSimpleAlphaBin;

class CStaticMesh {
public:
    void Init();

    unsigned char unknown00[4];
    StaticMesh* m_mesh;
};

static inline void WriteCP(char* p, char reg, unsigned long v) {
    p[0] = 8;
    p[1] = reg;
    *(unsigned long*)(p + 2) = v;
}

static inline void WriteVCD(MSHDisplayList* list, char* p) {
    WriteCP(p + 0, -96, (unsigned long)list->position & 0x3fffffff);
    WriteCP(p + 6, -80, 12);
    WriteCP(p + 12, -95, (unsigned long)list->color & 0x3fffffff);
    WriteCP(p + 18, -79, 3);
    WriteCP(p + 24, -94, (unsigned long)list->index & 0x3fffffff);
    WriteCP(p + 30, -78, 4);
    WriteCP(p + 36, -92, (unsigned long)list->texcoord & 0x3fffffff);
    WriteCP(p + 42, -76, 8);
}

static inline void FinishDL(MSHDisplayList* list, char* p) {
            p[48] = 0;
            if (list->flags & 4) {
                unsigned int c = list->count;
                p[49] = -104;
                *(short*)(p + 50) = c;
            } else {
                unsigned int c = list->count;
                p[49] = -112;
                *(short*)(p + 50) = c;
            }
            unsigned int size = list->count;
            int stride = 8;
            if (list->flags & 1)
                stride = 4;
            size = size * stride + 52;
            if (size & 31)
                size += 32 - (size & 31);
            list->count = size;
}

static inline void BuildDL(MSHDisplayList* list) {
    char* p = (char*)list->commands;
    WriteVCD(list, p);
    FinishDL(list, p);
}

void CStaticMesh::Init() {
    int base;
    CMSHMatBin* prev2;
    MSHDisplayList* list;
    CMSHMatBin* mat;
    CMSHMatBin* prev;
    int j;
    CMSHMatBin* mat2;
    int i;
    int n;
    MSHChunk* chunk;
    MSHNode* node;
    int k;
    CMSHMatBin* bin;
    base = (int)m_mesh;
    offsetPtr(m_mesh->materials, base);
    offsetPtr(m_mesh->alphaMaterials, base);
    offsetPtr(m_mesh->chunks, base);
    offsetPtr(m_mesh->nodes, base);
    offsetPtr(m_mesh->attachNodes, base);
    prev = 0;
    mat = m_mesh->materials;
    for (i = 0; i < m_mesh->materialCount; i++, mat = bin + 1) {
        bin = new (mat) CMSHMatBin(base);
        if (prev)
            prev->m_next = bin;
        bin->data00 = prev;
        prev = bin;
    }
    mat2 = m_mesh->alphaMaterials;
    prev2 = 0;
    for (n = 0; n < m_mesh->alphaMaterialCount; n++, mat2++) {
        new (mat2) CMSHMatBin(base);
        if (prev2)
            prev2->m_next = mat2;
        mat2->data00 = prev2;
        prev2 = mat2;
    }
    chunk = m_mesh->chunks;
    for (j = 0; j < m_mesh->chunkCount; j++, chunk++) {
        offsetPtr(chunk->node, base);
        offsetPtr(chunk->material, base);
        offsetPtr(chunk->list, base);
        list = (MSHDisplayList*)chunk->list;
        if (list) {
            offsetPtr(list->position, base);
            offsetPtr(list->color, base);
            offsetPtr(list->texcoord, base);
            offsetPtr(list->index, base);
            offsetPtr(list->commands, base);
            BuildDL(list);
            DCFlushRange(list->commands, 52);
        }
    }
    node = m_mesh->nodes;
    for (k = 0; k < m_mesh->nodeCount; k++, node++) {
        offsetPtr(node->first, base);
        offsetPtr(node->second, base);
        offsetPtr(node->chunk, base);
    }
    if (CMSHMatBin* first = m_mesh->materials) {
        CMSHMatBin* old = g_MshSimpleBin.m_children;
        g_MshSimpleBin.m_children = first;
        first[m_mesh->materialCount - 1].m_next = old;
    }
    if (CMSHMatBin* first = m_mesh->alphaMaterials) {
        CMSHMatBin* old = g_MshSimpleAlphaBin.m_children;
        g_MshSimpleAlphaBin.m_children = first;
        first[m_mesh->alphaMaterialCount - 1].m_next = old;
    }
}
