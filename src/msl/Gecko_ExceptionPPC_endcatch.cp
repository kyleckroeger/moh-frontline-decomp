// Runtime Gecko_ExceptionPPC.cp: __end__catch, which destroys the caught
// exception object (the catch information's location, with its destructor
// called with -1) when it has one. Reconstructed from the disassembly; the
// CatchInfo layout (location first, destructor at +8) is inferred from the
// offsets. The rest of the file is in other units.
typedef struct CatchInfo {
    void* location;
    void* typeinfo;
    void* dtor;
} CatchInfo;

extern "C" void __end__catch(CatchInfo* catchinfo) {
    if (catchinfo->location && catchinfo->dtor)
        ((void (*)(void*, short))catchinfo->dtor)(catchinfo->location, -1);
}
