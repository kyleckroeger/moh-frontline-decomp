// A fragment of publisher.cpp (0x800d7950): the weak instance of
// dwi::hash_map<ISubject*, CPublisher::SItem>::erase, emitted after
// CPublisher::PublishEvent (its first user). It closes the hole left by the
// erased bucket by pulling forward entries of the same home bucket (open
// addressing with linear probing), then re-inserts every entry of the rest
// of the cluster. The map, its iterator, SItem and ISubject are named by the
// mangled symbols; the members are the views of publisher_reset.cpp, and
// Hash, Next and insert are inline (names inferred). The rest of the file is
// not part of this unit.
class ISubject;
class IObserver;
enum ESubjectEvent {};

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
        iterator(const iterator& o) : m_p(o.m_p) {}
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

typedef void (*SubjectCallback)(IObserver&, ISubject**, ESubjectEvent);

class CPublisher {
public:
    struct SItem {
        IObserver* m_observer;
        ISubject** m_slot;
        SubjectCallback m_callback;
    };
};

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
        V value = hole->second;
        K key = hole->first;
        hole->first = (K)-1;
        m_count--;
        insert(key, value);
    }
}
}

template class dwi::hash_map<ISubject*, CPublisher::SItem>;
