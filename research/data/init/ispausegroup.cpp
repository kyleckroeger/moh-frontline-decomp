// ispausegroup.cpp: the pause screen group (.sdata): its screen files and PauseScreenInfo.
// The file of PauseScreenInfo is inferred (no file map range covers it; it follows
// szscreen_pauseexport in .sdata and points at it). Checked by research/tools/check_data.py.

struct ScreenInfoView { // inferred: the number of screens and their .SCR file names
    int count;
    const char** names;
};

const char* szscreen_pauseexport[2] = {
    "data\\pausescreen\\DDayIntro.SCR",
    "data\\pausescreen\\PauseScr.SCR",
};

ScreenInfoView PauseScreenInfo = {2, szscreen_pauseexport};
