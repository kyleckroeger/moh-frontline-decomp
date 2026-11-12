// A fragment of debug_prof.cpp (0x800f394c), which is compiled with deferred
// inlining and RTTI, so its functions are in reverse source order:
// DebugProfTimerVerifyNew (reports a slot already in use, a missing name and a
// bad or circular parent; the strlen and strcmp results were probably used by
// asserts removed from this build), DebugProfTimerRegister (checks a new
// timer, then names the 52-byte record, stores its parent and clears its four
// counters), DebugProfTimerInitAll (resets all 16 records through an inferred
// inline helper), DebugProfTimerDump (a timer's line: indented by its depth,
// padded to column 40, three counters), DebugProfTimerDumpRecurse (a timer,
// then every named child one level deeper; the compiler inlines both several
// levels deep) and DebugProfTimerDumpAll (banner, the whole tree, banner).
//
// The messages are reached from the file's .rodata base register, so the
// block's head is compiled here in the original order: the statistics
// aggregate's template (DebugStats_Draw, parsed first) and the RTTI names
// that CProfHistRenderBin's destructor (the first function of the file)
// creates with the class's virtual table. That destructor, Render (the class's
// key function) and DebugStats_Draw are file duplicates placed by their own
// units, and the block is pooled. The functions, globals and classes are named
// by the symbols and RTTI; the record view, the bins' layout before the table
// pointer, IsUsed's result and the helper are inferred, and CScreen's virtuals
// are declared in table order up to GetWidth (as in Screen_fieldmode.cpp).
// DebugProfTimerDump's statement count sets how deep the inliner expands the
// recursion in DebugProfTimerDumpRecurse; naming the printed values gives the
// original's depth.
extern "C" {
int printf(const char*, ...);
unsigned long strlen(const char*);
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);
}
void DebugMsg(const char*, ...);

/* inferred: a profile timer record (52 bytes) */
struct DebugProfTimerView {
    int value;
    char name[32];
    unsigned int counts[4];
};

extern DebugProfTimerView g_pDebugProfTimers[16];

void DebugProfTimerDumpRecurse(int, int);

class CColor;
class CRect;

class CScreen {
public:
    virtual ~CScreen();
    virtual void SetCurrent();
    virtual void Flip();
    virtual void Wait();
    virtual void SetClear(bool);
    virtual void SetClearColor(const CColor&);
    virtual void SetClearRect(const CRect&);
    virtual int GetHeight();
    virtual int GetWidth();
};

class CDmaTag;
class CDmaPacket;

class CRenderBinData {
    unsigned char unknown00[28];
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CDbgStatsRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
};

class CProfHistRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CProfHistRenderBin();
};

extern CScreen* g_pScreen;

int CProfHistRenderBin::Render(CDmaPacket&, void*) {
    g_pScreen->GetWidth();
    g_pScreen->GetHeight();
    return 0;
}

class CRenderList {
public:
    void Register(CRenderBin&);
    void Render(CRenderBin&, void*);
};

struct DebugStat {
    int data00;
    int data04;
    int data08;
    int data0c;
    int data10;
};

extern DebugStat debugStats[60];
extern int currentIndex;
extern bool g_bDebugStatsDisabled;
extern bool g_bDebugStatsDrawProfData;
extern int g_ProfileBinSelect;
extern int g_ProfileBinGraph;
extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern int g_screenWidth;
extern int g_screenHeight;

extern CDbgStatsRenderBin g_DbgStatsRenderBin;
extern CProfHistRenderBin g_ProfHistRenderBin;

void DebugStats_Draw(int a, int b, int c, int d, int e) {
    DebugStat stat = {a, b, c, d, e};
    currentIndex = (currentIndex + 1) % 60;
    debugStats[currentIndex] = stat;
    if (!g_bDebugStatsDisabled) {
        if (g_bDebugStatsDrawProfData) {
            g_ProfileBinGraph = g_ProfileBinSelect;
            if (g_ProfileBinSelect != -1)
                g_pRenderList->Render(g_ProfHistRenderBin, 0);
        } else {
            g_pRenderList->Render(g_DbgStatsRenderBin, 0);
        }
    }
}


void DebugProfTimerDumpAll() {
    printf("------------------------------\n");
    printf("--   Begin Profile Dump     --\n");
    printf("------------------------------\n");
    printf("\n");
    printf("Rank, Name, Time, Count\n");
    DebugProfTimerDumpRecurse(-1, -1);
    printf("\n");
    printf("------------------------------\n");
    printf("--    End Profile Dump      --\n");
    printf("------------------------------\n");
}

void DebugProfTimerDump(int index, int depth);

void DebugProfTimerDumpRecurse(int parent, int depth) {
    if (parent != -1)
        DebugProfTimerDump(parent, depth);
    for (int i = 0; i < 16; i++) {
        if (g_pDebugProfTimers[i].value == parent && g_pDebugProfTimers[i].name[0])
            DebugProfTimerDumpRecurse(i, depth + 1);
    }
}

void DebugProfTimerDump(int index, int depth) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    for (int i = 0; i < depth; i++)
        printf(" ");
    printf("%2d, %s", depth, timer->name);
    int column = depth + 4 + strlen(timer->name);
    for (; column < 40; column++)
        printf(" ");
    unsigned int count = timer->counts[0];
    unsigned int timeLow = timer->counts[1];
    unsigned int timeHigh = timer->counts[2];
    printf(", %u%09u, %u", timeHigh, timeLow, count);
    printf("\n");
}

/* inferred: resets one timer record */
static inline void ResetTimer(DebugProfTimerView* timer) {
    int* counts = (int*)timer->counts;
    timer->value = -1;
    timer->name[0] = 0;
    counts[0] = 0;
    counts[1] = 0;
    counts[2] = 0;
    counts[3] = 0;
}

void DebugProfTimerInitAll() {
    for (int i = 0; i < 16; i++)
        ResetTimer(&g_pDebugProfTimers[i]);
}

void DebugProfTimerVerifyNew(int, char*, int);

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

void DebugProfTimerVerifyNew(int slot, char* name, int parent) {
    if (g_pDebugProfTimers[slot].name[0]) {
        int i;
        for (i = 0; i < 16; i++) {
            if (g_pDebugProfTimers[i].name[0])
                break;
        }
        if (i == 16)
            DebugMsg("Debug Prof Timer: All slots used.  Increase DEBUG_PROF_TIMER_MAX.\n");
        else
            DebugMsg("Debug Prof Timer: Slot %d is already used.  The first available slot is %d.\n", slot, i);
    }
    if (!name || !name[0])
        DebugMsg("Debug Prof Timer: Slot name is NULL for slot %d.\n", slot);
    strlen(name);
    for (int j = 0; j < 16; j++)
        strcmp(name, g_pDebugProfTimers[j].name);
    for (int p = parent; p != -1; p = g_pDebugProfTimers[p].value) {
        if (p == slot)
            DebugMsg("Debug Prof Timer: Timer %d (%s) is trying to use %d as a parent, but that leads to a circular dependency.\n", slot, name, parent);
    }
    if (parent != -1 && (parent < 0 || parent >= 16))
        DebugMsg("Debug Prof Timer: Timer %d (%s) is trying to use %d as a parent, but %d is not a valid timer.\n", slot, name, parent, parent);
    if (parent != -1 && !g_pDebugProfTimers[parent].name[0])
        DebugMsg("Debug Prof Timer: Timer %d (%s) is trying to use %d as a parent, but %d is not initialized.\n", slot, name, parent, parent);
}

__declspec(weak) CProfHistRenderBin::~CProfHistRenderBin() {
}
