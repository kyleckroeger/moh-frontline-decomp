// A fragment of ShellMenu.cpp (0x800de420): the SKU mode (always 1) and
// setting up the multiplayer buttons of each
// open window's player in the controller configuration, and
// PassMP_PrefHitLocationToIStudio, which names the body part a player hit
// most (the first of the seven per-player hit counts with the highest
// positive count: head, arm, arm, torso, leg, leg, groin, else "None").
// CShellMenu,
// CDualConfig and the functions are named by the mangled symbols; the
// members, the 28-byte player records, the 256-byte statistics records and
// the configuration fields are inferred; the names are .sdata2 pool entries
// and the switch's jump table is this unit's .data. MoveStatsIntoProperAreaForIstudio before this (the start of the
// record) is drafted in scratch/game/movestats_wip.cpp; the rest of the file
// is not part of this unit.
class CDualConfig {
public:
    void SetupButtonsForMultiplayerGame();

    unsigned char unknown00[56];
    int m_player;
    unsigned char unknown3c[4];
    int m_controller;
};

extern CDualConfig g_ControllerConfig;

struct MPPlayerButtonsView {
    unsigned char unknown00[24];
    unsigned char controller;
    unsigned char unknown19[3];
};

extern "C" char* strcpy(char*, const char*);

/* inferred view of a player's 256-byte multiplayer statistics record */
struct MPStatsView {
    unsigned char unknown00[4];
    int hitLocations[7];
    unsigned char unknown20[224];
};

class CShellMenu {
public:
    int GetSKUMode();
    void PassMP_PrefHitLocationToIStudio(int, char*);
    void SetupAllPlayersButtonsForMultiplayerGame();

    unsigned char unknown0000[5196];
    MPPlayerButtonsView m_players[4];
    unsigned char unknown14bc[16];
    MPStatsView m_mpStats[4];
    int m_activeWindows[4];
};

int CShellMenu::GetSKUMode() {
    return 1;
}

void CShellMenu::SetupAllPlayersButtonsForMultiplayerGame() {
    int i;

    for (i = 0; i < 4; i++) {
        if (m_activeWindows[i] != 0) {
            g_ControllerConfig.m_controller = m_players[i].controller;
            g_ControllerConfig.m_player = i;
            g_ControllerConfig.SetupButtonsForMultiplayerGame();
        }
    }
}

void CShellMenu::PassMP_PrefHitLocationToIStudio(int player, char* location) {
    int most = 0;
    int preferred = -1;
    for (int i = 0; i < 7; i++) {
        if (m_mpStats[player].hitLocations[i] > most) {
            most = m_mpStats[player].hitLocations[i];
            preferred = i;
        }
    }
    switch (preferred) {
    case 1:
        strcpy(location, "Arm");
        break;
    case 2:
        strcpy(location, "Arm");
        break;
    case 4:
        strcpy(location, "Leg");
        break;
    case 5:
        strcpy(location, "Leg");
        break;
    case 6:
        strcpy(location, "Groin");
        break;
    case 0:
        strcpy(location, "Head");
        break;
    case 3:
        strcpy(location, "Torso");
        break;
    default:
        strcpy(location, "None");
        break;
    }
}
