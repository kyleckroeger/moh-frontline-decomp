// A fragment of location_collision.cpp (0x80072ea0): the
// CObjLocationCollisionMap constructor, which builds its location records
// (24, through the weak empty LocationInfo constructor emitted after it) and,
// for each collision box, looks up the box's name (at +80, at most 31
// characters copied) in the file's location table g_ClsnMap (file-local in
// the original, declared extern here) and copies the matching entry's two
// words and flag. The search bound is g_ClsnMap + sizeof(g_ClsnMap) entries
// (924 rather than 21), as in the original code. The lookup is an inline
// helper (its name, FindClsnMapEntry, is inferred; writing the search in the
// constructor changes register assignment), and the 48-byte name buffer is
// inferred from the frame size. The classes are named by the mangled
// symbols; the members, the box and table-entry views and their names are
// inferred from offsets. The static initialisation of the table
// and the rest of the file are not part of this unit.
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
    char m_name[32];
    int m_location;
    int m_value;
    bool m_flag;
};

extern ClsnMapEntry g_ClsnMap[21];

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
