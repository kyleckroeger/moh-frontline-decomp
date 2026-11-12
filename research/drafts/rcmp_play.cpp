// A fragment of rcmp_main.cpp (0x8010370c, the first function of the file):
// OPTION_PARSING_PLAYER::Play shows a movie frame by frame until it ends or a
// skip is requested, keeping frame-rate statistics that it prints at the end.
// The software path creates a GCSW display on the front buffer (clearing it
// once, through the guarded function static FirstTime); the hardware path a
// MOHFL display, centred by the player's scale. Frame timing reads the time
// base through inline asm (times 12, as the original). The classes and
// functions are named by the mangled symbols; the members, the display's
// virtual slots (destructor 0x08, SetPosition 0x0c, Display 0x14, Redisplay
// 0x18, Advance 0x1c; 0x10 is unused here), the frame-time record on the stack
// (index, history[16], ring[16]: its layout is fixed by the original frame) and
// the local names are inferred. The member at +0x26 is the flag ReadInput
// (rcmp_main_delete.cpp) reads; +0x24 its skip request. The strings and
// constants are items of the file's .rodata and .sdata2 pools.
// Code-exact in isolation (2026-11-12). Landing needs the file's ...rodata.0
// block from its start: the strings sit at +32..+120 behind "Reseting." and
// "Software Reseting.", which no remaining code uses (stripped functions), so
// it waits for the gap-function decision.
extern "C" {
void DCStoreRange(void*, unsigned long);
void VISetBlack(int);
void VIFlush();
void GXDrawDone();
void MEM_fill(void*, int, unsigned long);
void MEM_copy(void*, const void*, unsigned long);
unsigned int MEM_totalunused(int);
int TIMER_gettick();
void SYNCTASK_run(int);
void THREAD_yield(int);
}
extern int TIMERhz;
void PRINT_string(const char*, ...);
#define OS_BUS_CLOCK (*(unsigned int*)0x800000fc)

struct _GXRenderModeObj {
    unsigned char unknown00[4];
    unsigned short m_width;
    unsigned char unknown06[2];
    unsigned short m_height;
};
class CScreen {
public:
    int GetWidth();
    int GetHeight();
    void Wait();
    void Flip();
    unsigned char unknown00[0xb0];
    _GXRenderModeObj* m_mode;   // +0xb0
    unsigned char unknownB4[0x44];
    void* m_frontBuffer;        // +0xf8
};
extern CScreen g_screen;

namespace RCMP {
class DECODER;
struct FrameInfo {
    unsigned char unknown00[4];
    short m_width;
    short m_height;
};
class FRAME {
public:
    unsigned char unknown00[4];
    FrameInfo* m_info;
};
class AV_PLAYER {
public:
    FRAME* GetFirstFrame(unsigned int, int);
    void Pause();
    void UnPause();
    bool IsTimeForDecode();
    FRAME* GetFrame(float);
    bool IsAudioFinished();
    unsigned char unknown00[0x3c];
    unsigned int m_frameNumber;
    float m_time;
    unsigned char unknown44[0x18];
    DECODER* m_decoder;
};
class VIDEO_DISPLAY {
public:
    virtual ~VIDEO_DISPLAY();
    virtual void SetPosition(float, float, float);
    virtual void Unused10();
    virtual void Display(FRAME*);
    virtual void Redisplay();
    virtual void Advance();
};
VIDEO_DISPLAY* MOHFL_VD_create(DECODER*, FRAME*);
VIDEO_DISPLAY* GCSW_VD_create(DECODER*, FRAME*, _GXRenderModeObj*, void*);
}

// Frame-time record (DISP_OLD_FRAMES: name and empty inline destructor from the original's
// extab record and weak dtor): the ring fills, then is copied whole into the history (layout inferred).
struct DISP_OLD_FRAMES {
    int m_index;
    float m_history[16];
    float m_ring[16];
    DISP_OLD_FRAMES() {
        m_index = 0;
        for (int i = 0; i < 16; i++) {
            m_history[i] = 0.0f;
            m_ring[i] = 0.0f;
        }
    }
    void Add(float t) {
        m_ring[m_index] = t;
        if (++m_index >= 16) {
            m_index = 0;
            MEM_copy(m_history, m_ring, sizeof(m_ring));
        }
    }
    ~DISP_OLD_FRAMES() {}
};

static inline unsigned int Ticks() {
    register unsigned int t;
    asm { mftb t }
    return t * 12;
}

