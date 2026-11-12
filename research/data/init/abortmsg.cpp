// abortmsg.cpp: its initialised data (.sdata): REALabortmessage, the abort hook, set to
// SYSTEM_abortmessage. The file is inferred: no file map range covers it, but the hook
// points at this file's function and EA's libraries define each hook beside its function
// (src/ea/saemsmbf.c's SNDAEMS_asyncloadmodulebank). Checked by research/tools/check_data.py.
// settings: config/GMFE69/abortmsg.json

extern "C" void SYSTEM_abortmessage(const char* format, ...);

void (*REALabortmessage)(const char*, ...) = SYSTEM_abortmessage;
