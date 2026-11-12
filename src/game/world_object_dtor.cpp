// A fragment of world_object.cpp (0x800b7608): the CWorldObject destructor
// (its virtual table pointer; then the members in reverse order: the world
// volume at +272, the request list at +200 and the compartment list at +64
// (each returns its nodes to the free list, destroying the compartments,
// resets its sentinel and count and frees owned node storage; the
// compartment list's sentinel element is destroyed too) and the memory
// factory at +36 (frees owned storage, clears its words); then the inline
// ISceneNode destructor and IObserver's; the object freed when asked) and the
// default constructor (scene-node base chain; an empty factory; the two lists,
// each with its sentinel element copied from a value-initialised temporary
// (a CCompartment, and a zeroed 40-byte request from the compiler's .bss
// object); the world volume for 4 sub-volumes; a flag and a word; each list
// reserved for 4 under the names "World Object Cache" and "World Object
// Requests"; the shared compartment data loaded and the compartment block
// size set to a quarter of what 9 MB leaves after it, capped at the largest
// compartment size listed by index (until a negative size), plus 128; the
// factory set up over four such blocks from DWI_allocalign, chained into its
// free list from the last). The file name is this project's; the original
// record is world_object.cpp. The classes, templates and functions are named
// by the mangled symbols (dwi::list<CCompartment>,
// dwi::list<CWorldObject::SRequest>, dwi::CMemFactory); the list view is as
// in world_volume_list.cpp, with an inferred constructor; the factory's
// members and its setup helper are inferred from its weak destructor and the
// code; the request's 40 bytes come from the list layout; the other members
// and names are inferred. Only the virtuals these functions need are
// declared. CWorldObject declares Draw (defined elsewhere; the result type is
// not known) first so its global virtual table is not emitted here.
// ISceneNode's destructor is inline and its table weak in the original, so
// the compiler's copies are weak duplicates. The constructor's exception
// cleanup also makes the compiler emit the containers' weak destructors; the
// original keeps them later in this file (after the two reserve
// instantiations, which are not part of this unit), so they are weak
// duplicates here too, linked to those copies. The name strings are items of
// the file's .rodata pool, linked at their original addresses.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
int GetCompartmentSizeFromIndex(int);
class CDrawContext;







#include "game/ISceneNode.h"


namespace dwi {
template <class T>
struct list_node {
    list_node(const T& data) : m_prev(this), m_next(this), m_data(data) {}
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    list() : m_count(0), m_unknown4(0), m_pool(0), m_free(0), m_head(T()) { m_owned = true; }
    void reserve(int, char*);
    ~list() {
        list_node<T>* node = m_head.m_next;
        list_node<T>* next;

        while (node != &m_head) {
            next = node->m_next;
            node->~list_node<T>();
            node->m_prev = 0;
            node->m_next = m_free;
            m_free = node;
            node = next;
        }
        m_head.m_next = &m_head;
        m_head.m_prev = &m_head;
        m_count = 0;
        if (m_owned)
            MEM_free(m_pool);
        m_pool = 0;
    }

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

class CMemFactory {
public:
    CMemFactory() : m_data(0), m_unknown4(0), m_unknown8(0), m_unknownc(0), m_unknown10(0), m_owned(false) {}
    void Init(void* data, int size, int count) {
        m_data = data;
        m_unknown4 = 0;
        m_unknown8 = size;
        m_unknownc = count;
        m_unknown10 = 0;
        m_owned = true;
        for (int i = count - 1; i >= 0; i--) {
            *(void**)((char*)m_data + m_unknown8 * i) = (void*)m_unknown4;
            m_unknown4 = (int)((char*)m_data + m_unknown8 * i);
        }
    }
    ~CMemFactory() {
        if (m_owned)
            MEM_free(m_data);
        m_data = 0;
        m_unknown4 = 0;
        m_unknown8 = 0;
        m_unknownc = 0;
        m_unknown10 = 0;
    }

    void* m_data;
    int m_unknown4;
    int m_unknown8;
    int m_unknownc;
    int m_unknown10;
    bool m_owned;
};
}

class CCompartment {
public:
    CCompartment();
    CCompartment(const CCompartment&);
    ~CCompartment();
    static void LoadShared();
    static int GetSharedSize();

    unsigned char unknown00[104];
} __attribute__((aligned(8)));

class CWorldVolume {
public:
    CWorldVolume(int);
    virtual ~CWorldVolume();

    unsigned char unknown04[76];
};

class CWorldObject : public ISceneNode {
public:
    struct SRequest {
        unsigned char unknown00[40];
    };

    virtual void Draw(CDrawContext&); /* result type not known */
    virtual ~CWorldObject();
    CWorldObject();
    static int m_sCompartmentSize;

    dwi::CMemFactory m_factory;
    unsigned char unknown3c[4];
    dwi::list<CCompartment> m_compartments;
    dwi::list<SRequest> m_requests;
    unsigned char unknown10c[4];
    CWorldVolume m_volume;
    bool data160;
    int data164;
};

CWorldObject::~CWorldObject() {
}

CWorldObject::CWorldObject() : m_volume(4) {
    data160 = true;
    data164 = 0;
    m_compartments.reserve(4, "World Object Cache");
    m_requests.reserve(4, "World Object Requests");
    CCompartment::LoadShared();
    m_sCompartmentSize = (0x900000 - CCompartment::GetSharedSize()) / 4;
    int largest = 0;
    for (int i = 0;; i++) {
        int size = GetCompartmentSizeFromIndex(i);
        if (size > largest)
            largest = size;
        else if (size < 0)
            break;
    }
    if (largest < m_sCompartmentSize)
        m_sCompartmentSize = largest;
    m_sCompartmentSize += 128;
    m_factory.Init(DWI_allocalign(0, m_sCompartmentSize * 4, 16, 1024), m_sCompartmentSize, 4);
}
