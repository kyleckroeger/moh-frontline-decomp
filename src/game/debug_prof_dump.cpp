// A fragment of debug_prof.cpp (0x800f3cd0): DebugProfTimerDump prints a
// profile timer's line (indented by its depth, the depth and name, padded to
// column 40, then three of its counters). The function and the table are
// named by the symbols; the record view follows debug_prof_register.cpp. The
// rest of the file is in other units.
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
    int column = depth + strlen(timer->name);
    for (column += 4; column < 40; column++)
        printf(" ");
    printf(", %u%09u, %u", timer->counts[2], timer->counts[1], timer->counts[0]);
    printf("\n");
}
