// ShellMenu.cpp: its initialised globals (.sdata): the debug configuration file's name
// and the string returned for an empty text. g_ConfigFilename is a file static in the
// original, defined global here because the file's fragments reach it with extern.
// Checked by research/tools/check_data.py.

const char* g_ConfigFilename = "debug_REMOVE_FROM_FINAL.INI"; // file-local in the original
char* emptyString = " ";
