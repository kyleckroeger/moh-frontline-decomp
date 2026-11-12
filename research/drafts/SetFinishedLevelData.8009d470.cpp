extern bool g_bInMultiplayerMode;

enum EWeaponTypes {};

class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();
    void SetPlayersFinishedLevelSuccessfully(int, int);
    void SetPlayersFinishedLevelHealth(int, int, float);
    void SetPlayersFinishedLevelAmmo(int, int, EWeaponTypes, unsigned int);

    unsigned char unknown0000[15];
    bool m_levelFinished;
    unsigned char unknown0010[5132];
    int m_mpMission;
    int m_mpStage;
    unsigned char unknown1424[1464];
};

extern CShellMenu g_Shell;

class ShotCountView {
    unsigned char unknown00[12];

public:
    virtual void unknown008();
    virtual int* unknown00c();
};

class ShotStatsView {
public:
    unsigned char unknown00[12];
    ShotCountView* counter;
};

struct WeaponDumpInfoView {
    unsigned char unknown000[644];
    short flagBase;
    short flagOffset;
};

class CWeapon {
public:
    unsigned char unknown0000[12516];
    WeaponDumpInfoView* m_dumpInfo;
};

class CPlayerObject {
public:
    void SetFinishedLevelData();
    void SaveAmmo(int, int);
    unsigned char unknown000[592];
    ShotStatsView* m_stats;
    unsigned char unknown254[524];
    CWeapon* m_weapons[45];
};

// DRAFT NOTE (loop knots2, 2026-11-12): SaveAmmo is an inferred member inline (no symbol
// attests it; knots1 form, same 11 lines as the earlier void* free inline).
inline void CPlayerObject::SaveAmmo(int mission, int stage) {
    short offset; short base; int i; CWeapon* weapon;
    for (i = 0; i < 45; i++) {
        weapon = m_weapons[i];
        if (weapon && weapon->m_dumpInfo) {
            base = weapon->m_dumpInfo->flagBase;
            offset = weapon->m_dumpInfo->flagOffset;
            if (g_Shell.Get_currentMission() != 2 || g_Shell.Get_currentStage() != 2 || g_bInMultiplayerMode || i == 8) {
                g_Shell.SetPlayersFinishedLevelAmmo(mission, stage, (EWeaponTypes)i, base + offset);
                if (i == 8)
                    g_Shell.SetPlayersFinishedLevelAmmo(mission, stage, (EWeaponTypes)7, base + offset);
            }
        }
    }
}


void CPlayerObject::SetFinishedLevelData() {
    if (!g_Shell.m_levelFinished)
        return;
    int mission;
    int stage;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.m_mpMission;
        stage = g_Shell.m_mpStage;
    } else {
        mission = g_Shell.Get_currentMission();
        stage = g_Shell.Get_currentStage();
    }
    g_Shell.SetPlayersFinishedLevelSuccessfully(mission, stage);
    g_Shell.SetPlayersFinishedLevelHealth(mission, stage, *m_stats->counter->unknown00c());
    SaveAmmo(mission, stage);
}
