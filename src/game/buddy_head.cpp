// The start of buddy.cpp (0x80056728..0x80057164): the built-in script
// functions SetBuddyPathGroup (stores a path base in g_buddy_path_base),
// UpdateBuddyCheckpoints (records the player's checkpoint, gives every active
// attached buddy its orders, then dumps the attached list through DebugMsg)
// and UpdateBuddyProgress (moves an attached buddy on to a checkpoint, or
// updates the progress of the ready buddies of that soldier, then dumps the
// ready list), with the weak dwi::vector<BuddyInfo> begin/end and
// vec_iter<BuddyInfo> destructor CW emits after their first user.
// The file's .rodata block starts with the dwi header's prime table, so it is
// defined here and the block is pooled (the functions address their strings
// from the block base). The globals are defined by the rest of the file
// (their __sinit is not part of this unit). Views of the script objects, the
// soldier, the containers and the buddy records are inferred.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
inline void* operator new(unsigned long, void* p) { return p; }

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short firstArgument;
    unsigned char unknown0e[2];
};

void DebugMsg(const char*, ...);

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

/* inferred: a script object's placement (position +16, path index +56) */
struct BSPlacementView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    unsigned char unknown1c[28];
    short pathIndex;
};

struct BSNodeView {
    unsigned char unknown00[4];
    BSPlacementView* placement;
};

struct BSStateView {
    unsigned char unknown00[92];
    int state;
};

/* inferred: a script object's class (table pointer at +12, the state getter in its second slot) */
class BSClassView {
public:
    unsigned char unknown00[12];

    virtual void vf08();
    virtual BSStateView* GetState();
    virtual class BSOwnerView* GetOwner();
};

struct BSObject {
    unsigned char unknown00[8];
    BSNodeView* m_node;
    BSClassView* m_class;
};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
extern BSObject* g_pBSObject;

/* inferred: the buddy soldier as seen here (its script object, virtual slot +96) */
class CBuddySoldier {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual BSObject* GetScriptObject() const;
};

/* inferred: the owner of a script object's class; its +148 slot gives the soldier */
class BSOwnerView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual class CBuddySoldier* GetSoldier();
};

/* inferred: an attached buddy (soldier, checkpoint progress, path base, moving flag, in use) */
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) {
        moving = 0;
        active = false;
        path = 0;
    }
    BuddyInfo(const BuddyInfo& other) : soldier(other.soldier), progress(other.progress) {
        moving = other.moving;
        active = other.active;
        path = other.path;
    }

    CBuddySoldier* soldier;
    int progress;
    int path;
    int moving;
    bool active;
};

namespace dwi {
template <class T>
class vec_iter {
public:
    vec_iter() : m_p(0) {}
    vec_iter(T* p) : m_p(p) {}
    vec_iter(const vec_iter& other) : m_p(other.m_p) {}
    ~vec_iter() { m_p = 0; }
    bool operator!=(const vec_iter& other) const { return m_p != other.m_p; }
    vec_iter& operator++() {
        m_p++;
        return *this;
    }
    T* operator->() const { return m_p; }
    T& operator*() const { return *m_p; }

    T* m_p;
};

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
    list(int capacity, I first, I last, char* name)
        : m_count(0), m_capacity(capacity),
          m_pool((list_node<T>*)DWI_allocalign(name, m_capacity * sizeof(list_node<T>), 16, 1024)), m_free(0),
          m_head(&m_head, &m_head, T()), m_owned(true) {
        list_node<T>* next = 0;
        list_node<T>* node = m_pool + (m_capacity - 1);
        while (node != m_pool) {
            node->m_next = next;
            next = node;
            node--;
        }
        node->m_next = next;
        m_free = node;
        insert(end(), first, last);
    }
    template <class I>
    void insert(list_iter<T> pos, I first, I last) {
        list_node<T>* p = pos.m_node;
        list_node<T>* tail = p->m_prev;
        for (; first != last; ++first) {
            list_node<T>* added = new (alloc_node()) list_node<T>(0, 0, *first);
            tail->m_next = added;
            added->m_prev = tail;
            tail = added;
            m_count++;
        }
        tail->m_next = p;
        p->m_prev = tail;
    }
    list_iter<T> begin() { return list_iter<T>(m_head.m_next); }
    list_iter<T> end() { return list_iter<T>(&m_head); }
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
    list() : m_count(0), m_capacity(0), m_pool(0), m_free(0), m_head(&m_head, &m_head, T()), m_owned(true) {}
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
    void push_front(const T& value) {
        list_node<T>* pos = m_head.m_next;
        list_node<T>* prev = pos->m_prev;
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

    ~list();
};

template <class T>
class vector {
public:
    vec_iter<T> begin() { return vec_iter<T>(m_data); }
    vec_iter<T> end() { return vec_iter<T>(m_data + m_size); }
    vector() : m_size(0), m_capacity(0), m_data(0), m_owned(true) {}
    void reserve(int, char*);
    void clear() {
        for (vec_iter<T> it = begin(); it != end(); ++it) {
        }
        m_size = 0;
    }
    void push_back(const T& value) {
        new (end().operator->()) T(value);
        m_size++;
    }
    T& operator[](int i) { return m_data[i]; }
    ~vector();

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};
}

