// alarm.cpp (0x80059974..0x8005a3cc): the alarm list builtins (SetAlarmActivated,
// IsAlarmActivated, AlarmIsAvailable, GetClosestAlarm, SetAlarmOccupy, UnregisterAlarm,
// RegisterAlarm), the weak dwi::list<AlarmInfo> copies emitted here (reserve, the destructor,
// the range constructor) and the static initialisation of g_alarms. Merged from the
// fragments alarm_builtins, alarm_register, alarm_list_reserve, alarm_list, alarm_list_ctor
// and alarm. The dwi templates and AlarmInfo are named by the symbols; the list layout and
// its inline members are this file's own view (inferred, as the fragments had them: the
// range constructor through link_range/init_free, the destructor through clear(): the
// out-of-line ~list and its copy inlined into reserve colour alike only when both expand an
// inline holding the loop, research/notes/loop3-tricks-2026-11-12.md). AlarmInfo's members are inferred
// (the object registered and the script occupying it). CAIFilterRealPosition is named by its
// out-of-line constructor; deriving it from CVector3 (16 bytes, 8-aligned) is inferred.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
inline void* operator new(unsigned long, void* p) { return p; }

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CAnimObject;
class CSoldierObject;
struct AIDoodadView;

#include "game/ISceneNode.h"

class CSoldierObject : public ISceneNode {
};

class CAIFilterRealPosition : public CVector3 {
public:
    CAIFilterRealPosition();
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short firstArgument;
    unsigned char unknown0e[2];
};

union BSValueView {
    int i;
    BSObject* object;
};

struct AlarmInfo {
    AlarmInfo() {}
    AlarmInfo(void* object) : m_object((ISceneNode*)object), m_script(0) {}
    AlarmInfo(const AlarmInfo& o) : m_object(o.m_object), m_script(o.m_script) {}
    ISceneNode* m_object;
    BSObject* m_script;
};

namespace dwi {
template <class T>
struct list_node {
    list_node() {}
    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list_iter {
public:
    list_iter(list_node<T>* node) : m_node(node) {}
    list_iter(const list_iter& other) : m_node(other.m_node) {}
    T& operator*() const { return m_node->m_data; }
    list_iter& operator++() {
        m_node = m_node->m_next;
        return *this;
    }
    bool operator!=(const list_iter& other) const { return m_node != other.m_node; }

    list_node<T>* m_node;
};

template <class T>
class list {
public:
    template <class I>
    list(int, I, I, char*);
    list() : m_count(0), m_capacity(0), m_pool(0), m_free(0) {
        m_head.m_prev = &m_head;
        m_head.m_next = &m_head;
        m_head.m_data.m_object = 0;
        m_head.m_data.m_script = 0;
        m_owned = true;
    }
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
    template <class I2> void link_range(I2 end, I2 first, I2 last) {
        list_node<T>* head = end.m_node;
        list_node<T>* stop = last.m_node;
        list_node<T>* tail = m_head.m_prev;
        for (I2 it = first; it.m_node != stop; ++it) {
            list_node<T>* added = new (alloc_node()) list_node<T>(0, 0, *it);
            tail->m_next = added;
            added->m_prev = tail;
            tail = added;
            m_count++;
        }
        tail->m_next = head;
        head->m_prev = tail;
    }
    void init_free() {
        m_free = 0;
        m_head.m_prev = &m_head;
        m_head.m_next = &m_head;
        m_head.m_data.m_object = 0;
        m_head.m_data.m_script = 0;
        m_owned = true;
        list_node<T>* next = 0;
        list_node<T>* node = m_pool + (m_capacity - 1);
        while (node != m_pool) {
            node->m_next = next;
            next = node;
            node--;
        }
        node->m_next = next;
        m_free = node;
    }
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    list_iter<T> begin() { return list_iter<T>(m_head.m_next); }
    list_iter<T> end() { return list_iter<T>((list_node<T>*)((char*)this + 16)); }
    void clear() {
        list_node<T>* node = m_head.m_next;
        list_node<T>* next;
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
    }
    ~list() {
        clear();
        if (m_owned)
            MEM_free(m_pool);
        m_pool = 0;
    }

