// A fragment of bsevents.cpp (0x800356b4): BSEventDecrimentReferenceCount
// drops an event memory block's reference count (at +32) and, when it falls
// to zero or below and the block is not already marked free (0xffff0000),
// marks it free and returns it to the event-memory allocator. The function,
// the allocator class and g_pMemBlockAllocator are named by the symbols; the
// block view and the free marker's meaning are inferred. The rest of the
// file is in other units.
class BSUtilObjectInstanceMemoryAllocator {
public:
    void FreeElement(void*);

    unsigned char unknown00[28];
};

extern BSUtilObjectInstanceMemoryAllocator g_pMemBlockAllocator;

/* inferred: an event memory block, its reference count at +32 */
struct EventMemoryBlockView {
    unsigned char unknown00[32];
    int m_references;
};

void BSEventDecrimentReferenceCount(void* memory) {
    EventMemoryBlockView* block = (EventMemoryBlockView*)memory;
    block->m_references--;
    if (block->m_references <= 0 && block->m_references != (int)0xffff0000) {
        block->m_references = 0xffff0000;
        g_pMemBlockAllocator.FreeElement(block);
    }
}
