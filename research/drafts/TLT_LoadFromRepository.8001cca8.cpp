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
        if (m_count == 0)
            return end();
        value_type* p = m_data + (unsigned long)key % m_size;
        while ((int)p->first != -1) {
            if (p->first == key)
                return p;
            if (++p == m_data + m_size)
                p = m_data;
        }
        return end();
    }

    void insert(K key, V value) {
        value_type* p = m_data + (unsigned long)key % m_size;
        while ((int)p->first != -1) {
            if (++p == m_data + m_size)
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

inline char* LoadFromBig(char* big, const char* name, int* size) {
    const char* q = strchr(name, '\\');
    while (q) {
        name = q + 1;
        q = strchr(name, '\\');
    }
    int offset;
    if (big) {
        if (BIG_locateentryz(big, name, 0, &offset, size))
            return BIG_locate(big, name);
    }
    return 0;
}

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

inline char* TLT_LoadFileFromLevelBigFile(const char* name, int* size) {
    const char* p = strchr(name, ':');
    {
        const char* n = name;
        if (!p) {
            n = name;
        } else {
            n = p;
            n++;
            DebugMsg("WARNING: When loading from a big file, strip off the %s part.\nSince it's a DEBUG build, I'll do it for you\n", "");
        }
        name = n;
    }
    char* file = LoadFromBig(g_pLevelBigFile, name, size);
    if (!file)
        file = TLT_LoadFileNormal(name, size, 0);
    return file;
}

char* TLT_LoadFromRepository(const char* name, int* size, bool* cached) {
    char* data = TLT_LoadFileFromLevelBigFile(name, size);
    *cached = false;
    if (data) {
        dwi::hash_map<char*, char*>::iterator it;
        if (g_Repository.m_count == 0) {
            it = g_Repository.end();
        } else {
            it = g_Repository.m_data + (unsigned long)data % g_Repository.m_size;
            while ((int)it->first != -1) {
                if (it->first == data)
                    goto found;
                if (++it == g_Repository.m_data + g_Repository.m_size)
                    it = g_Repository.m_data;
            }
            it = g_Repository.end();
        }
    found:
        *cached = it != g_Repository.end();
        if (!*cached)
            g_Repository.insert(data, data);
    }
    return data;
}