    void swap(list& other) {
        int count = m_count;
        m_count = other.m_count;
        other.m_count = count;
        int capacity = m_capacity;
        m_capacity = other.m_capacity;
        other.m_capacity = capacity;
        list_node<T>* pool = m_pool;
        m_pool = other.m_pool;
        other.m_pool = pool;
        list_node<T>* free = m_free;
        m_free = other.m_free;
        other.m_free = free;
        bool owned = m_owned;
        m_owned = other.m_owned;
        other.m_owned = owned;
        if (m_count != 0) {
            if (other.m_count != 0) {
                list_node<T>* prev = m_head.m_prev;
                m_head.m_prev = other.m_head.m_prev;
                other.m_head.m_prev = prev;
                list_node<T>* next = m_head.m_next;
                m_head.m_next = other.m_head.m_next;
                other.m_head.m_next = next;
                m_head.m_prev->m_next = &m_head;
                other.m_head.m_prev->m_next = &other.m_head;
            } else {
                other.m_head.m_prev = m_head.m_prev;
                other.m_head.m_next = m_head.m_next;
                other.m_head.m_prev->m_next = &other.m_head;
                m_head.m_next = &m_head;
                m_head.m_prev = &m_head;
            }
        } else if (other.m_count != 0) {
            m_head.m_prev = other.m_head.m_prev;
            m_head.m_next = other.m_head.m_next;
            m_head.m_prev->m_next = &m_head;
            other.m_head.m_next = &other.m_head;
            other.m_head.m_prev = &other.m_head;
        }
    }

    void reserve(int, char*);

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

template <class T> template <class I> list<T>::list(int capacity, I first, I last, char* name) {
    m_count = 0;
    m_capacity = capacity;
    m_pool = (list_node<T>*)DWI_allocalign(name, m_capacity * sizeof(list_node<T>), 16, 1024);
    init_free();
    link_range(end(), first, last);
}

template <class T> __declspec(weak) void list<T>::reserve(int capacity, char* name) {
    if (capacity > m_capacity) {
        list<T> grown(capacity, begin(), end(), name);
        swap(grown);
    }
}
}


extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
extern bool g_alarm_activated;

dwi::list<AlarmInfo> g_alarms;

void BIFunc_SetAlarmActivated(int** stack, void*) {
    BSBuiltinView* builtin = &g_pBuiltInFunctions[g_iCurrentBIFIndex];
    g_alarm_activated = *(*stack - (builtin->firstArgument - 1)) != 0;
    *stack -= builtin->argumentCount;
}

void BIFunc_IsAlarmActivated(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = g_alarm_activated;
}

static inline float NodeDistance(ISceneNode* a, ISceneNode* b) {
    CAIFilterRealPosition pa;
    CAIFilterRealPosition pb;
    a->GetPosition(pa);
    b->GetPosition(pb);
    float dx = pa.x - pb.x;
    float dy = pa.y - pb.y;
    float dz = pa.z - pb.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

void BIFunc_AlarmIsAvailable(int** stack, void* object) {
    ISceneNode* alarm;
    dwi::list_node<AlarmInfo>* node;
    dwi::list_node<AlarmInfo>* best;
    dwi::list_node<AlarmInfo>* end;
    bool available;
    float bestDistance;
    float distance;
    available = g_alarms.m_count > 0;
    if (available) {
        end = &g_alarms.m_head;
        best = end;
        bestDistance = 50.0f;
        for (node = g_alarms.m_head.m_next; node != end; node = node->m_next) {
            alarm = node->m_data.m_object;
            distance = NodeDistance((ISceneNode*)object, alarm);
            if (distance < bestDistance && node->m_data.m_script == 0) {
                best = node;
                bestDistance = distance;
            }
        }
        if (best == end)
            available = false;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = available;
}

void BIFunc_GetClosestAlarm(int** stack, void* object) {
    ISceneNode* alarm;
    dwi::list_node<AlarmInfo>* node;
    dwi::list_node<AlarmInfo>* best;
    dwi::list_node<AlarmInfo>* end;
    float bestDistance;
    float distance;
    end = &g_alarms.m_head;
    best = end;
    bestDistance = 50.0f;
    for (node = g_alarms.m_head.m_next; node != end; node = node->m_next) {
        alarm = node->m_data.m_object;
        distance = NodeDistance((ISceneNode*)object, alarm);
        if (distance < bestDistance && node->m_data.m_script == 0) {
            best = node;
            bestDistance = distance;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    if (best != end) {
        BSValueView value;
        value.object = best->m_data.m_object->GetScriptObject();
        *(BSValueView*)*stack = value;
    } else {
        BSValueView value;
        value.i = 0;
        *(BSValueView*)*stack = value;
    }
}

void BIFunc_SetAlarmOccupy(int** stack, void* object) {
    bool occupy;
    dwi::list_node<AlarmInfo>* node;
    dwi::list_node<AlarmInfo>* end;
    BSObject* alarm;
    BSObject* script;
    int* args = *stack;
    short count = g_pBuiltInFunctions[g_iCurrentBIFIndex].firstArgument;
    alarm = *(BSObject**)(args - (count - 1));
    occupy = *(args - (count - 2)) != 0;
    script = ((ISceneNode*)object)->AsSoldierObject()->GetScriptObject();
    end = &g_alarms.m_head;
    for (node = g_alarms.m_head.m_next; node != end; node = node->m_next) {
        if (((ISceneNode*)node->m_data.m_object)->GetScriptObject() == alarm) {
            node->m_data.m_script = occupy ? script : 0;
            break;
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

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

template class dwi::list<AlarmInfo>;
