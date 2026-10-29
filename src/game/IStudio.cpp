// IStudio wrappers over the UI studio: iSirenEventProcessFnc (for each of
// the four controllers, or only the pause-locked one, turns the sixteen
// button inputs 121..136 into studio events 0..15 with value 1 and the four
// analog inputs 137..140 into events 16..19 with their nonzero value), then
// iSirenShutDown (shuts the studio down, clears its memory, sized by
// UISGetMemSize from the stored parameters, frees it and clears the
// parameters), the per-frame update (event processing, then the studio's
// idle pass) and draw pass (identity matrix, then the studio's objects),
// activating a screen and loading one (with no extra parameters), and
// ISirenStartup (stores the five memory parameters, allocates the studio
// memory sized by UISGetMemSize, initialises the studio and registers the
// resource, transform, message and plugin callbacks). IStudio
// and UISInfo_t are named by the symbols; the studio record pointer at the
// start of the object and the memory parameters after it (passed to
// UISGetMemSize; meanings unknown) are inferred, and the UIS functions'
// parameter types beyond the ids and the event arguments' meanings are left
// generic. The rest of the file is not part of this unit.
struct UISInfo_t;
struct UISTransform_t;
struct UISParam_t;
enum UISTransformAction {};

void* DWI_alloc(const char*, int, int);
int ResLoadScreen(unsigned short, unsigned short);
int ResUnloadScreen(unsigned short, unsigned short, void*);
int iSirenTransformfnc(UISTransformAction, UISTransform_t*);
int GameMessage(unsigned long, unsigned short, unsigned short, unsigned long, UISParam_t*, UISParam_t*);
int TibPolyProcessFnc(void*, unsigned long, unsigned long, UISParam_t*, UISParam_t*);
int TibTextProcessFnc(void*, unsigned long, unsigned long, UISParam_t*, UISParam_t*);
int DummyPluginProcessFnc(void*, unsigned long, unsigned long, UISParam_t*, UISParam_t*);

extern "C" {
int UISSetScreenActive(UISInfo_t*, unsigned short, unsigned short);
int UISLoadScreen(UISInfo_t*, unsigned short, unsigned short, int, int);
void UISIdleProcess(UISInfo_t*, unsigned long);
void UISDrawObjects(UISInfo_t*, unsigned long);
void UISShutdown(UISInfo_t*);
unsigned long UISGetMemSize(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
void UISInit(UISInfo_t*, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
void UISRegisterResourceFncs(UISInfo_t*, int (*)(unsigned short, unsigned short),
                             int (*)(unsigned short, unsigned short, void*));
void UISRegisterTransformFncs(UISInfo_t*, int (*)(UISTransformAction, UISTransform_t*));
void UISRegisterMessageFnc(UISInfo_t*,
                           int (*)(unsigned long, unsigned short, unsigned short, unsigned long, UISParam_t*, UISParam_t*));
void UISRegisterPluginFnc(UISInfo_t*, int, int (*)(void*, unsigned long, unsigned long, UISParam_t*, UISParam_t*));
void* memset(void*, int, unsigned long);
void MEM_free(void*);
int UISProcessEvent(UISInfo_t*, unsigned long, int, int, float*, int);
}

class CInputManager {
public:
    unsigned char unknown00[64];
    long GetValue(unsigned long, unsigned long);
};

extern CInputManager g_inputMgr;
extern int g_PauseScreenLockedController;

class CMatrixStack {
public:
    void Ident();
};

extern CMatrixStack* g_matStack;

class IStudio {
public:
    void iSirenUpdate(unsigned long);
    void iSirenDrawProcessFnc(unsigned long);
    void iSirenEventProcessFnc();
    void iSirenActivateScreen(unsigned short, unsigned short);
    void iSirenLoadScreen(unsigned short, unsigned short);
    void iSirenShutDown();
    void ISirenStartup(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);

    UISInfo_t* m_info;
    int m_memParam04;
    int m_memParam08;
    int m_memParam0c;
    int m_memParam10;
    int m_field14;
};

void IStudio::iSirenEventProcessFnc() {
    float value;
    for (int i = 0; i < 4; i++) {
        if (g_PauseScreenLockedController != -1 && i != g_PauseScreenLockedController)
            continue;
        if (g_inputMgr.GetValue(i, 121)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 0, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 122)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 1, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 123)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 2, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 124)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 3, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 125)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 4, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 126)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 5, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 127)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 6, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 128)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 7, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 129)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 8, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 130)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 9, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 131)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 10, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 132)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 11, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 133)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 12, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 134)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 13, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 135)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 14, 1, &value, 0);
        }
        if (g_inputMgr.GetValue(i, 136)) {
            value = 1.0f;
            UISProcessEvent(m_info, i + 1, 15, 1, &value, 0);
        }
        {
            float axis = g_inputMgr.GetValue(i, 137);
            if (axis) {
                value = axis;
                UISProcessEvent(m_info, i + 1, 16, 1, &value, 0);
            }
        }
        {
            float axis = g_inputMgr.GetValue(i, 138);
            if (axis) {
                value = axis;
                UISProcessEvent(m_info, i + 1, 17, 1, &value, 0);
            }
        }
        {
            float axis = g_inputMgr.GetValue(i, 139);
            if (axis) {
                value = axis;
                UISProcessEvent(m_info, i + 1, 18, 1, &value, 0);
            }
        }
        {
            float axis = g_inputMgr.GetValue(i, 140);
            if (axis) {
                value = axis;
                UISProcessEvent(m_info, i + 1, 19, 1, &value, 0);
            }
        }
    }
}

