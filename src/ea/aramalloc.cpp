// Fragment of the ARAM allocator: pool managers and pools. A pool keeps a
// free list and a used list of ARAM ranges; range nodes come from a BPoolMan.
// ARAM_alloc_node and ARAM_alloc that follow are not reconstructed. The
// record layouts are inferred from offsets; the type and member names are not
// original (ARAM_POOL_MANAGER is named by the mangled symbols).
struct BNode {
    BNode* next;
};

struct BList {
    BNode* head;
    BNode* tail;
};

struct BPoolMan {
    void* next;
    void* tail;
    BList free;
    unsigned int size;
};

struct ARAM_NODE {
    ARAM_NODE* next;
    ARAM_NODE* prev;
    unsigned int start;
    unsigned int size;
    void* pool;
};

struct ARAM_POOL;

struct ARAM_POOL_MANAGER {
    ARAM_POOL* next;
    ARAM_POOL* prev;
    unsigned int align;
    int field0C;
    BPoolMan* nodes;
};

struct ARAM_LIST {
    ARAM_NODE* head;
    ARAM_NODE* tail;
};

struct ARAM_POOL {
    ARAM_POOL* next;
    ARAM_POOL* prev;
    ARAM_LIST free;
    ARAM_LIST used;
    char* name;
};

// The list head doubles as the sentinel node of the ring (head = next,
// tail = prev); the append works on it through a node pointer.
static inline void AramListAppend(ARAM_LIST* list, ARAM_NODE* node) {
    ARAM_NODE* pos = (ARAM_NODE*)list;
    ARAM_NODE* last = pos->prev;

    pos->prev = node;
    last->next = node;
    node->next = pos;
    node->prev = last;
}

extern "C" {
extern int mb_default;
void* MEM_alloc(const char*, int, int);
BPoolMan* NEW_BPoolMan(unsigned int, unsigned int);
char* strcpy(char*, const char*);
void PRINT_string(int, const char*, ...);
}

// Inline list helpers (inferred: the target inlines a list pop and a list
// append); their names are not original.
static inline BNode* BListPop(BList* list) {
    BNode* node = list->head;

    if (node == list->tail)
        return 0;
    if (list->head == list->tail)
        list->tail = (BNode*)list;
    node = list->head;
    list->head = node->next;
    return node;
}

extern "C" ARAM_POOL_MANAGER* ARAM_NEW_poolmanager(unsigned int align, unsigned int maxnodes, int unused,
                                                   const char* name) {
    ARAM_POOL_MANAGER* manager = (ARAM_POOL_MANAGER*)MEM_alloc(name, 20, mb_default);

    manager->next = (ARAM_POOL*)manager;
    manager->prev = (ARAM_POOL*)manager;
    manager->align = align;
    manager->nodes = NEW_BPoolMan(maxnodes, 28);
    manager->field0C = 0;
    return manager;
}

extern "C" ARAM_POOL* ARAM_NEW_pool(ARAM_POOL_MANAGER* manager, unsigned int start, unsigned int size,
                                    const char* name) {
    ARAM_POOL* pool = (ARAM_POOL*)MEM_alloc(name, 28, mb_default);
    ARAM_POOL* tail;
    ARAM_NODE* node;

    pool->name = (char*)MEM_alloc("ARAM Pool Name", 28, mb_default);
    strcpy(pool->name, name);
    if (start & (manager->align - 1)) {
        PRINT_string(2, "ARAMStartAddress is not Aligned\n");
        start = start + (manager->align - 1) & ~(manager->align - 1);
    }
    if (size & (manager->align - 1)) {
        PRINT_string(2, "ARAMMemorySize is not Aligned\n");
        size &= ~(manager->align - 1);
    }
    tail = manager->prev;
    manager->prev = pool;
    tail->next = pool;
    pool->next = (ARAM_POOL*)manager;
    pool->prev = tail;
    pool->free.head = (ARAM_NODE*)&pool->free;
    pool->free.tail = (ARAM_NODE*)&pool->free;
    pool->used.head = (ARAM_NODE*)&pool->used;
    pool->used.tail = (ARAM_NODE*)&pool->used;
    node = (ARAM_NODE*)BListPop(&manager->nodes->free);
    node->start = start;
    node->size = size;
    AramListAppend(&pool->free, node);
    return pool;
}
