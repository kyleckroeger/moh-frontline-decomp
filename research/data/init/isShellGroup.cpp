// isShellGroup.cpp: the shell screen group (.data, .sdata): its screen files and
// ShellScreenInfo. The file of ShellScreenInfo is inferred (no file map range covers it;
// it points at szscreen_export). Checked by research/tools/check_data.py.

struct ScreenInfoView { // inferred: the number of screens and their .SCR file names
    int count;
    const char** names;
};

const char* szscreen_export[43] = {
    "data\\MOHFL_exports\\Master_Main.SCR",
    "data\\MOHFL_exports\\Options_Main.SCR",
    "data\\MOHFL_exports\\Gallery_Main.SCR",
    "data\\MOHFL_exports\\PersonalRecords_Main.SCR",
    "data\\MOHFL_exports\\PR_PlayerStats.SCR",
    "data\\MOHFL_exports\\PR_Medals.SCR",
    "data\\MOHFL_exports\\Save_Main.SCR",
    "data\\MOHFL_exports\\opt_Password_Main.SCR",
    "data\\MOHFL_exports\\opt_Audio_Main.SCR",
    "data\\MOHFL_exports\\opt_SecretCodes.SCR",
    "data\\MOHFL_exports\\opt_Difficulty.SCR",
    "data\\MOHFL_exports\\Controller_Main.SCR",
    "data\\MOHFL_exports\\ControllerConfigs.SCR",
    "data\\MOHFL_exports\\CustomController.SCR",
    "data\\MOHFL_exports\\PerformanceEvaluation.SCR",
    "data\\MOHFL_exports\\SaveContinueReplay.SCR",
    "data\\MOHFL_exports\\Load_MEM_Main.SCR",
    "data\\MOHFL_exports\\Save_MEM_Main.SCR",
    "data\\MOHFL_exports\\Briefing.SCR",
    "data\\MOHFL_exports\\Journal1_2.SCR",
    "data\\MOHFL_exports\\Journal1_3.SCR",
    "data\\MOHFL_exports\\Journal2_1.SCR",
    "data\\MOHFL_exports\\Journal2_2.SCR",
    "data\\MOHFL_exports\\Journal2_3.SCR",
    "data\\MOHFL_exports\\Journal3_1.SCR",
    "data\\MOHFL_exports\\Journal3_2.SCR",
    "data\\MOHFL_exports\\Journal3_3.SCR",
    "data\\MOHFL_exports\\Journal4_1.SCR",
    "data\\MOHFL_exports\\Journal4_2.SCR",
    "data\\MOHFL_exports\\Journal4_3.SCR",
    "data\\MOHFL_exports\\Journal1_1.SCR",
    "data\\MOHFL_exports\\Journal5_1.SCR",
    "data\\MOHFL_exports\\Journal5_2.SCR",
    "data\\MOHFL_exports\\Journal5_3.SCR",
    "data\\MOHFL_exports\\Journal5_4.SCR",
    "data\\MOHFL_exports\\RewardMovie.SCR",
    "data\\MOHFL_exports\\Journal1_4.SCR",
    "data\\MOHFL_exports\\Subtitle.SCR",
    "data\\MOHFL_exports\\MissionLog.SCR",
    "data\\MOHFL_exports\\ConfigPick_NewGame.SCR",
    "data\\MOHFL_exports\\Multiplayer_Main.SCR",
    "data\\MOHFL_exports\\MP_WeapLvlSets.SCR",
    "data\\MOHFL_exports\\MP_BattleReport.SCR",
};

ScreenInfoView ShellScreenInfo = {43, szscreen_export};
