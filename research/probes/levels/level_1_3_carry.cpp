// settings: config/GMFE69/GameMessage.json
// replaces: Get_currentMission__10CShellMenuFv
// replaces: Get_currentStage__10CShellMenuFv
//
// Test harness, not game code. As research/probes/levels/level_1_3.cpp (start a new
// game in level 1_3), and the shell also records level 1_2 as finished successfully
// with full health and 20 rounds for every weapon slot, as SetFinishedLevelData does
// at the end of a won level. CShellMenu::ShouldIUsePlayersFinishedLevelData(1, 3)
// then holds, so the player is created with 1_2's health and CPlayerObject::Init
// gives each weapon its carried ammunition (CPlayerWeaponObject::AddAmo).
// Offsets: m_finished[6][4] at 0x258, 0xbc bytes each: health (float), ammo[45],
// successful at +0xb8 (src/game/ShellMenu_levels.cpp).
struct FinishedLevel {
    float health;
    unsigned int ammo[45];
    bool successful;
    unsigned char pad[3];
};

class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();
    void Carry();
    int unknown00;
    unsigned int m_mission;
    unsigned int m_stage;
    unsigned char unknown0c[0x258 - 0xc];
    FinishedLevel m_finished[6][4];
};

void CShellMenu::Carry() {
    m_mission = 1;
    m_stage = 3;
    FinishedLevel& f = m_finished[0][1];
    f.health = 100.0f;
    for (int i = 0; i < 45; i++)
        f.ammo[i] = 20;
    f.successful = true;
}

unsigned int CShellMenu::Get_currentMission() {
    Carry();
    return m_mission;
}

unsigned int CShellMenu::Get_currentStage() {
    Carry();
    return m_stage;
}
