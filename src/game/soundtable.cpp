// CSoundTable: the two GetSoundEvent lookups (a binary search of the
// animation table by id, and of the key table by three values, retried with a
// looser key comparison up to three times), Init (loads the event and
// animation tables named by the level's resource 12, converts their byte
// order and relocates their record pointers), EndianSwap, the destructor
// (closes the table through the file layer) and the global singleton. The
// byte-order helpers are the inlined ones described in propdat.cpp
// (inferred). CSoundTable and its nested types are named by the mangled
// symbols; members, record layouts, the key comparison and the parameter
// copy are inferred and are not original.
#include "game/GamePtr32.h"

void TLT_CloseFile(void*);

enum TLTResourceID {};
struct LevelFileContentsStruct_;

// Inferred view of the level resource entry (the level contents file's entry:
// type, count, then the relocated pointer to its data).
struct LevelResourceNamesView {
    unsigned char unknown00[92];
    GAME_PTR32(const char) m_eventFile;
    unsigned char unknown60[4];
    GAME_PTR32(const char) m_animFile;
};
struct LevelResourceView {
    unsigned char unknown00[8];
    GAME_PTR32(LevelResourceNamesView) m_names;
};

LevelResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

inline void ChangeEndian(short& value) {
#ifndef GAME_LITTLE_ENDIAN
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
#endif
}

inline void ChangeEndian(int& value) {
#ifndef GAME_LITTLE_ENDIAN
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
#endif
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}


template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

class CSoundTable {
public:
    // Inferred record views; only their 16-bit fields are converted.
    struct SKey {
        void EndianSwap() {
            ChangeEndian(m_values[0]);
            ChangeEndian(m_values[1]);
            ChangeEndian(m_values[2]);
            ChangeEndian(m_values[3]);
        }

        short m_values[4];
    };
    struct SSoundParms {
        void EndianSwap() {
            ChangeEndian(m_values[0]);
            ChangeEndian(m_values[1]);
            ChangeEndian(m_values[2]);
            ChangeEndian(m_values[3]);
            ChangeEndian(m_values[4]);
        }

        SSoundParms& operator=(const SSoundParms& other) {
            m_bytes[0] = other.m_bytes[0];
            m_bytes[1] = other.m_bytes[1];
            m_bytes[2] = other.m_bytes[2];
            m_values[0] = other.m_values[0];
            m_values[1] = other.m_values[1];
            m_values[2] = other.m_values[2];
            m_values[3] = other.m_values[3];
            m_values[4] = other.m_values[4];
            return *this;
        }

        char m_bytes[3];
        short m_values[5];
    };
    struct SAnimEntry {
        void EndianSwap() {
            ChangeEndian(m_id);
            m_parms.EndianSwap();
        }

        short m_id;
        SSoundParms m_parms;
    };

    // Inferred views of the two loaded headers.
    struct FileView {
        unsigned char unknown00[4];
        int m_counts[5];
        GAME_PTR32(SKey) m_keys;
        GAME_PTR32(SSoundParms) m_parms;
    };
    struct DataView {
        unsigned char unknown00[8];
        int m_counts[3];
        GAME_PTR32(SAnimEntry) m_entries;
    };

    CSoundTable() : m_file(0) {}
    ~CSoundTable();
    bool GetSoundEvent(int, SSoundParms*) const;
    bool GetSoundEvent(int, int, int, SSoundParms*) const;
    void Init(int, int);
    void EndianSwap();

    FileView* m_file;
    DataView* m_data;
};

// Orders keys by their first and third values, then (on the first attempt, or
// on the second unless the entry's second value is the wildcard -1) by the
// second (inferred helper).
static inline int CompareKeys(CSoundTable::SKey entry, CSoundTable::SKey key, int attempt) {
    if (entry.m_values[0] < key.m_values[0])
        return -1;
    if (key.m_values[0] < entry.m_values[0])
        return 1;
    if (entry.m_values[2] < key.m_values[2])
        return -1;
    if (key.m_values[2] < entry.m_values[2])
        return 1;
    if (attempt <= 1 && (attempt <= 0 || entry.m_values[1] != -1)) {
        if (entry.m_values[1] < key.m_values[1])
            return -1;
        if (key.m_values[1] < entry.m_values[1])
            return 1;
    }
    return 0;
}

bool CSoundTable::GetSoundEvent(int id, SSoundParms* parms) const {
    if (m_data) {
        short low = 0;
        short high = m_data->m_counts[2] - 1;
        short mid = 0;
        while (low <= high) {
            mid = (low + high) / 2;
            int difference = m_data->m_entries[mid].m_id - id;
            if (difference < 0)
                low = mid + 1;
            else if (difference > 0)
                high = mid - 1;
            else
                break;
        }
        if (low <= high) {
            *parms = m_data->m_entries[mid].m_parms;
            return true;
        }
    }
    return false;
}

bool CSoundTable::GetSoundEvent(int a, int b, int c, SSoundParms* parms) const {
    if (m_file) {
        SKey key;
        key.m_values[0] = a;
        key.m_values[1] = b;
        key.m_values[2] = c;
        for (int attempt = 0; attempt < 3; attempt++) {
            short low = 0;
            short mid = 0;
            short high = m_file->m_counts[3] - 1;
            while (low <= high) {
                mid = (low + high) / 2;
                int order = CompareKeys(m_file->m_keys[mid], key, attempt);
                if (order < 0)
                    low = mid + 1;
                else if (order > 0)
                    high = mid - 1;
                else
                    break;
            }
            if (low <= high) {
                const SKey& found = m_file->m_keys[mid];
                key.m_values[3] = found.m_values[3];
                *parms = m_file->m_parms[key.m_values[3]];
                return true;
            }
        }
    }
    return false;
}

void CSoundTable::Init(int, int) {
    LevelResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)12, 0);
    m_file = (FileView*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_eventFile, 0);
    m_data = (DataView*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_animFile, 0);
    EndianSwap();
    if (m_file) {
        int base = GAME_ADDR32(m_file);
        offsetPtr(m_file->m_keys, base);
        offsetPtr(m_file->m_parms, base);
        int i;
        for (i = 0; i < m_file->m_counts[3]; i++) {
            m_file->m_keys[i].EndianSwap();
        }
        for (i = 0; i < m_file->m_counts[4]; i++) {
            m_file->m_parms[i].EndianSwap();
        }
    }
    if (m_data) {
        offsetPtr(m_data->m_entries, GAME_ADDR32(m_data));
        for (int i = 0; i < m_data->m_counts[2]; i++) {
            m_data->m_entries[i].EndianSwap();
        }
    }
}

void CSoundTable::EndianSwap() {
    if (m_file) {
        ChangeEndian(m_file->m_counts[0]);
        ChangeEndian(m_file->m_counts[1]);
        ChangeEndian(m_file->m_counts[2]);
        ChangeEndian(m_file->m_counts[3]);
        ChangeEndian(m_file->m_counts[4]);
        ChangeEndian(m_file->m_keys);
        ChangeEndian(m_file->m_parms);
    }
    if (m_data) {
        ChangeEndian(m_data->m_counts[0]);
        ChangeEndian(m_data->m_counts[1]);
        ChangeEndian(m_data->m_counts[2]);
        ChangeEndian(m_data->m_entries);
    }
}

// Init's offsetPtr instantiations are emitted after the destructor, weak as in
// the image.

CSoundTable::~CSoundTable() {
    TLT_CloseFile(m_file);
}

CSoundTable g_soundMapSingleton;
