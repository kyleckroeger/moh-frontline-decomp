extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
struct Info { int a; int b; };
namespace dwi {
template <class T> struct list_node { list_node* m_prev; list_node* m_next; T m_data; };
template <class T> class list {
public:
    list(int capacity);
    ~list() {
list_node<T>* next; list_node<T>* node = m_head.m_next; while (node != &m_head) { next = node->m_next; release(node); node = next; }         m_head.m_next = &m_head; m_head.m_prev = &m_head; m_count = 0; if (m_owned) MEM_free(m_pool); m_pool = 0;
    }
    void swap(list& o) { int c = m_count; m_count = o.m_count; o.m_count = c; list_node<T>* p = m_pool; m_pool = o.m_pool; o.m_pool = p; }
    void reserve(int, char*);
void release(list_node<T>* node) { node->m_prev = 0; node->m_next = m_free; m_free = node; }
    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
template <class T> __declspec(weak) void list<T>::reserve(int capacity, char* name) {
    if (capacity > m_capacity) {
        list<T> grown(capacity);
        swap(grown);
    }
}
}
dwi::list<Info> g_list(4);
void use() { g_list.reserve(8, "x"); }
