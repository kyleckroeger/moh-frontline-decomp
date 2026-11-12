// debug_prof.cpp: profile bin table and timer checks, as partial source. The
// timer checks keep the original's discarded strlen and strcmp calls, whose
// results were probably used by asserts removed from this build.
extern "C" int printf(const char*, ...);
extern "C" unsigned long strlen(const char*);
extern "C" int strcmp(const char*, const char*);
void DebugMsg(const char*, ...);

struct DebugProfTimer {
    int m_parent;
    char m_name[48];
};
extern DebugProfTimer g_pDebugProfTimers[16];

struct CProfileBin {
    void Set(const char* name, int parent) {
        m_name = name;
        m_parent = parent;
        m_field10 = 0;
        m_field0c = 0;
        m_field08 = 0;
    }
    int m_parent;
    const char* m_name;
    unsigned int m_field08;
    unsigned int m_field0c;
    unsigned int m_field10;
    unsigned int m_count;
};
extern CProfileBin g_ProfileBins[32];
class CPerformanceCount {
public:
    static int m_ModeCounter0;
    static int m_ModeCounter1;
};
class CProfileInitClass {
public:
    CProfileInitClass();
};

CProfileInitClass::CProfileInitClass() {
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
