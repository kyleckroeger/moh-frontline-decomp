// A fragment of world_volume.cpp (0x800cc29c): the CWorldVolume constructor
// (IVolume's and CWorldVolume's table pointers, the sub-volume list through
// the inline dwi::list constructor, the word at +72 cleared, then the list
// reserved for the given count under the name "World volume sub volumes"). The
// file name is this project's; the original record is world_volume.cpp, after
// world_volume_list.cpp. IVolume, CWorldVolume, CCDBObject and the dwi::list
// template are named by the mangled symbols; the list view is
// world_volume_list.cpp's (its constructor, an empty list owning its nodes, is
// inferred; the sentinel links are taken through a copy of the list pointer,
// which keeps the head address relative to the list rather than folded into
// the volume; its destructor, the weak copy placed by world_volume_list.cpp,
// is declared for the constructor's cleanup action), the other members are
// inferred, and only the virtuals the constructor needs are declared.
// CWorldVolume declares its TestCollision override for IVolume (defined
// elsewhere) before its destructor, so its global virtual table is not emitted
// here. IVolume's destructor is inline (weak in the original), so the
// compiler's copies of it and of IVolume's weak virtual table are weak
// duplicates, linked to the original copies. The name string is an item of the
// file's .rodata pool, linked at its original address.
class CCDBObject;
class CCollision;

namespace dwi {
template <class T>
struct list_node {
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    list() : m_count(0), m_unknown4(0), m_pool(0), m_free(0) {
        list* self = this;
        m_head.m_prev = &self->m_head;
        m_head.m_next = &self->m_head;
        m_head.m_data = 0;
        m_owned = true;
    }
    ~list();
    void reserve(int, char*);

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CWorldVolume : public IVolume {
public:
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual ~CWorldVolume();
    CWorldVolume(int);

    dwi::list<const CCDBObject*> m_subVolumes;
    unsigned char unknown24[36];
    int data48;
};

CWorldVolume::CWorldVolume(int count) {
    data48 = 0;
    m_subVolumes.reserve(count, "World volume sub volumes");
}
