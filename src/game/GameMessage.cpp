// GameMessage.cpp (0x8008d1ec): GameMessage, the shell/front-end message
// handler called by the interface studio. It switches on the message number
// and reads its arguments from the parameter array (slot numbers are 1-based
// there), writing the result to the first output parameter.
// GameMessage, UISParam_t and the called functions are named by the mangled
// symbols, the strings are the file's; the CShellMenu, Pause, CDualConfig,
// player and event views, the parameter string view and the message numbers'
// meanings are inferred (members at their offsets, names not original).
extern "C" {
void* memcpy(void*, const void*, unsigned long);
char* strcpy(char*, const char*);
unsigned long strlen(const char*);
}

// The header's prime table (unused here; this file's .rodata starts with it).
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

struct UISParam_t {
    long value;
};

// A string argument: its length and text.
struct UISString_t {
    int unknown0;
    int length;
    char* text;
};

void DebugMsg(const char*, ...);
void AEMS_SendEvent(void*);
void TLT_UnLoadShellBriefingLogBigFile();
void TLT_LoadShellBriefingLogBigFile(int, int);

struct ShellSoundEvent {
    int event;
    int sound;
};

struct PauseSoundEvent {
    int event;
    int type;
    int sound;
    int unknown0c;
    int unknown10;
    int unknown14;
    int unknown18;
    int unknown1c;
    int unknown20;
    int unknown24;
};

enum PauseState {};

class Pause {
public:
    void SetState(PauseState);
    void UnpauseGame();
    bool bIsGamePaused();

    unsigned char unknown00[148];
    bool m_flag94;
    bool m_flag95;
    unsigned char unknown96[2];
};

class CShellMenu {
public:
    bool bIsShellOn();
    void TurnShellOff();
    void ChangeNewGameSignStatus();
    void Set_currentMission(unsigned int);
    void Set_currentStage(unsigned int);
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();
    unsigned char Get_currentControllerSetting();
    void Set_currentControllerSetting(unsigned char);
    unsigned char Get_currentDifficulty();
    void Set_currentDifficulty(unsigned char);
    bool MemoryCardQueryPort(int);
    int MemoryCardIsFormated(int);
    int FadeTheVolume(int);
    bool MemoryCardFormat(int);
    bool MemoryCardArePrefsPresent(int);
    int MemoryCardSaveGameSize();
    int MemoryCardSavePrefsSize();
    int MemoryCardFreeSpace(int);
    void MemoryCardRewindSaveGameList(int);
    int MemoryCardGetNextSaveGame(int, unsigned int*, unsigned int*, char*);
    int MemoryCardSaveGame(int, int, char*);
    int MemoryCardLoadGame(int, unsigned int);
    void* GetCumulativeStats();
    void* GetLevelStats();
    void SetMoviePlaytime();
    int PassMedalStatusToIStudio(int*);
    char* GetWeaponMessage();
    char* GetEvalMessage();
    int PassLevelsCompletedToIStudio(int*);
    int PassLevelsRatingsToIStudio(int*);
    void PlayBriefingMovie(int);
    void PlayRewardMovie(int);
    int PassSecretsToIStudio(int*);
    void SetIndividualSecretsStatus(int, int);
    bool GetGalleryMoviesUnlocked(int*, int*);
    void PlayGalleryMovie(int, int, int, int);
    void ResetShell();
    int PassAudioSettingToIStudio(int*);
    void SetupAudioSettings(int*);
    bool CheckIfValidPassword(char*);
    char* GetWeaponMessageForTotalStats();
    char* GetEvalMessageForTotalStats();
    void NullOutJournalSpritesFromLibrary();
    int PassControllerOptionsSettingToIStudio(int*);
    void SetupControllerOptionsSettings(int*);
    void SetSoundFXValue(int);
    void SetMusicValue(int);
    void ResetAudio();
    void PlayShellSound(int);
    bool GetSKUMode();
    int MemoryCardGetMaxBlocks(int);
    bool MemoryCardDeleteSaveGame(int, unsigned int);
    bool MemoryCardIs8KSectorSize(int);
    bool MemoryCardIsEncodingCorrect(int);
    bool MemoryCardIsAnyObjectInSlot(int);
    int MemoryCardNumFilesFree(int);
    void MemoryCardGetCorruptFileName(char*, int);
    void SetupPlayersAdvancedMultiPlayerSettings(int, int*);
    void PassPlayersAdvancedMultiPlayerSettingToIStudio(int, int*);
    void PassPlayersMultiPlayerName(int, char*);
    void SetupPlayersMultiPlayerName(int, char*);
    void PassPlayersModelAndTeamColorSettingForMultiplayer(int, int*);
    void SetupPlayersModelAndTeamColorSettingForMultiplayer(int, int*);
    int PassPlayersMultiPlayerControllerValue(int);
    void SetupPlayersMultiPlayerControllerValue(int, int);
    int PassMultiPlayerLevelSettingsToIStudio(int*);
    void SetupMultiPlayerLevelSettings(int*);
    void Pass_ActiveWindowsToIStudio(int*);
    void SetActivePlayers(int*);
    void Save_ActiveWindowsFromIStudio(int*);
    void PassMP_StatsToIStudio(int, int*);
    void PassMP_PrefWeaponNameToIStudio(int, char*);
    void PassMP_PrefTargetToIStudio(int, char*);
    void PassMP_PrefHitLocationToIStudio(int, char*);
    void ResetMPPlayersStats();
    int Get_NumPlayers();

