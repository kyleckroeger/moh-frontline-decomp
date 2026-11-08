namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    typedef value_type* iterator;

    iterator end() { return m_data + m_size; }

    iterator find(K key) {
        if (m_count != 0) {
            value_type* last = m_data + m_size;
            value_type* p = m_data + (unsigned long)key % m_size;
            while ((int)p->first != -1) {
                if (p->first == key)
                    return p;
                if (++p == last)
                    p = m_data;
            }
            return last;
        }
        return end();
    }

    void insert(K key, V value) {
        value_type* last = m_data + m_size;
        value_type* p = m_data + (unsigned long)key % m_size;
        while ((int)p->first != -1) {
            if (++p == last)
                p = m_data;
        }
        p->first = key;
        p->second = value;
        m_count++;
    }

    value_type* m_data;
    int m_size;
    int m_count;
};
}

extern "C" {
int sprintf(char*, const char*, ...);
char* strchr(const char*, int);
int FILE_sizez(const char*);
char* FILE_loadz(const char*, int);
void* BIG_locateentryz(void*, const char*, int, int*, int*);
char* BIG_locate(void*, const char*);
}
void DebugMsg(const char*, ...);

extern char* g_pLevelBigFile;

// File-local in the original; declared extern so this fragment can refer to it.
extern dwi::hash_map<char*, char*> g_Repository;

char* TLT_LoadFromRepository(const char* name, int* size, bool* cached) {
    const char* colon = strchr(name, ':');
    const char* n;
    if (!colon) {
        n = name;
    } else {
        n = colon;
        n++;
        DebugMsg("WARNING: When loading from a big file, strip off the %s part.\nSince it's a DEBUG build, I'll do it for you\n", "");
    }
    name = n;
    char* big = g_pLevelBigFile;
    const char* base = name;
    for (const char* slash = strchr(name, '\\'); slash; slash = strchr(base, '\\'))
        base = slash + 1;
    int entry;
    char* data;
    if (big && BIG_locateentryz(big, base, 0, &entry, size))
        data = BIG_locate(big, base);
    else
        data = 0;
    if (!data) {
        char path[260];
        colon = strchr(name, ':');
        if (colon)
            name = colon + 1;
        sprintf(path, "%s", name);
        if (size)
            *size = FILE_sizez(path);
        data = FILE_loadz(path, 1024);
    }
    *cached = false;
    if (data) {
        *cached = g_Repository.find(data) != g_Repository.end();
        if (!*cached)
            g_Repository.insert(data, data);
    }
    return data;
}
