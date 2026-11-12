// exit.cpp (file per build/audit/file_map.json): its initialised data (.data): the
// timer and vertical-blank callback slots (src/ea/inittmr.cpp, initvblt.cpp), all empty.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/exit.json

void (*vbltmrsub[8])(void) = {0};
void (*tmrsub[8])(void) = {0};
