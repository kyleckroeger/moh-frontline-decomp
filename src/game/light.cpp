// light.cpp: CLight and its animated lights, the whole file, with the vtables of CLight,
// CPropertyAnimLight and CInstancedAnimLight (and the weak one of CAnimLightManager), the RTTI
// records of their chains, the default light set and its volume, the animated-light manager
// and its static initialisation: the weak CInstancedAnimLight destructor and CLight casts; the
// manager's Create overloads, CInstancedAnimLight's Destroy and constructor;
// CPropertyAnimLight's BeginUpdate, Destroy, destructor and constructor; CLight's matrix rows
// and transforms, IsVisible and the empty update hooks, the destructor and SetDefaultLight;
// the default light volume, MarkForDestruction, Destroy, Create, Register, ResetClass and
// InitClass; the weak manager destructor and Destroy; __sinit_light_cpp.
//
// The names come from the mangled symbols; the class chains and the vtables from the RTTI
// records and __vt__6CLight, __vt__18CPropertyAnimLight, __vt__19CInstancedAnimLight and
// __vt__17CAnimLightManager (ISceneNode's and IMovingSceneNode's slots from the shared headers).
//
// Inferred, not in any mangled name: the data members (named by offset or role), the result
// types, the inline helpers (CLight's constructor-time updates, SetRange and rows, the light
// pools' Alloc/Free/Reset/Init) and the views of the animated-light record, the default light
// records (research/data/init/compartment.cpp's views; the data moved here from there), the
// GX light object, the colour and the sphere volume. CVector3 is the shared header with the
// assignment through the double-pair union; SetDefaultLight normalises a double-pair/float
// union view of the direction (the shared vector swaps two registers there).
// The unit is compiled -inline auto,deferred, which emits the functions in reverse source
// order (and lets the weak CInstancedAnimLight destructor inline the later destructors), so
// they are written last to first.
//
// DestroyAnimLightManager is a gap function (accepted by the maintainer 2026-11-13,
// docs/Remaining.md): none of the 45 kept functions uses CAnimLightManager's virtual table,
// yet the original emits that weak table (and the weak ~CAnimLightManager/Destroy) where a
// function of the file that deletes a manager would put it. Its name and body are this
// project's; the linker strips it, as the original's was.
/* inferred: this file's CVector3 is the shared game/CVector3.h with the assignment through the
   double-pair union */
#define GAME_CVECTOR3_H
#include "game/msl_sqrtf.h"

struct CVector3XYZW {
    float x, y, z, w;
};

union CVector3Data {
    double pair[2];
    float v[4];
    CVector3XYZW c;
};

struct CVector3Pair {
    double a;
    double b;
};

struct CVector3Base {
    float x, y, z, w;
} __attribute__((aligned(8)));

class CVector3 : public CVector3Base {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    CVector3(const CVector3& other) : CVector3Base(other) {}
    CVector3& operator=(const CVector3& other) {
        *(CVector3Data*)this = *(const CVector3Data*)&other;
        return *this;
    }

    void Set(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    float LengthSq() const { return x * x + y * y + z * z; }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    float Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            x *= scale;
            y *= scale;
            z *= scale;
        }
        return length;
    }
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    void DotDotDot(const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& v) {
        x = a.x * v.x + a.y * v.y + a.z * v.z;
        y = b.x * v.x + b.y * v.y + b.z * v.z;
        z = c.x * v.x + c.y * v.y + c.z * v.z;
    }
    void Abs(const CVector3& o) { x = __fabs(o.x); y = __fabs(o.y); z = __fabs(o.z); }
    CVector3& operator+=(const CVector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    CVector3& operator-=(const CVector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; }
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }
    void Unitize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    void Constrain(const CVector3&, float);
} __attribute__((aligned(8)));


#include "game/IMovingSceneNode.h"
#include "game/CScene.h"

extern CScene g_scene;
inline void* operator new(unsigned long, void* p) { return p; }
void* DWI_alloc(const char*, int, int);

/* inferred: the GX light object and colour */
typedef struct {
    unsigned char r, g, b, a;
} GXColor;

typedef struct {
    unsigned long dummy[16];
} GXLightObj;

extern "C" void GXInitLightColor(GXLightObj*, GXColor);
extern "C" void GXInitLightDir(GXLightObj*, float, float, float);
extern "C" void GXInitLightDistAttn(GXLightObj*, float, float, int);
extern "C" void GXInitLightSpot(GXLightObj*, float, int);

