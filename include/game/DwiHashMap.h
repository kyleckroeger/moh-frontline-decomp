// dwi::hash_map and its prime table, shared by every file that uses them (PortReadiness step 3.3).
//
// Evidence (nm of the original):
// - dwi::hash_map<K, V> (destructors: <char*, char*>, <unsigned int, CPathNodeList::PathNodes>;
//   erase(iterator) weak for <unsigned int, PathNodes> and <ISubject*, CPublisher::SItem>);
//   the iterator is the nested class hash_map::iterator (erase's mangled parameter).
// - dwi_prime_list (27 words, .rodata, local): one copy in each of 25 files, so it is a static
//   table in the header that declares the map; every file that includes this header emits it.
//
// Inferred, not in any mangled name: the member names and value_type's first/second (the matched
// views'), the slot markers (-1 empty, -2 the sentinels before and after the table), the inline
// bodies no weak copy shows, and the Hash/Next/Resize/NextPrime helper names.
#ifndef GAME_DWIHASHMAP_H
#define GAME_DWIHASHMAP_H

extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};

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
        bool IsValid() const { return !((int)m_p->first == -1 || (int)m_p->first == -2); }
        iterator& operator++() {
            while ((int)(++m_p)->first == -1)
                ;
            return *this;
        }
        bool operator==(const iterator& other) const { return m_p == other.m_p; }
        bool operator!=(const iterator& other) const { return m_p != other.m_p; }
        value_type* operator->() const { return m_p; }
        value_type* m_p;
    };

    hash_map() : m_data(0), m_size(0), m_count(0) {}
    hash_map(int count) {
        int size = NextPrime(count + 1);
        m_data = 0;
        m_size = 0;
        m_count = 0;
        m_size = size;
        m_data = (value_type*)DWI_allocalign(0, (size + 2) * sizeof(value_type), 16, 1024) + 1;
        clear();
    }
    ~hash_map() {
        MEM_free(m_data - 1);
        m_data = 0;
        m_size = 0;
        m_count = 0;
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
    void Resize(int count) {
        int size = NextPrime(count + 1);
        m_size = size;
        m_data = (value_type*)DWI_allocalign(0, (size + 2) * sizeof(value_type), 16, 1024) + 1;
        clear();
    }
    void clear() {
        for (value_type* it = m_data; it != m_data + m_size; ++it)
            it->first = (K)-1;
        m_data[-1].first = (K)-2;
        m_data[m_size].first = (K)-2;
        m_count = 0;
    }
    int Hash(K key) const { return (unsigned long)key % m_size; }
    value_type* Next(value_type* p) const {
        if (++p == m_data + m_size)
            p = m_data;
        return p;
    }

    iterator begin() {
        iterator it(m_data);
        if (!it.IsValid())
            ++it;
        return it;
    }
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
    iterator find(K key, V value) {
        if (m_count == 0)
            return end();
        value_type* p = m_data + (unsigned long)key % m_size;
        while ((int)p->first != -1) {
            if (p->first == key && p->second == value)
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
    __declspec(weak) void erase(iterator);

    value_type* m_data;
    int m_size;
    int m_count;
};

// Open addressing: the hole is closed with entries of the same home slot, then the rest of the
// run is reinserted.
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
} // namespace dwi

#endif
