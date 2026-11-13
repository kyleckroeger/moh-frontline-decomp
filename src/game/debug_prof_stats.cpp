// A fragment of debug_prof.cpp (0x800f429c): DebugStats_Draw records five
// values in the next entry of the 60-entry statistics ring (an aggregate
// built from the arguments, then copied in) and, unless the statistics are
// disabled, renders the statistics bin, or, when profile data is shown, the
// selected profile histogram (none for -1); DebugStats_Init keeps the screen,
// its width and height and the render list, clears four of the five words of
// every entry and registers both bins. The file name is this project's; the
// original record is debug_prof.cpp, between debug_prof.cpp and
// debug_prof_bins.cpp. The functions, globals and classes are named by the
// symbols; the entry layout, the bin views and the result types are inferred,
// and CScreen's virtuals are declared in table order up to GetWidth. The
// file-local globals are declared non-static so the fragment links to the
// original objects. The aggregate's template is an item of the file's .rodata
// pool.
class CColor;
class CRect;

/* CScreen's first virtuals in table order (as in Screen_fieldmode.cpp). */
#include "game/CScreen.h"


/* 32-byte bins (CRenderBinData and the table pointer); non-virtual views */
class CRenderBin {
    unsigned char data[32];
};

class CDbgStatsRenderBin : public CRenderBin {};
class CProfHistRenderBin : public CRenderBin {};

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
