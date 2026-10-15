// A fragment of player.cpp (0x8009c938): CPlayerObject::IsPlayerCrouching.
// The player is crouching when the flag at bit 0x80 of +918 is set or its
// height (+664) is below 0.66, or in multiplayer when the height less 0.7 is
// below 0.24000002 (the constant's exact value; how the original
// wrote it is not known). The class is named by the mangled symbol; its
// members are inferred from offsets in a non-virtual view. The rest of the
// file is not part of this unit.
extern bool g_bInMultiplayerMode;

class CPlayerObject {
public:
    bool IsPlayerCrouching() const;

    unsigned char unknown000[664];
    float m_height;
    unsigned char unknown29c[250];
    unsigned char m_flag396 : 1;
    unsigned char m_flags396 : 7;
};

bool CPlayerObject::IsPlayerCrouching() const {
    if (g_bInMultiplayerMode) {
        bool crouching = false;
        if (m_flag396 || m_height - 0.7f < 0.24000002f)
            crouching = true;
        return crouching;
    }
    bool crouching = false;
    if (m_flag396 || m_height < 0.66f)
        crouching = true;
    return crouching;
}
