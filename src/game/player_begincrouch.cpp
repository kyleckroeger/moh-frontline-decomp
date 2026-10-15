// A fragment of player.cpp (0x8009d0f8): CPlayerObject::BeginCrouch, while
// the flag at bit 0x08 of +919 is set, sets the two crouch flags (bits 0x80
// and 0x40 of +917) and a downward height speed (+676), slower in
// multiplayer. The class is named by the mangled symbol; its members and
// flag names are inferred from offsets in a non-virtual view. The rest of
// the file is not part of this unit.
extern bool g_bInMultiplayerMode;

class CPlayerObject {
public:
    void BeginCrouch();

    unsigned char unknown000[676];
    float m_heightSpeed;
    unsigned char unknown2a8[237];
    bool m_crouching : 1;
    bool m_crouchStarted : 1;
    unsigned char m_flags395 : 6;
    unsigned char unknown396;
    unsigned char m_flags397a : 4;
    bool m_flag397b : 1;
    unsigned char m_flags397b : 3;
};

void CPlayerObject::BeginCrouch() {
    if (!m_flag397b)
        return;
    m_crouching = true;
    m_crouchStarted = true;
    m_heightSpeed = -(g_bInMultiplayerMode ? 0.02666667f : 0.07333334f);
}
