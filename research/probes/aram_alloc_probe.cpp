// settings: config/GMFE69/aramalloc_alloc.json
// replaces: ARAM_alloc
// Test harness (W1, pure image): ARAM_alloc as aramalloc_alloc.cpp has it, reporting every
// allocation (pool, name, size, direction, ARAM address, caller) to compare ARAM layouts.
struct ARAM_POOL_MANAGER;
enum ARAM_DIRECTION {};
struct ARAM_NODE {
    unsigned char unknown00[8];
    unsigned int address;
};
extern "C" void PRINT_string(int, const char*, ...);
extern "C" void OSReport(const char*, ...);
ARAM_NODE* ARAM_alloc_node(ARAM_POOL_MANAGER*, const char*, unsigned int, ARAM_DIRECTION); // file-local in the original
extern "C" unsigned int ARAM_alloc(ARAM_POOL_MANAGER* manager, const char* name, unsigned int size, ARAM_DIRECTION direction) {
    register unsigned long lr;
    asm { mflr lr }
    ARAM_NODE* node = ARAM_alloc_node(manager, name, size, direction);
    OSReport("ARAMALLOC pool %08x size %d dir %d -> %08x lr %08x %s\n", (unsigned long)manager, size, (int)direction,
             node ? node->address : 0, lr, name ? name : "");
    if (!node) {
        PRINT_string(2, "\nOut Of ARAM!!\n PoolManager 0x%p Size %d Type %d\n DebugString %s", manager, size, direction, name);
        return 0;
    }
    return node->address;
}
