// A fragment of Moh2.cpp (0x80019a4c): DrawStatusScreen draws the
// background sprite over the whole screen (when there is one) and the
// progress bar sprite (163, 396 and 312 by 16 in screen units, scaled by the
// screen's factors at +40 and +44, its right edge and texture coordinate at
// the given fraction), then waits for and draws the render list
// (WaitRenderList), flips the screen (WaitScreen), kicks the next list
// (SendRenderList), updates the input manager and runs the DVD error task.
// The three render steps follow Moh2_sendrenderlist.cpp (later in the
// original file; inline here so that they are inlined without being
// emitted). The function is file-local in the original. The vertex view
// (texture coordinates, position, depth and a colour word, 24 bytes) and
// the screen's factors are inferred; _spritepolyvert, CSprite, CScreen and
// CInputManager are named by the symbols. The rest of the file is in other
// units.
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
    int GetWidth();
    int GetHeight();

    unsigned char unknown00[40];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown30[224];
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


struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

class CInputManager {
public:
    void Update(float);

    unsigned char unknown0000[9296];
};

extern CInputManager g_inputMgr;

typedef CSprite::EPolyPrimType PolyPrimType;

void MohDVDErrorTask(int, int);

static void DrawStatusScreen(CSprite* background, CSprite* bar, float progress) {
    float width = g_screen.GetWidth();
    float height = g_screen.GetHeight();
    if (background) {
        _spritepolyvert screen[4] = {
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
            {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
            {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
            {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
        };
        screen[1].x = width;
        screen[2].x = width;
        screen[2].y = height;
        screen[3].y = height;
        background->DrawPoly(screen, 4, (PolyPrimType)5);
    }
    _spritepolyvert quad[4] = {
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
        {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
        {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080},
    };
    float scaleX = g_screen.m_scaleX;
    float left = 163.0f * scaleX;
    float top;
    float scaleY = g_screen.m_scaleY;
    top = 396.0f * scaleY;
    float right = progress * (312.0f * scaleX) + 163.0f * scaleX;
    float bottom = 16.0f * scaleY + top;
    quad[0].x = left;
    quad[0].y = top;
    quad[1].u = progress;
    quad[1].x = right;
    quad[1].y = top;
    quad[2].u = progress;
    quad[2].x = right;
    quad[2].y = bottom;
    quad[3].x = left;
    quad[3].y = bottom;
    bar->DrawPoly(quad, 4, (PolyPrimType)5);
    WaitRenderList();
    WaitScreen();
    SendRenderList();
    g_inputMgr.Update(1.0f);
    MohDVDErrorTask(0, 0);
}
