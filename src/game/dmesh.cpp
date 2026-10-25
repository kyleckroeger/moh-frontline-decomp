// The end of dmesh.cpp (0x800f6180): CDMesh::Draw (the object's parts queued
// into the mesh bins, remapped through the remap pool when a part's texture
// differs, with the camera's planes and fog kept for the bins), BindTextures
// (each material's texture looked up by name in the pack), BindSkeleton (the
// joint positions read from the skeleton), Init (once per mesh: the offsets
// relocated against the mesh, the cache flushed, each part bin constructed in
// place and its vertex offsets relocated, the parts linked in order unless
// the mesh is flagged as one block, and the list spliced into the weapon or
// world mesh bin), the weak CPart destructor, CDMesh::InitClass (the render
// list kept, both mesh bins registered with it, each given its finish bin as
// its child, and the remap pool's next index cleared), the weak
// CPart::GetName ("DMESH Part"), the weak CRemapBin and CPartBin destructors
// (each base's table pointer through the inline destructors), and the static
// initialisation of the file's render bins and remap pool, with the array
// destructor the compiler generates for the pool. The two mesh bins
// (CUcodeRenderBin-based; priority 3 and 8, flag cleared) and the two finish
// bins are built through the inline CRenderBinData and CRenderBin
// constructors with their destructors registered, the 32 remap bins are
// built through CRemapBin's constructor (__construct_array) with the array
// destructor registered, and the perspective matrix's inline constructor
// initialises the matrix class once. The classes, functions and objects are
// named by the symbols; the objects' sizes come from the symbols, and the
// members, the bin constructors' priority parameter, the base layout and
// GetName's result type are inferred. Init reads the placement-new result
// through a second pointer (a void* round trip), as the original keeps a
// register copy of it. Each bin declares an override defined elsewhere first
// so its global table stays elsewhere; CRemapBin's constructor is emitted
// elsewhere in the file, and the inline base destructors and tables are weak
// duplicates. The unit defines the file's .bss block (Draw addresses it
// through one base register); the render list and remap index are
// file-local .sbss objects declared extern here. The string links to its
// pool entry. Formerly also the dmesh_part_name, dmesh_cremapbin_dtor,
// dmesh_cpartbin_dtor and dmesh_cpart_dtor units. The rest of the file is in
// other units or not reconstructed.
class CDmaTag;
class CDmaPacket;

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    CMatrix& operator=(const CMatrix&);

    float m[4][4];
} __attribute__((aligned(16)));

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CUcodeRenderBin() {}
};

class CDMRenderBin : public CUcodeRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    CDMRenderBin(int priority) {
        m_priority = priority;
        m_flag = false;
    }
    virtual ~CDMRenderBin();

    bool m_flag;
};

class CFinishPartBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CFinishPartBin() {}
    virtual ~CFinishPartBin();
};

class CPart : public CRenderBin {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPart();
    const char* GetName();
};

class CPartBin : public CPart {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPartBin();

    int m_unknown20;
    unsigned long m_flags;
    unsigned long m_texture;
    unsigned long m_vertexCount;
    char* m_vertices;
    void* m_unknown34;
};

class CRemapBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CRemapBin();
    CRemapBin();

    CPartBin* m_bin;
    unsigned long m_remapData;
};

class CRenderList {
public:
    void Register(CRenderBin&);
    void Render(CRenderBin&, void*);
};

class CTexturePack {
public:
    int FindTexture(char*);
};

struct CharJointView {
    unsigned char unknown00[4];
    unsigned char* data;
};

struct CharSkel_t {
    unsigned char unknown00[12];
    struct {
        CharJointView* joint;
        unsigned char unknown04[16];
    }* joints;
};

struct CameraView {
    unsigned char unknown00[36];
    float nearPlane;
    float farPlane;
};

class CDrawContext {
public:
    unsigned char unknown00[200];
    CMatrix* m_projection;
    unsigned char unknown0cc[8];
    CameraView* m_camera;
    void* m_fog;
};

struct DMRenderPartView {
    unsigned short part;
    unsigned char unknown02[6];
};

struct DMRenderObject_T {
    unsigned char unknown000[336];
    unsigned long partCount;
    DMRenderPartView parts[1];
};

class CDMesh {
public:
    void Draw(CDrawContext&, DMRenderObject_T*);
    void BindTextures(CTexturePack*, char*);
    void BindSkeleton(CharSkel_t*);
    void Init(bool);
    static void InitClass(CRenderList*);

