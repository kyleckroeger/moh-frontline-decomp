// ShellMenu.cpp: CShellMenu::PlayBriefingMovie. Class layouts are inferred views of what this function
// uses. ShellMenu_block.h recreates the constant block its offsets depend on.
#include "ShellMenu_block.h"

extern "C" int sprintf(char*, const char*, ...);
void* DWI_alloc(const char*, int, int);
void UpdateREAL();
void WaitRenderList();
void WaitScreen();
void SendRenderList();
void SoundSetVolumes(float, float);
enum ERenderPriority {};
struct SSndStreamStatus {
    unsigned char unknown00[12];
};
struct SSndStrmRequestStatus {
    int m_state;
    unsigned char unknown04[12];
};
class CSoundSysLock {
public:
    CSoundSysLock();
    ~CSoundSysLock();
};
class CSoundStream {
public:
    CSoundStream(int, int);
    ~CSoundStream();
    int GetVolume() const;
    void FadeVolume(int, int);
    void SetVolume(int);
    void Pause();
    void Unpause();
    void PurgeQueue();
    int QueueFile(const char*, int, int);
    void GetStatus(SSndStreamStatus&) const;
    void GetRequestStatus(int, SSndStrmRequestStatus&) const;
    unsigned char unknown00[32];
};
extern CSoundStream* pSoundStream;
extern int g_soundStreamRequest;
struct CRenderBinData {
    unsigned char unknown00[28];
};
class CSprite : public CRenderBinData {
public:
    enum ESpriteLoadType {};
    enum EPolyPrimType {};
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    unsigned char unknown20[68];
};
#include "game/CInputManager.h"

extern CInputManager g_inputMgr;
struct AudioOptionsView {
    signed char m_settings[3];
    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
};
class CShellMenu {
public:
    void PlayBriefingMovie(int);
    unsigned char unknown00[40];
    AudioOptionsView m_audio;
};

static inline int FadeTheVolume(int target) {
    int old = 0;
    if (pSoundStream) {
        CSoundSysLock lock;
        old = pSoundStream->GetVolume();
        pSoundStream->FadeVolume(1500, target);
    }
    if (pSoundStream) {
        int volume;
        do {
            UpdateREAL();
            CSoundSysLock lock;
            volume = pSoundStream->GetVolume();
        } while (volume != target);
    }
    return old;
}
void CShellMenu::PlayBriefingMovie(int briefing) {
    CSprite** first;
    int oldVolume = FadeTheVolume(0);
    int frame = 0;
    int fade = 128;
    if (pSoundStream) {
        CSoundSysLock lock;
        pSoundStream->Pause();
    }
    UpdateREAL();
    CSoundStream* stream = new CSoundStream(1, 50);
    char soundName[128];
    sprintf(soundName, "data\\shell\\brief%d1.asf", briefing);
    int slide = 0;
    SMBBriefingCounts counts = s_briefingCounts;
    SMBBriefingTimes times = s_briefingTimes;
    int i;
    for (i = 0; i < 6; i++) {
        for (int j = 0; j < 11; j++)
            times.v[i][j] /= 2;
    }
    CSprite* sprites[11];
    first = sprites;
    CSprite** next = first;
    char name[128];
    for (i = 0; i < counts.v[briefing - 1]; i++) {
        sprintf(name, "slide%d%d.gsh", briefing, i + 1);
        *next = new CSprite(name, (CSprite::ESpriteLoadType)4, (ERenderPriority)52);
        next++;
    }
    g_soundStreamRequest = stream->QueueFile(soundName, 50, 0);
    {
        CSoundSysLock lock;
        stream->SetVolume(127.0f * (float)(m_audio.Get(1) - 1) / 6.0f);
    }
    while (true) {
        SMBQuad quad = s_briefingQuad;
        if (frame < 75) {
            fade = (frame << 7) / 75;
        } else {
            int left = times.v[briefing - 1][slide] - frame;
            if (left < 55)
                fade = (left * 128) / 55;
        }
        ((unsigned char*)&quad.v[0].color)[3] = fade;
        ((unsigned char*)&quad.v[1].color)[3] = fade;
        ((unsigned char*)&quad.v[2].color)[3] = fade;
        ((unsigned char*)&quad.v[3].color)[3] = fade;
        sprites[slide]->DrawPoly(quad.v, 4, (CSprite::EPolyPrimType)5);
        UpdateREAL();
        WaitRenderList();
        WaitScreen();
        SendRenderList();
        sprites[slide]->DrawPoly(quad.v, 4, (CSprite::EPolyPrimType)5);
        WaitRenderList();
        WaitScreen();
        SendRenderList();
        g_inputMgr.Update(1.0f);
        if (g_inputMgr.GetValueAnyPlayer(128)) {
            FadeTheVolume(0);
            stream->PurgeQueue();
            delete stream;
            break;
        }
        SSndStreamStatus status;
        SSndStrmRequestStatus request;
        {
            CSoundSysLock lock;
            stream->GetStatus(status);
            stream->GetRequestStatus(g_soundStreamRequest, request);
        }
        if (request.m_state == 3) {
            stream->PurgeQueue();
            delete stream;
            break;
        }
        frame++;
        if (frame % times.v[briefing - 1][slide] == 0) {
            frame = 0;
            slide = (slide + 1) % counts.v[briefing - 1];
        }
    }
    for (i = 0; i < counts.v[briefing - 1]; i++) {
        delete *first;
        first++;
    }
    int effects = m_audio.Get(0) - 1;
    int music = m_audio.Get(1) - 1;
    SoundSetVolumes((float)effects / 6.0f, (float)music / 6.0f);
    if (pSoundStream) {
        CSoundSysLock lock;
        pSoundStream->Unpause();
    }
    UpdateREAL();
    FadeTheVolume(oldVolume);
}
