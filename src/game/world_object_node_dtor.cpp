// A fragment of world_object.cpp (0x800b8248): the weak destructor of
// dwi::list_node<CCompartment>, which destroys the node's compartment. The
// template and CCompartment are named by the mangled symbols; the node layout
// (links before the element) is that of world_object_list.cpp and
// CCompartment is a 104-byte view. The destructor is a weak template copy
// emitted in this file, defined __declspec(weak) out of the class and
// instantiated explicitly. The rest of the file is not part of this unit.
class CCompartment {
public:
    ~CCompartment();

    unsigned char unknown00[104];
} __attribute__((aligned(8)));

namespace dwi {
template <class T>
struct list_node {
    ~list_node();

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T> __declspec(weak) list_node<T>::~list_node() {
}
}

template struct dwi::list_node<CCompartment>;
