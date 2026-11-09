// RCMP (the movie player) base: the decoder's codec selection and release
// and its destructor (which frees through the global RCMP_SYSTEM's hook). RCMP, DECODER, CODEC, CHUNK,
// CODEC_IDATA, RCMP_SYSTEM, STREAMER and FRAME are named by the mangled
// symbols and RTTI; members and the codec's virtual function names are
// inferred from the calls and are not original.
namespace RCMP {

class DECODER;
class STREAMER;
class FRAME;

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
    CODEC_IDATA() { Clear(); }
    void Clear() {
        m_value = 2;
        m_streamer = 0;
        m_getChunk = 0;
        m_releaseChunk = 0;
    }
    CODEC_IDATA(STREAMER*, GETCHUNKFUNC, RELEASECHUNKFUNC, unsigned int);

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

class RCMP_SYSTEM {
public:
    RCMP_SYSTEM() : m_alloc(0), m_free(0) {}
    virtual ~RCMP_SYSTEM() {}

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
    int m_field0C;
};

extern RCMP_SYSTEM rcmp_sys;

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

// The CODEC_IDATA and CHUNK constructors and the DECODER chunk/frame
// accessors come before these in the original file and are not part of this
// unit (drafts in scratch). The DECODER constructor, RCMP_SYSTEM's weak
// destructor and rcmp_sys (whose static initialisation registers that
// destructor) follow.

void DECODER::FreeChosenCodec() {
    if (m_codec) {
        delete m_codec;
        m_codec = 0;
    }
}

void DECODER::ChooseCodec(CODEC* codec, CHUNK* chunk) {
    m_codec = codec;
    m_pending = chunk;
    m_codec->Init(this);
}

DECODER::~DECODER() {
    FreeChosenCodec();
}

DECODER::DECODER(const CODEC_IDATA* idata) {
    m_idata = *idata;
    m_codec = 0;
    m_pending = 0;
}

RCMP_SYSTEM rcmp_sys;

}