/* inferred: a buddy path base entry */
struct BuddyPathInfo {
    BuddyPathInfo() : base(0) {}

    int base;
};

extern dwi::vector<BuddyInfo> g_attached_buddies;
extern dwi::list<BuddyInfo> g_ready_buddies;
extern dwi::vector<BuddyPathInfo> g_buddy_path_base;
extern bool g_bInitialized;
extern int g_player_checkpoint;

/* inferred inline: a script object's placement (none without a node) */
inline BSPlacementView* GetPlacement(BSObject* object) {
    return object->m_node ? object->m_node->placement : 0;
}

/* inferred inline: start the buddy along its next path while the player is ahead */
void AddBuddy(BuddyInfo, bool);

inline void GiveBuddyOrders(BuddyInfo* buddy) {
    DebugMsg("BUDDY_DEBUG: Attempting GiveBuddyOrders\n");
    if (buddy->progress < g_player_checkpoint) {
        if (!buddy->moving) {
            int path = buddy->progress + 1;
            BSObjectTriggerEvent(buddy->soldier->GetScriptObject(), 211, (void*)(buddy->path + path), g_pBSObject, false);
            buddy->moving = 1;
            BSObject* object = buddy->soldier->GetScriptObject();
            float z;
            float y;
            float x = 0.0f;
            y = x;
            z = x;
            int index = 0;
            if (object->m_node) {
                BSPlacementView* placement = object->m_node->placement;
                if (placement) {
                    x = placement->x;
                    y = placement->y;
                    z = placement->z;
                }
                if (placement)
                    index = placement->pathIndex;
            }
            object->m_class->GetState();
            DebugMsg("BUDDY_DEBUG: GiveBuddyOrders advancing buddy %x[%3d @(%4d,%4d,%4d)] (current=%d player=%d) using path %d\n",
                     buddy->soldier, index, (int)x, (int)y, (int)z, buddy->progress, g_player_checkpoint,
                     buddy->path + path);
        } else {
            DebugMsg("BUDDY_DEBUG: Already moving GiveBuddyOrders\n");
        }
    } else {
        DebugMsg("BUDDY_DEBUG: Stopping buddy\n");
        buddy->moving = 0;
    }
}

inline void SetBuddyProgress(dwi::vec_iter<BuddyInfo>& it, int checkpoint) {
    it->progress = checkpoint;
    it->moving = 0;
    GiveBuddyOrders(it.operator->());
}

inline void DumpAttachedBuddies() {
    DebugMsg("BUDDY_DEBUG: Dumping attached list...\n");
    CBuddySoldier* soldier;
    for (int i = 0; i < g_attached_buddies.m_size; i++) {
        if (g_attached_buddies[i].active == true) {
            soldier = g_attached_buddies[i].soldier;
            BSObject* object = soldier->GetScriptObject();
            float z;
            float y;
            float x = 0.0f;
            y = x;
            z = x;
            int index = 0;
            if (object->m_node) {
                BSPlacementView* placement = object->m_node->placement;
                if (placement) {
                    x = placement->x;
                    y = placement->y;
                    z = placement->z;
                }
                if (placement)
                    index = placement->pathIndex;
            }
            BSStateView* state = object->m_class->GetState();
            DebugMsg("BUDDY_DEBUG:   Attached=%x[%3d @(%4d,%4d,%4d) %4d] In Slot %d\n", soldier, index, (int)x, (int)y,
                     (int)z, state->state, i);
        }
    }
}

