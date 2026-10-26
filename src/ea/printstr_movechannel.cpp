// A fragment of printstr.cpp (0x80151234): PRINT_movechannel, a channel's
// enable switch. Channel 0 switches all 64 channels and channel 1 every
// channel marked as grouped; any other channel switches only itself. The file
// name is this project's; the original record is printstr.cpp. PRINTCHANNEL
// and the function are named by the mangled symbols; the channel record is an
// inferred view (8 bytes, the enable and grouped bits at +4). The function is
// file-local in the original. Then PRINT_init: on the first call every device
// and channel is disabled, the devices from 4 and the channels from 14 are
// cleared, and channel 2 and device 1 are enabled. The device record is an
// inferred view (12 bytes, the print callback at +4, the enable bit at +8).
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
