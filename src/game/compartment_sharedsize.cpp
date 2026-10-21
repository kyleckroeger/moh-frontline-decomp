// A fragment of compartment.cpp (0x80075b88): CCompartment::GetSharedSize
// returns the shared compartment size; CCompartment::LoadShared finds the
// level's shared compartment header (resource type 3) and collision
// database (type 2), loads both from the level big file into g_GlobalCpt
// (the header's size into g_SharedCompartmentSize), runs the compartment
// Init, initialises its collision object and the surface types from the
// database, and clears g_TextureSwapCache (the inline GlobalReset). The
// classes and functions are named by the symbols; g_SharedCompartmentSize,
// g_GlobalCpt and g_TextureSwapCache are file-local in the original and
// declared extern here (the size's type inferred).
// The resource views (the file record at +8, its name at +4) follow
// skybox_textures.cpp, the compartment members and cache layout follow
// compartment_reset.cpp; the resource type values and the loaded files'
// roles are inferred from the calls. The rest of the file is in other units.
class CDB;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

struct TLTFileView {
    int unknown00;
    const char* m_name;
};

struct TLTResourceView {
    unsigned char unknown00[8];
    TLTFileView* data;
};

TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

class CPTHeader;

struct STextureSwapEntry {
    unsigned short m_kind : 3;
    unsigned short m_flag : 1;
    unsigned short m_index : 12;
};

class CTextureSwapCache {
public:
    STextureSwapEntry m_entries[32][512];
    unsigned char m_rows[32];

    void Clear() {
        for (int i = 0; i < 32; i++) {
            m_rows[i] = 0;
            for (int j = 0; j < 512; j++) {
                m_entries[i][j].m_kind = 0;
                m_entries[i][j].m_index = 0;
                m_entries[i][j].m_flag = 0;
            }
        }
    }
};

extern CTextureSwapCache g_TextureSwapCache;

class CCDBObject {
public:
    void Init(CDB*);

    unsigned char unknown00[40];
};

class CSurfaceType {
public:
    static void InitClass(CDB*);
};

class CCompartment {
public:
    static int GetSharedSize();
    static void LoadShared();
    static void GlobalReset() { g_TextureSwapCache.Clear(); }
    void Init();

    unsigned char unknown00[40];
    CCDBObject m_collision;
    int m_swapSet;
    char* m_name;
    CPTHeader* m_header;
    CDB* m_database;
};

extern int g_SharedCompartmentSize; // file-local in the original
extern CCompartment g_GlobalCpt;

int CCompartment::GetSharedSize() {
    return g_SharedCompartmentSize;
}

void CCompartment::LoadShared() {
    g_GlobalCpt.m_header = 0;
    TLTResourceView* header = TLT_FindNextResourceByType((TLTResourceID)3, 0);
    if (!header)
        return;
    TLTResourceView* database = TLT_FindNextResourceByType((TLTResourceID)2, 0);
    if (!database)
        return;
    const char* databaseName = database->data->m_name;
    g_GlobalCpt.m_header = (CPTHeader*)TLT_LoadFileFromLevelBigFile(header->data->m_name, &g_SharedCompartmentSize);
    g_GlobalCpt.m_database = (CDB*)TLT_LoadFileFromLevelBigFile(databaseName, 0);
    g_GlobalCpt.Init();
    g_GlobalCpt.m_collision.Init(g_GlobalCpt.m_database);
    CSurfaceType::InitClass(g_GlobalCpt.m_database);
    GlobalReset();
}
