// renderlist.cpp: its initialised globals (.sdata). g_bEmptyList and g_bClearRenderList
// have no file map range; the file is inferred: the original places them in .sdata just
// before g_ClearValue and only the renderlist units use them.
// Checked by research/tools/check_data.py.

bool g_bEmptyList = true; // file inferred
bool g_bClearRenderList = true; // file inferred
unsigned int g_ClearValue = 0xbeefcace; // file-local in the original
