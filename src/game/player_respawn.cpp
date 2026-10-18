// A fragment of player.cpp (0x800a68cc): CPlayerObject::Respawn. Unless the
// multiplayer level is ending, the player's script data takes its handicap
// (multiplayer: from the shell's table; else the script core's flags, or
// 255), and the player's state is reset: the height (1.1), the local
// capsule's line from the origin to that height, the bounding box (unit
// basis, 0.75 x 1.5 x 0.6, centred at (0.25, 0.7, -0.1)), the fire delay
// (RESPAWN_FIRE_DELAY), the state flags of +916..+920 and a number of
// timers, counters and vectors; the controller's rumble byte is cleared, the
// multiplayer data is reinitialised from the shell's player entry and the
// player restarts at its restart point. The class, functions and globals are
// named by the mangled symbols; the members, flags and views are inferred.
// CVector3 has a user copy constructor copying the two doublewords one by
// one: it builds by-value temporaries in place yet leaves the dead
// parameter copies of the inlined CLine3 constructor (an implicit copy
// gives the latter but not the former). The constants are entries of the
// file's .sdata2 pool. The rest of the file is in other units.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; }
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3 operator=(const CVector3& other) {
        d = other.d;
        return *this;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start = other.m_start;
            m_end = other.m_end;
            m_dir = other.m_dir;
            m_value30 = other.m_value30;
            m_value34 = other.m_value34;
            m_flag38 = other.m_flag38;
            m_flag39 = other.m_flag39;
        }
        return *this;
    }
    void SetSE(CVector3 start, CVector3 end) {
        m_start.d = start.d;
        m_end.d = end.d;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class CVolBox {
public:
    void SetBasis(CVector3, CVector3, CVector3);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);

    unsigned char unknown00[172];
};

struct MOH_core_Struct {
    unsigned char unknown00[112];
    unsigned char flags;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    MOH_core_Struct* core;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual unsigned int* GetScriptData();
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

struct MPPlayerData;

struct ShellView {
    unsigned char unknown0000[5180];
    unsigned int m_handicaps[4];
    unsigned char m_players[4][28];
};

struct InputManagerView {
    unsigned char unknown0000[9286];
    unsigned char m_rumble[4];
};

extern int g_bMPEndLevel;
extern bool g_bInMultiplayerMode;
extern ShellView g_Shell;
extern InputManagerView g_inputMgr;
extern int RESPAWN_FIRE_DELAY;
void RestartPlayer(int);

class CPlayerObject {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual BSObjectView* GetScriptObject() const;

    void Respawn();
    void InitMultiplayer(MPPlayerData*, bool);

