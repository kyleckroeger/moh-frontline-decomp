// The start of buddy.cpp (0x80056728): the built-in script function
// SetBuddyPathGroup, which reports its two arguments through DebugMsg and
// stores the second (the path base) at the first's index of
// g_buddy_path_base's storage. The globals are named by the symbols; the
// built-in record view (as in alarm_builtins.cpp) and the path-base table's
// layout (its storage pointer at +8 of 16 bytes) are inferred. The rest of the
// file is not part of this unit.
struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short firstArgument;
    unsigned char unknown0e[2];
};

struct BuddyPathBaseView {
    unsigned char unknown00[8];
    int* m_data;
    unsigned char unknown0c[4];
};

void DebugMsg(const char*, ...);

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
extern BuddyPathBaseView g_buddy_path_base;

void BIFunc_SetBuddyPathGroup(int** stack, void*) {
    int* args = *stack;
    short first = g_pBuiltInFunctions[g_iCurrentBIFIndex].firstArgument;
    int index = *(args - (first - 1));
    int base = *(args - (first - 2));
    DebugMsg("BUDDY_DEBUG: SetBuddyPathGroup Index=%d PathBase=%d\n", index, base);
    g_buddy_path_base.m_data[index] = base;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
