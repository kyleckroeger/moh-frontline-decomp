// A fragment of publisher.cpp (0x800d7fac): the weak instance
// IObserver::SetSubject<ISubject>, which points an observer's subject slot at
// a new subject: unless it is already there, it cancels the old subscription
// (when there is a slot), stores the subject and, for a subject, subscribes
// the observer with the callback (CPublisher::AddSubscription, inlined: the
// map insert at the first free bucket from the key's hash, wrapping at the
// end). IObserver, CPublisher, its SItem and the dwi::hash_map template are
// named by the mangled symbols; the views follow publisher_reset.cpp (the
// map's insert and SItem's constructor are inferred inlines). The template is
// defined out of line and explicitly instantiated so that its weak copy is
// emitted here; CancelSubscription and the rest of the file are in other
// units.
class ISubject;
class IObserver;
enum ESubjectEvent {};

extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

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


    static void CancelSubscription(IObserver&, ISubject**);
    static void AddSubscription(IObserver&, ISubject**, SubjectCallback);
    static void RemoveObserver(IObserver&);
    static void Init(int);
    static void Shutdown();
    static void Reset();

    dwi::hash_map<ISubject*, SItem> m_map;

    static CPublisher* sm_pThePublisher;
};

inline void CPublisher::AddSubscription(IObserver& observer, ISubject** slot, SubjectCallback callback) {
    sm_pThePublisher->m_map.insert(*slot, SItem(observer, slot, callback));
}
class IObserver {
public:
    template <class T>
    void SetSubject(T** slot, T* subject, SubjectCallback callback);
};

template <class T>
__declspec(weak) void IObserver::SetSubject(T** slot, T* subject, SubjectCallback callback) {
    if (subject == *slot)
        return;
    if (slot)
        CPublisher::CancelSubscription(*this, slot);
    *slot = subject;
    if (subject)
        CPublisher::AddSubscription(*this, slot, callback);
}

template void IObserver::SetSubject<ISubject>(ISubject**, ISubject*, SubjectCallback);
