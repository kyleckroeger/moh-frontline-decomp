// ispauselibrary.cpp: the pause screen's image libraries (.data, .sdata), as the screen
// tool exported them: for each library its file names and a slot per file for the loaded
// object (zero until loaded), and PauseLibraryInfo, which lists the libraries.
// Checked by research/tools/check_data.py.

struct LibraryInfoView { // inferred: count, then names and objects (src/game/TibText.cpp's view)
    int count;
    const char** names;
    long* objects;
};

const char* szPauseLibfonts[3] = {
    "data\\pausescreen\\DDayIntro.gfn",
    "data\\pausescreen\\PauseScreenFont.gfn",
    "data\\pausescreen\\PopupDisplay.gfn",
};
long iPausefonts[3] = {0};

const char* szPauseLibDDay[8] = {
    "data\\pausescreen\\DDayIntro_Line1.gsh",
    "data\\pausescreen\\DDayIntro_Line2.gsh",
    "data\\pausescreen\\DDayIntro_Line3.gsh",
    "data\\pausescreen\\DDayIntro_Line4.gsh",
    "data\\pausescreen\\DDayIntro_Line5.gsh",
    "data\\pausescreen\\DDayIntro_Line6.gsh",
    "data\\pausescreen\\DDayIntro_Line7.gsh",
    "data\\pausescreen\\BlkBkgrnd2.gsh",
};
long iPauseDDay[8] = {0};

const char* szPauseLibPause[2] = {
    "data\\pausescreen\\BlkBkgrnd.gsh",
    "data\\pausescreen\\Pause.gsh",
};
long iPausePause[2] = {0};

const char* szPauseLibbuttons_controller[15] = {
    "data\\pausescreen\\DPad_Up.gsh",
    "data\\pausescreen\\DPad_Right.gsh",
    "data\\pausescreen\\DPad_Down.gsh",
    "data\\pausescreen\\DPad_Left.gsh",
    "data\\pausescreen\\L1_button.gsh",
    "data\\pausescreen\\L2_button.gsh",
    "data\\pausescreen\\R1_button.gsh",
    "data\\pausescreen\\R2_button.gsh",
    "data\\pausescreen\\X_button.gsh",
    "data\\pausescreen\\Tri_button.gsh",
    "data\\pausescreen\\DPad_blank.gsh",
    "data\\pausescreen\\DPad_UpHL.gsh",
    "data\\pausescreen\\DPad_RightHL.gsh",
    "data\\pausescreen\\DPad_DownHL.gsh",
    "data\\pausescreen\\DPad_LeftHL.gsh",
};
long iPausebuttons_controller[15] = {0};

LibraryInfoView PauseLibraryInfo[4] = {
    {3, szPauseLibfonts, iPausefonts},
    {8, szPauseLibDDay, iPauseDDay},
    {2, szPauseLibPause, iPausePause},
    {15, szPauseLibbuttons_controller, iPausebuttons_controller},
};