/* inferred: SetDefaultLight's working copy of the direction, a double-pair/float union */
struct LightDirectionView {
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* inferred: a property-data light record and the light volume that holds a set of them */
struct BPDLight {
    int type;
    float position[3];
    float direction[3];
    float color[3];
    float intensity;
    int unknown2c;
};

struct PropPlane4;

struct BPDLightVolume {
    float centre[3];
    float unknown0c;
    float unknown10;
    float unknown14;
    int planeCount;
    PropPlane4* planes;
    int lightCount;
    BPDLight* lights;
    int unknown28;
    int unknown2c;
};

BPDLight g_DefaultLights[4] = {
    {1, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {16.0f, 16.0f, 16.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {64.0f, 64.0f, 64.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {128.0f, 128.0f, 128.0f}, 1.0f, 0},
};

BPDLightVolume g_DefaultLightVolume = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0, 0, 4, g_DefaultLights, 0, 0};

/* inferred: an animated light's record (position, range, mode, frames, colours, intensities) */
struct MOH_animatedLight_Struct {
    unsigned char unknown00[16];
    float position[3];
    unsigned char unknown1c[18];
    short range;
    unsigned char unknown30[24];
    unsigned long crc;
    unsigned char unknown4c[36];
    unsigned char mode;
    unsigned char unknown71;
    short frameCount;
    unsigned char (*colors)[4];
    float* intensities;
};

class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual bool TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual bool TestCollision(CVector3, CCollision&, bool) const;
    virtual bool TestCollision(const CLine3&, CCollision&, bool) const;
    virtual bool TestCollision(const CPlane&, CCollision&, bool) const;
    virtual bool TestCollision(const CTriangle&, CCollision&, bool) const;
};

/* the sphere volume, built on the stack for the visibility test */
class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
    bool TestVisibility(CDrawContext&) const;
    CVolSphere(const CVector3& center, float radius) {
        m_radius = radius;
        m_center = center;
    }

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
} __attribute__((aligned(16)));

class CLight : public IMovingSceneNode {
public:
    enum EDefaultLights {};

    CLight() : data24(0), data28(0) {
        m_tm.Ident();
        m_color[0] = 0.0f;
        m_color[1] = 0.0f;
        m_color[2] = 0.0f;
        m_color[3] = 1.0f;
        m_spotAngle = 90.0f;
        m_range = 10.0f;
        UpdateColor();
        UpdateDirection();
        UpdateAttenuation();
        UpdateSpot();
    }
    virtual void MarkForDestruction(int);
    virtual ~CLight();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual CLight* AsLight() { return this; }
    virtual const CLight* AsLight() const { return this; }
    virtual void Reset();
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();

    static BPDLightVolume* GetDefaultLightVolume();
    static CLight* Create(void*);
    static CLight* Create(unsigned long);
    static void Register(void*, int);
    static void ResetClass();
    static void InitClass(CScene*);
    static void SetDefaultLight(EDefaultLights, CVector3, CColor, float);

    void UpdateColor() {
        GXColor color;
        color.r = (unsigned char)(255.0f * m_color[0]);
        color.g = (unsigned char)(255.0f * m_color[1]);
        color.b = (unsigned char)(255.0f * m_color[2]);
        color.a = 255;
        GXInitLightColor(&m_light, color);
    }
    void UpdateDirection() {
        CVector3 dir;
        dir = m_tm.GetRow(1);
        GXInitLightDir(&m_light, dir.x, dir.y, dir.z);
    }
    void UpdateAttenuation() { GXInitLightDistAttn(&m_light, 0.5f * m_range, 0.15f, 2); }
    void UpdateSpot() { GXInitLightSpot(&m_light, (180.0f / 3.1415927f) * m_spotAngle, 2); }
    void SetRange(float range) {
        m_range = range;
        GXInitLightDistAttn(&m_light, 0.5f * range, 0.15f, 2);
    }
    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.front; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.pos; }

    int data24;
    int data28;
    GXLightObj m_light;
    unsigned char unknown6c[4];
    CMatrix m_tm;
    float m_color[4];
    float m_spotAngle;
    float m_range;
    unsigned char unknownc8[8];
};

class CPropertyAnimLight : public CLight {
public:
    CPropertyAnimLight(MOH_animatedLight_Struct*);
    virtual ~CPropertyAnimLight();
    virtual void Destroy();
    virtual void BeginUpdate(float);

    MOH_animatedLight_Struct* m_data;
    short m_frame;
    short m_reverse : 1;
    short m_unused : 15;
    unsigned char unknownd8[8];
    CPropertyAnimLight* m_poolNext;
    unsigned char unknowne4[12];
};

class CInstancedAnimLight : public CPropertyAnimLight {
public:
    CInstancedAnimLight(MOH_animatedLight_Struct*);
    virtual ~CInstancedAnimLight() {}
    virtual void Destroy();
};

