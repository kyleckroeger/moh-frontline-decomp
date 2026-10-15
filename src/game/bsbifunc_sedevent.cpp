// A fragment of bsbifunc.cpp (0x8002a3e4): BIFunc_SEDEvent only pops its arguments from the script stack (the
// built-in record view follows bsbifunc_grenade.cpp). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_SEDEvent(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
