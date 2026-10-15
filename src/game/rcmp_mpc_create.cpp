// A fragment of rcmp_mpc_codec.cpp (0x801042f0): RCMP::MPC_CODEC_create
// makes the MPEG codec (allocated through the RCMP system's allocator with an
// empty name; the constructor empties both frame lists and sets the word at
// +0x1c), then the weak RCMP::CODEC destructor (freed through the RCMP
// system). The classes and functions are named by the mangled symbols; the
// RCMP_SYSTEM view, the codec's members and the inline constructors and
// operator new are inferred (the views follow rcmp_mpc_codec.cpp and
// rcmp_mpc_frames.cpp). MPC_CODEC_INTERNAL declares its destructor (defined
// elsewhere) first so its table stays elsewhere; CODEC's weak table is a
// duplicate of the copy linked from rcmp_mpc_codec.cpp; this unit is built
// without RTTI because CODEC's RTTI records belong to that unit. The string is an
// entry of the file's .sdata2 pool. The rest of the file is in other units.
namespace RCMP {
class FRAME;
class DECODER;
class CHUNK;

class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, int, int, int, int); /* inferred from the calls */
    void (*m_free)(void*);
    int m_heap;
};

extern RCMP_SYSTEM rcmp_sys;

class CODEC {
public:
    CODEC() {}
    virtual ~CODEC() {}
    virtual int Init(DECODER*, CHUNK*) = 0;
    virtual FRAME* GetFrame(unsigned int) = 0;
    virtual unsigned int GetCurrentFrameNumber() = 0;
    virtual float GetFrameRate() = 0;
    virtual void ReleaseFrame(FRAME*) = 0;
    void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_heap); }
    void operator delete(void* p) { rcmp_sys.m_free(p); }
};

CODEC* MPC_CODEC_create();
}

namespace RCON {
template <class T>
class TDLNODE {
public:
    TDLNODE* m_next;
    TDLNODE* m_prev;
};

template <class T>
class TDLIST {
public:
    TDLIST() {
        TDLNODE<T>* head = &m_head;
        head->m_next = head;
        head->m_prev = head;
    }

    TDLNODE<T> m_head;
};
}

class MPC_FRAME;

class MPC_CODEC_INTERNAL : public RCMP::CODEC {
public:
    MPC_CODEC_INTERNAL() { m_stopped = 1; }
    virtual ~MPC_CODEC_INTERNAL();
    virtual int Init(RCMP::DECODER*, RCMP::CHUNK*);
    virtual RCMP::FRAME* GetFrame(unsigned int);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual void ReleaseFrame(RCMP::FRAME*);

    unsigned char unknown04[24];
    int m_stopped;
    unsigned char unknown20[4];
    RCON::TDLIST<MPC_FRAME> m_used;
    RCON::TDLIST<MPC_FRAME> m_free;
    RCMP::DECODER* m_decoder;
};

RCMP::CODEC* RCMP::MPC_CODEC_create() {
    return new MPC_CODEC_INTERNAL;
}