/* inferred: a fixed pool of lights with in-use and free lists linked through the light */
template <class T>
struct LightPool {
    LightPool() : m_data(0), m_used(0), m_free(0), m_capacity(0), m_count(0) {}
    T* Alloc() {
        T* light = m_free;
        if (!light)
            return 0;
        m_free = (T*)light->m_poolNext;
        light->m_poolNext = m_used;
        m_used = light;
        m_count++;
        return light;
    }
    void Free(T* light) {
        if (light == m_used) {
            m_used = (T*)light->m_poolNext;
        } else {
            for (T* prev = m_used; prev; prev = (T*)prev->m_poolNext) {
                if (prev->m_poolNext == light) {
                    prev->m_poolNext = light->m_poolNext;
                    break;
                }
            }
        }
        m_count--;
        light->m_poolNext = m_free;
        m_free = light;
    }
    void Reset() {
        m_used = 0;
        m_free = m_data;
        m_count = 0;
        m_data[m_capacity - 1].m_poolNext = 0;
        for (int i = 0; i < m_capacity - 1; i++)
            m_free[i].m_poolNext = &m_free[i + 1];
    }
    void Init(int capacity) {
        m_capacity = capacity;
        m_data = (T*)DWI_alloc(0, m_capacity * sizeof(T), 1024);
        Reset();
    }

    T* m_data;
    T* m_used;
    T* m_free;
    int m_capacity;
    int m_count;
};

class CAnimLightManager : public ISceneNode {
public:
    virtual ~CAnimLightManager() {}
    virtual void Destroy() {}

    CPropertyAnimLight* Create(MOH_animatedLight_Struct*);
    CInstancedAnimLight* Create(unsigned long);

    MOH_animatedLight_Struct* m_lights;
    int m_lightCount;
    CScene* m_scene;
    LightPool<CInstancedAnimLight> m_instanced;
    LightPool<CPropertyAnimLight> m_animated;
};

static CAnimLightManager g_AnimLightManager;
static bool g_bReady;
// Gap function, stripped at link (see the file comment).
void DestroyAnimLightManager(CAnimLightManager* manager) {
    delete manager;
}

void CLight::InitClass(CScene* scene) {
    g_AnimLightManager.m_scene = scene;
    g_AnimLightManager.m_animated.Init(10);
    g_AnimLightManager.m_instanced.Init(10);
    g_bReady = true;
}

void CLight::ResetClass() {
    CPropertyAnimLight* node;
    for (node = g_AnimLightManager.m_instanced.m_used; node; node = node->m_poolNext)
        g_AnimLightManager.m_scene->Remove(*node);
    for (node = g_AnimLightManager.m_animated.m_used; node; node = node->m_poolNext)
        g_AnimLightManager.m_scene->Remove(*node);
    g_AnimLightManager.m_animated.Reset();
    g_AnimLightManager.m_instanced.Reset();
}

void CLight::Register(void* lights, int count) {
    g_AnimLightManager.m_lights = (MOH_animatedLight_Struct*)lights;
    g_AnimLightManager.m_lightCount = count;
}

CLight* CLight::Create(unsigned long id) {
    return g_AnimLightManager.Create(id);
}

CLight* CLight::Create(void* data) {
    return g_AnimLightManager.Create((MOH_animatedLight_Struct*)data);
}

void CLight::Destroy() {
}

void CLight::MarkForDestruction(int flag) {
    if (g_scene.IsNodeInScene((ISceneNode*)this))
        g_scene.Remove(*(ISceneNode*)this);
    ISubject::MarkForDestruction(flag);
}

BPDLightVolume* CLight::GetDefaultLightVolume() {
    return &g_DefaultLightVolume;
}

void CLight::SetDefaultLight(EDefaultLights light, CVector3 direction, CColor color, float intensity) {
    float red;
    float green;
    float blue;
    float luma;
    float cb;
    float cr;
    float r;
    float g;
    float b;
    LightDirectionView dir;
    red = color.r;
    green = color.g;
    blue = color.b;
    luma = 0.299 * red + 0.587 * green + 0.114 * blue;
    cb = 128.0 + (-0.1687 * red - 0.3313 * green + 0.5 * blue);
    cr = 128.0 + (0.5 * red - 0.4187 * green - 0.0813 * blue);
    luma *= 1.5f;
    r = luma + 1.402 * (cr - 128.0f);
    g = luma - 0.34414 * (cb - 128.0f) - 0.71414 * (cr - 128.0f);
    b = luma + 1.772 * (cb - 128.0f);
    r /= 255.0f;
    g /= 255.0f;
    b /= 255.0f;
    dir = *(const LightDirectionView*)&direction;
    dir.Normalize();
    g_DefaultLights[light].direction[0] = dir.d.v[0];
    g_DefaultLights[light].direction[1] = dir.d.v[1];
    g_DefaultLights[light].direction[2] = dir.d.v[2];
    g_DefaultLights[light].color[0] = r;
    g_DefaultLights[light].color[1] = g;
    g_DefaultLights[light].color[2] = b;
    g_DefaultLights[light].intensity = intensity;
}

