// RCMP (the movie player) base, the whole of rcmpbase.cpp: the global RCMP_SYSTEM
// (allocation hooks; its static initialisation registers the weak destructor),
// the CODEC_IDATA and CHUNK constructors, and the decoder that feeds stream chunks
// to the chosen codec (a pending chunk first, otherwise the get callback),
// forwards frame requests to it (GetFrame first fetches a pending chunk while no
// codec is chosen; without a codec the queries return null, 0.0f and 0), chooses
// and frees the codec, and its constructor and destructor. RCMP, DECODER, CODEC,
// CHUNK, CODEC_IDATA, RCMP_SYSTEM, STREAMER and FRAME are named by the mangled
// symbols and RTTI; members and the codec's virtual function names are inferred
// from the calls and are not original.
//
// Compiled with deferred inlining, so the functions are written last to first.
// CODEC_IDATA's empty destructor (a weak copy, as in avplayer, whose copy the
// linker keeps) changes the DECODER constructor's register choice: the vtable
// address shares r5 with the zero. DestroySystem is a gap function (accepted by
// the maintainer 2026-11-13, docs/Remaining.md): the original emits the weak
// RCMP_SYSTEM destructor before __sinit, where a function of the file that
// deletes an RCMP_SYSTEM puts it; its name and body are this project's, and the
// linker strips it, as the original's was.
namespace RCMP {

class DECODER;
class STREAMER;
class FRAME;

class RCMP_SYSTEM {
public:
    RCMP_SYSTEM() : m_alloc(0), m_free(0) {}
    virtual ~RCMP_SYSTEM() {}

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
    int m_field0C;
};

RCMP_SYSTEM rcmp_sys;

class CHUNK {
public:
    CHUNK();

    void* m_data;
    void* m_next;
    unsigned int m_size;
};

typedef void (*GETCHUNKFUNC)(DECODER*, STREAMER*, CHUNK**);
typedef void (*RELEASECHUNKFUNC)(DECODER*, STREAMER*, CHUNK*);

class CODEC_IDATA {
public:
    CODEC_IDATA() {
        m_value = 2;
        m_streamer = 0;
        m_getChunk = 0;
        m_releaseChunk = 0;
    }
    CODEC_IDATA(STREAMER*, GETCHUNKFUNC, RELEASECHUNKFUNC, unsigned int);
    ~CODEC_IDATA() {}
    void operator delete(void* p) { rcmp_sys.m_free(p); }

    STREAMER* m_streamer;
    GETCHUNKFUNC m_getChunk;
    RELEASECHUNKFUNC m_releaseChunk;
    unsigned int m_value;
};

class CODEC {
public:
    virtual ~CODEC();
    virtual void Init(DECODER*);
    virtual FRAME* GetFrame(unsigned int);
    virtual unsigned int GetCurrentFrameNumber();
    virtual float GetFrameRate();
    virtual void ReleaseFrame(FRAME*);
};

class DECODER {
public:
    DECODER(const CODEC_IDATA*);
    virtual ~DECODER();
    static void operator delete(void* p) { rcmp_sys.m_free(p); }
    void ChooseCodec(CODEC*, CHUNK*);
    void FreeChosenCodec();
    unsigned int GetCurrentFrameNumber();
    float GetFrameRate();
    FRAME* GetFrame(unsigned int);
    void ReleaseFrame(FRAME*);
    CHUNK* GetChunk();
    void ReleaseChunk(CHUNK*);

    CHUNK* m_pending;
    CODEC_IDATA m_idata;
    CODEC* m_codec;
};

// Gap function, stripped at link (see the file comment).
void DestroySystem(RCMP_SYSTEM* system) {
    delete system;
}

DECODER::DECODER(const CODEC_IDATA* idata) {
    m_idata = *idata;
    m_codec = 0;
    m_pending = 0;
}

DECODER::~DECODER() {
    FreeChosenCodec();
}

void DECODER::ChooseCodec(CODEC* codec, CHUNK* chunk) {
    m_codec = codec;
    m_pending = chunk;
    m_codec->Init(this);
}

void DECODER::FreeChosenCodec() {
    if (m_codec) {
        delete m_codec;
        m_codec = 0;
    }
}

unsigned int DECODER::GetCurrentFrameNumber() {
    return !m_codec ? 0 : m_codec->GetCurrentFrameNumber();
}

float DECODER::GetFrameRate() {
    return !m_codec ? 0.0f : m_codec->GetFrameRate();
}

FRAME* DECODER::GetFrame(unsigned int frame) {
    if (!m_codec && !m_pending)
        m_idata.m_getChunk(this, m_idata.m_streamer, &m_pending);
    return !m_codec ? 0 : m_codec->GetFrame(frame);
}

void DECODER::ReleaseFrame(FRAME* frame) {
    m_codec->ReleaseFrame(frame);
}

CHUNK* DECODER::GetChunk() {
    CHUNK* chunk;

    if (m_pending) {
        chunk = m_pending;
        m_pending = 0;
        return chunk;
    }
    m_idata.m_getChunk(this, m_idata.m_streamer, &chunk);
    return chunk;
}

void DECODER::ReleaseChunk(CHUNK* chunk) {
    m_idata.m_releaseChunk(this, m_idata.m_streamer, chunk);
}

CHUNK::CHUNK() {
    m_next = 0;
    m_size = 0;
    m_data = 0;
}

CODEC_IDATA::CODEC_IDATA(STREAMER* streamer, GETCHUNKFUNC getChunk, RELEASECHUNKFUNC releaseChunk,
                         unsigned int value) {
    m_value = value;
    m_streamer = streamer;
    m_getChunk = getChunk;
    m_releaseChunk = releaseChunk;
}

}
