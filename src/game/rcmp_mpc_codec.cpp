// rcmp_mpc_codec.cpp, the whole file: the MPEG codec behind RCMP::MPC_CODEC_create.
//
// RCMP::MPC_CODEC_create makes the codec (allocated through the RCMP system's
// allocator with an empty name; the constructor empties both frame lists and
// sets the stopped flag at +0x1c), then the weak RCMP::CODEC destructor.
// The empty Error hook and MPC_CODEC_INTERNAL's frame pool: ReleaseFrame (a
// frame whose reference count drops to zero is unlinked and returned to the
// free list) and CreateB and CreateIorP (the first free frame, if any, moved
// to the used list with a reference count of 0 for a B frame or 1 for an I or
// P frame) and GetFrame (takes the decoder's next chunk, noting the end of the
// stream, skips to the wanted frame, decodes it and releases the chunk) and
// SkipChunks (more than two frames ahead: reads to the next sequence start,
// decodes and releases that frame and skips B chunks; otherwise skips the B
// chunks among the frames in between) and DecodeChunk (at the end of the
// stream, releases the old reference frame and hands out the last one;
// otherwise decodes the chunk - first any held one - and hands out the
// current frame, or the previous one when the stream ran out) and
// LowLevelDecodeChunk (points the MPEG decoder at the chunk and reads its
// headers; for a sequence start it sets up the MPEG-1 defaults and picture
// sizes as the reference decoder's Initialize_Sequence does and adds the
// decoder's frame count plus two new frames, each with a cleared 16-byte
// picture header of the coded size, to the free list; then decodes the
// picture and, for a sequence start, defaults the frame rate to 30 and
// scales it by 0.8 for progressive frames of an interlaced sequence).
// The weak empty RCON::TDLNODE<MPC_FRAME> and RCMP::FRAME destructors, the
// frame's class delete through the RCMP system's free hook, the stream frame
// rate (the decoder's double MPDframe_rate) and the current frame number,
// then Init (allocates the decoder's clip table, fills it and the 8x8
// transpose table, checks the chunk starts with a sequence header 00 00 01
// b3, and resets the decoder flags, bit-stream pointer, IDCT and frame
// pointers; 0 on a bad header, else 2) and the destructor (deletes the used
// and free frames, frees the clip table and clears two decoder globals; the
// lists' and CODEC's destructors are inline, so the file holds
// __vt__18MPC_CODEC_INTERNAL and the weak __vt__Q24RCMP5CODEC).
//
// The classes, templates, functions and globals are named by the mangled
// symbols; the members, the hook slots and their signatures, the chunk,
// frame-header and bit-stream views, the list and node helpers, the inline
// constructors, NextChunk/ChunkType/UseFrame/Create and the result types are
// inferred. The file is compiled with deferred inlining and RTTI (its virtual
// tables come out in that order), so the functions are written in reverse
// image order. Inline functions of the original (weak symbols) are defined
// __declspec(weak).
#include <stddef.h>

/* inferred: a frame's picture header (type byte, then the size) */
struct FrameHeaderView {
    unsigned char m_type;
    unsigned char unknown1[3];
    short m_width;
    short m_height;
};

extern "C" void MEM_fill(void*, int, int);
void Initialize_Buffer(unsigned char*);
void Get_Hdr();
void Initialize_Fast_IDCT();

/* inferred: the decoder's bit-stream state (MPDbase, 0xa48 bytes) and its
   MPEG-2 flag */
struct layer_data {
    unsigned char unknown000[1056];
    int MPEG2_Flag;
    unsigned char unknown424[0xa48 - 1060];
};

extern layer_data MPDbase;
extern layer_data* MPDld;
extern double MPDframe_rate;
extern int MPDframe_pred_frame_dct;
extern int MPDhorizontal_size;
extern int MPDmatrix_coefficients;
extern int MPDpicture_structure;
extern int MPDprogressive_frame;
extern int MPDprogressive_sequence;
extern int MPDvertical_size;
extern int MPDmb_width;
extern int MPDmb_height;
extern int MPDcoded_picture_width;
extern int MPDcoded_picture_height;
extern int MPDchroma_width;
extern int MPDchroma_height;
extern unsigned char* MPDclip;
extern unsigned char MPDtranspose_table[64];
extern int MPDverbose_flag;
extern int MPDtrace_flag;
extern int MPDquiet_flag;
extern unsigned char* MPDbackward_reference_frame[3];

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

