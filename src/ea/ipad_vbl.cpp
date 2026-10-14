// A fragment of ipad.cpp (0x8014eef8); the file name is this project's. The
// REAL pad driver's sampling callback iPADVBLUpdate: each controller's status
// is read and clamped, and copied into vgPadStatus unless the read failed
// (error -3). PADStatus is Dolphin's 12-byte pad status (as in pad.cpp); the
// function and globals are named by the symbols. iPAD_init after it is not
// part of this unit.
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

extern "C" {
void PADRead(PADStatus*);
void PADClamp(PADStatus*);
void* MEM_copy(void*, const void*, int);
}

extern PADStatus vgPadStatus[4];

void iPADVBLUpdate(void) {
    PADStatus status[4];
    int i;

    PADRead(status);
    PADClamp(status);
    for (i = 0; i < 4; i++) {
        if (status[i].err != -3)
            MEM_copy(&vgPadStatus[i], &status[i], sizeof(PADStatus));
    }
}
