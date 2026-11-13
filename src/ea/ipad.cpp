// REAL pad driver: the sampling callback iPADVBLUpdate reads and clamps each
// controller's status and copies it into vgPadStatus unless the read failed
// (error -3); iPAD_init (once) clears both status arrays, marks every
// controller as not connected (error -1), installs the callback and sets the
// sampling rate; iPAD_update copies the sampled vgPadStatus to gPadStatus under
// disabled interrupts, then probes each port (standard controller or WaveBird),
// marks newly connected controllers not ready and resets them, and marks other
// ports as having no controller. PADStatus is Dolphin's 12-byte pad status (as
// in pad.cpp); the functions and globals are named by the symbols. vgPadStatus
// is written by the sampling callback, so it is volatile (the original keeps
// its stores in order).
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
int PADReset(unsigned long);
unsigned long SIProbe(int);
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
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

void iPAD_update(void) {
    int i;
    unsigned long bit;
    unsigned long reset;
    int level;

    level = OSDisableInterrupts();
    MEM_copy(gPadStatus, (PADStatus*)vgPadStatus, sizeof(gPadStatus));
    OSRestoreInterrupts(level);
    reset = 0;
    for (i = 0; i < 4; i++) {
        bit = 0x80000000 >> i;
        switch ((int)SIProbe(i)) {
        case 0x09000000:
        case (int)0x8B100000:
            if (gPadStatus[i].err == -1) {
                vgPadStatus[i].err = -2;
                reset |= bit;
            }
            break;
        default:
            vgPadStatus[i].err = -1;
            break;
        }
    }
    if (reset)
        PADReset(reset);
}
