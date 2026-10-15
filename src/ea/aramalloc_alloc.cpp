// A fragment of the ARAM allocator, aramalloc.cpp (0x8015067c): ARAM_alloc
// allocates a node from the pool manager (through the file-local
// ARAM_alloc_node, declared extern here) and returns its ARAM address, or
// prints an out-of-ARAM message with the request and returns 0. The record layouts and
// parameter names are inferred; ARAM_POOL_MANAGER, ARAM_DIRECTION and the
// functions are named by the symbols. The string is an entry of the file's
// data pool. The rest of the file is in other units.
struct ARAM_POOL_MANAGER;
enum ARAM_DIRECTION {};

/* Inferred: an allocated node; its ARAM address at +8. */
struct ARAM_NODE {
    unsigned char unknown00[8];
    unsigned int address;
};

extern "C" void PRINT_string(int, const char*, ...);

ARAM_NODE* ARAM_alloc_node(ARAM_POOL_MANAGER*, const char*, unsigned int, ARAM_DIRECTION); // file-local in the original

extern "C" unsigned int ARAM_alloc(ARAM_POOL_MANAGER* manager, const char* name, unsigned int size, ARAM_DIRECTION direction) {
    ARAM_NODE* node = ARAM_alloc_node(manager, name, size, direction);
    if (!node) {
        PRINT_string(2, "\nOut Of ARAM!!\n PoolManager 0x%p Size %d Type %d\n DebugString %s", manager, size, direction, name);
        return 0;
    }
    return node->address;
}