    unsigned char unknown0000[12];
    unsigned char m_lastMission;
    unsigned char m_lastStage;
    unsigned char unknown000e;
    unsigned char m_retryAvailable;
    unsigned char unknown0010[520];
    unsigned char m_flag536_0 : 1;
    unsigned char m_flag536_1 : 1;
    unsigned char m_flag536_2 : 1;
    unsigned char m_flag536_3 : 1;
    unsigned char unknown536 : 4;
    unsigned char unknown0219[3];
    int m_language;
    unsigned char unknown0220[52];
    unsigned char m_value596;
    unsigned char unknown0255[4539];
    int m_multiplayer;
    unsigned char unknown1414[8];
    int m_value5148;
    int m_value5152;
    unsigned char unknown1424[1398];
    bool m_moviePlaying;
    bool m_rewardMovie;
    unsigned char unknown199c[8];
    bool m_logLoaded;
    unsigned char unknown19a5[3];
    bool m_flag6568;
    bool m_flag6569;
    unsigned char m_logMission;
    unsigned char m_logStage;
    unsigned char unknown19ac[48];
};

class CDualConfig {
public:
    void SetUpDefaultConfig(int);
    void OnEnter();
    void SetAppliedState(bool);
    bool GetAppliedState();
    void SetupButtonsForMultiplayerGame();

    unsigned char unknown00[13];
    bool m_flag0d;
    unsigned char unknown0e[42];
    int m_player;
    unsigned char unknown3c[4];
    int m_setting;
    unsigned char unknown44[4];
};

class CDeviceManager {
public:
    bool IsReady(int);

    unsigned char unknown00[16];
};

class CPlayerObject {
public:
    unsigned char unknown000[1892];
    int m_controller;
    int m_value1896;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown00[16];
};

struct SecretStatus {
    int unlocked;
    int found;
};

extern Pause g_Pause;
extern CShellMenu g_Shell;
extern CDualConfig g_ControllerConfig;
extern CDeviceManager g_inputMgr;
extern CScene g_scene;
extern unsigned int g_uiGameState;
extern int g_FinalSinglePlayerController;
extern int DisableReset;
extern int g_PauseScreenLockedController;
extern int g_NumPlayers;

#define PARAM_STRING(n) ((UISString_t*)in[n].value)

