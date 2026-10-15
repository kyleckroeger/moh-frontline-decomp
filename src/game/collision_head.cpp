// The start of collision.cpp (0x800c74c8): CCollision::ClearContacts returns
// every contact to the file's contact pool (g_contactPool, file-local in the
// original and defined elsewhere) and counts them off. CCollision,
// SClsnContact and dwi::CFactory are named by the mangled symbols; the
// members are inferred (see collision_contacts.cpp). The rest of the file
// (CCollision::Test onwards) is not part of this unit.
struct SClsnContact {
    unsigned char unknown00[44];
    SClsnContact* m_next;
};

namespace dwi {
template <class T, class P>
class CFactory {
public:
    void Free(P p) {
        m_used--;
        *(P*)p = m_free;
        m_free = p;
    }

    T* m_memory;
    P m_free;
    int m_capacity;
    int m_used;
    bool m_owned;
};
}

extern dwi::CFactory<SClsnContact, SClsnContact*> g_contactPool;

class CCollision {
public:
    void ClearContacts();

    unsigned char unknown00[24];
    SClsnContact* m_contacts;
    int m_numContacts;
};

void CCollision::ClearContacts() {
    while (m_contacts) {
        SClsnContact* next = m_contacts->m_next;
        g_contactPool.Free(m_contacts);
        m_contacts = next;
        m_numContacts--;
    }
}
