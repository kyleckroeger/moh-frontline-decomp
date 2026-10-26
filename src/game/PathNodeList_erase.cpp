// A fragment of PathNodeList.cpp (0x80054dc8): the weak instance of
// dwi::hash_map<unsigned int, CPathNodeList::PathNodes>::erase, emitted after
// its first user. The same open-addressing erase as the publisher map's
// (publisher_erase.cpp): it closes the hole with entries of the same home
// bucket, then re-inserts the rest of the cluster. The map, its iterator and
// PathNodes are named by the mangled symbols; the members are inferred views
// (PathNodes as in PathNodeList_disable.cpp), and Hash, Next and insert are
// inline (names inferred). The rest of the file is not part of this unit.
class CPathNodeList {
public:
    struct PathNodes {
        int data00;
        int data04;
        unsigned char data08;
    };
};

namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    class iterator {
    public:
        iterator(value_type* p) : m_p(p) {}
        value_type* m_p;
    };

    int Hash(K key) const { return (unsigned long)key % m_size; }
    value_type* Next(value_type* p) const {
        if (++p == m_data + m_size)
            p = m_data;
        return p;
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

    __declspec(weak) void erase(iterator);

    value_type* m_data;
    int m_size;
    int m_count;
};
}

namespace dwi {
template <class K, class V>
__declspec(weak) void hash_map<K, V>::erase(iterator it) {
    if ((int)it.m_p->first == -1)
        return;
    value_type* hole = (value_type*)(void*)it.m_p;
    value_type* p = Next(it.m_p);
    int home = Hash(hole->first);
    for (;;) {
        bool used = (int)p->first != -1;
        bool moved = Hash(p->first) != home;
        while (used && moved) {
            p = Next(p);
            used = (int)p->first != -1;
            moved = Hash(p->first) != home;
        }
        *hole = *p;
        if (used) {
            hole = Next(hole);
            p = Next(p);
            while (home != Hash(hole->first))
                hole = Next(hole);
        } else {
            m_count--;
            break;
        }
    }
    while ((int)(hole = Next(hole))->first != -1) {
        V value;
        value = hole->second;
        K key = hole->first;
        hole->first = (K)-1;
        m_count--;
        insert(key, value);
    }
}
}

template class dwi::hash_map<unsigned int, CPathNodeList::PathNodes>;
