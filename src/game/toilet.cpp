// The end of toilet.cpp (0x8001de64): TLT_Init loads the level big file
// (its name built from the mission and stage and TLT_LoadFileNormal inlined,
// keeping its size), the level contents, re-adds the compartment big file
// and loads its header, then sizes the file's string repository for the
// level big file's entry count (the hash map's prime-sized table allocated
// with a spare entry on each side and cleared; NextPrime and clear as in
// publisher_reset.cpp, the sizing member's name is inferred). The static
// initialisation of toilet.cpp: the string repository (a hash map from
// names to names) starts with no table, its weak destructor registered.
// g_Repository, the globals and the hash map are named by the symbols; the
// map's members and default constructor are inferred (as in
// toilet_hashmap.cpp). The file is compiled with -inline deferred,auto; the
// other globals are defined elsewhere. The rest of the file is in other
// units.
extern "C" {
int sprintf(char*, const char*, ...);
char* strchr(const char*, int);
int FILE_sizez(const char*);
char* FILE_loadz(const char*, int);
void* FILE_loadbigheader(const char*, int);
void FILESYS_addbigsync(const char*, int, int, int*);
void FILESYS_delbigsync(int, int);
int BIG_count(void*);
void MEM_free(void*);
}

void* DWI_allocalign(const char*, int, int, int);
void TLT_LoadLevelContentsFile(int, int);

extern char* g_pLevelBigFile;
extern int g_LevelBigFileSize;
extern void* g_pCompartmentBigFileHeader;
extern int g_CompartmentBigFile;
extern char* szTLT_LevelBigFileName;
extern char* szTLT_CompartmentBigFileName;

// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    hash_map() : m_data(0), m_size(0), m_count(0) {}
    ~hash_map();

    static int NextPrime(int n) {
        const unsigned long* p = dwi_prime_list;
        const unsigned long* end = dwi_prime_list + 27;
        for (; p != end; p++) {
            if ((int)*p >= n)
                return *p;
        }
        return *(end - 1);
    }

    void clear() {
        for (value_type* it = m_data; it != m_data + m_size; ++it)
            it->first = (K)-1;
        m_data[-1].first = (K)-2;
        m_data[m_size].first = (K)-2;
        m_count = 0;
    }

    void Resize(int count) {
        int size = NextPrime(count + 1);
        m_size = size;
        m_data = (value_type*)DWI_allocalign(0, (size + 2) * sizeof(value_type), 16, 1024) + 1;
        clear();
    }

    value_type* m_data;
    int m_size;
    int m_count;
};
}

static dwi::hash_map<char*, char*> g_Repository;

inline char* TLT_LoadFileNormal(const char* name, int* size, int flags) {
    char path[256];
    const char* file = strchr(name, ':');
    if (file == 0)
        file = name;
    else
        file = file + 1;
    sprintf(path, "%s", file);
    if (size)
        *size = FILE_sizez(path);
    return FILE_loadz(path, flags | 1024);
}

void TLT_Init(int mission, int stage) {
    char name[128];
    char compartment[128];
    if (g_pLevelBigFile)
        MEM_free(g_pLevelBigFile);
    sprintf(name, "data/%d/%d_%d/%s", mission, mission, stage, szTLT_LevelBigFileName);
    g_pLevelBigFile = TLT_LoadFileNormal(name, &g_LevelBigFileSize, 0);
    TLT_LoadLevelContentsFile(mission, stage);
    if (g_CompartmentBigFile)
        FILESYS_delbigsync(g_CompartmentBigFile, 100);
    sprintf(compartment, "data/%d/%d_%d/%s", mission, mission, stage, szTLT_CompartmentBigFileName);
    FILESYS_addbigsync(compartment, 1024, 100, &g_CompartmentBigFile);
    g_pCompartmentBigFileHeader = FILE_loadbigheader(compartment, 1024);
    g_Repository.Resize(BIG_count(g_pLevelBigFile));
}
