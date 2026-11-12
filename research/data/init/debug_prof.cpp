// debug_prof.cpp: its initialised globals (.sdata). The file is inferred: no file map
// range covers them, but the original places them in .sdata just before debug_prof.cpp's
// RTTI records and only the debug_prof units and drafts use them.
// Checked by research/tools/check_data.py.

bool g_bDebugStatsDisabled = true; // file inferred
bool g_bProfileUpdateHistory = true; // file inferred