CLight::~CLight() {
}

void CLight::BeginUpdate(float) {
}

void CLight::AttemptUpdate(float) {
}

void CLight::CommitUpdate() {
}

bool CLight::IsVisible(CDrawContext& context) const {
    CVector3 position;

    GetPosition(position);
    CVolSphere sphere(position, m_range);
    return sphere.TestVisibility(context);
}

void CLight::Reset() {
    m_tm.Ident();
}

void CLight::PreTransform(const CMatrix& matrix) {
    m_tm.Multiply(matrix, m_tm);
}

void CLight::Transform(const CMatrix& matrix) {
    m_tm.Multiply(m_tm, matrix);
}

void CLight::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
}

void CLight::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
}

void CLight::SetBasis(CVector3 right, CVector3 forward, CVector3 up) {
    m_tm.SetRight(right);
    m_tm.SetFront(forward);
    m_tm.SetUp(up);
}

void CLight::Move(CVector3 delta) {
    m_tm.Translate(delta);
}

void CLight::Rotate(CVector3 axis, float angle) {
    m_tm.Rotate(axis, angle);
}

void CLight::Pitch(float angle) {
    m_tm.RotateX(angle);
}

void CLight::Roll(float angle) {
    m_tm.RotateY(angle);
}

void CLight::Yaw(float angle) {
    m_tm.RotateZ(angle);
}

void CLight::Orthonormalize() {
    m_tm.Orthonormalize();
}

void CLight::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

void CLight::GetPosition(CVector3& v) const {
    v = PositionRow();
}

void CLight::GetRightward(CVector3& v) const {
    v = RightRow();
}

void CLight::GetForward(CVector3& v) const {
    v = ForwardRow();
}

void CLight::GetUpward(CVector3& v) const {
    v = UpRow();
}

CPropertyAnimLight::CPropertyAnimLight(MOH_animatedLight_Struct* data) : m_data(data), m_frame(0), m_reverse(0) {
    SetPosition(CVector3(data->position[0], data->position[1], data->position[2]));
    SetRange((float)data->range / 16.0f);
}

CPropertyAnimLight::~CPropertyAnimLight() {
}

void CPropertyAnimLight::Destroy() {
    g_AnimLightManager.m_animated.Free(this);
}

void CPropertyAnimLight::BeginUpdate(float delta) {
    int frames = (int)delta;
    float intensity;
    switch (m_data->mode) {
    case 0:
        m_frame += frames;
        if (m_frame >= m_data->frameCount) {
            m_frame = m_data->frameCount - 1;
            MarkForDestruction(0);
        }
        break;
    case 1:
        m_frame += frames;
        while (m_frame >= m_data->frameCount)
            m_frame = m_frame - m_data->frameCount;
        break;
    case 2:
        for (int i = frames; i != 0; i--) {
            if (m_reverse) {
                m_frame--;
                if (m_frame <= 0) {
                    m_frame = 1;
                    m_reverse = 0;
                }
            } else {
                m_frame++;
                if (m_frame >= m_data->frameCount) {
                    m_frame = m_data->frameCount - 2;
                    m_reverse = 1;
                }
            }
        }
        break;
    }
    unsigned char* color = m_data->colors[m_frame];
    float r;
    float g;
    float b;
    float a;
    r = color[0];
    g = color[1];
    b = color[2];
    intensity = m_data->intensities[m_frame];
    a = color[3];
    r *= intensity;
    g *= intensity;
    b *= intensity;
    a *= intensity;
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
    m_color[3] = a;
}

CInstancedAnimLight::CInstancedAnimLight(MOH_animatedLight_Struct* data) : CPropertyAnimLight(data) {
}

void CInstancedAnimLight::Destroy() {
    g_AnimLightManager.m_instanced.Free(this);
}

CInstancedAnimLight* CAnimLightManager::Create(unsigned long crc) {
    for (int i = 0; i < m_lightCount; i++) {
        if (crc == m_lights[i].crc) {
            CInstancedAnimLight* light = m_instanced.Alloc();
            if (light) {
                light = new (light) CInstancedAnimLight(&m_lights[i]);
                m_scene->Add(*light);
                return light;
            }
            return 0;
        }
    }
    return 0;
}

CPropertyAnimLight* CAnimLightManager::Create(MOH_animatedLight_Struct* data) {
    void* memory = m_animated.Alloc();
    CPropertyAnimLight* light = (CPropertyAnimLight*)memory;
    if (memory) {
        new (light) CPropertyAnimLight(data);
        m_scene->Add(*light);
        return light;
    }
    return 0;
}
