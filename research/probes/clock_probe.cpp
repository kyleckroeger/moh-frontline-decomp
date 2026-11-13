// settings: config/GMFE69/Screen.json
// replaces: Wait__7CScreenFv
// replaces: MathFunSRandom__FUi
//
// Test harness, not game code. Linked into a test image by
// `link_drafts.py --probe`, it takes CPU timing out of the game's state, so a
// drafted image and its control replay the same input script identically:
//
// - CScreen::Wait holds every frame to exactly FRAME_VSYNCS (3) vertical retraces
//   and returns that number instead of the number that really passed. The game
//   scales every update by that number, so with the real count a draft that
//   runs a little slower or faster moves the game's state ("timing drift",
//   11-09). Holding the frame also keeps updates in step with emulated time,
//   which paces the audio streams and movies that scripts wait on: without it,
//   a scripted event (the landing craft setting off) fell one update later in
//   an image whose code ran faster. A frame that needs longer than the budget
//   is logged as "SLIP frame vsyncs"; comparisons past a slip may drift.
// - MathFunSRandom ignores its argument (InitCORE passes OSGetTick(), which
//   depends on how long the code before it took) and seeds with SEED.
//
// It also prints a hash of every FRAME_EVERY-th finished frame from the
// external frame buffer in main memory: "FRAME n hash h". Dolphin writes the
// frame buffer to memory only with "Store XFB Copies to Texture Only" off
// (GFX.ini [Hacks] XFBToTextureEnable = False, set by dolphin_boot.py prepare).
//
// Body of Wait from src/game/Screen.cpp; only the frame count differs.
typedef unsigned short u16;
typedef unsigned long u32;
#include "game/CScreen.h"

extern "C" {
void GXSetViewport(float, float, float, float, float, float);
void GXSetViewportJitter(float, float, float, float, float, float, unsigned long);
unsigned long VIGetNextField();
void GXInvalidateVtxCache();
void GXSetColorUpdate(int);
void OSReport(const char*, ...);
void srand(unsigned int);
}

class CFader {
public:
    static void Update(float);
};

static volatile unsigned long g_FrameNum;
static unsigned long g_WaitFrameNum;
static volatile bool g_bSwap;
extern bool g_bMathFunRandomSeeded;

static const unsigned long FRAME_VSYNCS = 3;
static const unsigned int FRAME_EVERY = 30;
static const unsigned int SEED = 0x1234;

static unsigned int s_frame;

static unsigned int HashFrame(const void* buffer, int words) {
    const unsigned int* p = (const unsigned int*)buffer;
    unsigned int h = 2166136261u;
    for (int i = 0; i < words; i++)
        h = (h ^ p[i]) * 16777619u;
    return h;
}

unsigned long CScreen::Wait() {
    while (g_bSwap == 1) {
    }
    unsigned long passed = g_FrameNum - g_WaitFrameNum;
    if (passed > FRAME_VSYNCS && s_frame > 0)
        OSReport("SLIP %u %u\n", s_frame, passed);
    while (g_FrameNum - g_WaitFrameNum < FRAME_VSYNCS) {
    }
    g_WaitFrameNum = g_FrameNum;
    unsigned long frames = FRAME_VSYNCS;
    if (s_frame % FRAME_EVERY == 0 && m_copyBuffer) {
        // m_copyBuffer still holds the frame Flip just copied out (GXDrawDone waited for it).
        int words = m_mode->fbWidth * m_mode->xfbHeight / 2;
        OSReport("FRAME %u hash %08x\n", s_frame, HashFrame(m_copyBuffer, words));
    }
    s_frame++;
    CFader::Update(frames);
    if (m_copyBuffer == m_frameBuffer0)
        m_copyBuffer = m_frameBuffer1;
    else
        m_copyBuffer = m_frameBuffer0;
    if (m_mode->field_rendering)
        GXSetViewportJitter(0.0f, 0.0f, m_mode->fbWidth, m_mode->efbHeight, 0.0f, 1.0f, VIGetNextField());
    else
        GXSetViewport(0.0f, 0.0f, m_mode->fbWidth, m_mode->efbHeight, 0.0f, 1.0f);
    GXInvalidateVtxCache();
    if (!m_clear)
        GXSetColorUpdate(1);
    return frames;
}

void MathFunSRandom(unsigned int) {
    srand(SEED);
    g_bMathFunRandomSeeded = true;
}
