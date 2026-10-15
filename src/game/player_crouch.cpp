// A fragment of player.cpp (0x8009c894): CPlayerObject::SetHideWeapon
// triggers script event 275 when a hidden weapon is shown and 274 when a
// shown one is hidden, then records the state (a bool bit field, bit 0x80 of
// +919). IsPlayerCrouching: the player is crouching when the flag at bit 0x80
// of +918 is set or its height (+664) is below 0.66, or in multiplayer when
// the height less 0.7 is below 0.24000002 (the constant's exact value; how
// the original wrote it is not known). The class is named by the mangled
// symbol; its members are inferred from offsets, and its virtual functions
// before TriggerScriptEvent (+100, named by the vtable) are placeholders. The
// event numbers are not named. The rest of the file is not part of this unit.
extern bool g_bInMultiplayerMode;

class CPlayerObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void TriggerScriptEvent(int, void*, bool);
    void SetHideWeapon(bool);
    bool IsPlayerCrouching() const;

    unsigned char unknown004[660];
    float m_height;
    unsigned char unknown29c[250];
    unsigned char m_flag396 : 1;
    unsigned char m_flags396 : 7;
    bool m_weaponHidden : 1;
    unsigned char m_flags397 : 7;
};

void CPlayerObject::SetHideWeapon(bool hide) {
    if (m_weaponHidden && !hide)
        TriggerScriptEvent(275, 0, false);
    else if (!m_weaponHidden && hide)
        TriggerScriptEvent(274, 0, false);
    m_weaponHidden = hide;
}

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
