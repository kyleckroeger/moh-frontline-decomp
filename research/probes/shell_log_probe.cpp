// settings: config/GMFE69/IStudioFuncs.json
// replaces: ResLoadScreen__FUsUs
//
// Test harness (dol4), not game code: ResLoadScreen from src/game/IStudioFuncs.cpp plus a
// log line naming the shell screen being loaded ("SCREEN group id"). Keep it in the control.
// shell_reset_probe.cpp is this file with SEND_340 defined.

extern "C" void OSReport(const char*, ...);
void* TLT_LoadFileFromShellBigFile(const char*, int*);
struct ScreenInfoView {
    unsigned long count;
    const char** files;
};
extern ScreenInfoView* ScreenInfo;

void* ResLoadScreen(unsigned short screen, unsigned short resolution) {
    OSReport("SCREEN %d %d\n", screen, resolution);
    void* data = TLT_LoadFileFromShellBigFile(ScreenInfo[screen].files[resolution], 0);
    if (data)
        return data;
    return 0;
}

// replaces: UISRegisterMessageFnc
// The studio's message callback (GameMessage) is registered through a wrapper that logs
// every shell message number ("UISMSG n") and then calls GameMessage unchanged.
union UISParam_t;
int GameMessage(unsigned long, unsigned short, unsigned short, unsigned long, UISParam_t*, UISParam_t*);
typedef int (*UISMessageFnc)(unsigned long, unsigned short, unsigned short, unsigned long, UISParam_t*, UISParam_t*);
struct UISInfoMsgView {
    char field00[12];
    UISMessageFnc message;
};

static int LoggedGameMessage(unsigned long msg, unsigned short a, unsigned short b, unsigned long n, UISParam_t* in,
                             UISParam_t* out) {
    OSReport("UISMSG %lu %u %u n %lu p0 %ld\n", msg, a, b, n, n ? *(long*)in : 0);
#ifdef SEND_340
    // dol4: the script's own sender (missionlog.scr, control 217's select handler, sub-choice 0)
    // is not reached by the routes; send message 340 once when the Mission Log screen asks for
    // its records (message 140), as that handler would.
    static int sent;
    if (msg == 140 && !sent) {
        long result[4];
        sent = 1;
        OSReport("SEND 340\n");
        GameMessage(340, a, b, 0, 0, (UISParam_t*)result);
    }
#endif
    return GameMessage(msg, a, b, n, in, out);
}

extern "C" void UISRegisterMessageFnc(UISInfoMsgView* info, UISMessageFnc) {
    info->message = LoggedGameMessage;
}
