// A fragment of Moh2.cpp (0x80017b80): EndErrorText waits for the render
// list (WaitRenderList), flips and waits for the screen (WaitScreen) and
// kicks the next render list (SendRenderList), all three inlined. Their
// bodies follow Moh2_sendrenderlist.cpp (they are later in the original file,
// which inlines across the file; this unit marks them inline so that they are
// inlined without being emitted). The function is file-local in the original.
// The rest of the file is in other units.
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

inline void SendRenderList() {
    g_ProfileBins[19].m_values[1] -= OSGetTick();
    theRenderList.Kick();
    g_ProfileBins[19].m_values[1] += OSGetTick();
    g_ProfileBins[19].m_values[4]++;
}

inline void WaitScreen() {
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

inline void WaitRenderList() {
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

static void EndErrorText() {
    WaitRenderList();
    WaitScreen();
    SendRenderList();
}
