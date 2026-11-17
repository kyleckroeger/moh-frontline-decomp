// A fragment of scene.cpp (0x800dd294): the weak
// dwi::list<ISceneNode::SExtent> destructor, which returns every node to
// the free list, resets the sentinel and the count, and frees the node
// storage when the list owns it. The template is named by the mangled
// symbols; its members, the node layout (links before the element, the
// sentinel node held in the list) and the element view (16 bytes, from the
// offset of the ownership flag) are inferred. The destructor is a weak
// template copy emitted in this file, defined __declspec(weak) out of the
// class and instantiated explicitly. The rest of the file is not part of
// this unit.
#include "game/ISceneNode.h"


extern "C" void MEM_free(void*);

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
    ~list();

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

template <class T> __declspec(weak) list<T>::~list() {
    list_node<T>* next;
    list_node<T>* node = m_head.m_next;

    while (node != &m_head) {
        next = node->m_next;
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
}

template class dwi::list<ISceneNode::SExtent>;
