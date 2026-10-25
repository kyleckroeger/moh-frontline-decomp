// A fragment of scene.cpp (0x800da178): CScene::RemoveNodeExtents removes the
// node's two extents (the max entry, then the min entry) from each of the
// scene's two extent lists, returning each list node to the list's free list,
// and clears the node's four extent links; CScene::AddNodeExtents appends the
// node's max and min extents to each list and keeps the four links. The file
// name is this project's. CScene, ISceneNode, its SExtent and the dwi::list
// template are named by the mangled symbols; the list layout is that of
// scene_list_extent; the extent view (node, max flag), the node's extent
// links, the inline search/removal helper and its predicate, and the list's
// inline push_back (a free node constructed in place, then linked before the
// sentinel) and node constructor are inferred. The compiler emits the two
// functions in reverse source order here, so AddNodeExtents is written
// first. The rest of the file is not part of this unit.
class ISceneNode {
public:
    struct SExtent {
        ISceneNode* m_node;
        bool m_max;
        unsigned char unknown05[11];
    };

    unsigned char unknown00[12];
    void* m_extentNodes[4];
};

/* the standard placement new (inferred; no header in this unit) */
inline void* operator new(unsigned long, void* p) { return p; }

namespace dwi {
template <class T>
struct list_node {
    list_node(const T& value) : m_prev(0), m_next(0), m_data(value) {}
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    T* push_back(const T& value) {
        list_node<T>* pos = &m_head;
        list_node<T>* node = m_free;
        list_node<T>* tail = pos->m_prev;
        m_free = node->m_next;
        new (node) list_node<T>(value);
        m_count++;
        tail->m_next = node;
        node->m_prev = tail;
        node->m_next = pos;
        pos->m_prev = node;
        return &node->m_data;
    }

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

/* inferred inline predicate */
inline bool IsExtent(const ISceneNode::SExtent& e, ISceneNode* node, bool max) {
    return e.m_node == node && e.m_max == max;
}

class CScene {
public:
    void RemoveNodeExtents(ISceneNode&);
    void AddNodeExtents(ISceneNode&);

    unsigned char unknown000[128];
    dwi::list<ISceneNode::SExtent> m_extents128;
    dwi::list<ISceneNode::SExtent> m_extents172;
};

/* inferred inline helper: removes the node's extent of the given kind */
inline void RemoveExtent(dwi::list<ISceneNode::SExtent>& extents, ISceneNode* node, bool max) {
    dwi::list_node<ISceneNode::SExtent>* p;
    for (p = extents.m_head.m_next; p != &extents.m_head; p = p->m_next) {
        if (IsExtent(p->m_data, node, max))
            break;
    }
    if (p != &extents.m_head) {
        dwi::list_node<ISceneNode::SExtent>* prev = p->m_prev;
        dwi::list_node<ISceneNode::SExtent>* next = p->m_next;
        prev->m_next = next;
        next->m_prev = prev;
        p->m_prev = 0;
        p->m_next = extents.m_free;
        extents.m_free = p;
        extents.m_count--;
    }
}

void CScene::AddNodeExtents(ISceneNode& node) {
    ISceneNode::SExtent extent;
    extent.m_node = &node;
    extent.m_max = true;
    node.m_extentNodes[0] = m_extents128.push_back(extent);
    extent.m_max = false;
    node.m_extentNodes[1] = m_extents128.push_back(extent);
    extent.m_max = true;
    node.m_extentNodes[2] = m_extents172.push_back(extent);
    extent.m_max = false;
    node.m_extentNodes[3] = m_extents172.push_back(extent);
}

void CScene::RemoveNodeExtents(ISceneNode& node) {
    RemoveExtent(m_extents128, &node, true);
    node.m_extentNodes[0] = 0;
    RemoveExtent(m_extents128, &node, false);
    node.m_extentNodes[1] = 0;
    RemoveExtent(m_extents172, &node, true);
    node.m_extentNodes[2] = 0;
    RemoveExtent(m_extents172, &node, false);
    node.m_extentNodes[3] = 0;
}
