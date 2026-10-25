// A fragment of ipad.cpp (0x8014eef8); the file name is this project's. The
// REAL pad driver's sampling callback iPADVBLUpdate: each controller's status
// is read and clamped, and copied into vgPadStatus unless the read failed
// (error -3). iPAD_init (once) clears both status arrays, marks every
// controller as not connected (error -1), installs the callback and sets the
// sampling rate. PADStatus is Dolphin's 12-byte pad status (as in pad.cpp);
// the functions and globals are named by the symbols. vgPadStatus is written
// by the sampling callback, so it is volatile (the original keeps its stores
// in order).
struct PADStatus {
    unsigned short button;
    signed char stickX;
    signed char stickY;
    signed char substickX;
    signed char substickY;
    unsigned char triggerLeft;
    unsigned char triggerRight;
    unsigned char analogA;
    unsigned char analogB;
    signed char err;
};

typedef void (*PADSamplingCallback)(void);

extern "C" {
PADSamplingCallback PADSetSamplingCallback(PADSamplingCallback);
void PADRead(PADStatus*);
void PADClamp(PADStatus*);
void* MEM_copy(void*, const void*, int);
void* MEM_fill(void*, int, int);
void SISetSamplingRate(unsigned long);
}

extern volatile PADStatus vgPadStatus[4];
extern PADStatus gPadStatus[4];
extern int giPADStarted;
extern PADSamplingCallback OldCallBack;

void iPADVBLUpdate(void) {
    PADStatus status[4];
    int i;

    PADRead(status);
    PADClamp(status);
    for (i = 0; i < 4; i++) {
        if (status[i].err != -3)
            MEM_copy((PADStatus*)&vgPadStatus[i], &status[i], sizeof(PADStatus));
    }
}

int iPAD_init(void) {
    if (!giPADStarted) {
        giPADStarted = 1;
        MEM_fill((PADStatus*)vgPadStatus, 0, sizeof(vgPadStatus));
        MEM_fill(gPadStatus, 0, sizeof(gPadStatus));
        for (int i = 0; i < 4; i++) {
            vgPadStatus[i].err = -1;
            gPadStatus[i].err = -1;
        }
        OldCallBack = PADSetSamplingCallback(iPADVBLUpdate);
        SISetSamplingRate(0);
    }
    return 1;
}
