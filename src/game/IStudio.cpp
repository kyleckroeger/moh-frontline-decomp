// IStudio wrappers over the UI studio: the per-frame update (event processing,
// then the studio's idle pass) and draw pass (identity matrix, then the
// studio's objects), activating a screen and loading one (with no extra
// parameters), preceded by iSirenShutDown (shuts the studio down, clears its
// memory, sized by UISGetMemSize from the stored parameters, frees it and
// clears the parameters). IStudio and UISInfo_t are named by the symbols;
// the studio record pointer at the start of the object and the memory
// parameters after it (passed to UISGetMemSize; meanings unknown) are inferred and the
// UIS functions' parameter types beyond the ids are left generic. The rest of
// the file is not part of this unit.
struct UISInfo_t;

extern "C" {
int UISSetScreenActive(UISInfo_t*, unsigned short, unsigned short);
int UISLoadScreen(UISInfo_t*, unsigned short, unsigned short, int, int);
void UISIdleProcess(UISInfo_t*, unsigned long);
void UISDrawObjects(UISInfo_t*, unsigned long);
void UISShutdown(UISInfo_t*);
unsigned long UISGetMemSize(int, int, int, int, int, int);
void* memset(void*, int, unsigned long);
void MEM_free(void*);
}

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

    UISInfo_t* m_info;
    int m_memParam04;
    int m_memParam08;
    int m_memParam0c;
    int m_memParam10;
    int m_field14;
};

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
