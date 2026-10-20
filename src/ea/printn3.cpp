// REAL print output devices: the device table (an empty first slot, then the
// print console and the debugger) and the output function both devices use,
// which hands the text to OSReport. PRINTCHANNEL is named by the mangled
// symbol; the device record is an inferred view (name, output function,
// enable bit).
enum PRINTCHANNEL {};

struct PrintDeviceView {
    const char* name;
    void (*print)(PRINTCHANNEL, const char*);
    unsigned char enabled : 1;
    unsigned char flags : 7;
    unsigned char unknown09[3];
};

extern "C" void OSReport(const char*, ...);

static void PRINT_debuggerfunc(PRINTCHANNEL, const char* text) {
    OSReport("%s", text);
}

PrintDeviceView PRINTdevicelist[8] = {
    {""},
    {"print console", PRINT_debuggerfunc},
    {"debugger", PRINT_debuggerfunc},
};
