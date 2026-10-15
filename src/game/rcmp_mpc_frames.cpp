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
// current frame, or the previous one when the stream ran out). The
// classes and templates are named by the mangled symbols (RCON::TDLNODE by
// rcmp_tdlnode_dtor.cpp); MPC_FRAME derives from RCMP::FRAME and its list
// node, and the list and node members (Unlink, RemoveHead, AddTail, First,
// End), the lists' offsets, the counters, the chunk view, the NextChunk and
// ChunkType inline helpers, the four frame pointers and the UseFrame inline
// (unlink, add to the used list, take a reference) and the result types
// are inferred; the codec
// declares its virtual functions in the order of __vt__18MPC_CODEC_INTERNAL,
// its destructor (defined elsewhere) first. The rest of the file is not
// part of this unit.
namespace RCMP {
class FRAME {
public:
    unsigned char unknown00[8];
};

class CHUNK;

class DECODER {
public:
    CHUNK* GetChunk();
    void ReleaseChunk(CHUNK*);
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
