// The start of avplayer.cpp (0x800fb0ec..0x800fbc1c): the AV player's chunk
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
// IsAudioFinished asks the audio player (true without one). IsTimeForDecode
// takes the timer's milliseconds (ticks over bus clock / 4 / 1000) less the
// player's offset; with audio sync on it reads the audio stream's played
// position (less the output latency) and, when that advanced, keeps a
// running drift (seven eighths of the old one plus the new difference),
// moving the offset by an eighth of it and restarting once it exceeds 264;
// the time in frames ((elapsed + start) / 1000 * frame rate) is stored and
// compared with the next frame. GetFrame stores the time, asks the decoder
// for that frame and records the decoder's current frame number.
// GetFirstFrame raises both streams' priority, queues any preloaded file data
// (the audio stream shares the video queue unless it is separate), creates
// the audio player (audio sync while its stream is valid) unless there is
// none, the decoder with this player's chunk callbacks once the RCMP system
// hooks are set, decodes frame 0, starts the timer, the sound, and unpauses.
// The weak class deletes of AV_MS_TIMER, DECODER and AUDIO_PLAYER and the
// weak empty CODEC_IDATA destructor (its class delete inlined) follow it, as
// in the original.
// RCMP, RCMP_SYSTEM, rcmp_sys, AV_PLAYER, DECODER, STREAMER, CHUNK, CODEC and
// the functions are named by the symbols. Inferred: the allocation hook's
// arguments (name, size, two zeros, the system's flags word at +12), the
// member layouts (the player's stream handle at +40, the decoder's codec at
// +24, the streamer's player first, the audio player first and the timer at
// +84 in AV_PLAYER; the file, stream and queue members), the 20-byte audio
// player, the 24-byte timer's inline constructor, the system's IsReady
// helper, the chunk type that is only ever 0 or
// 1 (the original keeps a test for 2), ChooseCodec's int result, the
// trivial inline CHUNK destructor (delete tests for null), the timer layout
// and its inline Update/SetSpeed/GetTime, the player's time and drift
// members and the stream status views (the fields used at +4), the int
// results of Pause/UnPause, and the unused
// 5-byte chunk name built for an empty debug hook. The rest of the file is
// not part of this unit.
extern "C" {
long long OSGetTime();
void STREAM_setpriority(int, int, int);
int STREAM_queuemem(int, void*, int, int);
void ASYNCFILE_release(int, void**, int*);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
int SNDSTRM_status(int, void*);
int SNDSTRM_requeststatus(int, void*);
int SNDPROFILE_outputlatency();
void* STREAM_get(int);
void STREAM_release(int, void*);
unsigned char STREAM_isendofstream(int);
void SYNCTASK_run(int);
}

namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    bool IsReady() const {
        bool ready = false;
        if (m_alloc && m_free)
            ready = true;
        return ready;
    }

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

class FRAME;
class DECODER;
class STREAMER;

typedef void (*GETCHUNKFUNC)(DECODER*, STREAMER*, CHUNK**);
typedef void (*RELEASECHUNKFUNC)(DECODER*, STREAMER*, CHUNK*);

class CODEC_IDATA {
public:
    CODEC_IDATA(STREAMER*, GETCHUNKFUNC, RELEASECHUNKFUNC, unsigned int);
    ~CODEC_IDATA() {}
    void operator delete(void* p) { rcmp_sys.m_free(p); }

    STREAMER* m_streamer;
    GETCHUNKFUNC m_getChunk;
    RELEASECHUNKFUNC m_releaseChunk;
    unsigned int m_value;
};

class DECODER {
public:
    DECODER(const CODEC_IDATA*);
    void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_allocFlags); }
    void operator delete(void* p) { rcmp_sys.m_free(p); }
    int ChooseCodec(CODEC*, CHUNK*);
    float GetFrameRate();
    FRAME* GetFrame(unsigned int);
    unsigned int GetCurrentFrameNumber();
    void FreeChosenCodec();

    unsigned char unknown00[24];
    CODEC* m_codec;
};


bool MPC_CODEC_is_chunk_for_codec(unsigned int);
CODEC* MPC_CODEC_create();

inline void DebugChunk(const char*) {}

class STREAMER {
public:
    AV_PLAYER* m_player;
};

class AUDIO_PLAYER {
public:
    AUDIO_PLAYER(int, int);
    void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_allocFlags); }
    void operator delete(void* p) { rcmp_sys.m_free(p); }
    void SetSpeed(unsigned int);
    bool IsAudioFinished();
    void StartSound();


    unsigned char unknown00[12];
    int m_stream;
    unsigned char unknown10[4];
};

struct SNDSTRMSTATUSVIEW {
    int field00;
    int request;
    int field08[2];
};

struct SNDSTRMREQUESTVIEW {
    int field00;
    int position;
    int field08[2];
};