class OPTION_PARSING_PLAYER {
public:
    void Play(bool);
    void ReadInput();
    unsigned char unknown00[4];
    float m_scaleX;
    float m_scaleY;
    float m_x;
    float m_y;
    float m_height;
    float m_width;
    float m_screenHeight;
    float m_screenWidth;
    bool m_stop;       // +0x24, skip request
    unsigned char unknown25;
    bool m_finished;   // +0x26, read by ReadInput
    unsigned char unknown27[5];
    bool m_singleStep; // +0x2c
    bool m_software;   // +0x2d
    unsigned char unknown2e[2];
    bool m_waitForDecode; // +0x30
    unsigned char unknown31[0xf];
    RCMP::AV_PLAYER* m_player; // +0x40
    RCMP::VIDEO_DISPLAY* m_display; // +0x44
};

void OPTION_PARSING_PLAYER::Play(bool finished) {
    bool audioDone;
    int minRate;
    long long totalTicks;
    int frames;
    bool hold;
    RCMP::FRAME* frame;
    unsigned int freeMemory;
    int startTick;
    int endTick;
    unsigned int start;
    int elapsed;
    int rate;
    float cpuTime;
    int flags;
    unsigned int format;
    DISP_OLD_FRAMES times;
    audioDone = false;
    totalTicks = 0;
    frames = 0;
    minRate = 99999999;
    hold = false;
    m_finished = finished;
    flags = 0x21;
    format = 2;
    if (m_software) {
        format = 1;
        flags = 0x10;
    }
    frame = m_player->GetFirstFrame(format, flags);
    m_player->Pause();
    if (frame) {
        if (m_software) {
            _GXRenderModeObj* mode = g_screen.m_mode;
            void* frontBuffer = g_screen.m_frontBuffer;
            m_display = RCMP::GCSW_VD_create(m_player->m_decoder, frame, mode, frontBuffer);
            static int FirstTime = 1;
            if (FirstTime) {
                MEM_fill(frontBuffer, 0, mode->m_height * mode->m_width * 2);
                DCStoreRange(frontBuffer, mode->m_height * mode->m_width * 2);
                VISetBlack(0);
                VIFlush();
                FirstTime = 0;
            }
        } else {
            m_display = RCMP::MOHFL_VD_create(m_player->m_decoder, frame);
        }
        RCMP::FrameInfo* info = frame->m_info;
        m_width = info->m_width;
        m_height = info->m_height;
        m_screenWidth = g_screen.GetWidth();
        m_screenHeight = g_screen.GetHeight();
        m_x = 0.5f * -(m_width * m_scaleX - m_screenWidth);
        m_y = 0.5f * -(m_height * m_scaleY - m_screenHeight);
        m_display->SetPosition(m_x, m_y, 0.0f);
    }
    freeMemory = MEM_totalunused(0);
    m_player->UnPause();
    startTick = TIMER_gettick();

    for (;;) {
        start = Ticks();
        if (!m_software)
            g_screen.Wait();
        if (hold)
            m_display->Redisplay();
        else
            m_display->Display(frame);
        if (!m_software) {
            GXDrawDone();
            g_screen.Flip();
        }
        SYNCTASK_run(0);
        THREAD_yield(0);

        if (!frame) {
            ReadInput();
        } else if (m_singleStep) {
            m_display->Advance();
            frame = m_player->GetFrame(1.0f + (float)m_player->m_frameNumber);
            hold = false;
            ReadInput();
        } else if (m_waitForDecode) {
            while (!m_player->IsTimeForDecode() && !m_stop) {
                ReadInput();
                SYNCTASK_run(0);
                THREAD_yield(0);
            }
            m_display->Advance();
            frame = m_player->GetFrame(m_player->m_time);
            hold = false;
        } else {
            if (m_player->IsTimeForDecode()) {
                m_display->Advance();
                frame = m_player->GetFrame(m_player->m_time);
                hold = false;
            } else {
                hold = true;
            }
            ReadInput();
        }
        if (!frame) {
            hold = true;
            audioDone = m_player->IsAudioFinished();
        }
        if (audioDone || m_stop)
            break;

        elapsed = Ticks() - start;
        frames++;
        totalTicks += elapsed;
        rate = (int)((float)OS_BUS_CLOCK / (float)elapsed);
        if (minRate > rate)
            minRate = rate;
        times.Add((float)elapsed / (0.001f * (float)OS_BUS_CLOCK));
    }

    endTick = TIMER_gettick();
    cpuTime = (float)totalTicks / (float)OS_BUS_CLOCK;
    PRINT_string("Frames %d\n", frames);
    PRINT_string("Min FrameRate %d\n", minRate);
    PRINT_string("Ave FrameRate %.2f\n", (float)OS_BUS_CLOCK / (float)(totalTicks / frames));
    PRINT_string("CPU Time %.2f\n", cpuTime);
    PRINT_string("TIMER Time %.2f\n", (float)(endTick - startTick) / (float)TIMERhz);
    PRINT_string("Memory Available Durring Moive %.2fk %dBytes\n", (float)(int)freeMemory / 1024.0f, freeMemory);
    m_finished = true;
    if (m_display) {
        delete m_display;
        m_display = 0;
    }
}
