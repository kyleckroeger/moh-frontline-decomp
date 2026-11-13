// printstr.cpp, the whole file: REAL print channels. PRINT_movechannel, a channel's
// enable switch. Channel 0 switches all 64 channels and channel 1 every
// channel marked as grouped; any other channel switches only itself. The
// function is file-local. PRINTCHANNEL and the functions are named by the
// mangled symbols; the channel record is an inferred view (8 bytes, the enable
// and grouped bits at +4). Then PRINT_init: on the first call every device
// and channel is disabled, the devices from 4 and the channels from 14 are
// cleared, and channel 2 and device 1 are enabled. The device record is an
// inferred view (12 bytes, the print callback at +4, the enable bit at +8).
// Last, formatting a message for a channel and passing it to each enabled
// output device, the C and C++ variadic entry points (the latter on channel
// 2), and the device and channel enable switches (which initialise the print
// system first). The va_list tag is named by the mangled symbols (va_list is
// MSL's std::__tag_va_List array). The channel table and the device list are
// defined with the file's data (research/data/init/printstr.cpp, unit
// printn3). PRINT_movechannel and PRINT_init are called, not inlined, in the
// original: '#pragma auto_inline off' around their definitions reproduces that.
namespace std {
struct __tag_va_List {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
};
typedef __tag_va_List va_list[1];
}

extern "C" int vsprintf(char*, const char*, std::va_list);

enum PRINTCHANNEL {};

struct PrintChannelView {
    void* unknown00;
    unsigned char enabled : 1;
    unsigned char grouped : 1;
    unsigned char flags : 6;
    unsigned char unknown05[3];
};

struct PrintDeviceView {
    void* unknown00;
    void (*print)(PRINTCHANNEL, const char*);
    unsigned char enabled : 1;
    unsigned char flags : 7;
    unsigned char unknown09[3];
};

extern PrintChannelView PRINTchannellist[];
extern PrintDeviceView PRINTdevicelist[];
extern int PRINTinitialized;

extern "C" void PRINT_setdevicestate(int device, int state);
extern "C" void PRINT_setchannelstate(PRINTCHANNEL channel, int state);

#pragma auto_inline off
static void PRINT_movechannel(PRINTCHANNEL channel, int state) {
    int i;

    if (channel == 0) {
        for (i = 0; i < 64; i++)
            PRINTchannellist[i].enabled = state ? 1 : 0;
        return;
    }
    if (channel == 1) {
        for (i = 0; i < 64; i++) {
            if (PRINTchannellist[i].grouped)
                PRINTchannellist[i].enabled = state ? 1 : 0;
        }
        return;
    }
    PRINTchannellist[channel].enabled = state != 0;
}
#pragma auto_inline reset

#pragma auto_inline off
extern "C" void PRINT_init(void) {
    int i;

    if (!PRINTinitialized) {
        PRINTinitialized = 1;
        for (i = 0; i < 8; i++)
            PRINTdevicelist[i].enabled = 0;
        for (i = 0; i < 64; i++)
            PRINTchannellist[i].enabled = 0;
        for (i = 4; i < 8; i++) {
            PRINTdevicelist[i].unknown00 = 0;
            PRINTdevicelist[i].print = 0;
        }
        for (i = 14; i < 64; i++) {
            PRINTchannellist[i].unknown00 = 0;
            PRINTchannellist[i].grouped = 0;
        }
        PRINT_setchannelstate((PRINTCHANNEL)2, 1);
        PRINT_setdevicestate(1, 1);
    }
}
#pragma auto_inline reset

static void PRINT_vstring(PRINTCHANNEL channel, const char* format, std::va_list args) {
    char buffer[8192];

    if (!PRINTinitialized)
        PRINT_init();
    if (PRINTchannellist[channel].enabled) {
        vsprintf(buffer, format, args);
        for (int i = 0; i < 8; i++) {
            if (PRINTdevicelist[i].enabled && PRINTdevicelist[i].print)
                PRINTdevicelist[i].print(channel, buffer);
        }
    }
}

extern "C" void PRINT_string(PRINTCHANNEL channel, const char* format, ...) {
    std::va_list args;

    __builtin_va_info(&args);
    PRINT_vstring(channel, format, args);
}

void PRINT_string(const char* format, ...) {
    std::va_list args;

    __builtin_va_info(&args);
    PRINT_vstring((PRINTCHANNEL)2, format, args);
}

extern "C" void PRINT_setdevicestate(int device, int state) {
    if (PRINTdevicelist[device].print) {
        if (!PRINTinitialized)
            PRINT_init();
        PRINTdevicelist[device].enabled = state;
    }
}

extern "C" void PRINT_setchannelstate(PRINTCHANNEL channel, int state) {
    if (!PRINTinitialized)
        PRINT_init();
    PRINT_movechannel(channel, state);
}
