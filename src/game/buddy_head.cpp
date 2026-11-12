// The start of buddy.cpp (0x80056728..0x80056b6c): the built-in script
// functions SetBuddyPathGroup (stores a path base in g_buddy_path_base) and
// UpdateBuddyCheckpoints (records the player's checkpoint, gives every active
// attached buddy its orders, then dumps the attached list through DebugMsg).
// The file's .rodata block starts with the dwi header's prime table, so it is
// defined here and the block is pooled (UpdateBuddyCheckpoints addresses its
// strings from the block base). The globals are defined by the rest of the
// file (their __sinit is not part of this unit); the vector<BuddyInfo>
// begin/end and vec_iter destructor weak copies are placed by their own units.
// Views of the script objects, the soldier and the buddy records are inferred.
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

/* inferred: an attached buddy (soldier, checkpoint progress, path base, moving flag, in use) */
template <class T> inline void Zero(T& x) { x = 0; }
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) {
        Clear(this);
    }
    static void Clear(BuddyInfo* b) {
        b->moving = 0;
        b->active = false;
        b->path = 0;
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
extern dwi::vector<BuddyPathInfo> g_buddy_path_base;
extern int g_player_checkpoint;

/* inferred inline: start the buddy along its next path while the player is ahead */
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