class CHUNK;

class DECODER {
public:
    CHUNK* GetChunk();
    void ReleaseChunk(CHUNK*);

    unsigned char unknown00[20];
    int m_frameCount;
};
}

namespace RCON {
template <class T>
class TDLNODE {
public:
    ~TDLNODE();
    void Unlink() {
        TDLNODE* next = m_next;
        TDLNODE* prev = m_prev;
        prev->m_next = next;
        next->m_prev = prev;
    }

    TDLNODE* m_next;
    TDLNODE* m_prev;
};

template <class T> __declspec(weak) TDLNODE<T>::~TDLNODE() {
}

template <class T>
class TDLIST {
public:
    TDLIST() {
        TDLNODE<T>* head = &m_head;
        head->m_next = head;
        head->m_prev = head;
    }
    /* inferred: the list's destructor deletes every node */
    ~TDLIST() { DeleteAll(); }
    void DeleteAll() {
        TDLNODE<T>* node;
        while ((node = m_head.m_next) != &m_head) {
            node->Unlink();
            delete static_cast<T*>(node);
        }
    }
    TDLNODE<T>* First() { return m_head.m_next; }
    TDLNODE<T>* End() { return &m_head; }
    T* RemoveHead() {
        TDLNODE<T>* node = m_head.m_next;
        node->Unlink();
        return static_cast<T*>(node);
    }
    void AddHead(T* item) {
        TDLNODE<T>* node = item;
        TDLNODE<T>* head = &m_head;
        TDLNODE<T>* first = m_head.m_next;
        head->m_next = node;
        first->m_prev = node;
        node->m_prev = head;
        node->m_next = first;
    }
    void AddTail(T* item) {
        TDLNODE<T>* node = item;
        TDLNODE<T>* head = &m_head;
        TDLNODE<T>* tail = m_head.m_prev;
        head->m_prev = node;
        tail->m_next = node;
        node->m_next = head;
        node->m_prev = tail;
    }

    TDLNODE<T> m_head;
};
}

class MPC_FRAME : public RCMP::FRAME, public RCON::TDLNODE<MPC_FRAME> {
public:
    MPC_FRAME(unsigned int width, unsigned int height) {
        m_refs = 0;
        m_value = 0;
        m_buffer = RCMP::rcmp_sys.m_alloc("MPC_FRAME::m_Shape",
            height * width + (height >> 1) * (width >> 1) * 2 + 16, 0, 16, RCMP::rcmp_sys.m_heap);
        MEM_fill(m_buffer, 0, 16);
        ((FrameHeaderView*)m_buffer)->m_type = 'l';
        ((FrameHeaderView*)m_buffer)->m_width = width;
        ((FrameHeaderView*)m_buffer)->m_height = height;
    }
    ~MPC_FRAME() {
        if (m_buffer) {
            RCMP::rcmp_sys.m_free(m_buffer);
            m_buffer = 0;
        }
    }
    static MPC_FRAME* Create(unsigned int width, unsigned int height) {
        return new MPC_FRAME(width, height);
    }
    static void* operator new(unsigned long size) {
        return RCMP::rcmp_sys.m_alloc("", size, 0, 0, RCMP::rcmp_sys.m_heap);
    }
    void operator delete(void*);

    int m_refs;
};

/* inferred: a chunk's data, its start code at +3 and picture type in +5 */
struct ChunkDataView {
    unsigned char unknown0[3];
    unsigned char m_code;
    unsigned char unknown4;
    unsigned char m_flags;
};

