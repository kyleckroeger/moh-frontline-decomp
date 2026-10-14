// The tail of bsevents.cpp (0x800359fc): BSEventGetMemoryRequirements (the
// allocator's size for 128 blocks of 36 bytes plus a fixed 4224 bytes), then
// the static initialisation (0x80035a34). The static initialisation registers the
// destructor of the global event-memory allocator, whose constructor is
// empty; the weak empty destructor follows (emitted here for the
// registration). The rest of the file is in other units. g_pMemBlockAllocator and
// BSUtilObjectInstanceMemoryAllocator are named by the symbols; the object
// size comes from the symbol.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator() {}
    int ComputeNeededSize(int, int, int);

    unsigned char unknown00[28];
};

BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;

int BSEventGetMemoryRequirements() {
    return g_pMemBlockAllocator.ComputeNeededSize(36, 128, 0) + 4224;
}