    unsigned char unknown00[5];
    unsigned char m_version;
    unsigned char unknown06[2];
    unsigned long m_flags;
    char m_name[20];
    unsigned long m_refCount;
    unsigned char* m_refs;
    unsigned long m_partCount;
    CPartBin* m_parts;
    unsigned char unknown30[4];
    char* m_textureNames;
    unsigned char unknown38[4];
    char* m_unknown3c;
    char* m_unknown40;
    char* m_unknown44;
    unsigned long m_jointCount;
    char* m_jointNames;
    short* m_jointPositions;
    unsigned char unknown54[8];
    unsigned long m_count5c;
    char* m_data60;
    unsigned long m_count64;
    char* m_data68;
    unsigned long m_count6c;
    char* m_data70;
};

inline void* operator new(unsigned long, void* place) { return place; }

extern "C" int stricmp(const char*, const char*);
extern "C" void* memcpy(void*, const void*, unsigned long);
extern "C" void DCStoreRange(void*, unsigned long);
int CharSkelGetJointByName(CharSkel_t*, char*);
void DebugMsg(const char*, ...);
extern bool g_bInMultiplayerMode;
extern void* g_pFog;
extern float g_NearPlane;
extern float g_FarPlane;
extern bool _finish_draw;
extern bool _finish_draw_weapon;

extern CRenderList* _renderList;
extern int _remap_next;

static CDMRenderBin _DMRenderBin(3);
static CDMRenderBin _DMRenderBinWeapon(8);
static CFinishPartBin _finish_bin;
static CFinishPartBin _finish_bin_weapon;
static CRemapBin _remap_pool[32];
static CMatrix g_PerspectiveMatrix;

void CDMesh::Draw(CDrawContext& context, DMRenderObject_T* object) {
    g_pFog = context.m_fog;
    g_NearPlane = context.m_camera->nearPlane;
    g_FarPlane = context.m_camera->farPlane;
    g_PerspectiveMatrix = *context.m_projection;
    if (!(m_flags & 0x10)) {
        _DMRenderBin.m_flag = true;
        if (_finish_draw) {
            _finish_draw = false;
            _renderList->Render(_finish_bin, 0);
        }
    } else {
        _DMRenderBinWeapon.m_flag = true;
        if (_finish_draw_weapon) {
            _finish_draw_weapon = false;
            _renderList->Render(_finish_bin_weapon, 0);
        }
    }
    DMRenderPartView* entry = object->parts;
    for (unsigned long i = 0; i < object->partCount; i++) {
        CPartBin* part = &m_parts[entry->part];
        if (part->m_flags & 0x200) {
            CRemapBin* remap = (CRemapBin*)part->m_field8;
            while (remap) {
                if (remap->m_remapData == part->m_texture)
                    break;
                remap = (CRemapBin*)remap->m_field4;
            }
            if (!remap) {
                remap = &_remap_pool[_remap_next++];
                remap->m_fieldC = 0;
                remap->m_field10 = 0;
                remap->m_bin = part;
                remap->m_remapData = part->m_texture;
                remap->m_field0 = 0;
                remap->m_field4 = part->m_field8;
                if ((CRemapBin*)part->m_field8)
                    ((CRemapBin*)part->m_field8)->m_field0 = (int)remap;
                part->m_field8 = (int)remap;
            }
            _renderList->Render(*remap, object);
        } else {
            _renderList->Render(*part, object);
        }
        entry++;
    }
}

void CDMesh::BindTextures(CTexturePack* pack, char*) {
    for (unsigned long i = 0; i < m_partCount; i++) {
        bool other;
        char* name;
        bool body;
        bool head;
        name = m_textureNames + m_parts[i].m_texture * 16;
        body = stricmp(name, "mohf_body") == 0;
        head = stricmp(name, "mohf_head") == 0;
        other = false;
        if (g_bInMultiplayerMode && stricmp(name, "gi_256") == 0)
            other = true;
        if (!body && !head && !other) {
            m_parts[i].m_texture = pack ? pack->FindTexture(name) : 0;
            m_parts[i].m_flags &= ~0x200;
        } else {
            m_parts[i].m_flags |= 0x200;
            if (other)
                m_parts[i].m_flags |= 0x200;
        }
    }
    m_flags |= 4;
}

