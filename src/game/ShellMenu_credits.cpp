// ShellMenu.cpp: CShellMenu::PlayCreditsSlideShow. Class layouts are inferred views of what this function
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
class CInputManager {
public:
    void Update(float);
    bool GetValueAnyPlayer(unsigned long);
    unsigned char unknown[0x2500];
};
extern CInputManager g_inputMgr;
struct AudioOptionsView {
    signed char m_settings[3];
    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
};
class CShellMenu {
public:
    void PlayCreditsSlideShow();
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
void CShellMenu::PlayCreditsSlideShow() {
    CSprite** next;
    int* time;
    CSprite** current;
    CSprite** first;
    int oldVolume = FadeTheVolume(0);
    int frame = 0;
    int slide = 0;
    int fade = 128;
    if (pSoundStream) {
        CSoundSysLock lock;
        pSoundStream->Pause();
    }
    UpdateREAL();
    CSoundStream* stream = new CSoundStream(1, 50);
    char soundName[128];
    sprintf(soundName, "data\\shell\\credits.asf");
    SMBCreditTimes times = s_creditTimes;
    CSprite* sprites[20];
    char name[128];
    first = sprites;
    next = first;
    int i = 0;
    do {
        sprintf(name, "credits%d.gsh", i + 1);
        CSprite* sprite = new CSprite(name, (CSprite::ESpriteLoadType)4, (ERenderPriority)52);
        i++;
        *next = sprite;
        next++;
    } while (i < 20);
    g_soundStreamRequest = stream->QueueFile(soundName, 50, 0);
    {
        CSoundSysLock lock;
        stream->SetVolume(127.0f * (float)(m_audio.Get(1) - 1) / 6.0f);
    }
    time = times.v;
    current = first;
    while (true) {
        SMBQuad quad = s_creditQuad;
        if (frame < 75) {
            fade = (frame << 7) / 75;
        } else {
            int left = *time - frame;
            if (left < 55)
                fade = (left * 128) / 55;
        }
        ((unsigned char*)&quad.v[0].color)[3] = fade;
        ((unsigned char*)&quad.v[1].color)[3] = fade;
        ((unsigned char*)&quad.v[2].color)[3] = fade;
        ((unsigned char*)&quad.v[3].color)[3] = fade;
        (*current)->DrawPoly(quad.v, 4, (CSprite::EPolyPrimType)5);
        UpdateREAL();
        WaitRenderList();
        WaitScreen();
        SendRenderList();
        (*current)->DrawPoly(quad.v, 4, (CSprite::EPolyPrimType)5);
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
            g_soundStreamRequest = stream->QueueFile(soundName, 50, 0);
            CSoundSysLock lock;
            stream->SetVolume(127.0f * (float)(m_audio.Get(1) - 1) / 6.0f);
        }
        frame++;
        if (frame % *time == 0) {
            slide++;
            frame = 0;
            time++;
            current++;
            if (slide >= 20) {
                FadeTheVolume(0);
                stream->PurgeQueue();
                delete stream;
                break;
            }
        }
    }
    i = 0;
    do {
        delete *first;
        i++;
        first++;
    } while (i < 20);
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
