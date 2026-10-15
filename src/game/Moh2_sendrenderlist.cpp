// A fragment of Moh2.cpp (0x8001a108): SendRenderList kicks the global render
// list, timing it in profile bin 19 (the tick count subtracted before and
// added after, then the bin's call count incremented). WaitScreen flips the
// screen and waits for it, timed in bins 20 and 22, recording the flip wait in
// ticks and the frame delta returned by the wait. WaitRenderList draws the
// render list (timed in bin 18, keeping the previous draw time) and waits for
// it (bins 20 and 21). The functions, classes and globals are named by the
// symbols (g_flipWait, g_frameDel, g_listDraw, g_prevListDraw and g_listWait
// are file-local in the original and declared extern here); the profile-bin
// layout (as in debug_prof_profilebin_ctor.cpp), the render-list and screen
// views (the screen's size is its symbol's) are inferred, and the bin indices
// are not named. The rest of the file is not part of this unit.
extern "C" unsigned long OSGetTick();

struct SProfileBin {
    int m_id;
    int m_values[5];
};

class CRenderList {
public:
    void Kick();
    void Draw();
    void Wait();

    unsigned char unknown00[36];
};

class CScreen {
public:
    void Flip();
    int Wait();

    unsigned char unknown00[272];
};

extern SProfileBin g_ProfileBins[31];
extern CRenderList theRenderList;
extern CScreen g_screen;
extern int g_flipWait;
extern int g_listDraw;
extern int g_prevListDraw;
extern int g_listWait;
extern float g_frameDel;

void SendRenderList() {
    g_ProfileBins[19].m_values[1] -= OSGetTick();
    theRenderList.Kick();
    g_ProfileBins[19].m_values[1] += OSGetTick();
    g_ProfileBins[19].m_values[4]++;
}

void WaitScreen() {
    g_ProfileBins[20].m_values[1] -= OSGetTick();
    g_ProfileBins[22].m_values[1] -= OSGetTick();
    g_flipWait = 0;
    g_flipWait -= OSGetTick();
    g_screen.Flip();
    g_frameDel = g_screen.Wait();
    g_flipWait += OSGetTick();
    g_ProfileBins[20].m_values[1] += OSGetTick();
    g_ProfileBins[20].m_values[4]++;
    g_ProfileBins[22].m_values[1] += OSGetTick();
    g_ProfileBins[22].m_values[4]++;
}

void WaitRenderList() {
    g_ProfileBins[18].m_values[1] -= OSGetTick();
    g_prevListDraw = g_listDraw;
    g_listDraw = 0;
    g_listDraw -= OSGetTick();
    theRenderList.Draw();
    g_listDraw += OSGetTick();
    g_ProfileBins[18].m_values[1] += OSGetTick();
    g_ProfileBins[18].m_values[4]++;
    g_ProfileBins[20].m_values[1] -= OSGetTick();
    g_ProfileBins[21].m_values[1] -= OSGetTick();
    g_listWait = 0;
    g_listWait -= OSGetTick();
    theRenderList.Wait();
    g_listWait += OSGetTick();
    g_ProfileBins[20].m_values[1] += OSGetTick();
    g_ProfileBins[20].m_values[4]++;
    g_ProfileBins[21].m_values[1] += OSGetTick();
    g_ProfileBins[21].m_values[4]++;
}
