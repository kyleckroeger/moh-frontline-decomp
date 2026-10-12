// The start of avplayer.cpp (0x800fb0ec..0x800fb4cc): the AV player's chunk
// callbacks for its decoder. StaticReleaseRCMPChunk returns a chunk's stream
// data and deletes it; StaticGetRCMPChunk forwards to the streamer's player;
// GetRCMPChunk pulls stream buffers (running the sync tasks after each) until
// one is a chunk for the MPC codec or the stream ends, wraps it in a new
// CHUNK (allocated through the RCMP system hook with an empty name; the
// little-endian size from bytes 4..7, the payload after the 8-byte header)
// and lets the decoder choose the MPC codec if it has none; other buffers are
// released. The weak class operator delete of CHUNK (freeing through the
// RCMP system's free hook) follows, as in the original. UnPause and Pause
// set the audio player's speed (4096 = normal, 0 = stopped) and the player's
// millisecond timer, which first adds the OSGetTime ticks since its last
// change (scaled by its speed in 1/4096 steps) to its elapsed count.
// RCMP, RCMP_SYSTEM, rcmp_sys, AV_PLAYER, DECODER, STREAMER, CHUNK, CODEC and
// the functions are named by the symbols. Inferred: the allocation hook's
// arguments (name, size, two zeros, the system's flags word at +12), the
// member layouts (the player's stream handle at +40, the decoder's codec at
// +24, the streamer's player first, the audio player first and the timer at
// +84 in AV_PLAYER), the chunk type that is only ever 0 or
// 1 (the original keeps a test for 2), ChooseCodec's int result, the
// trivial inline CHUNK destructor (delete tests for null), the timer layout
// and its inline SetSpeed, the int results of Pause/UnPause, and the unused
// 5-byte chunk name built for an empty debug hook. The rest of the file is
// not part of this unit.
extern "C" {
long long OSGetTime();
void* STREAM_get(int);
void STREAM_release(int, void*);
unsigned char STREAM_isendofstream(int);
void SYNCTASK_run(int);
}

namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(const char*, unsigned long, int, int, int);
    void (*m_free)(void*);
    int m_allocFlags;
};

extern RCMP_SYSTEM rcmp_sys;

class CODEC;
class AV_PLAYER;

class CHUNK {
public:
    CHUNK();
    ~CHUNK() {}
    void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_allocFlags); }
    void operator delete(void* p) { rcmp_sys.m_free(p); }

    void* m_data;
    void* m_next;
    unsigned int m_size;
};

class DECODER {
public:
    int ChooseCodec(CODEC*, CHUNK*);
    void FreeChosenCodec();

    unsigned char unknown00[24];
    CODEC* m_codec;
};

class STREAMER {
public:
    AV_PLAYER* m_player;
};

bool MPC_CODEC_is_chunk_for_codec(unsigned int);
CODEC* MPC_CODEC_create();

inline void DebugChunk(const char*) {}

class AUDIO_PLAYER {
public:
    void SetSpeed(unsigned int);
};

class AV_MS_TIMER {
public:
    void SetSpeed(unsigned int speed) {
        long long now = OSGetTime();
        if (m_speed == 0x1000)
            m_elapsed += now - m_last;
        else
            m_elapsed += ((unsigned long long)(now - m_last) * m_speed) >> 12;
        m_last = now;
        m_speed = speed;
    }

    long long m_last;
    unsigned long long m_elapsed;
    unsigned int m_speed;
};

class AV_PLAYER {
public:
    static void StaticReleaseRCMPChunk(DECODER*, STREAMER*, CHUNK*);
    static void StaticGetRCMPChunk(DECODER*, STREAMER*, CHUNK**);
    void GetRCMPChunk(DECODER*, CHUNK**);
    int UnPause();
    int Pause();

    AUDIO_PLAYER* m_audio;
    unsigned char unknown04[36];
    int m_stream;
    unsigned char unknown2c[40];
    AV_MS_TIMER* m_timer;
};
}

using namespace RCMP;

void AV_PLAYER::StaticReleaseRCMPChunk(DECODER*, STREAMER* streamer, CHUNK* chunk) {
    STREAM_release(streamer->m_player->m_stream, chunk->m_data);
    delete chunk;
}

void AV_PLAYER::StaticGetRCMPChunk(DECODER* decoder, STREAMER* streamer, CHUNK** chunk) {
    streamer->m_player->GetRCMPChunk(decoder, chunk);
}

void AV_PLAYER::GetRCMPChunk(DECODER* decoder, CHUNK** result) {
    unsigned int id;
    int type = 0;
    int stream = m_stream;
    *result = 0;
    do {
        unsigned char* data = (unsigned char*)STREAM_get(stream);
        SYNCTASK_run(0);
        if (data) {
            id = *(unsigned int*)data;
            if (MPC_CODEC_is_chunk_for_codec(id))
                type = 1;
            if (type) {
                CHUNK* chunk = new CHUNK;
                *result = chunk;
                chunk->m_data = data;
                if (type == 2) {
                    chunk->m_size = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];
                    chunk->m_next = data;
                } else {
                    chunk->m_size = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];
                    chunk->m_next = data + 8;
                }
                if (decoder->m_codec == 0 && type == 1) {
                    CODEC* codec = MPC_CODEC_create();
                    if (decoder->ChooseCodec(codec, chunk) != 2)
                        decoder->FreeChosenCodec();
                }
                return;
            }
            char name[5];
            *(unsigned int*)name = id;
            name[4] = 0;
            DebugChunk(name);
            STREAM_release(stream, data);
        }
    } while (!STREAM_isendofstream(stream));
}

int AV_PLAYER::UnPause() {
    if (m_audio)
        m_audio->SetSpeed(0x1000);
    m_timer->SetSpeed(0x1000);
    return 0;
}

int AV_PLAYER::Pause() {
    if (m_audio)
        m_audio->SetSpeed(0);
    m_timer->SetSpeed(0);
    return 0;
}