class AV_MS_TIMER {
public:
    AV_MS_TIMER() {
        m_elapsed = 0;
        m_last = OSGetTime();
        m_speed = 0x1000;
    }
    void* operator new(unsigned long size) { return rcmp_sys.m_alloc("", size, 0, 0, rcmp_sys.m_allocFlags); }
    void operator delete(void* p) { rcmp_sys.m_free(p); }
    void Update() {
        long long now = OSGetTime();
        if (m_speed == 0x1000)
            m_elapsed += now - m_last;
        else
            m_elapsed += ((unsigned long long)(now - m_last) * m_speed) >> 12;
        m_last = now;
    }
    void SetSpeed(unsigned int speed) {
        Update();
        m_speed = speed;
    }
    unsigned int GetTime() {
        Update();
        return m_elapsed / (*(unsigned long*)0x800000F8 / 4 / 1000);
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
    bool IsAudioFinished();
    bool IsTimeForDecode();
    FRAME* GetFrame(float);
    FRAME* GetFirstFrame(unsigned int, int);

    AUDIO_PLAYER* m_audio;
    unsigned char unknown04[8];
    unsigned char m_separateAudio;
    unsigned char unknown0d[7];
    int m_noAudio;
    void* m_videoData;
    int m_videoFile;
    void* m_audioData;
    int m_audioFile;
    int m_stream;
    int m_audioStream;
    int m_videoQueue;
    int m_audioQueue;
    unsigned int m_startTime;
    unsigned int m_frame;
    float m_time;
    int m_offset;
    int m_audioPosition;
    int m_audioSynced;
    int m_drift;
    AV_MS_TIMER* m_timer;
    STREAMER m_streamer;
    DECODER* m_decoder;
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

bool AV_PLAYER::IsAudioFinished() {
    if (m_audio)
        return m_audio->IsAudioFinished();
    return true;
}

bool AV_PLAYER::IsTimeForDecode() {
    unsigned int time = m_timer->GetTime();
    int elapsed = time - m_offset;
    if (m_audioSynced) {
        SNDSTRMSTATUSVIEW status;
        SNDSTRMREQUESTVIEW request;
        SNDSYS_entercritical();
        SNDSTRM_status(m_audio->m_stream, &status);
        SNDSTRM_requeststatus(status.request, &request);
        SNDSYS_leavecritical();
        int position = request.position - SNDPROFILE_outputlatency();
        if (position > m_audioPosition) {
            m_audioPosition = position;
            m_drift = (position - elapsed) + (m_drift - m_drift / 8);
            int drift = m_drift;
            int magnitude = -drift;
            if (drift > magnitude)
                magnitude = drift;
            if (magnitude > 264) {
                m_offset -= drift / 8;
                m_drift = 0;
                elapsed = time - m_offset;
            }
        }
    }
    m_time = (float)(unsigned int)(elapsed + m_startTime) * (0.001f * m_decoder->GetFrameRate());
    return !(m_time < (float)m_frame + 1.0f);
}

FRAME* AV_PLAYER::GetFrame(float time) {
    m_time = time;
    FRAME* frame = m_decoder->GetFrame((unsigned int)m_time);
    m_frame = m_decoder->GetCurrentFrameNumber();
    return frame;
}

FRAME* AV_PLAYER::GetFirstFrame(unsigned int value, int startTime) {
    STREAM_setpriority(m_audioStream, 150, 50);
    STREAM_setpriority(m_stream, 150, 50);
    if (m_videoFile) {
        int size;
        ASYNCFILE_release(m_videoFile, &m_videoData, &size);
        m_videoQueue = STREAM_queuemem(m_stream, m_videoData, size, 0);
    }
    if (m_audioFile) {
        int size;
        ASYNCFILE_release(m_audioFile, &m_audioData, &size);
        m_audioQueue = STREAM_queuemem(m_audioStream, m_audioData, size, 0);
    }
    if (!m_separateAudio)
        m_audioQueue = m_videoQueue;
    if (m_noAudio == 0) {
        m_audio = new AUDIO_PLAYER(m_audioStream, m_audioQueue);
        m_audioSynced = m_audio->m_stream >= 0;
    } else {
        m_audioSynced = 0;
    }
    m_decoder = 0;
    if (rcmp_sys.IsReady()) {
        m_streamer.m_player = this;
        CODEC_IDATA idata(&m_streamer, StaticGetRCMPChunk, StaticReleaseRCMPChunk, value);
        m_decoder = new DECODER(&idata);
    }
    FRAME* frame = m_decoder->GetFrame(0);
    m_timer = new AV_MS_TIMER;
    m_offset = m_timer->GetTime();
    m_audioPosition = 0;
    m_drift = 0;
    m_startTime = startTime;
    if (m_audio)
        m_audio->StartSound();
    UnPause();
    return frame;
}
