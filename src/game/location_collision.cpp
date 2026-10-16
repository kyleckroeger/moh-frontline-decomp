// location_collision.cpp: the CObjLocationCollisionMap constructor, which
// builds its location records (24, through the weak empty LocationInfo
// constructor emitted after it) and, for each collision box, looks up the
// box's name (at +80, at most 31 characters copied) in the file's location
// table g_ClsnMap and copies the matching entry's two words and flag; then the
// static initialisation of that table: 21 entries (the body-part collision
// shapes "co_HeadBShape" to "co_HipFShape", each with its location index, a
// value and a set flag), each built by the entry's inline constructor into a
// temporary and copied into place. The search bound is g_ClsnMap +
// sizeof(g_ClsnMap) entries (924 rather than 21), as in the original code.
// The lookup is an inline helper (its name, FindClsnMapEntry, is inferred;
// writing the search in the constructor changes register assignment), and the
// 48-byte name buffer is inferred from the frame size. The unit owns the
// file's .rodata (the header's hash-table primes, then the shape names). The
// classes and g_ClsnMap are named by the symbols; the members, the box and
// table-entry views, the entry constructor and their names are inferred from
// offsets and the code; the table values are read from the original code.
extern "C" char* strncpy(char*, const char*, unsigned long);
extern "C" int stricmp(const char*, const char*);

struct CObjLocationCollisionBox {
    unsigned char unknown00[80];
    char m_name[48];
};

struct CObjLocationCollisionData {
    CObjLocationCollisionBox boxes[24];
    int count;
    int field0c04;
};

struct ClsnMapEntry {
    ClsnMapEntry(const char* name, int location, int value, bool flag) : m_location(location), m_value(value), m_flag(flag) {
        strncpy(m_name, name, 31);
    }

    char m_name[32];
    int m_location;
    int m_value;
    bool m_flag;
};

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

static ClsnMapEntry g_ClsnMap[21] = {
    ClsnMapEntry("co_HeadBShape", 0, 5, true),
    ClsnMapEntry("co_HeadFShape", 1, 5, true),
    ClsnMapEntry("co_NeckShape", 2, 5, true),
    ClsnMapEntry("co_handRShape", 6, 1, true),
    ClsnMapEntry("co_loarm_RShape", 7, 2, true),
    ClsnMapEntry("co_uparm_RShape", 8, 2, true),
    ClsnMapEntry("co_handLShape", 3, 1, true),
    ClsnMapEntry("co_loarm_LShape", 4, 2, true),
    ClsnMapEntry("co_uparm_LShape", 5, 2, true),
    ClsnMapEntry("co_ChestShape", 9, 5, true),
    ClsnMapEntry("co_Back1Shape", 10, 5, true),
    ClsnMapEntry("co_AbdFShape", 12, 5, true),
    ClsnMapEntry("co_Back2Shape", 11, 5, true),
    ClsnMapEntry("co_ankle_RShape", 16, 1, true),
    ClsnMapEntry("co_calf_LShape", 14, 2, true),
    ClsnMapEntry("co_calf_RShape", 17, 2, true),
    ClsnMapEntry("co_thigh_RShape", 18, 3, true),
    ClsnMapEntry("co_ankle_LShape", 13, 1, true),
    ClsnMapEntry("co_thigh_LShape", 15, 3, true),
    ClsnMapEntry("co_HipBShape", 19, 4, true),
    ClsnMapEntry("co_HipFShape", 20, 4, true),
};

class CObjLocationCollisionMap {
public:
    struct LocationInfo {
        LocationInfo() {}

        int m_location;
        int m_value;
        bool m_flag;
    };

    CObjLocationCollisionMap(const CObjLocationCollisionData& data);

    LocationInfo m_info[24];
    int m_count;
};

static inline ClsnMapEntry* FindClsnMapEntry(const char* name) {
    ClsnMapEntry* entry = g_ClsnMap;
    while (entry != g_ClsnMap + sizeof(g_ClsnMap)) {
        if (stricmp(entry->m_name, name) == 0)
            break;
        entry++;
    }
    return entry;
}

CObjLocationCollisionMap::CObjLocationCollisionMap(const CObjLocationCollisionData& data) {
    m_count = data.count;
    for (int i = 0; i < m_count; i++) {
        char name[48];
        strncpy(name, data.boxes[i].m_name, 31);
        ClsnMapEntry* entry = FindClsnMapEntry(name);
        m_info[i].m_location = entry->m_location;
        m_info[i].m_value = entry->m_value;
        m_info[i].m_flag = entry->m_flag;
    }
}
