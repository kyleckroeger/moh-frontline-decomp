// A fragment of publisher.cpp (0x800d7bb0): CPublisher::CancelSubscription
// finds the observer's subscription to the subject in the slot (same key,
// observer and slot) and erases it; AddSubscription inserts a subscription
// keyed by the slot's subject at the first free bucket from the key's hash
// (wrapping at the end); RemoveObserver erases every subscription of the
// observer. CPublisher::Shutdown deletes the
// publisher (the hash map's destructor frees the table, whose allocation
// starts one entry before the first bucket, and clears the members) and
// CPublisher::Reset empties the map (every bucket key set to -1, the sentinel
// keys before the first and after the last bucket set to -2, the count
// cleared). CPublisher::Init creates the publisher (its map sized to the first
// prime above the count from the header's prime list, allocated with a spare
// entry on each side, then cleared). CPublisher, its SItem, ISubject,
// IObserver and the dwi::hash_map template and its iterator are named by the
// mangled symbols (hash_map<ISubject*, CPublisher::SItem>); the members are
// inferred views, and the map's constructor, destructor, clear, find, insert,
// begin and end, the iterator's members, SItem's constructor and comparison
// (observer and slot) and NextPrime are inline (the names of NextPrime, find,
// insert and IsValid are inferred). The map's erase is the weak instance
// before this fragment, called here. The compiler also emits an unreferenced
// weak copy of the map's destructor, which the original does not contain; it
// is discarded. The rest of the file is not part of this unit.
class ISubject;
class IObserver;
enum ESubjectEvent {};

extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

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

    hash_map(int count) {
        int size = NextPrime(count + 1);
        m_data = 0;
        m_size = 0;
        m_count = 0;
        m_size = size;
        m_data = (value_type*)DWI_allocalign(0, (size + 2) * sizeof(value_type), 16, 1024) + 1;
        clear();
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

    ~hash_map() {
        MEM_free(m_data - 1);
        m_data = 0;
        m_size = 0;
        m_count = 0;
    }

    void clear() {
        for (value_type* it = m_data; it != m_data + m_size; ++it)
            it->first = (K)-1;
        m_data[-1].first = (K)-2;
        m_data[m_size].first = (K)-2;
        m_count = 0;
    }

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
        bool operator!=(const iterator& other) const { return m_p != other.m_p; }
        value_type* operator->() const { return m_p; }
        value_type* m_p;
    };

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

    iterator begin() {
        iterator it(m_data);
        if (!it.IsValid())
            ++it;
        return it;
    }
    iterator end() { return iterator(m_data + m_size); }
    void erase(iterator);

    value_type* m_data;
    int m_size;
    int m_count;
};
}

typedef void (*SubjectCallback)(IObserver&, ISubject**, ESubjectEvent);

class CPublisher {
public:
    struct SItem {
        SItem(IObserver& observer, ISubject** slot, SubjectCallback callback)
            : m_observer(&observer), m_slot(slot), m_callback(callback) {}
        bool operator==(const SItem& other) const {
            return m_observer == other.m_observer && m_slot == other.m_slot;
        }
        IObserver* m_observer;
        ISubject** m_slot;
        SubjectCallback m_callback;
    };

    CPublisher(int count) : m_map(count) {}

    static void CancelSubscription(IObserver&, ISubject**);
    static void AddSubscription(IObserver&, ISubject**, SubjectCallback);
    static void RemoveObserver(IObserver&);
    static void Init(int);
    static void Shutdown();
    static void Reset();

    dwi::hash_map<ISubject*, SItem> m_map;

    static CPublisher* sm_pThePublisher;
};

void CPublisher::CancelSubscription(IObserver& observer, ISubject** slot) {
    dwi::hash_map<ISubject*, SItem>::iterator it = sm_pThePublisher->m_map.find(*slot, SItem(observer, slot, 0));
    if (it != sm_pThePublisher->m_map.end())
        sm_pThePublisher->m_map.erase(it);
}

void CPublisher::AddSubscription(IObserver& observer, ISubject** slot, SubjectCallback callback) {
    sm_pThePublisher->m_map.insert(*slot, SItem(observer, slot, callback));
}

void CPublisher::RemoveObserver(IObserver& observer) {
    dwi::hash_map<ISubject*, SItem>::value_type* p = sm_pThePublisher->m_map.m_data;
    bool valid = (int)p->first != -1 && (int)p->first != -2;
    if (!valid) {
        while ((int)(++p)->first == -1)
            ;
    }
    while (p != sm_pThePublisher->m_map.m_data + sm_pThePublisher->m_map.m_size) {
        if (p->second.m_observer == &observer) {
            sm_pThePublisher->m_map.erase(p);
        } else {
            while ((int)(++p)->first == -1)
                ;
        }
    }
}

void CPublisher::Shutdown() {
    delete sm_pThePublisher;
}

void CPublisher::Reset() {
    sm_pThePublisher->m_map.clear();
}

void CPublisher::Init(int count) {
    sm_pThePublisher = new CPublisher(count);
}
