// A fragment of player.cpp (0x8009d810): CPlayerObject::StartDeathSequence.
// Runs once (while the death timer at +952 is still zero): the player is
// marked dying and its state set to 25; the current weapon's flag is
// cleared, the weapon is fired once with shoot type 1 and its attachment's
// value at +648 reset; the death timer starts at 180, a flag at +918 is
// cleared and, when the flag tested at +919 is set, two flags at +917 are
// raised and the value at +676 is set to minus 0.02666667 in multiplayer or
// minus 0.07333334 otherwise; outside multiplayer the screen fades out
// through a function-static CFadeOut (from transparent to half-opaque black
// over 180) and the game pauses with state 7; finally the pair at +668 is
// copied to +4796. The fade's guard is a one-byte .sbss object at an odd
// address. CFadeOut's weak table is a duplicate: the original kept Moh2.cpp's
// copy. The file name is this project's; the original record is player.cpp. The classes, functions and globals are named by the symbols;
// the members, the flag positions (from the masks), the shoot-type and pause
// values and the CFadeOut constructor are inferred, and the constants are
// entries of the file's .sdata2 pool.
struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFaderControl {
public:
    CFaderControl(CColor, CColor, float);
    virtual void Update(float);

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

class CFadeOut : public CFaderControl {
public:
    CFadeOut() : CFaderControl(CColor(0, 0, 0, 0), CColor(0, 0, 0, 128), 180.0f) {}
};

class CFader {
public:
    static void SetControl(CFaderControl*);
};

enum PauseState {};

class Pause {
public:
    void SetState(PauseState);

    unsigned char unknown00[152];
};

extern Pause g_Pause;
extern bool g_bInMultiplayerMode;

enum EWeaponShootType {};

/* Inferred: the weapon attachment's value at +648. */
struct WeaponAttachmentView {
    unsigned char unknown000[648];
    float value288;
};

class CPlayerWeaponObject {
public:
    void Shoot(EWeaponShootType, int);

    unsigned char unknown0000[12516];
    WeaponAttachmentView* m_attachment;
    unsigned char unknown30e8[234];
    unsigned char unknown31d2a : 1;
    unsigned char m_flag31d2 : 1;
    unsigned char unknown31d2b : 6;
};

class CPlayerObject {
public:
    void StartDeathSequence();

    unsigned char unknown000[668];
    float m_value29c;
    float m_value2a0;
    float m_value2a4;
    unsigned char unknown2a8[236];
    unsigned char unknown394;
    unsigned char m_flag395a : 1;
    unsigned char m_flag395b : 1;
    unsigned char unknown395 : 6;
    unsigned char unknown396 : 7;
    unsigned char m_flag396 : 1;
    unsigned char unknown397a : 1;
    unsigned char m_dying : 1;
    unsigned char unknown397b : 2;
    unsigned char m_flag397 : 1;
    unsigned char unknown397c : 3;
    unsigned char unknown398[32];
    float m_deathTime;
    unsigned char unknown3bc[160];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[440];
    int m_state;
    unsigned char unknown6d0[3052];
    float m_value12bc;
    float m_value12c0;
};

void CPlayerObject::StartDeathSequence() {
    if (m_deathTime != 0.0f)
        return;
    m_dying = true;
    m_state = 25;
    if (m_currentWeapon >= 0) {
        m_weapons[m_currentWeapon]->m_flag31d2 = false;
        m_weapons[m_currentWeapon]->Shoot((EWeaponShootType)1, 0);
        WeaponAttachmentView* attachment = m_weapons[m_currentWeapon]->m_attachment;
        if (attachment)
            attachment->value288 = 0.0f;
    }
    m_deathTime = 180.0f;
    m_flag396 = false;
    if (m_flag397) {
        m_flag395a = true;
        m_flag395b = true;
        m_value2a4 = -(g_bInMultiplayerMode ? 0.02666667f : 0.07333334f);
    }
    if (!g_bInMultiplayerMode) {
        static CFadeOut fadeOut;
        CFader::SetControl(&fadeOut);
        g_Pause.SetState((PauseState)7);
    }
    m_value12bc = m_value29c;
    m_value12c0 = m_value2a0;
}
