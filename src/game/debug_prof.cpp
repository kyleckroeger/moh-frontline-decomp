// debug_prof.cpp from DebugProfTimerVerifyNew (0x800f394c) to the end of the file: the
// profiling timers, the debug statistics and their two render bins, the static
// initialisation and CProfileInitClass's weak constructor (the file's earlier functions are
// in debug_prof_cprofhistrenderbin_dtor.cpp and debug_prof_profilebin_ctor.cpp). The file
// is compiled with deferred inlining and RTTI, so the functions are written last to first
// and come out in reverse order; this unit owns the file's .rodata (the statistics
// template, the RTTI names, the timer messages and banners, the PS2 performance counters'
// event names, the profile bin names), its two virtual tables with their RTTI records,
// .bss and .sbss, while .sdata2 is pooled with PROFILE_UPDATE. CProfHistRenderBin's
// destructor (the file's first function, which creates that class's table and RTTI names
// first) is a file duplicate and CRenderBin's inline destructor a weak duplicate.
//
// The timers (DebugProfTimerStart/Pause/Resume/Stop) accumulate OS ticks into a 52-byte
// record and count starts and running timers, carrying billions of ticks into a second
// counter; DebugProfTimerVerifyNew reports a slot already in use, a missing name and a bad
// or circular parent (the strlen and strcmp results were probably used by asserts removed
// from this build); DebugProfTimerRegister names a record, stores its parent and clears its
// counters; DebugProfTimerDump/DumpRecurse/DumpAll print the timer tree (DumpRecurse's
// expansion depth follows Dump's statement count). DebugStats_Draw records five values in
// the 60-entry statistics ring and renders the statistics bin, or the selected profile
// histogram; DebugStats_Init keeps the screen, its size and the render list and registers
// both bins. __sinit builds the 31 profile and history bins, the cursors and the bins
// (priority 9) and runs CProfileInitClass's constructor, which names the profile bins and
// their parents and sets both performance counter modes to 6; its zeroing chain
// 'a = b = c = 0' gives the original's store order. The functions, globals and classes are
// named by the symbols and RTTI; the record and entry layouts, member names, the 9 priority
// and IsUsed's result are inferred, and CScreen's virtuals come from game/CScreen.h.
extern "C" {
int printf(const char*, ...);
unsigned long strlen(const char*);
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);
unsigned long OSGetTick(void);
}
void DebugMsg(const char*, ...);

class CColor;
class CRect;
#include "game/CScreen.h"

class CDmaTag;
class CDmaPacket;

class CRenderBinData {
public:
    CRenderBinData() : m_prev(0), m_next(0), m_children(0), m_lastTag(0), m_firstTag(0), m_priority(3) {}

    int m_prev;
    int m_next;
    int m_children;
    int m_lastTag;
    int m_firstTag;
    int m_priority;
    int data18;
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
    CDbgStatsRenderBin() { m_priority = 9; }
    virtual ~CDbgStatsRenderBin();
};

class CProfHistRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CProfHistRenderBin() { m_priority = 9; }
    virtual ~CProfHistRenderBin();
};

struct SProfileBin {
    SProfileBin();
    void Set(const char* name, int parent) {
        m_name = name;
        m_parent = parent;
        m_field08 = m_field0c = m_field10 = 0;
    }

    int m_parent;
    const char* m_name;
    unsigned int m_field08;
    unsigned int m_field0c;
    unsigned int m_field10;
    unsigned int m_count;
};

struct SProfHistBin {
    SProfHistBin();

    unsigned char unknown00[7212];
};

class CPerformanceCount {
public:
    static int m_ModeCounter0;
    static int m_ModeCounter1;
};

extern SProfileBin g_ProfileBins[31];

class CProfileInitClass {
public:
    CProfileInitClass() {
        g_ProfileBins[0].Set("Main", -1);
        g_ProfileBins[1].Set("Update", 0);
        g_ProfileBins[2].Set("Scene", 1);
        g_ProfileBins[3].Set("Begin", 2);
        g_ProfileBins[4].Set("Constrain", 2);
        g_ProfileBins[5].Set("AI", 2);
        g_ProfileBins[6].Set("Update", 5);
        g_ProfileBins[7].Set("Commit", 5);
        g_ProfileBins[8].Set("Collision", 2);
        g_ProfileBins[9].Set("Slow", 8);
        g_ProfileBins[10].Set("Fast", 8);
        g_ProfileBins[11].Set("Script", 1);
        g_ProfileBins[12].Set("Draw", 0);
        g_ProfileBins[13].Set("cpt", 12);
        g_ProfileBins[14].Set("cull", 13);
        g_ProfileBins[15].Set("pkt", 13);
        g_ProfileBins[16].Set("msh", 12);
        g_ProfileBins[17].Set("anim", 12);
        g_ProfileBins[20].Set("Idle", 0);
        g_ProfileBins[21].Set("GSync", 20);
        g_ProfileBins[22].Set("VSync", 20);
        g_ProfileBins[18].Set("DrawFinish", 0);
        g_ProfileBins[19].Set("DrawKick", 0);
        g_ProfileBins[23].Set("Collision", -1);
        g_ProfileBins[24].Set("Line", 23);
        g_ProfileBins[25].Set("Leaf", 24);
        g_ProfileBins[26].Set("Group", 25);
        g_ProfileBins[27].Set("Tri", 26);
        g_ProfileBins[28].Set("Fiber", 23);
        g_ProfileBins[29].Set("Sphere", 23);
        g_ProfileBins[30].Set("Capsule", 23);

        CPerformanceCount::m_ModeCounter0 = 6;
        CPerformanceCount::m_ModeCounter1 = 6;
    }
};

