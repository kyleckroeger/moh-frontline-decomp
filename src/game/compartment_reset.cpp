// A fragment of compartment.cpp (0x80076c0c): CCompartment::Reset clears
// each of the header's items (a byte at +41 and a flag bit of +43, and +20
// copied back to +8); CCompartment::GlobalReset clears the file-local
// g_TextureSwapCache (every entry's fields and each row's trailing byte);
// CCompartment::Init(int, char*, CPTHeader*, CDB*) stores its arguments,
// initialises the collision object (+40) from the database and runs the
// other Init. CCompartment, CPTHeader, CDB and CCDBObject are named by the
// mangled symbols; the header's item list (+40, count +44), the item fields,
// the cache entry's bitfields (3, 1 and 12 bits) and the members are inferred
// from offsets; the cache layout follows compartment_texswap.cpp (declared
// extern here). The rest of the file is in other units.
class CDB;

/* inferred: a header item */
struct CPTHeaderItem {
    unsigned char unknown00[8];
    int m_current;
    unsigned char unknown0c[8];
    int m_initial;
    unsigned char unknown18[17];
    unsigned char m_byte29;
    unsigned char unknown2a;
    unsigned char m_bits7 : 2;
    unsigned char m_flag : 1;
    unsigned char m_bits0 : 5;
};

class CPTHeader {
public:
    unsigned char unknown00[40];
    CPTHeaderItem** m_items;
    int m_count;
};

/* inferred: a cache entry's fields */
struct STextureSwapEntry {
    unsigned short m_kind : 3;
    unsigned short m_flag : 1;
    unsigned short m_index : 12;
};

class CTextureSwapCache {
public:
    STextureSwapEntry m_entries[32][512];
    unsigned char m_rows[32];

    /* inferred inline member: the clear compiles to the original's register
       use only as one */
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

class CCompartment {
public:
    void Reset();
    static void GlobalReset();
    void Init(int, char*, CPTHeader*, CDB*);
    void Init();

    unsigned char unknown00[40];
    CCDBObject m_collision;
    int m_swapSet;
    char* m_name;
    CPTHeader* m_header;
    CDB* m_database;
};

void CCompartment::Reset() {
    for (int i = 0; i < m_header->m_count; i++) {
        CPTHeaderItem* item = m_header->m_items[i];
        item->m_byte29 = 0;
        item->m_flag = 0;
        item->m_current = item->m_initial;
    }
}

void CCompartment::GlobalReset() {
    g_TextureSwapCache.Clear();
}

void CCompartment::Init(int swapSet, char* name, CPTHeader* header, CDB* database) {
    m_swapSet = swapSet;
    m_name = name;
    m_header = header;
    m_database = database;
    m_collision.Init(m_database);
    Init();
}
