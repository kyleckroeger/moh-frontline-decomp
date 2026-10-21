// A fragment of world_volume.cpp (0x800cbf1c): CWorldVolume::RemoveSubVolume
// (const CCDBObject&) walks the sub-volume list and removes the first node
// holding the object through the iterator overload (defined in
// world_volume_edit.cpp; kept in its own unit because the original calls it
// rather than inlining it). CWorldVolume, CCDBObject and the dwi list
// templates are named by the mangled symbols; the list view is
// world_volume_list.cpp's; the iterator's user copy constructor lets the by-value
// argument be built in place, as in the original.
class CCDBObject;

namespace dwi {
template <class T>
struct list_node {
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list_iter {
public:
    list_iter(list_node<T>* node) : m_node(node) {}
    list_iter(const list_iter& other) : m_node(other.m_node) {}
    list_node<T>* m_node;
};

template <class T>
class list {
public:
    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

class CWorldVolume {
public:
    void RemoveSubVolume(dwi::list_iter<const CCDBObject*>);
    void RemoveSubVolume(const CCDBObject&);

    unsigned char unknown00[4];
    dwi::list<const CCDBObject*> m_subVolumes;
};

void CWorldVolume::RemoveSubVolume(const CCDBObject& object) {
    dwi::list_node<const CCDBObject*>* node = m_subVolumes.m_head.m_next;
    dwi::list_node<const CCDBObject*>* end = &m_subVolumes.m_head;
    for (; node != end; node = node->m_next) {
        if (node->m_data == &object) {
            RemoveSubVolume(dwi::list_iter<const CCDBObject*>(node));
            return;
        }
    }
}
