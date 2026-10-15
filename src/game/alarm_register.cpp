// A fragment of alarm.cpp (0x80059e6c): the built-in script functions
// UnregisterAlarm (after popping its arguments, the calling object's entry
// is found, its script object, if any, is sent event 247 and the entry is
// erased through the list's inline erase) and RegisterAlarm. The alarm list is given a pool of 15 nodes on first use
// (through the weak reserve, which follows in the image and is only declared
// here), then the calling object is appended with a cleared second word
// through the list's inline push_back. The dwi templates, AlarmInfo and
// g_alarms are named by the symbols; the list layout (as in
// alarm_list_reserve.cpp), push_back, erase, AlarmInfo's members and constructor and the
// built-in record view (as in alarm_builtins.cpp) are inferred. The rest of
// the file is not part of this unit.
inline void* operator new(unsigned long, void* p) { return p; }

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

struct AlarmInfo {
    AlarmInfo(void* object) : m_object(object), m_script(0) {}

    void* m_object;
    BSObject* m_script;
};

namespace dwi {
template <class T>
struct list_node {
    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    void reserve(int, char*);
    void erase(list_node<T>* node) {
        list_node<T>* prev = node->m_prev;
        list_node<T>* next = node->m_next;
        prev->m_next = next;
        next->m_prev = prev;
        node->m_prev = 0;
        node->m_next = m_free;
        m_free = node;
        m_count--;
    }
    void push_back(const T& value) {
        list_node<T>* prev = m_head.m_prev;
        list_node<T>* pos = &m_head;
        list_node<T>* node = alloc_node();
        new (node) list_node<T>(0, 0, value);
        m_count++;
        prev->m_next = node;
        node->m_prev = prev;
        node->m_next = pos;
        pos->m_prev = node;
    }

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short firstArgument;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
extern dwi::list<AlarmInfo> g_alarms;

void BIFunc_UnregisterAlarm(int** stack, void* object) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    for (dwi::list_node<AlarmInfo>* node = g_alarms.m_head.m_next; node != &g_alarms.m_head; node = node->m_next) {
        if (node->m_data.m_object == object) {
            if (node->m_data.m_script)
                BSObjectTriggerEvent(node->m_data.m_script, 247, 0, node->m_data.m_script, false);
            g_alarms.erase(node);
            break;
        }
    }
}

void BIFunc_RegisterAlarm(int** stack, void* object) {
    if (g_alarms.m_capacity == 0)
        g_alarms.reserve(15, "Alarm Pool");
    g_alarms.push_back(AlarmInfo(object));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
