// A fragment of debug_prof.cpp (0x800f3b74): DebugProfTimerRegister checks a
// new timer, then names the timer record (52 bytes in g_pDebugProfTimers),
// stores its value and clears its four counters. The functions and the
// table are named by the symbols; the record view is inferred. The rest of
// the file is in other units.
extern "C" char* strcpy(char*, const char*);

void DebugProfTimerVerifyNew(int, char*, int);

/* inferred: a profile timer record (52 bytes) */
struct DebugProfTimerView {
    int value;
    char name[32];
    int counts[4];
};

extern DebugProfTimerView g_pDebugProfTimers[];

void DebugProfTimerRegister(int index, char* name, int value) {
    DebugProfTimerVerifyNew(index, name, value);
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    strcpy(timer->name, name);
    timer->value = value;
    timer->counts[0] = 0;
    timer->counts[1] = 0;
    timer->counts[2] = 0;
    timer->counts[3] = 0;
}
