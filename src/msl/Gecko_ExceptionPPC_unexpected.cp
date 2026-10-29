// Runtime Gecko_ExceptionPPC.cp: __unexpected and the weak
// std::bad_exception destructor it emits. Reconstructed from the disassembly
// (the runtime source is not available): when std::unexpected() throws,
// the new exception is rethrown if the function's exception specification
// allows it, otherwise std::bad_exception is thrown if the specification
// lists it (two spellings are checked), and std::terminate() is called
// otherwise. The specification check is an inline helper (one 'offset'
// slot per use). Compiled with C++ exceptions and RTTI on, like the rest of
// the original file, and with the 1.3.2 compiler: 1.3 differs here. The
// strings are not pooled (each starts on a 4-byte boundary). The
// <exception> classes are declared with the inline members the original
// emits (the weak std::exception copies come from another file; the
// bad_exception::what copy is placed by Gecko_ExceptionPPC_what). The rest
// of the file is in other units.
namespace std {
class exception {
public:
    exception() {}
    virtual ~exception() {}
    virtual const char* what() const { return "exception"; }
};

class bad_exception : public exception {
public:
    bad_exception() {}
    virtual ~bad_exception() {}
    virtual const char* what() const { return "bad_exception"; }
};

void unexpected();
void terminate();
}

typedef struct ex_specification_full {
    unsigned char action;
    unsigned char unused;
    unsigned short specs;
    long pcoffset;
    long cinfo_ref;
    char* spec[1];
} ex_specification_full;

typedef struct CatchInfo {
    void* location;
    void* typeinfo;
    void* dtor;
    void* sublocation;
    long pointercopy;
    void* stacktop;
} CatchInfo;

extern "C" char __throw_catch_compare(const char* throwtype, const char* catchtype, long* offset_result);

static inline int ExPPC_IsInSpecification(char* extype, ex_specification_full* spec) {
    long offset;
    int i;

    for (i = 0; i < spec->specs; i++) {
        if (__throw_catch_compare(extype, spec->spec[i], &offset))
            return 1;
    }
    return 0;
}

extern "C" void __unexpected(CatchInfo* catchinfo) {
    ex_specification_full* unexp = (ex_specification_full*)catchinfo->stacktop;

#pragma exception_magic
    try {
        std::unexpected();
    } catch (...) {
        if (ExPPC_IsInSpecification((char*)((CatchInfo*)&__exception_magic)->typeinfo, unexp))
            throw;
        if (ExPPC_IsInSpecification("!bad_exception!!", unexp))
            throw std::bad_exception();
        if (ExPPC_IsInSpecification("!std::bad_exception!!", unexp))
            throw std::bad_exception();
    }
    std::terminate();
}
