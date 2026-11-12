// printstr.cpp: its initialised data (.data): PRINTchannellist, the print channels by
// number, 16 named and the rest of the 64 empty. Each has a name, an enabled bit
// (set for "all" and "alert") and a second bit set for every named channel; the layout
// is inferred from src/ea/printstr.cpp's and printstr_movechannel.cpp's views. A file static
// in the original, defined global here because the file's fragments reach it with extern.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/printstr.json

struct PrintChannelView { // inferred
    const char* name;
    unsigned char enabled : 1;
    unsigned char grouped : 1;
    unsigned char flags : 6;
};

PrintChannelView PRINTchannellist[64] = { // file-local in the original
    {"all", 1, 1},
    {"real", 0, 1},
    {"alert", 1, 1},
    {"general", 0, 1},
    {"memory", 0, 1},
    {"connection", 0, 1},
    {"graphics", 0, 1},
    {"file", 0, 1},
    {"input", 0, 1},
    {"system", 0, 1},
    {"3d", 0, 1},
    {"font", 0, 1},
    {"codec", 0, 1},
    {"math", 0, 1},
    {"eagl", 0, 1},
    {"capture", 0, 1},
};
