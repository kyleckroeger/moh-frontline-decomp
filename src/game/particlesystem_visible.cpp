// A fragment of particlesystem.cpp (0x8007f7ec): CParticleSystem::IsVisible.
// An inactive system is not visible. Otherwise a box volume with the unit
// axes and the system's bounding corners is moved into the world by the
// system's local-to-world matrix and tested by the file-local IsBoxVisible;
// the system's centre is transformed into the world as well and its squared
// distance from the camera (the draw context's scene node at +212) is kept
// at +112. The file name is this project's; the original record is
// particlesystem.cpp. The classes and functions are named by the mangled
// symbols; the members, the draw-context view, the vector constructors and
// the result types are inferred, and the unit-axis constants are entries of
// the file's .sdata2 pool. The virtuals are declared in table order up to
// GetLocalToWorld (as in particlesystem_link.cpp); the destructor is declared
// first, so the table stays elsewhere.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

class CDmaTag;
class CDmaPacket;

/* Inferred: vectors are copied as two doubles. */
struct CVector3Pair {
    double d[2];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& o) { *(CVector3Pair*)this = *(const CVector3Pair*)&o; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void TransformPoint(CVector3&, CVector3) const;
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCorners(CVector3, CVector3);
    void TransformedCopy(const IVolume&, const CMatrix&);

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    VECTOR3VIEW m_basis[3];
    VECTOR3VIEW m_center;
};

/* Only GetPosition (table offset 64) of the camera node is used. */
class ISceneNode {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void GetPosition(CVector3&) const;
};

/* Inferred: the camera node at +212. */
class CDrawContext {
public:
    unsigned char unknown000[212];
    ISceneNode* m_camera;
};

static bool IsBoxVisible(const CVolBox&, CDrawContext&);

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    virtual ~CParticleSystem();
    virtual void DeActivate();
    virtual void Terminate();
    virtual bool IsActive() const;
    virtual void IsEmitting() const;
    virtual void IsBoundingBoxValid() const;
    virtual void IsMoving() const;
    virtual void HasRotation() const;
    virtual void GetTexture() const;
    virtual void GetRenderType() const;
    virtual void GetSeed() const;
    virtual void GetEmmisionRate() const;
    virtual void GetEmmisionDelay() const;
    virtual void GetSystemLifetime() const;
    virtual void GetParticleLifetime() const;
    virtual void GetLocalToWorld(CMatrix&) const;
    bool IsVisible(CDrawContext&);

    CVector3 m_boundsMin;
    CVector3 m_boundsMax;
    unsigned char unknown40[32];
    CVector3 m_center;
    float m_cameraDistSq;
};

bool CParticleSystem::IsVisible(CDrawContext& context) {
    if (!IsActive())
        return false;

    CVolBox box;
    CMatrix localToWorld;
    GetLocalToWorld(localToWorld);
    box.SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
    box.SetCorners(m_boundsMin, m_boundsMax);
    box.TransformedCopy(box, localToWorld);
    bool visible = IsBoxVisible(box, context);
    CVector3 center;
    localToWorld.TransformPoint(center, m_center);
    CVector3 camera;
    context.m_camera->GetPosition(camera);
    float dx = center.x - camera.x;
    float dy = center.y - camera.y;
    float dz = center.z - camera.z;
    m_cameraDistSq = dx * dx + dy * dy + dz * dz;
    return visible;
}

inline float Dot(const CVector3& a, const CVector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* Inferred: a vector scaled by a factor. */
inline CVector3 Scale(float s, const CVector3& v) {
    CVector3 r;
    r.x = s * v.x;
    r.y = s * v.y;
    r.z = s * v.z;
    return r;
}

/* Inferred: a frustum plane (normal first, distance at +16, 32 bytes). */
struct FrustumPlaneView {
    CVector3 normal;
    float d;
    unsigned char unknown14[12];

    float Distance(CVector3 p) const { return Dot(normal, p) - d; }
};

static bool IsBoxVisible(const CVolBox& box, CDrawContext& context) {
    CVector3 c = *(const CVector3*)&box.m_center;
    CVector3 b0 = *(const CVector3*)&box.m_basis[0];
    CVector3 b1 = *(const CVector3*)&box.m_basis[1];
    CVector3 b2 = *(const CVector3*)&box.m_basis[2];
    const FrustumPlaneView* plane = (const FrustumPlaneView*)((char*)context.m_camera + 0x150);
    CVector3 a0;
    a0.x = box.m_halfWidth * b0.x;
    a0.y = box.m_halfWidth * b0.y;
    a0.z = box.m_halfWidth * b0.z;
    CVector3 a1;
    a1.x = box.m_halfDepth * b1.x;
    a1.y = box.m_halfDepth * b1.y;
    a1.z = box.m_halfDepth * b1.z;
    CVector3 a2;
    a2.x = box.m_halfHeight * b2.x;
    a2.y = box.m_halfHeight * b2.y;
    a2.z = box.m_halfHeight * b2.z;
    for (int i = 0; i < 6; i++) {
        float r = (float)fabs(Dot(a0, plane->normal)) + (float)fabs(Dot(a1, plane->normal)) + (float)fabs(Dot(a2, plane->normal));
        if (plane->Distance(c) > r)
            return false;
        plane++;
    }
    return true;
}
