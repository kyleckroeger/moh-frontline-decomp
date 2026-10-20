// A fragment of printstr.cpp (0x80151234): PRINT_movechannel, a channel's
// enable switch. Channel 0 switches all 64 channels and channel 1 every
// channel marked as grouped; any other channel switches only itself. The file
// name is this project's; the original record is printstr.cpp. PRINTCHANNEL
// and the function are named by the mangled symbols; the channel record is an
// inferred view (8 bytes, the enable and grouped bits at +4). The function is
// file-local in the original. PRINT_init follows it and is not part of this
// unit.
enum PRINTCHANNEL {};

struct PrintChannelView {
    void* unknown00;
    unsigned char enabled : 1;
    unsigned char grouped : 1;
    unsigned char flags : 6;
    unsigned char unknown05[3];
};

extern PrintChannelView PRINTchannellist[];

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