class CRenderList {
public:
    void Register(CRenderBin&);
    void Render(CRenderBin&, void*);
};

/* inferred: a profile timer record (52 bytes): parent, name, then the start count, the
   ticks, the billions of ticks and the running count */
struct DebugProfTimerView {
    int value;
    char name[32];
    unsigned int counts[4];
};

struct DebugStat {
    int data00;
    int data04;
    int data08;
    int data0c;
    int data10;
};

extern bool g_bDebugStatsDisabled;
extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;

static DebugStat debugStats[60];
SProfileBin g_ProfileBins[31];
static SProfHistBin g_ProfileBinsHistory[31];
static int g_screenWidth;
static int g_screenHeight;
static int currentIndex;
bool g_bDebugStatsDrawProfData;
int g_HistoryCursor;
int g_GraphCursor;
float g_fGraphCursor = g_GraphCursor;
int g_ProfileBinTOP;
int g_ProfileBinSelect = g_ProfileBinTOP;
int g_ProfileBinGraph = g_ProfileBinTOP;
DebugProfTimerView g_pDebugProfTimers[16];
static CDbgStatsRenderBin g_DbgStatsRenderBin;
static CProfHistRenderBin g_ProfHistRenderBin;
static CProfileInitClass g_ProfileDataInit;

void DebugProfTimerDumpRecurse(int, int);

__declspec(weak) CDbgStatsRenderBin::~CDbgStatsRenderBin() {
}

int CDbgStatsRenderBin::Render(CDmaPacket&, void*) {
    g_pScreen->GetWidth();
    g_pScreen->GetHeight();
    return 0;
}

int CProfHistRenderBin::Render(CDmaPacket&, void*) {
    g_pScreen->GetWidth();
    g_pScreen->GetHeight();
    return 0;
}

void DebugStats_Init(CScreen* screen, CRenderList* list) {
    g_pScreen = screen;
    g_screenWidth = screen->GetWidth();
    g_screenHeight = g_pScreen->GetHeight();
    g_pRenderList = list;
    for (int i = 0; i < 60; i++) {
        debugStats[i].data00 = 0;
        debugStats[i].data04 = 0;
        debugStats[i].data0c = 0;
        debugStats[i].data10 = 0;
    }
    list->Register(g_DbgStatsRenderBin);
    g_pRenderList->Register(g_ProfHistRenderBin);
}

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

void DebugProfTimerStart(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->counts[0]++;
    timer->counts[3]++;
    timer->counts[1] -= OSGetTick();
}

void DebugProfTimerPause(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->counts[3]--;
    timer->counts[1] += OSGetTick();
}

void DebugProfTimerResume(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->counts[3]++;
    timer->counts[1] -= OSGetTick();
}

void DebugProfTimerStop(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->counts[3]--;
    timer->counts[1] += OSGetTick();
    if (timer->counts[1] >= 1000000000) {
        timer->counts[2]++;
        timer->counts[1] -= 1000000000;
    }
}

/* inferred: gap function (accepted form, docs/Remaining.md 2026-11-13), stripped at link
   as the original's was. Nothing in the image references the PS2 performance counters'
   event names that the file's .rodata keeps between DebugProfTimerDumpAll's banners and
   CDbgStatsRenderBin's RTTI name, so the function that used them was removed by the
   linker. They are the EE's counter 1 and counter 0 event lists in event order (counter 1
   first in .rodata; counter 0's event 5 reuses "TLB miss"). This lookup of a counter's
   event field in the counter control register (EVENT1 at bit 15, EVENT0 at bit 5) is
   this project's; switching on the field in place keeps the cases sparse, so no jump
   table is compiled. */
const char* DebugProfCounterEventName(int counter, unsigned int control) {
    if (counter == 1) {
        switch (control & (31 << 15)) {
        case 0 << 15:
            return "Low-order branch issued";
        case 1 << 15:
            return "Processor cycle";
        case 2 << 15:
            return "Dual instructions issue";
        case 3 << 15:
            return "Branch miss-predicted";
        case 4 << 15:
            return "TLB miss";
        case 5 << 15:
            return "DTLB miss";
        case 6 << 15:
            return "Data cache miss";
        case 7 << 15:
            return "WBB single request unavailable";
        case 8 << 15:
            return "WBB burst request unavailable";
        case 9 << 15:
            return "WBB burst request almost full";
        case 10 << 15:
            return "WBB burst request full";
        case 11 << 15:
            return "CPU data bus busy";
        case 12 << 15:
            return "Instruction completed";
        case 13 << 15:
            return "Non-BDS instruction completed";
        case 14 << 15:
            return "COP1 instruction completed";
        case 15 << 15:
            return "Store completed";
        case 16 << 15:
            return "No event";
        }
    } else {
        switch (control & (31 << 5)) {
        case 0 << 5:
            return "Reserved";
        case 1 << 5:
            return "Processor cycle";
        case 2 << 5:
            return "Single instructions issue";
        case 3 << 5:
            return "Branch issued";
        case 4 << 5:
            return "BTAC miss";
        case 5 << 5:
            return "TLB miss";
        case 6 << 5:
            return "Instruction cache miss";
        case 7 << 5:
            return "DTLB accessed";
        case 8 << 5:
            return "Non-blocking load";
        case 9 << 5:
            return "WBB single request";
        case 10 << 5:
            return "WBB burst request";
        case 11 << 5:
            return "CPU address bus busy";
        case 12 << 5:
            return "Instruction completed";
        case 13 << 5:
            return "Non-BDS instruction completed";
        case 14 << 5:
            return "COP2 instruction completed";
        case 15 << 5:
            return "Load completed";
        case 16 << 5:
            return "No event";
        }
    }
    return 0;
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
