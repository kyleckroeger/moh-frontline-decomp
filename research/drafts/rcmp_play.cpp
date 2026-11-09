// rcmp_main.cpp: plays a movie to the end, or until asked to stop, as partial source.
// Keeps frame-rate statistics, printed when it finishes.
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
    virtual void Function08();
};
VIDEO_DISPLAY* MOHFL_VD_create(DECODER*, FRAME*);
VIDEO_DISPLAY* GCSW_VD_create(DECODER*, FRAME*, _GXRenderModeObj*, void*);
}

static inline void* Slot(void* object, int offset) { return *(void**)(*(char**)object + offset); }
typedef void (*DeleteFn)(void*, int);
typedef void (*PositionFn)(void*, float, float, float);
typedef void (*DisplayFn)(void*, RCMP::FRAME*);
typedef void (*NoArgFn)(void*);

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
    bool m_stop;       // +0x24
    unsigned char unknown25;
    bool m_finished;   // +0x26
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
    int ringIndex = 0;
    int minRate = 99999999;
    float history[16];
    float ring[16];
    for (int i = 0; i < 16; i++) {
        history[i] = 0.0f;
        ring[i] = 0.0f;
    }
    bool audioDone = false;
    long long totalTicks = 0;
    int frames = 0;
    bool hold = false;
    m_finished = finished;
    unsigned int format = 2;
    int flags = 0x21;
    if (m_software) {
        format = 1;
        flags = 0x10;
    }
    RCMP::FRAME* frame = m_player->GetFirstFrame(format, flags);
    m_player->Pause();
    _GXRenderModeObj* mode = g_screen.m_mode;
    void* frontBuffer = g_screen.m_frontBuffer;
    if (frame) {
        if (!m_software) {
            m_display = RCMP::MOHFL_VD_create(m_player->m_decoder, frame);
        } else {
            m_display = RCMP::GCSW_VD_create(m_player->m_decoder, frame, mode, frontBuffer);
            static int FirstTime = 1;
            if (FirstTime) {
                MEM_fill(frontBuffer, 0, mode->m_height * mode->m_width * 2);
                DCStoreRange(frontBuffer, mode->m_height * mode->m_width * 2);
                VISetBlack(0);
                VIFlush();
                FirstTime = 0;
            }
        }
        m_width = frame->m_info->m_width;
        m_height = frame->m_info->m_height;
        m_screenWidth = g_screen.GetWidth();
        m_screenHeight = g_screen.GetHeight();
        m_x = 0.5f * -(m_width * m_scaleX - m_screenWidth);
        m_y = 0.5f * -(m_height * m_scaleY - m_screenHeight);
        ((PositionFn)Slot(m_display, 0xc))(m_display, m_x, m_y, 0.0f);
    }
    unsigned int freeMemory = MEM_totalunused(0);
    m_player->UnPause();
    int startTick = TIMER_gettick();

    register unsigned int previous;
    register unsigned int tick;
    asm { mftb previous }
    for (;;) {
        asm { mftb tick }
        if (!m_software)
            g_screen.Wait();
        if (hold)
            ((NoArgFn)Slot(m_display, 0x18))(m_display);
        else
            ((DisplayFn)Slot(m_display, 0x14))(m_display, frame);
        if (!m_software) {
            GXDrawDone();
            g_screen.Flip();
        }
        SYNCTASK_run(0);
        THREAD_yield(0);

        if (!frame) {
            ReadInput();
        } else if (m_singleStep) {
            ((NoArgFn)Slot(m_display, 0x1c))(m_display);
            frame = m_player->GetFrame(1.0f + (float)m_player->m_frameNumber);
            hold = false;
            ReadInput();
        } else if (!m_waitForDecode) {
            hold = !m_player->IsTimeForDecode();
            if (!hold) {
                ((NoArgFn)Slot(m_display, 0x1c))(m_display);
                frame = m_player->GetFrame(m_player->m_time);
            }
            ReadInput();
        } else {
            while (!m_player->IsTimeForDecode() && !m_stop) {
                ReadInput();
                SYNCTASK_run(0);
                THREAD_yield(0);
            }
            ((NoArgFn)Slot(m_display, 0x1c))(m_display);
            frame = m_player->GetFrame(m_player->m_time);
            hold = false;
        }
        if (!frame) {
            hold = true;
            audioDone = m_player->IsAudioFinished();
        }
        if (audioDone || m_stop)
            break;

        // Elapsed bus ticks; the time base runs at a twelfth of the value kept here.
        frames++;
        int elapsed = (tick - previous) * 12;
        totalTicks += elapsed;
        int rate = (int)((float)OS_BUS_CLOCK / (float)elapsed);
        if (rate < minRate)
            minRate = rate;
        ring[ringIndex] = (float)elapsed / (0.001f * (float)OS_BUS_CLOCK);
        ringIndex++;
        if (ringIndex > 15) {
            ringIndex = 0;
            MEM_copy(history, ring, sizeof(ring));
        }
        previous = tick;
    }

    int endTick = TIMER_gettick();
    float total = (float)totalTicks;
    float cpuTime = total / (float)OS_BUS_CLOCK;
    PRINT_string("Frames %d\n", frames);
    PRINT_string("Min FrameRate %d\n", minRate);
    float average = (float)(totalTicks / frames);
    PRINT_string("Ave FrameRate %.2f\n", (float)OS_BUS_CLOCK / average);
    PRINT_string("CPU Time %.2f\n", cpuTime);
    PRINT_string("TIMER Time %.2f\n", (float)(endTick - startTick) / (float)TIMERhz);
    PRINT_string("Memory Available Durring Moive %.2fk %dBytes\n", (float)(int)freeMemory * (1.0f / 1024.0f), freeMemory);
    m_finished = true;
    if (m_display) {
        ((DeleteFn)Slot(m_display, 8))(m_display, 1);
        m_display = 0;
    }
}
