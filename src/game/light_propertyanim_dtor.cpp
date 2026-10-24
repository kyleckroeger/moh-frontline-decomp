// A fragment of light.cpp (0x80077bd4): CPropertyAnimLight::Destroy (the
// light, itself a node of the animated-light manager's second pool, unlinked
// from the in-use list and returned to the free list; the pool view and its
// Free inline are inferred, as in light_statics.cpp), then the
// CPropertyAnimLight destructor (its
// virtual table pointer, then the CLight destructor inlined (global, later in
// this file; how the original inlined it is not known, so this view declares
// it inline), then the inline IMovingSceneNode and ISceneNode destructors and
// IObserver's, and the object freed when asked). The file name is this
// project's; the original record is light.cpp. The classes and functions are
// named by the mangled symbols; only the virtuals the destructor needs are
// declared. Each class declares its own override (MarkForDestruction or
// Destroy, defined elsewhere; BeginUpdate for CPropertyAnimLight) first so
// the global virtual tables are not emitted here.
// IMovingSceneNode's and ISceneNode's destructors are inline and their
// tables weak in the original, so the compiler's copies are weak duplicates,
// linked to the original copies.
// Then the CPropertyAnimLight constructor (0x80077cd0): the inline base
// constructors (table pointers, ISceneNode's zeroed members) and CLight's
// inline constructor (identity matrix, black colour with alpha 1, a 90 degree
// spot and range 10, then the GX light object's colour, direction (the
// matrix's second row, copied through a local), distance attenuation and
// spot set up through inline helpers), then the record pointer, frame and
// reverse bit (member initialisers: the bit is written as a halfword), the
// position from the record (virtual setter) and the range (record value / 16;
// attenuation from the new value). The GX declarations are this view's; the
// CLight members, the record fields and helper names are inferred.
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

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { d.v[0] = ax; d.v[1] = ay; d.v[2] = az; }
    CVector3(const CVector3& other) { d.pair[0] = other.d.pair[0]; d.pair[1] = other.d.pair[1]; }

    CVector3Pair d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Ident();
    CVector3 GetRow(int i) const { CVector3 r = *(const CVector3*)m[i]; return r; }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

struct MOH_animatedLight_Struct {
    unsigned char unknown00[16];
    float position[3];
    unsigned char unknown1c[18];
    short range;
    unsigned char unknown30[64];
    unsigned char mode;
    unsigned char unknown71;
    short frameCount;
    unsigned char (*colors)[4];
    float* intensities;
};

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

class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

class CLight : public IMovingSceneNode {
public:
    CLight() : data24(0), data28(0) {
        m_matrix.Ident();
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
    virtual ~CLight() {}
    virtual void v010();
    virtual void v014();
    virtual void v018();
    virtual void v01c();
    virtual void v020();
    virtual void v024();
    virtual void v028();
    virtual void v02c();
    virtual void v030();
    virtual void v034();
    virtual void v038();
    virtual void v03c();
    virtual void v040();
    virtual void v044();
    virtual void v048();
    virtual void v04c();
    virtual void v050();
    virtual void v054();
    virtual void v058();
    virtual void v05c();
    virtual void v060();
    virtual void v064();
    virtual void v068();
    virtual void v06c();
    virtual void v070();
    virtual void v074();
    virtual void v078();
    virtual void v07c();
    virtual void v080();
    virtual void v084();
    virtual void v088();
    virtual void v08c();
    virtual void v090();
    virtual void v094();
    virtual void v098();
    virtual void v09c();
    virtual void v0a0();
    virtual void v0a4();
    virtual void v0a8();
    virtual void v0ac();
    virtual void v0b0();
    virtual void v0b4();
    virtual void v0b8();
    virtual void v0bc();
    virtual void v0c0();
    virtual void v0c4();
    virtual void v0c8();
    virtual void v0cc();
    virtual void v0d0();
    virtual void v0d4();
    virtual void v0d8();
    virtual void v0dc();
    virtual void v0e0();
    virtual void v0e4();
    virtual void v0e8();
    virtual void v0ec();
    virtual void v0f0();
    virtual void v0f4();
    virtual void v0f8();
    virtual void v0fc();
    virtual void v100();
    virtual void v104();
    virtual void SetPosition(CVector3);

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
        dir = m_matrix.GetRow(1);
        GXInitLightDir(&m_light, dir.d.v[0], dir.d.v[1], dir.d.v[2]);
    }
    void UpdateAttenuation() { GXInitLightDistAttn(&m_light, 0.5f * m_range, 0.15f, 2); }
    void UpdateSpot() { GXInitLightSpot(&m_light, (180.0f / 3.1415927f) * m_spotAngle, 2); }
    void SetRange(float range) {
        m_range = range;
        GXInitLightDistAttn(&m_light, 0.5f * range, 0.15f, 2);
    }

    int data24;
    int data28;
    GXLightObj m_light;
    unsigned char unknown6c[4];
    CMatrix m_matrix;
    float m_color[4];
    float m_spotAngle;
    float m_range;
    unsigned char unknownc8[8];
};

/* Inferred: a pool node, a light with the next link after it. */
struct AnimLightNodeView {
    unsigned char light[224];
    AnimLightNodeView* next;
    unsigned char unknownE4[12];
};

/* Inferred: a fixed pool of light nodes with in-use and free lists. */
struct AnimLightPoolView {
    void Free(AnimLightNodeView* node) {
        if (node == m_used) {
            m_used = node->next;
        } else {
            for (AnimLightNodeView* n = m_used; n; n = n->next) {
                if (n->next == node) {
                    n->next = node->next;
                    break;
                }
            }
        }
        m_count--;
        node->next = m_free;
        m_free = node;
    }

    AnimLightNodeView* m_data;
    AnimLightNodeView* m_used;
    AnimLightNodeView* m_free;
    int m_capacity;
    int m_count;
};

class CAnimLightManager {
public:
    unsigned char unknown00[48];
    AnimLightPoolView m_pool30;
    AnimLightPoolView m_pool44;
};

extern CAnimLightManager g_AnimLightManager;

class CPropertyAnimLight : public CLight {
public:
    CPropertyAnimLight(MOH_animatedLight_Struct*);
    virtual void BeginUpdate(float);
    virtual void Destroy();
    virtual ~CPropertyAnimLight();

    MOH_animatedLight_Struct* m_data;
    short m_frame;
    short m_reverse : 1;
    short m_unused : 15;
};

void CPropertyAnimLight::Destroy() {
    g_AnimLightManager.m_pool44.Free((AnimLightNodeView*)this);
}

CPropertyAnimLight::~CPropertyAnimLight() {
}

CPropertyAnimLight::CPropertyAnimLight(MOH_animatedLight_Struct* data) : m_data(data), m_frame(0), m_reverse(0) {
    SetPosition(CVector3(data->position[0], data->position[1], data->position[2]));
    SetRange((float)data->range / 16.0f);
}