void GameMessage(unsigned long message, unsigned short, unsigned short, unsigned long count, UISParam_t* in, UISParam_t* out) {
    switch (message) {
    case 1: {
        DebugMsg("PAUSE_QUIT_GAME\n");
        g_Pause.m_flag95 = true;
        PauseSoundEvent event = {7, 3, 0, 0x1000, 0, 0, 2500, 0, 0, 0x7fff};
        AEMS_SendEvent(&event);
        g_uiGameState |= 1;
        g_uiGameState |= 8;
        out->value = 0;
        break;
    }
    case 2: {
        DebugMsg("PAUSE_CONTINUE_GAME\n");
        PauseSoundEvent event = {7, 3, 0, 0x1000, 0, 0, 2500, 0, 0, 0x7fff};
        AEMS_SendEvent(&event);
        g_Pause.UnpauseGame();
        out->value = 0;
        break;
    }
    case 0:
        DebugMsg("GM_KILL_SHELL\n");
        if (g_Shell.bIsShellOn())
            g_Shell.TurnShellOff();
        break;
    case 130:
        DebugMsg("GM_LOAD_MISSION\n");
        g_Shell.ChangeNewGameSignStatus();
        g_Shell.m_multiplayer = 0;
        if (in[0].value == 0) {
            g_Shell.Set_currentMission(1);
            g_Shell.Set_currentStage(1);
        } else {
            g_Shell.Set_currentMission(in[0].value);
            g_Shell.Set_currentStage(in[1].value);
        }
        if (!g_Shell.m_flag536_2)
            g_Shell.m_flag536_2 = 1;
        if (!g_Shell.m_flag536_1)
            g_Shell.m_flag536_1 = 1;
        if (!g_Shell.m_flag536_3)
            g_Shell.m_flag536_3 = 1;
        g_FinalSinglePlayerController = in[2].value - 1;
        out->value = 0;
        break;
    case 50:
        out->value = g_Shell.Get_currentControllerSetting();
        break;
    case 40: {
        int setting = in[0].value;
        g_ControllerConfig.m_setting = setting;
        if (setting < 6) {
            g_ControllerConfig.SetUpDefaultConfig(setting);
            g_Shell.Set_currentControllerSetting(setting);
        }
        if (!g_Shell.m_flag536_1 && g_Shell.m_flag6568)
            g_Shell.m_flag536_1 = 1;
        break;
    }
    case 500: {
        ShellSoundEvent event;
        event.event = 11;
        event.sound = 0;
        AEMS_SendEvent(&event);
        break;
    }
    case 510: {
        ShellSoundEvent event;
        event.event = 11;
        event.sound = 1;
        AEMS_SendEvent(&event);
        break;
    }
    case 520: {
        ShellSoundEvent event;
        event.event = 11;
        event.sound = 2;
        AEMS_SendEvent(&event);
        break;
    }
    case 3:
        DebugMsg("DDAYINTRO_DONE\n");
        g_Pause.SetState((PauseState)6);
        out->value = 0;
        break;
    case 4:
        g_Pause.SetState((PauseState)4);
        DebugMsg("DDAYINTRO_START_ENGINE\n");
        out->value = 0;
        break;
    case 666:
        DebugMsg("DDAYINTRO_FADE_OUT\n");
        g_Pause.SetState((PauseState)5);
        out->value = 0;
        break;
    case 210:
        out->value = g_Shell.MemoryCardQueryPort(in[0].value - 1);
        break;
    case 220:
        out->value = g_Shell.MemoryCardIsFormated(in[0].value - 1);
        DebugMsg("Memory Card slot %d Formatted Query: %d\n", in[0].value - 1, out->value);
        break;
    case 230: {
        int volume = g_Shell.FadeTheVolume(0);
        DebugMsg("Memory Card Format\n");
        out->value = g_Shell.MemoryCardFormat(in[0].value - 1);
        g_Shell.FadeTheVolume(volume);
        break;
    }
    case 240: {
        int slot = 0;
        if (count == 1)
            slot = in[0].value - 1;
        DebugMsg("Memory Card Save Game Size\n");
        if (g_Shell.MemoryCardArePrefsPresent(slot))
            out->value = g_Shell.MemoryCardSaveGameSize();
        else
            out->value = g_Shell.MemoryCardSaveGameSize() + g_Shell.MemoryCardSavePrefsSize();
        break;
    }
    case 250:
        DebugMsg("Memory Card Free Space\n");
        out->value = g_Shell.MemoryCardFreeSpace(in[0].value - 1);
        break;
    case 260:
        DebugMsg("Memory Card Rewind Save List\n");
        g_Shell.MemoryCardRewindSaveGameList(in[0].value - 1);
        break;
    case 270: {
        unsigned int unused;
        DebugMsg("Memory Card Get Next Save Game %d %d\n", in[0].value, in[1].value);
        out->value = g_Shell.MemoryCardGetNextSaveGame(in[0].value - 1, (unsigned int*)in[1].value, &unused, PARAM_STRING(2)->text);
        PARAM_STRING(2)->length = strlen(PARAM_STRING(2)->text);
        if (out->value == 1)
            DebugMsg("Memory Card Get Next Save Game returning %s\n", PARAM_STRING(2)->text);
        else
            DebugMsg("Memory Card Get Next Save Game returning no more games\n");
        break;
    }
    case 280:
        DebugMsg("Memory Card Save Game %d %d %s\n", in[0].value, in[1].value, PARAM_STRING(2)->text);
        DisableReset = 1;
        out->value = g_Shell.MemoryCardSaveGame(in[0].value - 1, in[1].value, PARAM_STRING(2)->text);
        DisableReset = 0;
        break;
    case 285:
        DebugMsg("Memory Card Load Game %d, %d\n", in[0].value - 1, in[1].value);
        out->value = g_Shell.MemoryCardLoadGame(in[0].value - 1, in[1].value);
        break;
    case 150:
        DebugMsg("Getting cumulative player stats for the level\n");
        if (!g_Shell.m_flag6569) {
            void* stats = g_Shell.GetCumulativeStats();
            memcpy((void*)in[0].value, stats, 64);
            out->value = 16;
        }
        break;
    case 160:
        DebugMsg("Getting per level player stats\n");
        if (!g_Shell.m_flag6569) {
            void* stats = g_Shell.GetLevelStats();
            memcpy((void*)in[0].value, stats, 84);
        }
        out->value = 21;
        break;
    case 777:
        out->value = 1;
        break;
    case 400:
        g_Shell.m_moviePlaying = true;
        g_Shell.m_rewardMovie = false;
        g_Shell.SetMoviePlaytime();
        g_Shell.m_flag6568 = false;
        DebugMsg("ENTERING MAIN SCREEN: Resetting Movie Timer\n");
        break;
    case 410:
        g_Shell.m_moviePlaying = true;
        g_Shell.SetMoviePlaytime();
        DebugMsg("Resetting Movie Timer\n");
        break;
    case 420:
        g_Shell.m_moviePlaying = false;
        break;
    case 170:
        out->value = g_Shell.PassMedalStatusToIStudio((int*)in[0].value);
        break;
    case 165:
        DebugMsg("Getting Evaluation messages\n");
        strcpy(PARAM_STRING(0)->text, g_Shell.GetWeaponMessage());
        strcpy(PARAM_STRING(1)->text, g_Shell.GetEvalMessage());
        break;
    case 190:
        out->value = g_Shell.PassLevelsCompletedToIStudio((int*)in[0].value);
        break;
    case 195:
        out->value = g_Shell.PassLevelsRatingsToIStudio((int*)in[0].value);
        break;
    case 350:
        out->value = 1;
        break;
    case 320:
        g_Shell.PlayBriefingMovie(in[0].value);
        break;
    case 365:
        out->value = g_Shell.Get_currentMission();
        break;
    case 360:
        out->value = 1;
        break;
    case 355:
        g_Shell.PlayRewardMovie(in[0].value);
        break;
    case 30:
        out->value = g_Shell.PassSecretsToIStudio((int*)in[0].value);
        break;
    case 20: {
        SecretStatus secrets[10];
        memcpy(secrets, (void*)in[0].value, sizeof(secrets));
        for (int i = 0; i < 10; i++) {
            int secret = i + 1;
            if (secrets[i].unlocked == 0)
                g_Shell.SetIndividualSecretsStatus(secret, 0);
            else if (secrets[i].found > 0)
                g_Shell.SetIndividualSecretsStatus(secret, 2);
            else
                g_Shell.SetIndividualSecretsStatus(secret, 1);
        }
        break;
    }
    case 300:
        out->value = g_Shell.GetGalleryMoviesUnlocked((int*)in[0].value, (int*)in[1].value);
        break;
    case 290:
        g_Shell.PlayGalleryMovie(in[0].value, in[1].value, in[2].value, in[3].value);
        break;
    case 35:
        g_ControllerConfig.m_setting = in[0].value;
        g_ControllerConfig.OnEnter();
        g_ControllerConfig.SetAppliedState(false);
        g_ControllerConfig.m_flag0d = true;
        break;
    case 45:
        if (g_ControllerConfig.GetAppliedState()) {
            out->value = 1;
            g_ControllerConfig.m_flag0d = false;
            g_Shell.m_flag536_1 = 1;
        } else {
            out->value = 0;
        }
        break;
    case 310: {
        unsigned int mission = g_Shell.m_lastMission;
        unsigned int stage = g_Shell.m_lastStage;
        g_Shell.Set_currentMission(mission);
        g_Shell.Set_currentStage(stage);
        memcpy((void*)in[0].value, &mission, 4);
        memcpy((void*)in[1].value, &stage, 4);
        out->value = 1;
        break;
    }
    case 430:
        out->value = g_Shell.m_value596;
        break;
    case 340:
        g_Shell.ResetShell();
        out->value = 1;
        break;
    case 100:
        out->value = g_Shell.PassAudioSettingToIStudio((int*)in[0].value);
        break;
    case 90:
        g_Shell.SetupAudioSettings((int*)in[0].value);
        break;
    case 120:
        out->value = g_Shell.Get_currentDifficulty();
        break;
    case 110:
        g_Shell.Set_currentDifficulty(in[0].value);
        if (!g_Shell.m_flag536_2 && g_Shell.m_flag6568)
            g_Shell.m_flag536_2 = 1;
        break;
    case 10:
        out->value = g_Shell.CheckIfValidPassword(PARAM_STRING(0)->text);
        break;
    case 155:
        DebugMsg("Getting total player stat in the suitcase\n");
        strcpy(PARAM_STRING(0)->text, g_Shell.GetWeaponMessageForTotalStats());
        strcpy(PARAM_STRING(1)->text, g_Shell.GetEvalMessageForTotalStats());
        break;
    case 330: {
        DebugMsg("Getting level to retry again\n");
        unsigned int mission = g_Shell.Get_currentMission();
        unsigned int stage = g_Shell.Get_currentStage();
        memcpy((void*)in[0].value, &mission, 4);
        memcpy((void*)in[1].value, &stage, 4);
        out->value = g_Shell.m_retryAvailable != 0;
        break;
    }
    case 816:
        DebugMsg("Look at me, I'm being activated in the beginning of the Mission Log!\n");
        if (g_Shell.m_logLoaded) {
            DebugMsg("Look at me, I'm unloading the previous Mission Log!\n");
            g_Shell.NullOutJournalSpritesFromLibrary();
            TLT_UnLoadShellBriefingLogBigFile();
            g_Shell.m_logLoaded = false;
        }
        break;
    case 817: {
        int mission = in[0].value;
        int stage = in[1].value;
        if (mission != 6) {
            g_Shell.m_logMission = mission;
            g_Shell.m_logStage = stage;
            DebugMsg("Look at me, I'm trying to load a  Mission Log!\n");
            TLT_LoadShellBriefingLogBigFile(mission, stage);
            g_Shell.m_logLoaded = true;
        }
        break;
    }
    case 5:
        g_Pause.m_flag94 = true;
        break;
    case 6:
        g_Pause.m_flag94 = false;
        break;
    case 80:
        out->value = g_Shell.PassControllerOptionsSettingToIStudio((int*)in[0].value);
        break;
    case 70:
        g_Shell.SetupControllerOptionsSettings((int*)in[0].value);
        break;
    case 394:
        out->value = g_inputMgr.IsReady(in[0].value - 1);
        break;
    case 7:
        if (!g_Shell.bIsShellOn())
            out->value = 1;
        else
            out->value = 1;
        break;
    case 60:
        out->value = g_Shell.m_flag536_0;
        break;
    case 65:
        g_Shell.m_flag536_0 = in[0].value != 0;
        break;
    case 546:
        g_Shell.SetSoundFXValue(in[0].value);
        g_Shell.ResetAudio();
        break;
    case 547:
        g_Shell.SetMusicValue(in[0].value);
        g_Shell.ResetAudio();
        break;
    case 435:
        out->value = g_Shell.m_flag536_2;
        break;
    case 437:
        out->value = g_Shell.m_flag536_1;
        break;
    case 530: {
        int sound = in[0].value;
        if (g_Shell.bIsShellOn()) {
            g_Shell.PlayShellSound(sound);
        } else if (g_Pause.bIsGamePaused()) {
            PauseSoundEvent event = {7, 3, 0, 0x1000, 0, 0, 2500, 0, 0, 0x7fff};
            event.sound = sound;
            AEMS_SendEvent(&event);
        }
        break;
    }
    case 971:
        g_Shell.m_flag6568 = true;
        break;
    case 540:
        switch (g_Shell.m_language) {
        case 1:
            out->value = 1;
            break;
        case 2:
            out->value = 4;
            break;
        case 3:
            out->value = 5;
            break;
        case 4:
            out->value = 2;
            break;
        case 5:
            out->value = 3;
            break;
        default:
            out->value = 1;
            break;
        }
        break;
    case 605:
        out->value = g_Shell.GetSKUMode();
        break;
    case 252:
        out->value = g_Shell.MemoryCardGetMaxBlocks(in[0].value - 1);
        break;
    case 281:
        if (g_Shell.MemoryCardDeleteSaveGame(in[0].value - 1, in[1].value))
            out->value = 1;
        else
            out->value = 2;
        break;
    case 372:
        out->value = g_Shell.MemoryCardIs8KSectorSize(in[0].value - 1);
        break;
    case 374:
        out->value = g_Shell.MemoryCardIsEncodingCorrect(in[0].value - 1);
        break;
    case 376:
        out->value = g_Shell.MemoryCardIsAnyObjectInSlot(in[0].value - 1);
        break;
    case 378: {
        int slot = 0;
        if (count == 1)
            slot = in[0].value - 1;
        if (g_Shell.MemoryCardArePrefsPresent(slot))
            out->value = 1;
        else
            out->value = 2;
        break;
    }
    case 380:
        out->value = g_Shell.MemoryCardNumFilesFree(in[0].value - 1);
        break;
    case 603:
        g_Shell.MemoryCardGetCorruptFileName(PARAM_STRING(0)->text, in[1].value - 1);
        PARAM_STRING(0)->length = strlen(PARAM_STRING(0)->text);
        break;
    case 367:
        g_Shell.SetupPlayersAdvancedMultiPlayerSettings(in[0].value - 1, (int*)in[1].value);
        break;
    case 368:
        g_Shell.PassPlayersAdvancedMultiPlayerSettingToIStudio(in[0].value - 1, (int*)in[1].value);
        break;
    case 382:
        g_Shell.PassPlayersMultiPlayerName(in[0].value - 1, PARAM_STRING(1)->text);
        break;
    case 384:
        g_Shell.SetupPlayersMultiPlayerName(in[0].value - 1, PARAM_STRING(1)->text);
        break;
    case 386:
        g_Shell.PassPlayersModelAndTeamColorSettingForMultiplayer(in[0].value - 1, (int*)in[1].value);
        break;
    case 388:
        g_Shell.SetupPlayersModelAndTeamColorSettingForMultiplayer(in[0].value - 1, (int*)in[1].value);
        break;
    case 390:
        out->value = g_Shell.PassPlayersMultiPlayerControllerValue(in[0].value - 1);
        break;
    case 392: {
        int player = in[0].value;
        int value = in[1].value;
        g_Shell.SetupPlayersMultiPlayerControllerValue(player - 1, value);
        g_ControllerConfig.m_setting = value;
        g_ControllerConfig.m_player = player - 1;
        g_ControllerConfig.SetupButtonsForMultiplayerGame();
        break;
    }
    case 396:
        g_Shell.PassMultiPlayerLevelSettingsToIStudio((int*)in[0].value);
        break;
    case 398:
        g_Shell.SetupMultiPlayerLevelSettings((int*)in[0].value);
        break;
    case 422:
        g_Shell.m_value5148 = in[0].value;
        g_Shell.m_value5152 = in[1].value;
        g_Shell.m_multiplayer = 1;
        break;
    case 412:
    case 418:
        g_Shell.Pass_ActiveWindowsToIStudio((int*)in[0].value);
        break;
    case 416: {
        int* windows = (int*)in[0].value;
        g_Shell.SetActivePlayers(windows);
        g_Shell.Save_ActiveWindowsFromIStudio(windows);
        break;
    }
    case 402:
        g_Shell.PassMP_StatsToIStudio(in[0].value - 1, (int*)in[1].value);
        break;
    case 404:
        g_Shell.PassMP_PrefWeaponNameToIStudio(in[0].value - 1, PARAM_STRING(1)->text);
        break;
    case 406:
        g_Shell.PassMP_PrefTargetToIStudio(in[0].value - 1, PARAM_STRING(1)->text);
        out->value = 1;
        break;
    case 408:
        g_Shell.PassMP_PrefHitLocationToIStudio(in[0].value - 1, PARAM_STRING(1)->text);
        break;
    case 414:
        DebugMsg("GM_MULTI_RESTART_GAME \n");
        g_Shell.ResetMPPlayersStats();
        g_Shell.m_multiplayer = 1;
        g_Shell.Get_NumPlayers();
        if (g_Shell.bIsShellOn())
            g_Shell.TurnShellOff();
        break;
    case 124:
        if (g_Shell.m_multiplayer)
            out->value = g_Shell.Get_NumPlayers();
        else
            out->value = 1;
        break;
    case 1234: {
        CPlayerObject* player = 0;
        if (g_PauseScreenLockedController >= 0 && g_PauseScreenLockedController <= 4) {
            for (int i = 0; i < g_NumPlayers; i++) {
                player = g_scene.GetPlayer(i);
                if (player && g_PauseScreenLockedController == player->m_controller)
                    break;
            }
            out->value = player->m_value1896;
        } else {
            out->value = g_scene.GetPlayer(0)->m_value1896;
        }
        break;
    }
    case 501:
        g_Shell.m_rewardMovie = true;
        break;
    case 503:
        g_Shell.m_rewardMovie = false;
        break;
    case 375:
    case 377:
    case 383:
    case 389:
    case 395:
    case 401:
    case 407:
    case 413:
    case 415:
    case 417:
    case 502:
    case 604:
    default:
        DebugMsg("UNKNOWN GAME MESSAGE\n");
        out->value = 0;
        break;
    }
}
