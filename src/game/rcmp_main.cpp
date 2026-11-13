// rcmp_main.cpp:
// OPTION_PARSING_PLAYER::Play shows a movie frame by frame until it ends or a
// skip is requested, keeping frame-rate statistics that it prints at the end.
// The software path creates a GCSW display on the front buffer (clearing it
// once, through the guarded function static FirstTime); the hardware path a
// MOHFL display, centred by the player's scale. Frame timing reads the time
// base through inline asm (times 12, as the original). Then the weak (empty)
// destructor of DISP_OLD_FRAMES, Play's frame-time record (a header inline,
// emitted after its first user); OPTION_PARSING_PLAYER::ReadInput updates the
// input manager and, while input is being read, marks a skip request when any
// of the listed controls (ids 128, 121, 127, 130, 129, 133, 132, 134, 123, 124,
// 125, 126) is active for any player; then the weak class operator delete of
// RCMP::AV_PLAYER, which frees through the RCMP system's free hook.
// The classes and functions are named by the mangled symbols (DISP_OLD_FRAMES
// also by Play's extab record); the members, the display's virtual slots
// (destructor 0x08, SetPosition 0x0c, Display 0x14, Redisplay 0x18, Advance
// 0x1c; 0x10 is unused here), the frame-time record's layout (index,
// history[16], ring[16]: fixed by the original frame), the player's flags
// (+0x24 skip request, +0x26 reading input), the input-manager view, the hook
// slot (as in rcmp_mpc_codec.cpp) and the local names are inferred. The strings
// and constants are items of the file's .rodata and .sdata2 pools;
// rcmp_main_block.h recreates the head of the constant block Play's offsets
// depend on. RCMP_PlayMovie creates the option-parsing player (its inline
// constructor sets three scales to 1.0f and clears the rest), loads the movie
// (an inferred inline helper: a message when the file does not exist,
// otherwise an AV_PLAYER for it, 1 MB of buffer, allocated through the RCMP
// system's allocator hook by the class operator new), and if that worked marks
// the player as playing, plays it, deletes it (its inline destructor deletes
// the AV_PLAYER) and clears the playing flag through the stale pointer, as the
// original does; the constructor's store order is inferred. Last, start-up and
// shutdown: initialise the global framework g_fw and set the RCMP system's
// REAL defaults, or restore the framework (an opaque view), then the static
// initialisation, which registers g_fw's destructor, and that weak empty
// destructor. The player's members are read under two sets of names (Play's
// and the constructor's), so the class has anonymous unions of both.
#include "rcmp_main_block.h"

class FRAMEWORK {
public:
    void Init();
    void Restore();
    ~FRAMEWORK() {}

    int m_state;
};


FRAMEWORK g_fw;
class OPTION_PARSING_PLAYER;
// The player RCMP_PlayMovie is showing (the .sbss word after g_fw).
OPTION_PARSING_PLAYER* CurentOPPlayer;

extern "C" bool FILE_exists(const char*);
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
    enum LOAD_ENUM {};
    enum SOUND_ENUM {};
    AV_PLAYER(const char*, int, const char*, int, LOAD_ENUM, SOUND_ENUM);
    ~AV_PLAYER();
    static void* operator new(unsigned long size);
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
    static void operator delete(void*);
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

class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();
    void SetREALDefaults();

    void* (*m_alloc)(const char*, int, int, int, int);
    void (*m_free)(void*);
    int m_userValue;
};

extern RCMP_SYSTEM rcmp_sys;

inline void* AV_PLAYER::operator new(unsigned long size) {
    return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_userValue);
}
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
    OPTION_PARSING_PLAYER() {
        data2c = 0;
        data30 = 0;
        data31 = 0;
        data32 = 0;
        data2e = 0;
        data2f = 0;
        data2d = 0;
        data33 = 0;
        data34 = 0;
        data28 = 0;
        data38 = 0;
        data3c = 0;
        m_player = 0;
        data00 = 1.0f;
        data04 = 1.0f;
        data08 = 1.0f;
        data0c = 0.0f;
        data10 = 0.0f;
        data14 = 0.0f;
        data18 = 0.0f;
        data20 = 0.0f;
        data1c = 0.0f;
        data24 = 0;
        data44 = 0;
    }
    ~OPTION_PARSING_PLAYER() {
        if (m_player) {
            delete m_player;
            m_player = 0;
        }
    }
    bool Load(const char* name) {
        if (!FILE_exists(name)) {
            PRINT_string("Couldn't find movie file %s\n", name);
            return false;
        }
        m_player = new RCMP::AV_PLAYER(name, 0x100000, 0, 0, (RCMP::AV_PLAYER::LOAD_ENUM)0, (RCMP::AV_PLAYER::SOUND_ENUM)0);
        return true;
    }
    void Play(bool);
    void ReadInput();

    float data00;
    union {
        float data04;
        float m_scaleX;
    };
    union {
        float data08;
        float m_scaleY;
    };
    union {
        float data0c;
        float m_x;
    };
    union {
        float data10;
        float m_y;
    };
    union {
        float data14;
        float m_height;
    };
    union {
        float data18;
        float m_width;
    };
    union {
        float data1c;
        float m_screenHeight;
    };
    union {
        float data20;
        float m_screenWidth;
    };
    union {
        unsigned char data24;
        bool m_skip;       // +0x24, skip request
    };
    bool m_playing;
    bool m_readInput;   // +0x26, read by ReadInput
    unsigned char unknown27;
    int data28;
    union {
        unsigned char data2c;
        bool m_singleStep; // +0x2c
    };
    union {
        unsigned char data2d;
        bool m_software;   // +0x2d
    };
    unsigned char data2e;
    unsigned char data2f;
    union {
        unsigned char data30;
        bool m_waitForDecode; // +0x30
    };
    unsigned char data31;
    unsigned char data32;
    unsigned char data33;
    unsigned char data34;
    unsigned char unknown35[3];
    int data38;
    int data3c;
    RCMP::AV_PLAYER* m_player; // +0x40
    union {
        int data44;
        RCMP::VIDEO_DISPLAY* m_display; // +0x44
    };
};

void OPTION_PARSING_PLAYER::Play(bool readInput) {
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
    m_readInput = readInput;
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
            while (!m_player->IsTimeForDecode() && !m_skip) {
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
        if (audioDone || m_skip)
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
    m_readInput = true;
    if (m_display) {
        delete m_display;
        m_display = 0;
    }
}

#include "game/CInputManager.h"


extern CInputManager g_inputMgr;

void OPTION_PARSING_PLAYER::ReadInput() {
    g_inputMgr.Update(0.0f);
    if (m_readInput) {
        if (g_inputMgr.GetValueAnyPlayer(128))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(121))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(127))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(130))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(129))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(133))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(132))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(134))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(123))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(124))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(125))
            m_skip = true;
        if (g_inputMgr.GetValueAnyPlayer(126))
            m_skip = true;
    }
}

__declspec(weak) void RCMP::AV_PLAYER::operator delete(void* p) {
    rcmp_sys.m_free(p);
}

void RCMP_PlayMovie(char* name, bool loop) {
    CurentOPPlayer = new OPTION_PARSING_PLAYER;
    if (CurentOPPlayer->Load(name)) {
        CurentOPPlayer->m_playing = true;
        CurentOPPlayer->Play(loop);
        delete CurentOPPlayer;
        CurentOPPlayer->m_playing = false;
    }
}


void RCMP_Shutdown() {
    g_fw.Restore();
}

void RCMP_Initialize() {
    g_fw.Init();
    RCMP::rcmp_sys.SetREALDefaults();
}
