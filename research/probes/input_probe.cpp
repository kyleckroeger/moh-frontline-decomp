// settings: config/GMFE69/ipad.json
// replaces: iPAD_update__Fv
//
// Test harness, not game code. Linked into a test image by
// `link_drafts.py --probe`, it replaces the pad driver's per-frame update:
// controller 1 follows a fixed input script instead of the real pad, and every
// TRACE_EVERY updates the game's state is printed with OSReport, which Dolphin
// logs. Two images built with the same probe get identical input, so their
// traces can be compared line by line.
//
// research/tools/input_script.py writes the script table below from a text
// script; this file holds a short default (wait, then press Start and A).

struct PADStatus {
    unsigned short button;
    signed char stickX;
    signed char stickY;
    signed char substickX;
    signed char substickY;
    unsigned char triggerLeft;
    unsigned char triggerRight;
    unsigned char analogA;
    unsigned char analogB;
    signed char err;
};

extern PADStatus gPadStatus[4];
extern unsigned char* g_pPlayers[4];
extern "C" void OSReport(const char*, ...);

// One step: from `frame` on, hold these inputs until the next step.
struct Step {
    unsigned int frame;
    unsigned short button;
    signed char stickX, stickY, substickX, substickY;
    unsigned char triggerLeft, triggerRight;
};

// SCRIPT BEGIN
static const Step s_script[] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
};
static const unsigned int TRACE_EVERY = 30;
// SCRIPT END

static unsigned int s_update;
static unsigned int s_step;

static float F(const unsigned char* p, int offset) {
    return *(const float*)(p + offset);
}

static unsigned int Hash(const unsigned char* p, int size) {
    unsigned int h = 2166136261u;
    for (int i = 0; i < size; i++)
        h = (h ^ p[i]) * 16777619u;
    return h;
}

void iPAD_update(void) {
    const unsigned int steps = sizeof(s_script) / sizeof(s_script[0]);
    while (s_step + 1 < steps && s_script[s_step + 1].frame <= s_update)
        s_step++;
    const Step& s = s_script[s_step];
    PADStatus& pad = gPadStatus[0];
    pad.button = s.button;
    pad.stickX = s.stickX;
    pad.stickY = s.stickY;
    pad.substickX = s.substickX;
    pad.substickY = s.substickY;
    pad.triggerLeft = s.triggerLeft;
    pad.triggerRight = s.triggerRight;
    pad.analogA = (s.button & 0x100) ? 0xff : 0;
    pad.analogB = (s.button & 0x200) ? 0xff : 0;
    pad.err = 0;
    for (int i = 1; i < 4; i++)
        gPadStatus[i].err = -1;

    if (s_update % TRACE_EVERY == 0) {
        const unsigned char* p = g_pPlayers[0];
        if (p) {
            // Offsets from CPlayerObject in player_beginupdate.cpp, where
            // CVector3 is 16 bytes: m_tm's position at 0xb0, m_yaw 0x29c,
            // m_pitch 0x2a0, m_currentWeapon 0x45c.
            OSReport("TRACE %u step %u pos %.3f %.3f %.3f yaw %.4f pitch %.4f weapon %d hash %08x\n",
                     s_update, s_step, F(p, 0xb0), F(p, 0xb4), F(p, 0xb8), F(p, 0x29c), F(p, 0x2a0),
                     *(const int*)(p + 0x45c), Hash(p, 0x800));
        } else {
            OSReport("TRACE %u step %u noplayer\n", s_update, s_step);
        }
    }
    s_update++;
}