inline void DumpReadyBuddies() {
    dwi::list_iter<BuddyInfo> node(0);
    CBuddySoldier* soldier;
    BSObject* object;
    float x;
    float y;
    float z;
    int index;
    int i;
    BSStateView* state;
    DebugMsg("BUDDY_DEBUG: Dumping ready list...\n");
    i = 0;
    for (node = g_ready_buddies.begin(); node != g_ready_buddies.end(); ++node) {
        soldier = (*node).soldier;
        object = soldier->GetScriptObject();
        x = 0.0f;
        y = x;
        z = x;
        index = 0;
        if (object->m_node) {
            BSPlacementView* placement = object->m_node->placement;
            if (placement) {
                x = placement->x;
                y = placement->y;
                z = placement->z;
            }
            if (placement)
                index = placement->pathIndex;
        }
        state = object->m_class->GetState();
        DebugMsg("BUDDY_DEBUG:   Ready=%x[%3d @(%4d,%4d,%4d) %4d] In Slot %d\n", soldier, index, (int)x, (int)y, (int)z,
                 state->state, i);
        i++;
    }
}

void BIFunc_SetBuddyPathGroup(int** stack, void*) {
    int* args = *stack;
    short first = g_pBuiltInFunctions[g_iCurrentBIFIndex].firstArgument;
    int index = *(args - (first - 1));
    int base = *(args - (first - 2));
    DebugMsg("BUDDY_DEBUG: SetBuddyPathGroup Index=%d PathBase=%d\n", index, base);
    g_buddy_path_base[index].base = base;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_UpdateBuddyCheckpoints(int** stack, void*) {
    g_player_checkpoint = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].firstArgument - 1));
    DebugMsg("BUDDY_DEBUG: UpdateBuddyCheckpoints Checkpoint=%d\n", g_player_checkpoint);
    dwi::vec_iter<BuddyInfo> it;
    for (it = g_attached_buddies.begin(); it != g_attached_buddies.end(); ++it) {
        if (it->active)
            GiveBuddyOrders(it.operator->());
    }
    DumpAttachedBuddies();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_UpdateBuddyProgress(int** stack, void*) {
    float z;
    float y;
    float x;
    short first = g_pBuiltInFunctions[g_iCurrentBIFIndex].firstArgument;
    BSObject* object = (BSObject*)*(*stack - (first - 1));
    int checkpoint = *(*stack - (first - 2));
    CBuddySoldier* soldier = object->m_class->GetOwner()->GetSoldier();
    bool found = false;
    bool result = true;
    DebugMsg("BUDDY_DEBUG: Attempting UpdateBuddyProgress\n");
    dwi::vec_iter<BuddyInfo> it;
    for (it = g_attached_buddies.begin(); it != g_attached_buddies.end(); ++it) {
        if (it->soldier == soldier && it->active) {
            x = 0.0f;
            y = x;
            z = x;
            int index = 0;
            if (object->m_node) {
                BSPlacementView* placement = object->m_node->placement;
                if (placement) {
                    x = placement->x;
                    y = placement->y;
                    z = placement->z;
                }
                if (placement)
                    index = placement->pathIndex;
            }
            BSStateView* state = object->m_class->GetState();
            DebugMsg("BUDDY_DEBUG: UpdateBuddyProgress Buddy=%x[%3d @(%4d,%4d,%4d) %4d] Checkpoint=%d->%d\n", soldier,
                     index, (int)x, (int)y, (int)z, state->state, it->progress, checkpoint);
            if (it->progress >= checkpoint) {
                DebugMsg("BUDDY_DEBUG: Already past there\n");
            } else {
                DebugMsg("BUDDY_DEBUG: Haven't reached yet\n");
                SetBuddyProgress(it, checkpoint);
                result = it->moving;
            }
            found = true;
            break;
        }
    }
    if (!found) {
        for (dwi::list_iter<BuddyInfo> node = g_ready_buddies.begin(); node != g_ready_buddies.end(); ++node) {
            if ((*node).soldier == soldier && (*node).progress < checkpoint) {
                x = 0.0f;
                y = x;
                z = x;
                int index = 0;
                if (object->m_node) {
                    BSPlacementView* placement = object->m_node->placement;
                    if (placement) {
                        x = placement->x;
                        y = placement->y;
                        z = placement->z;
                    }
                    if (placement)
                        index = placement->pathIndex;
                }
                object->m_class->GetState();
                DebugMsg("BUDDY_DEBUG: Update Potential Buddy Progress Buddy=%x Checkpoint=%d\n", soldier, index, (int)x,
                         (int)y, (int)z, checkpoint);
                (*node).progress = checkpoint;
            }
        }
    }
    DumpReadyBuddies();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