namespace RCMP {
class CHUNK {
public:
    unsigned char unknown0[4];
    ChunkDataView* m_data;
};

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

class MPC_CODEC_INTERNAL : public RCMP::CODEC {
public:
    MPC_CODEC_INTERNAL() { m_stopped = 1; }
    virtual ~MPC_CODEC_INTERNAL();
    virtual int Init(RCMP::DECODER*, RCMP::CHUNK*);
    virtual RCMP::FRAME* GetFrame(unsigned int);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual void ReleaseFrame(RCMP::FRAME*);

    MPC_FRAME* CreateB();
    MPC_FRAME* CreateIorP();

    /* inferred inline helpers */
    void NextChunk(RCMP::CHUNK** chunk) {
        if (*chunk)
            m_decoder->ReleaseChunk(*chunk);
        *chunk = m_decoder->GetChunk();
        m_chunksRead++;
        if (!*chunk) {
            if (m_endState == 0)
                m_endState = 1;
            else
                m_endState = 2;
        }
    }
    void UseFrame(MPC_FRAME* frame) {
        frame->Unlink();
        m_used.AddTail(frame);
        frame->m_refs++;
    }
    void LowLevelDecodeChunk(RCMP::CHUNK*);
    static int ChunkType(RCMP::CHUNK* chunk) {
        if (chunk) {
            ChunkDataView* data = chunk->m_data;
            if (data->m_code == 0xb8)
                return 1;
            return (data->m_flags >> 3) & 3;
        }
        return 1;
    }
    void SkipChunks(RCMP::CHUNK**, unsigned int);
    RCMP::FRAME* DecodeChunk(RCMP::CHUNK**);

