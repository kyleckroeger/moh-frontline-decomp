// A fragment of the ARAM allocator, aramalloc.cpp (0x801503b4): the
// file-local ARAM_alloc_node rounds the request up to the manager's
// alignment and tries each pool in turn: a low request (direction 0) takes
// the first free block from the start of the pool's free list that is large
// enough, a high request (0x100) the first one from its end (a sentinel
// node of size 0xffffffff on the stack ends each search); the block is split
// (taken from its start or its end) or removed and released to the node
// pool, and a node taken from the manager's node pool records the
// allocation on the pool's used list (or an out-of-nodes message is
// printed). ARAM_alloc allocates a node and returns its ARAM address, or
// prints an out-of-ARAM message with the request and returns 0. The record
// layouts, list helpers and parameter names are inferred; ARAM_POOL_MANAGER,
// ARAM_DIRECTION and the functions are named by the symbols. The strings are
// entries of the file's data pool. The rest of the file is in other units.
struct ARAM_POOL;

enum ARAM_DIRECTION {};

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
    unsigned int address;
    unsigned int size;
    ARAM_POOL* pool;
    unsigned char unknown14[8];
};

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

extern "C" void PRINT_string(int, const char*, ...);
extern "C" void BFree(BPoolMan*, void*);

static inline void AramListAppend(ARAM_LIST* list, ARAM_NODE* node) {
    ARAM_NODE* last;
    ARAM_NODE* pos;
    pos = (ARAM_NODE*)list;
    last = pos->prev;

    pos->prev = node;
    last->next = node;
    node->next = pos;
    node->prev = last;
}

static inline void AramListAdd(ARAM_LIST* list, ARAM_NODE* node) {
    ARAM_NODE* pos = (ARAM_NODE*)list;
    ARAM_NODE* last = pos->prev;

    pos->prev = node;
    last->next = node;
    node->next = pos;
    node->prev = last;
}

static inline void AramListPrepend(ARAM_LIST* list, ARAM_NODE* node) {
    ARAM_NODE* first;
    ARAM_NODE* pos = (ARAM_NODE*)list;
    first = pos->next;

    pos->next = node;
    first->prev = node;
    node->prev = pos;
    node->next = first;
}

static inline void AramNodeUnlink(ARAM_NODE* node) {
    ARAM_NODE* next = node->next;
    ARAM_NODE* prev = node->prev;

    prev->next = next;
    next->prev = prev;
}

static inline BNode* BListPop(BList* list) {
    BNode* node;

    if (list->head == list->tail)
        return 0;
    if (list->head == list->tail)
        list->tail = (BNode*)list;
    node = list->head;
    list->head = node->next;
    return node;
}

static inline ARAM_NODE* AramRecord(ARAM_POOL_MANAGER* manager, ARAM_POOL* pool, unsigned int address,
                                    unsigned int size) {
    BList* list = &manager->nodes->free;
    ARAM_NODE* node = (ARAM_NODE*)BListPop(list);

    if (!node) {
        PRINT_string(2, "Out Of ARAM malloc nodes add to your MaxNumberOfMallocs with ARAM_NEW_poolmanager\n");
        return 0;
    }
    node->size = size;
    node->address = address;
    node->pool = pool;
    AramListAdd(&pool->used, node);
    return node;
}

static inline ARAM_NODE* AramAllocLow(ARAM_POOL_MANAGER* manager, ARAM_POOL* pool, unsigned int size) {
    unsigned int address;
    ARAM_NODE* node;
    ARAM_NODE sentinel;

    sentinel.size = 0xffffffff;
    AramListAppend(&pool->free, &sentinel);
    for (node = pool->free.head;; node = node->next) {
        if (node->size >= size) {
            AramNodeUnlink(&sentinel);
            if (&sentinel == node)
                return 0;
            if (node->size > size) {
                address = node->address;
                node->address = address + size;
                node->size -= size;
            } else {
                address = node->address;
                AramNodeUnlink(node);
                BFree(manager->nodes, node);
            }
            break;
        }
    }
    return AramRecord(manager, pool, address, size);
}

static inline ARAM_NODE* AramAllocHigh(ARAM_POOL_MANAGER* manager, ARAM_POOL* pool, unsigned int size) {
    unsigned int address;
    ARAM_NODE* node;
    ARAM_NODE sentinel;

    sentinel.size = 0xffffffff;
    AramListPrepend(&pool->free, &sentinel);
    for (node = pool->free.tail;; node = node->prev) {
        if (node->size >= size) {
            AramNodeUnlink(&sentinel);
            if (&sentinel == node)
                return 0;
            if (node->size > size) {
                address = node->address + (node->size -= size);
            } else {
                address = node->address;
                AramNodeUnlink(node);
                BFree(manager->nodes, node);
            }
            break;
        }
    }
    return AramRecord(manager, pool, address, size);
}

static inline unsigned int AramAlign(ARAM_POOL_MANAGER* manager, unsigned int value) {
    return value + (manager->align - 1) & ~(manager->align - 1);
}

static ARAM_NODE* ARAM_alloc_node(ARAM_POOL_MANAGER* manager, const char* name, unsigned int size,
                                  ARAM_DIRECTION direction) {
    ARAM_NODE* node = 0;
    ARAM_POOL* pool;

    size = AramAlign(manager, size);
    if (direction == 0) {
        for (pool = manager->next; pool != (ARAM_POOL*)manager; pool = pool->next) {
            node = AramAllocLow(manager, pool, size);
            if (node)
                break;
        }
    } else if (direction == 0x100) {
        for (pool = manager->next; pool != (ARAM_POOL*)manager; pool = pool->next) {
            node = AramAllocHigh(manager, pool, size);
            if (node)
                break;
        }
    }
    return node;
}

extern "C" unsigned int ARAM_alloc(ARAM_POOL_MANAGER* manager, const char* name, unsigned int size, ARAM_DIRECTION direction) {
    ARAM_NODE* node = ARAM_alloc_node(manager, name, size, direction);
    if (!node) {
        PRINT_string(2, "\nOut Of ARAM!!\n PoolManager 0x%p Size %d Type %d\n DebugString %s", manager, size, direction, name);
        return 0;
    }
    return node->address;
}
