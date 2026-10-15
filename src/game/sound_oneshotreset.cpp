// A fragment of sound.cpp (0x800ec834): ResetOneShotBank waits (two vsyncs
// at a time, updating the REAL streams and polling the asynchronous load)
// while the one-shot bank is loading, then, when it is loaded, removes it and
// clears its id, handle, load state and memory offset. The functions and
// globals are named by the symbols; the load-state values (1 loading,
// 2 loaded), the CScreen view (sized by g_screen) and the bank function
// pointer's type (as in sound_init.cpp) are inferred. The bank id and handle
// are file-local in the original. The rest of the file is in other units.
void DebugMsg(const char*, ...);
void UpdateREAL();
extern "C" int SNDAEMS_asyncloadmodulebankdone(void);
extern int (*SNDAEMS_removemodulebank)(int);

class CScreen {
public:
    unsigned long GetVSyncs();

    unsigned char unknown000[0x110];
};

extern CScreen g_screen;

static int g_AEMSOneShotBankID = -1;
static int g_AEMSOneShotBankHandle = -1;
extern int g_AEMSOneShotMemOffset;
extern int g_AEMSOneShotLoadState;

void ResetOneShotBank() {
    while (g_AEMSOneShotLoadState == 1) {
        DebugMsg("Waiting for one shot sound bank to finish loading...\n");
        int start = g_screen.GetVSyncs();
        while ((int)g_screen.GetVSyncs() - start < 2) {
        }
        UpdateREAL();
        if (g_AEMSOneShotLoadState == 1 && SNDAEMS_asyncloadmodulebankdone() > 0)
            g_AEMSOneShotLoadState = 2;
    }
    if (g_AEMSOneShotLoadState == 2) {
        SNDAEMS_removemodulebank(g_AEMSOneShotBankHandle);
        g_AEMSOneShotBankID = -1;
        g_AEMSOneShotBankHandle = -1;
        g_AEMSOneShotLoadState = 0;
        g_AEMSOneShotMemOffset = 0;
    }
}
