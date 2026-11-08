extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
void* operator new[](unsigned long, char*);

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};

template <class T> inline const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <class K, class V>
class hash_map {
public:
    struct value_type {
        value_type(const K& k, const V& v) : first(k), second(v) {}
        K first;
        V second;
    };

    class iterator {
    public:
        iterator(value_type* p) : m_p(p) {}
        bool operator==(const iterator& other) const { return m_p == other.m_p; }
        bool operator!=(const iterator& other) const { return m_p != other.m_p; }
        value_type* operator->() const { return m_p; }
        value_type* m_p;
    };

    hash_map() : m_data(0), m_size(0), m_count(0) {}
    ~hash_map();

    iterator end() { return iterator(m_data + m_size); }

    iterator find(K key) {
        if (m_count == 0)
            return end();
        value_type* p = m_data + (unsigned long)key % m_size;
        while ((int)p->first != -1) {
            if (p->first == key)
                return iterator(p);
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

    void Resize(int count, const char* name) {
        int size = NextPrime(count);
        m_size = size;
        m_data = (value_type*)DWI_allocalign(name, (size + 2) * sizeof(value_type), 16, 1024) + 1;
        clear();
    }

    value_type* m_data;
    int m_size;
    int m_count;
};

template <class K, class V> __declspec(weak) hash_map<K, V>::~hash_map() {
    MEM_free(m_data - 1);
    m_data = 0;
    m_size = 0;
    m_count = 0;
}
}

struct BPDPathFindingNode;

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void ChangeEndian(float& value) {
    float& f = value;
    EndianSwap(f, true);
}

class CPathNode {
public:
    float x;
    float y;
    float z;
    int field0c;
    unsigned char m_linkCount;
    unsigned char m_enabled : 1;
    unsigned char m_flags : 7;
    unsigned char pad12[2];
    short* m_links;
};

class CPathNodeList {
public:
    struct PathNodes {
        PathNodes() : count(0), nodes(0), marked(true) {}
        int count;
        CPathNode** nodes;
        bool marked;
    };

    CPathNodeList(void* base, BPDPathFindingNode* nodes, int count);

    static unsigned int MakeKey(float x, float y) {
        int cy = (int)((y + 2.5f) / 5.0f) + 16384;
        int cx = (int)((x + 2.5f) / 5.0f) + 16384;
        return ((cy & 0x7fff) << 16) + ((cx & 0x7fff) << 1);
    }
    static unsigned int MakeKey(int cx, int cy) {
        return ((cy & 0x7fff) << 16) + ((cx & 0x7fff) << 1);
    }
    static int GetX(unsigned int key) { return (key >> 1) & 0x7fff; }
    static int GetY(unsigned int key) { return (key >> 16) & 0x7fff; }

    static dwi::hash_map<unsigned int, PathNodes> s_pathNodeMap;
    static CPathNode** s_pPathNodeMapEntries;
    static CPathNodeList* s_pnlCurrent;

    CPathNode* m_nodes;
    int m_count;
};

CPathNodeList::CPathNodeList(void* base, BPDPathFindingNode* nodes, int count) {
    m_nodes = (CPathNode*)((char*)base + (int)nodes);
    m_count = count;
    CPathNode* end = m_nodes + m_count;
    for (CPathNode* node = m_nodes; node < end; node++) {
        ChangeEndian(node->x);
        ChangeEndian(node->y);
        ChangeEndian(node->z);
        ChangeEndian(node->field0c);
        ChangeEndian(node->m_links);
        node->m_enabled = 1;
        node->field0c = 0;
        node->m_links = (short*)((char*)base + (int)node->m_links);
        for (int j = 0; j < node->m_linkCount; j++)
            ChangeEndian(node->m_links[j]);
    }

    s_pathNodeMap.Resize(dwi::max(m_count * 2 + 1, 1543), "AI Path Node Map");

    int total = 0;
    for (int i = 0; i < m_count; i++) {
        unsigned int key = MakeKey(m_nodes[i].x, m_nodes[i].y);
        int x = GetX(key);
        int y = GetY(key);
        unsigned int keys[9] = {
            MakeKey(x - 1, y - 1), MakeKey(x - 1, y), MakeKey(x - 1, y + 1),
            MakeKey(x, y - 1),     MakeKey(x, y),     MakeKey(x, y + 1),
            MakeKey(x + 1, y - 1), MakeKey(x + 1, y), MakeKey(x + 1, y + 1),
        };
        for (int k = 0; k < 9; k++) {
            unsigned int cellKey = keys[k];
            dwi::hash_map<unsigned int, PathNodes>::iterator it = s_pathNodeMap.find(cellKey);
            if (it == s_pathNodeMap.end()) {
                PathNodes entry;
                entry.count = 1;
                entry.nodes = 0;
                s_pathNodeMap.insert(cellKey, entry);
            } else {
                it->second.count++;
            }
            total++;
        }
    }

    CPathNode** entries = new ("AI Path Node Map Entries") CPathNode*[total];
    s_pPathNodeMapEntries = entries;
    for (int i = 0; i < m_count; i++) {
        CPathNode* node = &m_nodes[i];
        unsigned int key = MakeKey(node->x, node->y);
        int x = GetX(key);
        int y = GetY(key);
        unsigned int keys[9] = {
            MakeKey(x - 1, y - 1), MakeKey(x - 1, y), MakeKey(x - 1, y + 1),
            MakeKey(x, y - 1),     MakeKey(x, y),     MakeKey(x, y + 1),
            MakeKey(x + 1, y - 1), MakeKey(x + 1, y), MakeKey(x + 1, y + 1),
        };
        for (int k = 0; k < 9; k++) {
            dwi::hash_map<unsigned int, PathNodes>::iterator it = s_pathNodeMap.find(keys[k]);
            PathNodes& cell = it->second;
            if (cell.nodes == 0) {
                cell.nodes = entries;
                for (int j = 0; j < cell.count; j++)
                    cell.nodes[j] = 0;
                entries += cell.count;
            }
            CPathNode** slots = cell.nodes;
            int j;
            for (j = 0; slots[j]; j++)
                ;
            slots[j] = node;
        }
    }
    s_pnlCurrent = this;
}

dwi::hash_map<unsigned int, CPathNodeList::PathNodes> CPathNodeList::s_pathNodeMap;
