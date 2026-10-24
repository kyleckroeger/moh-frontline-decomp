// A fragment of rcmp_mpc_codec.cpp (0x801043cc): the empty Error hook and
// MPC_CODEC_INTERNAL's frame pool: ReleaseFrame (a frame whose reference
// count drops to zero is unlinked and returned to the free list) and CreateB
// and CreateIorP (the first free frame, if any, moved to the used list with
// a reference count of 0 for a B frame or 1 for an I or P frame) and
// GetFrame (takes the decoder's next chunk, noting the end of the stream,
// skips to the wanted frame, decodes it and releases the chunk) and
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
// scales it by 0.8 for progressive frames of an interlaced sequence). The
// classes and templates are named by the mangled symbols (RCON::TDLNODE by
// rcmp_tdlnode_dtor.cpp); MPC_FRAME derives from RCMP::FRAME and its list
// node, and the list and node members (Unlink, RemoveHead, AddTail, First,
// End), the lists' offsets, the counters, the chunk view, the NextChunk and
// ChunkType inline helpers, the four frame pointers and the UseFrame inline
// (unlink, add to the used list, take a reference), the frame's picture
// header view, constructor and Create helper, AddHead, the decoder's frame
// count and the MPEG-2 flag view of the decoder globals and the result types
// are inferred; the codec
// declares its virtual functions in the order of __vt__18MPC_CODEC_INTERNAL,
// its destructor (defined elsewhere) first. The rest of the file is not
// part of this unit.
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

extern struct layer_data MPDbase;
extern struct layer_data* MPDld;
/* inferred view: the base layer's MPEG-2 flag */
struct layer_data {
    unsigned char unknown000[1056];
    int MPEG2_Flag;
};
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

namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, int, int, int, int);
    void (*m_free)(void*);
    int m_userValue;
};

extern RCMP_SYSTEM rcmp_sys;

class FRAME {
public:
    ~FRAME();

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
    void Unlink() {
        TDLNODE* next = m_next;
        TDLNODE* prev = m_prev;
        prev->m_next = next;
        next->m_prev = prev;
    }

    ~TDLNODE();

    TDLNODE* m_next;
    TDLNODE* m_prev;
};

template <class T>
class TDLIST {
public:
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
            height * width + (height >> 1) * (width >> 1) * 2 + 16, 0, 16, RCMP::rcmp_sys.m_userValue);
        MEM_fill(m_buffer, 0, 16);
        ((FrameHeaderView*)m_buffer)->m_type = 'l';
        ((FrameHeaderView*)m_buffer)->m_width = width;
        ((FrameHeaderView*)m_buffer)->m_height = height;
    }
    static MPC_FRAME* Create(unsigned int width, unsigned int height) {
        return new MPC_FRAME(width, height);
    }
    static void* operator new(unsigned long size) {
        return RCMP::rcmp_sys.m_alloc("", size, 0, 0, RCMP::rcmp_sys.m_userValue);
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
}

class MPC_CODEC_INTERNAL {
public:
    virtual ~MPC_CODEC_INTERNAL();
    virtual void Init(RCMP::DECODER*, RCMP::CHUNK*);
    virtual RCMP::FRAME* GetFrame(unsigned int);
    virtual int GetCurrentFrameNumber();
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
    int m_chunksRead;
    RCON::TDLIST<MPC_FRAME> m_used;
    RCON::TDLIST<MPC_FRAME> m_free;
    RCMP::DECODER* m_decoder;
};

void Error(char*) {
}

void MPC_CODEC_INTERNAL::ReleaseFrame(RCMP::FRAME* f) {
    MPC_FRAME* frame = (MPC_FRAME*)f;
    frame->m_refs--;
    if (frame->m_refs > 0)
        return;
    frame->Unlink();
    m_free.AddTail(frame);
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

MPC_FRAME* MPC_CODEC_INTERNAL::CreateIorP() {
    if (m_free.First() != m_free.End()) {
        MPC_FRAME* frame = m_free.RemoveHead();
        m_used.AddTail(frame);
        frame->m_refs = 1;
        return frame;
    }
    return 0;
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

void Decode_Picture(MPC_CODEC_INTERNAL*, int);

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