    MPC_FRAME* m_frame04;
    MPC_FRAME* m_frame08;
    MPC_FRAME* m_frame0c;
    MPC_FRAME* m_frame10;
    int m_endState;
    unsigned char unknown18[4];
    int m_stopped;
    int m_chunksRead; /* the current frame number */
    RCON::TDLIST<MPC_FRAME> m_used;
    RCON::TDLIST<MPC_FRAME> m_free;
    RCMP::DECODER* m_decoder;
};

void Decode_Picture(MPC_CODEC_INTERNAL*, int);

/* ---- definitions ---- */
MPC_CODEC_INTERNAL::~MPC_CODEC_INTERNAL() {
    m_used.DeleteAll();
    m_free.DeleteAll();
    MPDclip -= 384;
    if (MPDclip) {
        RCMP::rcmp_sys.m_free(MPDclip);
        MPDclip = 0;
    }
    MPDbase.MPEG2_Flag = 0;
    MPDbackward_reference_frame[0] = 0;
}

int MPC_CODEC_INTERNAL::Init(RCMP::DECODER* decoder, RCMP::CHUNK* chunk) {
    unsigned char* data = (unsigned char*)chunk->m_data;
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
    m_chunksRead = 0;
    return 2;
}

unsigned int MPC_CODEC_INTERNAL::GetCurrentFrameNumber() {
    return m_chunksRead;
}

float MPC_CODEC_INTERNAL::GetFrameRate() {
    return MPDframe_rate;
}

__declspec(weak) void MPC_FRAME::operator delete(void* p) {
    RCMP::rcmp_sys.m_free(p);
}

__declspec(weak) RCMP::FRAME::~FRAME() {
}

template class RCON::TDLNODE<MPC_FRAME>;

void MPC_CODEC_INTERNAL::LowLevelDecodeChunk(RCMP::CHUNK* chunk) {
    unsigned char* data;
    unsigned int count;
    unsigned int i;

    data = (unsigned char*)chunk->m_data;
    MPDld = &MPDbase;
    Initialize_Buffer(data);
    Get_Hdr();
    if (m_stopped) {
        if (!MPDbase.MPEG2_Flag) {
            MPDprogressive_sequence = 1;
            MPDprogressive_frame = 1;
            MPDpicture_structure = 3;
            MPDframe_pred_frame_dct = 1;
            MPDmatrix_coefficients = 5;
        }
        MPDmb_width = (MPDhorizontal_size + 15) / 16;
        MPDmb_height = (MPDbase.MPEG2_Flag && !MPDprogressive_sequence) ? 2 * ((MPDvertical_size + 31) / 32)
                                                                        : (MPDvertical_size + 15) / 16;
        MPDcoded_picture_width = 16 * MPDmb_width;
        MPDcoded_picture_height = 16 * MPDmb_height;
        MPDchroma_width = MPDcoded_picture_width >> 1;
        MPDchroma_height = MPDcoded_picture_height >> 1;
        count = m_decoder->m_frameCount + 2;
        for (i = 0; i < count; i++) {
            MPC_FRAME* frame = MPC_FRAME::Create(MPDcoded_picture_width, MPDcoded_picture_height);
            m_free.AddHead(frame);
        }
    }
    Decode_Picture(this, m_chunksRead);
    if (m_stopped) {
        if (0.0 == MPDframe_rate)
            MPDframe_rate = 30.0;
        if (!MPDprogressive_sequence && MPDprogressive_frame == 1)
            MPDframe_rate *= 0.8f;
    }
}

RCMP::FRAME* MPC_CODEC_INTERNAL::DecodeChunk(RCMP::CHUNK** chunk) {
    if (m_endState == 1 || m_endState == 2) {
        if (m_frame04 && m_frame04->m_refs > 0)
            ReleaseFrame(m_frame04);
        m_frame10 = m_frame04 = m_frame08;
        m_frame08 = 0;
        m_frame0c = 0;
        if (m_frame10)
            UseFrame(m_frame10);
        return m_frame10;
    }
    if (m_stopped) {
        LowLevelDecodeChunk(*chunk);
        NextChunk(chunk);
        m_stopped = 0;
    }
    if (!*chunk) {
        UseFrame(m_frame0c);
        return m_frame0c;
    }
    LowLevelDecodeChunk(*chunk);
    UseFrame(m_frame10);
    return m_frame10;
}

void MPC_CODEC_INTERNAL::SkipChunks(RCMP::CHUNK** chunk, unsigned int frame) {
    if (m_stopped == 0 && m_endState == 0) {
        int count = frame - m_chunksRead;
        if (count > 2) {
            do {
                NextChunk(chunk);
            } while (ChunkType(*chunk) != 1);
            RCMP::FRAME* decoded = DecodeChunk(chunk);
            if (decoded)
                ReleaseFrame(decoded);
            while (ChunkType(*chunk) == 3)
                NextChunk(chunk);
        } else if (count > 0) {
            for (int i = 0; i < count; i++) {
                if (ChunkType(*chunk) == 3)
                    NextChunk(chunk);
            }
        }
    }
}

RCMP::FRAME* MPC_CODEC_INTERNAL::GetFrame(unsigned int frame) {
    RCMP::CHUNK* chunk = 0;
    chunk = m_decoder->GetChunk();
    m_chunksRead++;
    if (!chunk) {
        if (m_endState == 0)
            m_endState = 1;
        else
            m_endState = 2;
    }
    SkipChunks(&chunk, frame);
    RCMP::FRAME* result = DecodeChunk(&chunk);
    if (chunk)
        m_decoder->ReleaseChunk(chunk);
    return result;
}

MPC_FRAME* MPC_CODEC_INTERNAL::CreateIorP() {
    if (m_free.First() != m_free.End()) {
        MPC_FRAME* frame = m_free.RemoveHead();
        m_used.AddTail(frame);
        frame->m_refs = 1;
        return frame;
    }
    return 0;
}

MPC_FRAME* MPC_CODEC_INTERNAL::CreateB() {
    if (m_free.First() != m_free.End()) {
        MPC_FRAME* frame = m_free.RemoveHead();
        m_used.AddTail(frame);
        frame->m_refs = 0;
        return frame;
    }
    return 0;
}

void MPC_CODEC_INTERNAL::ReleaseFrame(RCMP::FRAME* f) {
    MPC_FRAME* frame = (MPC_FRAME*)f;
    frame->m_refs--;
    if (frame->m_refs > 0)
        return;
    frame->Unlink();
    m_free.AddTail(frame);
}

void Error(char*) {
}

RCMP::CODEC* RCMP::MPC_CODEC_create() {
    return new MPC_CODEC_INTERNAL;
}
