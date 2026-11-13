// settings: config/GMFE69/GameMessage.json
// replaces: Get_currentMission__10CShellMenuFv
// replaces: Get_currentStage__10CShellMenuFv
//
// Test harness, not game code. Starts a new game in level MISSION_STAGE instead of
// 1_1: both getters (originals: `return m_mission;` at +4, `return m_stage;` at +8)
// first store the route's mission and stage into the shell, so every reader, including
// the shell's own code that reads the members directly, sees the same level.
class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();
    int unknown00;
    unsigned int m_mission;
    unsigned int m_stage;
};

unsigned int CShellMenu::Get_currentMission() {
    m_mission = 4;
    m_stage = 1;
    return m_mission;
}

unsigned int CShellMenu::Get_currentStage() {
    m_mission = 4;
    m_stage = 1;
    return m_stage;
}
