// A fragment of hwvideodisplaylist.cpp (0x80102cf4): the weak class operator
// delete of GCHW_VD, which frees through the RCMP system's free hook, and the
// weak empty RCMP::VIDEO_DRIVER::CompleteDraw. RCMP, RCMP_SYSTEM, rcmp_sys,
// GCHW_VD and VIDEO_DRIVER are named by the symbols; the hook slot is as in
// rcmp_mpc_codec.cpp. The delete and CompleteDraw are inline in the original
// (weak symbols), so they are defined __declspec(weak). The rest of the file
// is not part of this unit.
namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
};

extern RCMP_SYSTEM rcmp_sys;

class VIDEO_DRIVER {
public:
    void CompleteDraw();
};
}

class GCHW_VD {
public:
    void operator delete(void*);
};

__declspec(weak) void GCHW_VD::operator delete(void* p) {
    RCMP::rcmp_sys.m_free(p);
}

__declspec(weak) void RCMP::VIDEO_DRIVER::CompleteDraw() {
}
