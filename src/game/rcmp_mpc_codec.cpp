// First in this unit (0x80104cb4): the weak empty RCMP::FRAME destructor (its
// class delete, the RCMP system's free hook, inlined).
// MPC codec helpers: the frame's class delete through the RCMP system's free
// hook, the stream frame rate (the decoder's double MPDframe_rate) and the
// current frame number, then Init (allocates the decoder's clip table,
// fills it and the 8x8 transpose table, checks the chunk starts with a
// sequence header 00 00 01 b3, and resets the decoder flags, bit-stream
// pointer, IDCT and frame pointers; 0 on a bad header, else 2). MPC_FRAME,
// MPC_CODEC_INTERNAL, RCMP_SYSTEM, the decoder globals and MPDframe_rate are
// named by the symbols; the members, the hook slots and their signatures,
// the chunk and bit-stream views and the result type are inferred, then the
// destructor (deletes the used and free frames, frees the clip table and
// clears two decoder globals; the lists' and CODEC's destructors are inline,
// so the file holds __vt__18MPC_CODEC_INTERNAL and the weak
// __vt__Q24RCMP5CODEC). RCON::TDLIST's deleting destructor and the frame's
// buffer-freeing destructor are inferred inlines. The file is compiled with
// deferred inlining (its virtual tables come out in that order), so the
// functions are written in reverse image order. The
// delete is inline in the original (a weak symbol), so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, int, int, int, int); /* inferred from the calls */
    void (*m_free)(void*);
    int m_heap;
};

extern RCMP_SYSTEM rcmp_sys;

class FRAME {
public:
    ~FRAME();
    void operator delete(void* p) { rcmp_sys.m_free(p); }

    int m_value;
    void* m_buffer;
};
}

namespace RCON {
template <class T>
class TDLNODE {
public:
    ~TDLNODE() {}
    void Unlink() {
        TDLNODE* next = m_next;
        TDLNODE* prev = m_prev;
        prev->m_next = next;
        next->m_prev = prev;
    }

    TDLNODE* m_next;
    TDLNODE* m_prev;
};

/* inferred: the list's destructor deletes every node */
template <class T>
class TDLIST {
public:
    ~TDLIST() { DeleteAll(); }
    void DeleteAll() {
        TDLNODE<T>* node;
        while ((node = m_head.m_next) != &m_head) {
            node->Unlink();
            delete static_cast<T*>(node);
        }
    }

    TDLNODE<T> m_head;
};
}

extern double MPDframe_rate;

class MPC_FRAME : public RCMP::FRAME, public RCON::TDLNODE<MPC_FRAME> {
public:
    ~MPC_FRAME() {
        if (m_buffer) {
            RCMP::rcmp_sys.m_free(m_buffer);
            m_buffer = 0;
        }
    }
    void operator delete(void*);

    int m_refs;
};

namespace RCMP {
class DECODER;
class CHUNK;

class CODEC {
public:
    virtual ~CODEC() {}
    virtual int Init(DECODER*, CHUNK*) = 0;
    virtual FRAME* GetFrame(unsigned int) = 0;
    virtual unsigned int GetCurrentFrameNumber() = 0;
    virtual float GetFrameRate() = 0;
    virtual void ReleaseFrame(FRAME*) = 0;
    void operator delete(void* p) { rcmp_sys.m_free(p); }
};

class CHUNK {
public:
    unsigned char unknown0[4];
    unsigned char* m_data;
};
}

/* inferred: the decoder's bit-stream state (MPDbase, 0xa48 bytes) */
struct MPDLayerData {
    unsigned char unknown000[1056];
    int m_value420;
    unsigned char unknown424[0xa48 - 1060];
};

extern unsigned char* MPDclip;
extern unsigned char MPDtranspose_table[64];
extern int MPDverbose_flag;
extern int MPDtrace_flag;
extern int MPDquiet_flag;
extern MPDLayerData MPDbase;
extern MPDLayerData* MPDld;
extern unsigned char* MPDbackward_reference_frame[3];

void Initialize_Fast_IDCT();

class MPC_CODEC_INTERNAL : public RCMP::CODEC {
public:
    virtual ~MPC_CODEC_INTERNAL();
    virtual int Init(RCMP::DECODER*, RCMP::CHUNK*);
    virtual RCMP::FRAME* GetFrame(unsigned int);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual void ReleaseFrame(RCMP::FRAME*);

    MPC_FRAME* m_frame04;
    MPC_FRAME* m_frame08;
    MPC_FRAME* m_frame0c;
    MPC_FRAME* m_frame10;
    int m_endState;
    unsigned char unknown18[8];
    unsigned int m_frameNumber;
    RCON::TDLIST<MPC_FRAME> m_used;
    RCON::TDLIST<MPC_FRAME> m_free;
    RCMP::DECODER* m_decoder;
};

MPC_CODEC_INTERNAL::~MPC_CODEC_INTERNAL() {
    m_used.DeleteAll();
    m_free.DeleteAll();
    MPDclip -= 384;
    if (MPDclip) {
        RCMP::rcmp_sys.m_free(MPDclip);
        MPDclip = 0;
    }
    MPDbase.m_value420 = 0;
    MPDbackward_reference_frame[0] = 0;
}

int MPC_CODEC_INTERNAL::Init(RCMP::DECODER* decoder, RCMP::CHUNK* chunk) {
    unsigned char* data = chunk->m_data;
    unsigned char start[4] = {0x00, 0x00, 0x01, 0xb3};
    MPDclip = (unsigned char*)RCMP::rcmp_sys.m_alloc("MPDclip", 1024, 0, 0, RCMP::rcmp_sys.m_heap);
    MPDclip += 384;
    int i;
    for (i = -384; i < 640; i++)
        MPDclip[i] = (i < 0) ? 0 : ((i > 255) ? 255 : i);
    for (i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++)
            MPDtranspose_table[i * 8 + j] = i * 8 + j;
    }
    for (i = 0; i < 4; i++) {
        if (start[i] != data[i])
            return 0;
    }
    m_endState = 0;
    m_decoder = decoder;
    MPDverbose_flag = 0;
    MPDtrace_flag = 0;
    MPDquiet_flag = 0;
    MPDld = &MPDbase;
    Initialize_Fast_IDCT();
    m_frame04 = 0;
    m_frame08 = 0;
    m_frame0c = 0;
    m_frame10 = 0;
    m_frameNumber = 0;
    return 2;
}

unsigned int MPC_CODEC_INTERNAL::GetCurrentFrameNumber() {
    return m_frameNumber;
}

float MPC_CODEC_INTERNAL::GetFrameRate() {
    return MPDframe_rate;
}

__declspec(weak) void MPC_FRAME::operator delete(void* p) {
    RCMP::rcmp_sys.m_free(p);
}

__declspec(weak) RCMP::FRAME::~FRAME() {
}