    unsigned char unknown0000[116];
    float m_value078;
    float m_value07c;
    unsigned char unknown007c[128];
    int m_value100;
    int m_value104;
    unsigned char unknown0104[8];
    CVolBox m_box;
    float m_value1bc;
    CLine3 m_line;
    unsigned char unknown01fc[152];
    float m_height;
    unsigned char unknown0298[4];
    float m_value2a0;
    unsigned char unknown02a0[240];
    unsigned char m_flag394_80 : 1;
    unsigned char m_flag394_40 : 1;
    unsigned char m_flag394_20 : 1;
    unsigned char m_flag394_10 : 1;
    unsigned char m_flag394_08 : 1;
    unsigned char m_flag394_04 : 1;
    unsigned char m_flag394_02 : 1;
    unsigned char m_flag394_01 : 1;
    unsigned char m_flag395_80 : 1;
    unsigned char m_flag395_40 : 1;
    unsigned char m_flag395_20 : 1;
    unsigned char m_flag395_10 : 1;
    unsigned char m_flag395_08 : 1;
    unsigned char m_flag395_04 : 1;
    unsigned char m_flag395_02 : 1;
    unsigned char m_flag395_01 : 1;
    unsigned char m_flag396_80 : 1;
    unsigned char m_flag396_40 : 1;
    unsigned char m_flag396_20 : 1;
    unsigned char m_flag396_10 : 1;
    unsigned char m_flag396_08 : 1;
    unsigned char m_flag396_04 : 1;
    unsigned char m_flag396_02 : 1;
    unsigned char m_flag396_01 : 1;
    unsigned char m_flag397_80 : 1;
    unsigned char m_flag397_40 : 1;
    unsigned char m_flag397_20 : 1;
    unsigned char m_flag397_10 : 1;
    unsigned char m_flag397_08 : 1;
    unsigned char m_flag397_04 : 1;
    unsigned char m_flag397_02 : 1;
    unsigned char m_flag397_01 : 1;
    unsigned char m_flag398_80 : 1;
    unsigned char m_flag398_40 : 1;
    unsigned char m_flag398_20 : 1;
    unsigned char m_flag398_10 : 1;
    unsigned char m_flag398_08 : 1;
    unsigned char m_flag398_04 : 1;
    unsigned char m_flag398_02 : 1;
    unsigned char m_flag398_01 : 1;
    unsigned char unknown0395[3];
    float m_value39c;
    unsigned char unknown039c[64];
    CVector3 m_vector3e0;
    unsigned char unknown03ec[32];
    float m_value410;
    unsigned char unknown0410[24];
    float m_value42c;
    float m_value430;
    unsigned char unknown0430[4];
    float m_value438;
    unsigned char unknown0438[4];
    float m_value440;
    float m_value444;
    unsigned char unknown0444[4];
    float m_value44c;
    float m_value450;
    float m_value454;
    float m_value458;
    int m_value45c;
    unsigned char unknown045c[368];
    int m_value5d0;
    unsigned char unknown05d0[12];
    int m_value5e0;
    unsigned char unknown05e0[232];
    int m_value6cc;
    unsigned char unknown06cc[32];
    int m_value6f0;
    unsigned char unknown06f0[92];
    short m_value750;
    short m_value752;
    float m_value754;
    float m_value758;
    unsigned char unknown0758[4];
    int m_value760;
    int m_shellIndex;
    int m_controller;
    int m_restartIndex;
    unsigned char unknown076c[64];
    int m_value7b0;
    float m_value7b4;
    float m_value7b8;
    float m_value7bc;
    float m_value7c0;
    unsigned char m_value7c4;
    unsigned char unknown07c1[2803];
    int m_fireDelay;
};

void CPlayerObject::Respawn() {
    if (!g_bMPEndLevel) {
        BSObjectView* script = GetScriptObject();
        if (script) {
            MOH_core_Struct* core = script->trigger->core;
            unsigned int* data = script->user->GetScriptData();
            if (g_bInMultiplayerMode)
                *data = g_Shell.m_handicaps[m_shellIndex];
            else if (core)
                *data = core->flags;
            else
                *data = 255;
        }
        m_value6f0 = -1;
        m_value6cc = 1;
        m_value100 = 0;
        m_value104 = 0;
        m_value1bc = 0.4f;
        m_height = 1.1f;
        m_line = CLine3(CVector3(0.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, m_height));
        m_box.SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
        m_box.SetWidth(0.75f);
        m_box.SetHeight(1.5f);
        m_box.SetDepth(0.6f);
        m_box.SetCenter(CVector3(0.25f, 0.7f, -0.1f));
        m_value5d0 = 0;
        m_value45c = -1;
        m_value454 = -1.0f;
        m_value458 = -1.0f;
        m_fireDelay = RESPAWN_FIRE_DELAY;
        m_flag394_04 = 0;
        m_flag397_02 = 0;
        m_flag395_80 = 0;
        m_flag395_40 = 0;
        m_flag397_08 = 1;
        m_flag397_04 = 0;
        m_flag395_20 = 0;
        m_flag395_08 = 0;
        m_flag394_01 = 0;
        m_flag395_01 = 0;
        m_value410 = 0.0f;
        m_flag396_80 = 0;
        m_flag396_08 = 0;
        m_flag394_10 = 1;
        m_flag396_01 = 1;
        m_flag396_02 = 0;
        m_flag397_80 = 0;
        m_flag397_01 = 0;
        m_flag397_20 = 1;
        m_flag397_10 = 1;
        m_value758 = 1.0f;
        m_value39c = 0.0f;
        m_value5e0 = 0;
        m_flag398_80 = 1;
        m_flag398_40 = 0;
        m_value7b0 = 0;
        m_value7b4 = 0.0f;
        m_value7b8 = 0.0f;
        m_value7bc = 0.0f;
        m_value7c0 = 0.0f;
        m_value7c4 = 0;
        m_value42c = 35.0f;
        m_value430 = 35.0f;
        m_flag396_40 = 0;
        m_value438 = 0.0f;
        m_vector3e0 = CVector3(0.0f, 0.0f, 0.0f);
        m_value44c = 0.0f;
        m_value450 = 0.0f;
        m_value078 = 0.0f;
        m_value07c = 0.0f;
        m_value440 = 0.0f;
        m_value444 = 0.0f;
        m_value750 = 1;
        m_value752 = 0;
        m_flag395_10 = 0;
        m_flag395_04 = 0;
        m_value754 = 0.0f;
        m_flag397_40 = 0;
        m_value760 = -1;
        m_value2a0 = 0.0f;
        g_inputMgr.m_rumble[m_controller] = 0;
        InitMultiplayer((MPPlayerData*)g_Shell.m_players[m_shellIndex], true);
        RestartPlayer(m_restartIndex);
    }
}