void IStudio::iSirenShutDown() {
    UISShutdown(m_info);
    unsigned long size = UISGetMemSize(m_memParam04, m_memParam08, m_memParam0c, m_memParam04, m_memParam10, m_memParam10);
    memset(m_info, 0, size);
    MEM_free(m_info);
    m_memParam04 = 0;
    m_memParam08 = 0;
    m_memParam0c = 0;
    m_memParam10 = 0;
    m_field14 = 0;
}

void IStudio::iSirenUpdate(unsigned long time) {
    iSirenEventProcessFnc();
    UISIdleProcess(m_info, time);
}

void IStudio::iSirenDrawProcessFnc(unsigned long time) {
    g_matStack->Ident();
    UISDrawObjects(m_info, time);
}

void IStudio::iSirenActivateScreen(unsigned short group, unsigned short id) {
    UISSetScreenActive(m_info, group, id);
}

void IStudio::iSirenLoadScreen(unsigned short group, unsigned short id) {
    UISLoadScreen(m_info, group, id, 0, 0);
}

void IStudio::ISirenStartup(unsigned long p1, unsigned long p2, unsigned long p3, unsigned long p4,
                            unsigned long p5) {
    m_memParam04 = p1;
    m_memParam08 = p2;
    m_memParam0c = p3;
    m_memParam10 = p4;
    m_field14 = p5;
    unsigned long size = UISGetMemSize(p1, p2, p3, p1, p4, p4);
    m_info = (UISInfo_t*)DWI_alloc("ISiren memory", size, 1024);
    UISInit(m_info, p1, p2, p3, p1, p4, p4, p5);
    UISRegisterResourceFncs(m_info, ResLoadScreen, ResUnloadScreen);
    UISRegisterTransformFncs(m_info, iSirenTransformfnc);
    UISRegisterMessageFnc(m_info, GameMessage);
    UISRegisterPluginFnc(m_info, 0, TibPolyProcessFnc);
    UISRegisterPluginFnc(m_info, 7, TibTextProcessFnc);
    UISRegisterPluginFnc(m_info, 1, DummyPluginProcessFnc);
    UISRegisterPluginFnc(m_info, 2, DummyPluginProcessFnc);
    UISRegisterPluginFnc(m_info, 3, DummyPluginProcessFnc);
    UISRegisterPluginFnc(m_info, 4, DummyPluginProcessFnc);
    UISRegisterPluginFnc(m_info, 5, DummyPluginProcessFnc);
    UISRegisterPluginFnc(m_info, 6, DummyPluginProcessFnc);
}
