// The end of particlesystem.cpp (0x8008128c): weak copies of header inlines
// emitted in this file: CParticleSystem::IsEmitting (a flag bit at +136) and
// the empty default setters (render type, texture, emission rate and delay,
// lifetimes, particle colour, size, position, velocity and acceleration, fog
// and system acceleration), then the static initialisation of the file's
// matrices (each inline constructor initialises the matrix class once; the
// model-to-screen matrix is set to the identity), the particle system manager
// (a CUcodeRenderBin with ISceneNode as a second base at +32: base tables,
// members cleared, priority 7, destructor registered) and the camera forward
// vector (0, 0, -1; its constants are entries of the file's .sdata2 pool).
// CParticleSystem, ShapeFile, CColor, CVector3, ERenderType and the globals
// are named by the mangled symbols; CParticleSystem is a non-virtual view and
// its flag bit-field, the manager's members, the vector constructor and the
// identity-matrix constructor are inferred. The weak this-adjusting thunks
// after the initialiser belong to the manager's table, which is emitted with
// its key function elsewhere in the file; they are not part of this unit.
struct ShapeFile;

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

#include "game/CVector3.h"

class CParticleSystem {
public:
    enum ERenderType {};

    bool IsEmitting() const;
    void SetRenderType(ERenderType);
    void SetTexture(ShapeFile*);
    void SetEmmisionRate(float);
    void SetEmmisionDelay(float);
    void SetSystemLifetime(float);
    void SetParticleLifetime(float);
    void SetParticleColor(CColor, CColor);
    void SetParticleSize(CVector3, CVector3, float);
    void SetParticlePosition(CVector3, CVector3);
    void SetParticleVelocity(CVector3, CVector3);
    void SetParticleAcceleration(CVector3);
    void SetFogEnable(bool);
    void SetSystemAcceleration(CVector3);

    unsigned char unknown000[136];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

__declspec(weak) bool CParticleSystem::IsEmitting() const {
    return m_emitting;
}

__declspec(weak) void CParticleSystem::SetRenderType(ERenderType) {
}

__declspec(weak) void CParticleSystem::SetTexture(ShapeFile*) {
}

__declspec(weak) void CParticleSystem::SetEmmisionRate(float) {
}

__declspec(weak) void CParticleSystem::SetEmmisionDelay(float) {
}

__declspec(weak) void CParticleSystem::SetSystemLifetime(float) {
}

__declspec(weak) void CParticleSystem::SetParticleLifetime(float) {
}

__declspec(weak) void CParticleSystem::SetParticleColor(CColor, CColor) {
}

__declspec(weak) void CParticleSystem::SetParticleSize(CVector3, CVector3, float) {
}

__declspec(weak) void CParticleSystem::SetParticlePosition(CVector3, CVector3) {
}

__declspec(weak) void CParticleSystem::SetParticleVelocity(CVector3, CVector3) {
}

__declspec(weak) void CParticleSystem::SetParticleAcceleration(CVector3) {
}

__declspec(weak) void CParticleSystem::SetFogEnable(bool) {
}

__declspec(weak) void CParticleSystem::SetSystemAcceleration(CVector3) {
}

class CDmaTag;
class CDmaPacket;

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    /* Inferred: a constructor that also sets the identity. */
    CMatrix(bool identity) {
        if (!s_ClassInit)
            InitClass();
        if (identity)
            Ident();
    }
    static void InitClass();
    void Ident();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CUcodeRenderBin() {}
};

class CParticleSystemManager : public CUcodeRenderBin, public ISceneNode {
public:
    virtual void Init();
    CParticleSystemManager()
        : data54(0), data58(0), data5c(0), data60(0), data64(0), data68(0), data6c(0), data70(0), data74(0),
          data78(0) {
        m_priority = 7;
    }
    virtual ~CParticleSystemManager();

    int unknown44[4];
    int data54;
    int data58;
    int data5c;
    int data60;
    int data64;
    int data68;
    int data6c;
    int data70;
    int data74;
    int data78;
};

static CMatrix g_ModelToScreenMatrix(true);
static CVector3 g_XAxis;
static CVector3 g_YAxis;
static CVector3 g_ZAxis;
static CMatrix g_WorldToCamera;
static CParticleSystemManager g_ParticleSystemManager;
static CVector3 vCameraForward(0.0f, 0.0f, -1.0f);
