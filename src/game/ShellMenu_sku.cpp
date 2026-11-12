#include "ShellMenu_block.h"

// A fragment of ShellMenu.cpp (0x800de334): MoveStatsIntoProperAreaForIstudio,
// which with two or three open windows moves the players' statistics records
// (stored in player order) into the slots of the windows they play in, the
// SKU mode (always 1) and
// setting up the multiplayer buttons of each
// open window's player in the controller configuration, and
// PassMP_PrefHitLocationToIStudio, which names the body part a player hit
// most (the first of the seven per-player hit counts with the highest
// positive count: head, arm, arm, torso, leg, leg, groin, else "None"), and
// PassMP_PrefTargetToIStudio, which names the player this player hit most
// (else "None") and returns that player's index or -1, and
// PassMP_PrefWeaponNameToIStudio, which copies the name of the weapon the
// player killed most with (of 45 counts; message 407 + index of the user
// message table, else "None") and shortens a few long names.
// CShellMenu,
// CDualConfig and the functions are named by the mangled symbols; the
// members, the 28-byte player records, the 256-byte statistics records and
// the configuration fields and the message table are inferred; the names are .sdata2 (and .rodata) pool entries
// and the switch's jump table is this unit's .data. The rest of the file is
// not part of this unit.
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
    unsigned char unknown00[8];
    char name[11];
    unsigned char unknown13[5];
    unsigned char controller;
    unsigned char unknown19[3];
};

extern "C" char* strcpy(char*, const char*);
extern "C" char* strstr(const char*, const char*);
extern "C" void* memcpy(void*, const void*, unsigned long);
extern "C" void* memset(void*, int, unsigned long);

/* inferred view of the user message table (8-byte entries, text first) */
struct UserMessageView {
    char* text;
    int id;
};

class CUserMessageTable {
public:
    char* GetMessage(int id) const { return id < m_count ? m_entries[id].text : 0; }

    UserMessageView* m_entries;
    unsigned char unknown04[8];
    int m_count;
};


/* inferred view of a player's 256-byte multiplayer statistics record */
struct MPStatsView {
    unsigned char unknown00[4];
    int hitLocations[7];
    int weaponKills[45];
    int targets[4];
    unsigned char unknowne4[28];
};

class CShellMenu {
public:
    /* counts the open windows (the same count as Get_NumPlayers) */
    int CountWindows() {
        int count = 0;
        if (m_activeWindows[0] != 0)
            count++;
        if (m_activeWindows[1] != 0)
            count++;
        if (m_activeWindows[2] != 0)
            count++;
        if (m_activeWindows[3] != 0)
            count++;
        m_activeWindowCount = count;
        return count;
    }
    void MoveStatsIntoProperAreaForIstudio();
    int GetSKUMode();
    void PassMP_PrefHitLocationToIStudio(int, char*);
    int PassMP_PrefTargetToIStudio(int, char*);
    void PassMP_PrefWeaponNameToIStudio(int, char*);
    void SetupAllPlayersButtonsForMultiplayerGame();

    unsigned char unknown0000[5140];
    int m_activeWindowCount;
    unsigned char unknown1418[52];
    MPPlayerButtonsView m_players[4];
    unsigned char unknown14bc[16];
    MPStatsView m_mpStats[4];
    int m_activeWindows[4];
    unsigned char unknown18dc[232];
    CUserMessageTable m_messages;
};

void CShellMenu::MoveStatsIntoProperAreaForIstudio() {
    int count = CountWindows();
    if (count < 2 || count > 3)
        return;
    {
        MPStatsView stats[4];
        memset(stats, 0, sizeof(stats));
        MPStatsView* dest = stats;
        int window = 0;
        for (int i = 0; i < count; window++, dest++) {
            if (m_activeWindows[window]) {
                memcpy(dest, &m_mpStats[i], sizeof(MPStatsView));
                i++;
            }
        }
        memcpy(m_mpStats, stats, sizeof(stats));
    }
}

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

int CShellMenu::PassMP_PrefTargetToIStudio(int player, char* name) {
    int most = 0;
    int target = -1;
    for (int i = 0; i < 4; i++) {
        int count = m_mpStats[player].targets[i];
        if (count > most) {
            most = count;
            target = i;
        }
    }
    if (target >= 0)
        strcpy(name, m_players[target].name);
    else
        strcpy(name, "None");
    return target;
}

void CShellMenu::PassMP_PrefWeaponNameToIStudio(int player, char* name) {
    int preferred = -1;
    int most = 0;
    for (int i = 0; i < 45; i++) {
        if (m_mpStats[player].weaponKills[i] > most) {
            most = m_mpStats[player].weaponKills[i];
            preferred = i;
        }
    }
    if (preferred >= 0) {
        strcpy(name, preferred + 407 < m_messages.m_count ? m_messages.m_entries[preferred + 407].text : 0);
        if (strstr(name, "Walther"))
            strcpy(name, "P38");
        else if (strstr(name, "Stiel Gran"))
            strcpy(name, "S. Granate");
        else if (strstr(name, "Silenced"))
            strcpy(name, "S. Pistol");
        else if (strstr(name, "Panzerschr"))
            strcpy(name, "Panzer");
        else if (strstr(name, "Gewehr"))
            strcpy(name, "Gewehr 43");
        else if (strstr(name, "Springfiel"))
            strcpy(name, "Spring.");
    } else {
        strcpy(name, "None");
    }
}
