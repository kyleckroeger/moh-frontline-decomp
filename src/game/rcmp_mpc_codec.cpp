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
// the chunk and bit-stream views and the result type are inferred. The
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
};
}

extern double MPDframe_rate;

class MPC_FRAME {
public:
    void operator delete(void*);
};

namespace RCMP {
class DECODER;

class CHUNK {
public:
    unsigned char unknown0[4];
    unsigned char* m_data;
};
}

/* inferred: the decoder's bit-stream state (MPDbase, 0xa48 bytes) */
struct MPDLayerData {
    unsigned char unknown000[0xa48];
};

extern unsigned char* MPDclip;
extern unsigned char MPDtranspose_table[64];
extern int MPDverbose_flag;
extern int MPDtrace_flag;
extern int MPDquiet_flag;
extern MPDLayerData MPDbase;
extern MPDLayerData* MPDld;

void Initialize_Fast_IDCT();

class MPC_FRAME;

class MPC_CODEC_INTERNAL {
public:
    float GetFrameRate();
    unsigned int GetCurrentFrameNumber();
    int Init(RCMP::DECODER*, RCMP::CHUNK*);

    unsigned char unknown00[4];
    MPC_FRAME* m_frame04;
    MPC_FRAME* m_frame08;
    MPC_FRAME* m_frame0c;
    MPC_FRAME* m_frame10;
    int m_endState;
    unsigned char unknown18[8];
    unsigned int m_frameNumber;
    unsigned char unknown24[16];
    RCMP::DECODER* m_decoder;
};

__declspec(weak) RCMP::FRAME::~FRAME() {
}

__declspec(weak) void MPC_FRAME::operator delete(void* p) {
    RCMP::rcmp_sys.m_free(p);
}

float MPC_CODEC_INTERNAL::GetFrameRate() {
    return MPDframe_rate;
}

unsigned int MPC_CODEC_INTERNAL::GetCurrentFrameNumber() {
    return m_frameNumber;
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
