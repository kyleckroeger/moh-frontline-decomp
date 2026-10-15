// A fragment of rcmp_mpc_codec.cpp (0x801043cc): the empty Error hook and
// MPC_CODEC_INTERNAL's frame pool: ReleaseFrame (a frame whose reference
// count drops to zero is unlinked and returned to the free list) and CreateB
// and CreateIorP (the first free frame, if any, moved to the used list with
// a reference count of 0 for a B frame or 1 for an I or P frame) and
// GetFrame (takes the decoder's next chunk, noting the end of the stream,
// skips to the wanted frame, decodes it and releases the chunk). The
// classes and templates are named by the mangled symbols (RCON::TDLNODE by
// rcmp_tdlnode_dtor.cpp); MPC_FRAME derives from RCMP::FRAME and its list
// node, and the list and node members (Unlink, RemoveHead, AddTail, First,
// End), the lists' offsets, the counters and the result types are inferred. The rest of the
// file is not part of this unit.
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

class MPC_CODEC_INTERNAL {
public:
    void ReleaseFrame(RCMP::FRAME*);
    MPC_FRAME* CreateB();
    MPC_FRAME* CreateIorP();
    RCMP::FRAME* GetFrame(unsigned int);
    void SkipChunks(RCMP::CHUNK**, unsigned int);
    RCMP::FRAME* DecodeChunk(RCMP::CHUNK**);

    unsigned char unknown00[20];
    int m_endState;
    unsigned char unknown18[8];
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
