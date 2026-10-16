// A fragment of animplayer.cpp (0x8010a8cc): CAnimatedPlayer::InitAI, which
// is empty, and CAnimatedPlayer::Init: CAnimObject's Init, cleared weapon
// tables, the morph initialised on the object's frames, cleared face blend
// targets, a random value among 13, 6, 7 and 5 at +0x4480, the soldier mood
// (2 in multiplayer, else 0), an animated sphere volume (radius 1.2, owned by
// the object) and a box volume (basis axes, 0.75 x 1.6 x 0.6, centred at
// height 0.8) with their world copies, collision id 28 and the reset member
// state. The classes and functions are named by the symbols; the members are
// inferred from offsets (the face blend members as in animplayer.cpp), the
// class is a view whose virtual functions other than SetCollisionId are
// placeholders, and the two reset helpers are inferred inlines (names not
// original) that reproduce the repeated stores. The rest of the file is in
// other units.
extern "C" void* memset(void*, int, unsigned long);
extern "C" int rand();

struct CharFrame_t;
class CAnimObject;
enum EClsnId {};
enum EMoodType {};

struct CVector3Pair {
    double pair[2];
};

class CVector3 {
public:
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CVolSphere {
public:
    void SetRadius(float);
    void SetCenter(CVector3);

    unsigned char unknown000[1968];
    void* m_owner;
};

class CVolBox {
public:
    void SetBasis(CVector3, CVector3, CVector3);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);
};

class CAnimMorph {
public:
    void Init(CAnimObject*, unsigned short, CharFrame_t*);
};

class CSoldierObject {
public:
    void SetMood(EMoodType);
};

class CAnimObject {
public:
    void Init();
};

void* AllocateBoxVolume();
void* AllocateAnimatedVolume();

extern bool g_bInMultiplayerMode;

class CAnimatedPlayer {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void SetCollisionId(EClsnId);
    void InitAI();
    void Init();
    void ResetState4498() {
        m_flag4498 = false;
        m_vectorX = m_vectorY = m_vectorZ = 0.0f;
    }
    void ResetState() {
        m_flag3fc0 = false;
        m_flag4280 = false;
        m_value43a0 = -1;
        m_value43a4 = 1;
        m_value43a8 = 40;
        m_value43ac = 0.75f;
        m_flag43b0 = false;
        m_value43bc = 0.0f;
        m_self = this;
        m_value4438 = 1.0f;
        m_flag43c1 = false;
        m_value43c4 = 0;
        m_value43c8 = 0;
        m_value315c = 1;
        m_value3150 = 0.0f;
        m_value3140 = 0.0f;
        m_value3144 = 0.0f;
        m_value3148 = 0.0f;
        m_value314c = 0;
        m_value3154 = 0;
        m_flag3160 = false;
        m_flag3161 = false;
        m_flag4484 = false;
        m_value4488 = 0;
        m_value448c = 0;
        m_value4490 = 0.0f;
        m_value4494 = 0;
        ResetState4498();
    }

    unsigned char unknown0004[3324];
    unsigned char m_frames[5760];
    void* m_localVolume3;
    void* m_localVolume;
    void* m_worldVolume3;
    void* m_worldVolume;
    unsigned char unknown2390[8];
    CVolSphere* m_sphere;
    unsigned char unknown239c[3492];
    float m_value3140;
    float m_value3144;
    float m_value3148;
    int m_value314c;
    float m_value3150;
    int m_value3154;
    unsigned char unknown3158[4];
    int m_value315c;
    bool m_flag3160;
    bool m_flag3161;
    unsigned char unknown3162[14];
    CAnimMorph m_morph;
    unsigned char unknown3171[3663];
    bool m_flag3fc0;
    unsigned char unknown3fc1[3];
    int m_value3fc4;
    unsigned char unknown3fc8[72];
    unsigned char m_weapons[36];
    unsigned char m_value4034[32];
    unsigned char m_value4054[32];
    int m_currentWeapon;
    unsigned char unknown4078[520];
    bool m_flag4280;
    unsigned char unknown4281[79];
    long m_morphTarget;
    long m_blendTargets[2];
    float m_blendWeights[2];
    float m_blendMax[2];
    float m_blendSpeeds[2];
    unsigned char unknown4318[40];
    bool m_statePending;
    unsigned char unknown431d[131];
    int m_value43a0;
    int m_value43a4;
    int m_value43a8;
    float m_value43ac;
    bool m_flag43b0;
    unsigned char unknown43b1[11];
    float m_value43bc;
    unsigned char unknown43c0;
    bool m_flag43c1;
    unsigned char unknown43c2[2];
    int m_value43c4;
    int m_value43c8;
    unsigned char unknown43cc[104];
    CAnimatedPlayer* m_self;
    float m_value4438;
    unsigned char unknown443c[68];
    int m_value4480;
    bool m_flag4484;
    unsigned char unknown4485[3];
    int m_value4488;
    int m_value448c;
    float m_value4490;
    int m_value4494;
    bool m_flag4498;
    unsigned char unknown4499[7];
    float m_vectorX;
    float m_vectorY;
    float m_vectorZ;
    unsigned char unknown44ac[8];
    float m_flocoParam;
    long m_flocoCount;
};

void CAnimatedPlayer::InitAI() {
}

void CAnimatedPlayer::Init() {
    ((CAnimObject*)this)->Init();
    memset(m_value4034, 0, 32);
    memset(m_value4054, 0, 32);
    memset(m_weapons, 0, 36);
    m_value3fc4 = 0;
    m_morph.Init((CAnimObject*)this, 0, (CharFrame_t*)m_frames);
    m_morphTarget = -1;
    m_blendTargets[1] = -1;
    m_blendTargets[0] = -1;
    m_blendWeights[1] = 0.0f;
    m_blendWeights[0] = 0.0f;
    m_blendMax[1] = 0.0f;
    m_blendMax[0] = 0.0f;
    m_blendSpeeds[1] = 0.0f;
    m_blendSpeeds[0] = 0.0f;
    int roll = rand() % 100;
    if (roll < 25)
        m_value4480 = 13;
    else if (roll < 50)
        m_value4480 = 6;
    else if (roll < 75)
        m_value4480 = 7;
    else
        m_value4480 = 5;
    if (g_bInMultiplayerMode)
        ((CSoldierObject*)this)->SetMood((EMoodType)2);
    else
        ((CSoldierObject*)this)->SetMood((EMoodType)0);
    m_statePending = false;
    CVolSphere* sphere = (CVolSphere*)AllocateAnimatedVolume();
    sphere->SetRadius(1.2f);
    sphere->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
    sphere->m_owner = this;
    m_sphere = sphere;
    m_localVolume = sphere;
    m_worldVolume = AllocateAnimatedVolume();
    CVolBox* box = (CVolBox*)AllocateBoxVolume();
    box->SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
    box->SetWidth(0.75f);
    box->SetHeight(1.6f);
    box->SetDepth(0.6f);
    box->SetCenter(CVector3(0.0f, 0.0f, 0.8f));
    m_localVolume3 = box;
    m_worldVolume3 = AllocateBoxVolume();
    SetCollisionId((EClsnId)28);
    m_currentWeapon = 0;
    ResetState4498();
    ResetState();
    ResetState();
    m_flocoParam = 0.0f;
    m_flocoCount = 0;
}
