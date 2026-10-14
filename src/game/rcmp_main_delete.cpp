// A fragment of rcmp_main.cpp (0x80103e58): the weak (empty) destructor of
// DISP_OLD_FRAMES (a header inline, so defined __declspec(weak)), then
// OPTION_PARSING_PLAYER::ReadInput
// updates the input manager and, while input is being read, marks a skip
// request when any of the listed controls (ids 128, 121, 127, 130, 129, 133,
// 132, 134, 123, 124, 125, 126) is active for any player; then the weak class
// operator delete of RCMP::AV_PLAYER, which frees through the RCMP system's free hook. RCMP, RCMP_SYSTEM, rcmp_sys and AV_PLAYER
// and OPTION_PARSING_PLAYER are named by the symbols; the hook slot is as in
// rcmp_mpc_codec.cpp; the player's flags (+36 skip, +38 reading input) and
// the input-manager view are inferred. The
// delete is inline in the original (a weak symbol), so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
class DISP_OLD_FRAMES {
public:
    ~DISP_OLD_FRAMES();
};

__declspec(weak) DISP_OLD_FRAMES::~DISP_OLD_FRAMES() {
}

class CInputManager {
public:
    void Update(float);
    unsigned char GetValueAnyPlayer(unsigned long);

    unsigned char unknown0000[9296];
};

extern CInputManager g_inputMgr;

class OPTION_PARSING_PLAYER {
public:
    void ReadInput();

    unsigned char unknown00[36];
    bool m_skip;
    unsigned char unknown25;
    bool m_readInput;
};

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

namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
};

extern RCMP_SYSTEM rcmp_sys;

class AV_PLAYER {
public:
    void operator delete(void*);
};
}

__declspec(weak) void RCMP::AV_PLAYER::operator delete(void* p) {
    rcmp_sys.m_free(p);
}
