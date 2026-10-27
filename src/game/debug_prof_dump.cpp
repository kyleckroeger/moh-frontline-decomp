// A fragment of debug_prof.cpp (0x800f3cd0): DebugProfTimerDump prints a
// profile timer's line (indented by its depth, the depth and name, padded to
// column 40, then three of its counters); DebugProfTimerDumpRecurse prints a
// timer (none for -1) and then, one level deeper, every named timer whose
// parent it is (the compiler inlines both several levels deep). The
// functions and the table are named by the symbols; the record view follows
// debug_prof_register.cpp (the first word is the parent index). The
// '(void)0;' stands in for compiled-out debug code: without it the inliner
// expands the recursion one level deeper than the original. The rest of the
// file is in other units.
extern "C" {
int printf(const char*, ...);
unsigned long strlen(const char*);
}

/* inferred: a profile timer record (52 bytes) */
struct DebugProfTimerView {
    int value;
    char name[32];
    unsigned int counts[4];
};

extern DebugProfTimerView g_pDebugProfTimers[16];

void DebugProfTimerDump(int index, int depth) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    for (int i = 0; i < depth; i++)
        printf(" ");
    printf("%2d, %s", depth, timer->name);
    int column = depth + 4 + strlen(timer->name);
    (void)0; // stand-in for compiled-out debug code
    for (; column < 40; column++)
        printf(" ");
    printf(", %u%09u, %u", timer->counts[2], timer->counts[1], timer->counts[0]);
    printf("\n");
}

void DebugProfTimerDumpRecurse(int parent, int depth) {
    if (parent != -1)
        DebugProfTimerDump(parent, depth);
    for (int i = 0; i < 16; i++) {
        if (g_pDebugProfTimers[i].value == parent && g_pDebugProfTimers[i].name[0])
            DebugProfTimerDumpRecurse(i, depth + 1);
    }
}
