// pumhandler.cpp: its initialised globals (.sdata): pop-up messages per section and the
// multiplayer pop-up text scales for two and four players. All are file statics in the
// original, defined global here because the file's fragments reach them with extern.
// Checked by research/tools/check_data.py.

int POPUPS_IN_SECTION = 4; // file-local in the original
float MP_PUM_TEXT_SCALE_X_2P = 1.0f; // file-local in the original
float MP_PUM_TEXT_SCALE_Y_2P = 1.0f; // file-local in the original
float MP_PUM_TEXT_SCALE_X_4P = 0.9f; // file-local in the original
float MP_PUM_TEXT_SCALE_Y_4P = 0.9f; // file-local in the original