void CDMesh::BindSkeleton(CharSkel_t* skeleton) {
    char map[128];
    short positions[384];
    if (!(m_flags & 2)) {
        char* remap = map;
        for (unsigned long i = 0; i < m_jointCount; i++) {
            int index = CharSkelGetJointByName(skeleton, m_jointNames + i * 16);
            if (index >= 0)
                map[i] = index;
            else
                map[i] = -1;
        }
        for (unsigned long i = 0; i < m_refCount; i++) {
            int index = m_refs[i * 4];
            if (map[index] == -1)
                DebugMsg("ERROR: Skeleton node '%s' not found in skeleton for model '%s'\n", m_jointNames + index * 16, m_name);
            m_refs[i * 4] = map[m_refs[i * 4]];
            unsigned char* ref = &m_refs[i * 4];
            ref[1] = skeleton->joints[ref[0]].joint->data[2];
        }
        if (m_version == 1) {
            memcpy(positions, m_jointPositions, m_jointCount * 6);
            short* position = positions;
            for (unsigned long i = 0; i < m_jointCount; i++) {
                if (*remap != -1) {
                    m_jointPositions[*remap * 3] = position[0] >> 4;
                    m_jointPositions[*remap * 3 + 1] = position[1] >> 4;
                    m_jointPositions[*remap * 3 + 2] = position[2] >> 4;
                }
                remap++;
                position += 3;
            }
        }
        m_flags |= 2;
    }
}

void CDMesh::Init(bool weapon) {
    CPartBin* previous = 0;
    m_flags |= weapon ? 0x10 : 0;
    if (!(m_flags & 1)) {
        if (m_refs)
            m_refs = (unsigned char*)((int)m_refs + (int)this);
        if (m_parts)
            m_parts = (CPartBin*)((int)m_parts + (int)this);
        if (m_textureNames)
            m_textureNames = (char*)((int)m_textureNames + (int)this);
        if (m_unknown3c)
            m_unknown3c = (char*)((int)m_unknown3c + (int)this);
        if (m_unknown40)
            m_unknown40 = (char*)((int)m_unknown40 + (int)this);
        if (m_unknown44)
            m_unknown44 = (char*)((int)m_unknown44 + (int)this);
        if (m_jointNames)
            m_jointNames = (char*)((int)m_jointNames + (int)this);
        if (m_jointPositions)
            m_jointPositions = (short*)((int)m_jointPositions + (int)this);
        if (m_data60)
            m_data60 = (char*)((int)m_data60 + (int)this);
        if (m_data68)
            m_data68 = (char*)((int)m_data68 + (int)this);
        if (m_data70)
            m_data70 = (char*)((int)m_data70 + (int)this);
        DCStoreRange(m_data60, m_count5c * 6);
        DCStoreRange(m_data68, m_count64 * 6);
        DCStoreRange(m_data70, m_count6c * 4);
        if (m_partCount > 1 && (m_parts[1].m_flags & 0x100))
            m_flags |= 8;
        for (unsigned long i = 0; i < m_partCount; i++) {
            CPartBin* bin = new (&m_parts[i]) CPartBin;
            CPartBin* part = (CPartBin*)(void*)bin;
            if (bin->m_unknown34)
                bin->m_unknown34 = (void*)((int)bin->m_unknown34 + (int)this);
            if (part->m_vertices)
                part->m_vertices = (char*)((int)part->m_vertices + (int)this);
            for (unsigned long j = 0; j < part->m_vertexCount; j++) {
                unsigned long* vertex = (unsigned long*)(part->m_vertices + j * 18);
                if (*vertex)
                    *vertex += (unsigned long)this;
            }
            if (!(m_flags & 8)) {
                if (previous)
                    previous->m_field4 = (int)part;
                part->m_field0 = (int)previous;
                previous = part;
            }
            if (m_flags & 0x10)
                part->m_flags |= 0x10;
        }
        if (m_partCount) {
            CPartBin* old;
            if (weapon) {
                old = (CPartBin*)_DMRenderBinWeapon.m_field8;
                _DMRenderBinWeapon.m_field8 = (int)m_parts;
            } else {
                old = (CPartBin*)_DMRenderBin.m_field8;
                _DMRenderBin.m_field8 = (int)m_parts;
            }
            if (m_flags & 8) {
                m_parts[0].m_field4 = (int)old;
                if (old)
                    old->m_field0 = (int)m_parts;
            } else {
                m_parts[m_partCount - 1].m_field4 = (int)old;
                if (old)
                    old->m_field0 = (int)&m_parts[m_partCount - 1];
            }
        }
        m_flags |= 1;
    }
}

__declspec(weak) CPart::~CPart() {
}

void CDMesh::InitClass(CRenderList* list) {
    _renderList = list;
    _renderList->Register(_DMRenderBin);
    _renderList->Register(_DMRenderBinWeapon);
    _DMRenderBin.m_field8 = (int)&_finish_bin;
    _DMRenderBinWeapon.m_field8 = (int)&_finish_bin_weapon;
    _remap_next = 0;
}

__declspec(weak) const char* CPart::GetName() {
    return "DMESH Part";
}

__declspec(weak) CRemapBin::~CRemapBin() {
}

__declspec(weak) CPartBin::~CPartBin() {
}
