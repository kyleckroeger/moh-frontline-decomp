// A fragment of particlesystem.cpp (0x8007ea84): CParticleSystem::IsActive,
// then CParticleSystemManager's IsDrawEnabled (unless the configuration flag
// at +6 turns particles off), IsVisible (always) and Draw, which caches the
// camera axes, world-to-camera matrix and fog of the draw context, inserts
// every visible system of the manager's two lists into its depth-sorted bin
// list, renders the bins and sets the paused flag.
// CParticleSystem is named by the mangled symbols; it is a non-virtual view
// and its flag bits at +136 are inferred (named after the accessors). The
// function is a weak copy of a header inline emitted in this file, so it is
// defined __declspec(weak). The configuration view, the draw-context, camera
// and bin views and the manager's members are inferred from offsets. The
// file's .bss globals are defined here so Draw addresses them from the
// section base. The rest of the file is not part of this unit.
class CParticleSystem {
public:
    bool HasRotation() const;
    bool IsActive() const;
    void ValidateBoundingBox(bool);
    bool IsMoving() const;
    bool IsBoundingBoxValid() const;

    unsigned char unknown000[136];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

__declspec(weak) bool CParticleSystem::IsActive() const {
    return m_active;
}

class CDrawContext;

/* inferred: the configuration flag at +6 that turns particles off */
struct ConfigView {
    unsigned char unknown00[6];
    bool noParticles;
    unsigned char unknown07[13];
};

extern ConfigView g_config;

struct CVector3 {
    float x;
    float y;
    float z;
    float w;
};

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);

    float m[4][4];
};

/* inferred: the camera interface at +212 of the draw context */
class CameraView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void GetXAxis(CVector3&);
    virtual void GetZAxis(CVector3&);
    virtual void GetYAxis(CVector3&);
};

class CDrawContext {
public:
    unsigned char unknown000[196];
    CMatrix* m_worldToCamera;
    unsigned char unknown0c8[12];
    CameraView* m_camera;
    void* m_fog;
};

/* inferred: a render bin in the manager's depth-sorted draw list */
class CRenderBin {
public:
    CRenderBin* m_prev;
    CRenderBin* m_next;
    unsigned char unknown08[20];
};

class CRenderList {
public:
    void Render(CRenderBin&, void*);
};

/* inferred: a drawable system (a render bin with its virtual table at +28) */
class DrawableView : public CRenderBin {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual bool IsVisible(CDrawContext&);

    unsigned char unknown020[80];
    float m_depth;
};

struct SystemView : public DrawableView {
    unsigned char unknown074[172];
    SystemView* m_next;
};

struct OtherView : public DrawableView {
    unsigned char unknown074[140];
    OtherView* m_next;
};

static CMatrix g_ModelToScreenMatrix;
static CVector3 g_XAxis;
static CVector3 g_YAxis;
static CVector3 g_ZAxis;
static CMatrix g_WorldToCamera;

extern CDrawContext* g_pContext;
extern void* g_pFog;
extern bool g_bPaused;
extern CRenderList* g_pRenderList;

class CParticleSystemManager {
public:
    bool IsDrawEnabled() const;
    bool IsVisible(CDrawContext&) const;
    void Draw(CDrawContext&);
    void AddBin(DrawableView* bin) {
        bin->m_next = 0;
        bin->m_prev = 0;
        DrawableView* head = m_head;
        if (head) {
            if (head->m_depth < bin->m_depth) {
                bin->m_prev = head->m_prev;
                if (head->m_prev)
                    head->m_prev->m_next = bin;
                bin->m_next = head;
                head->m_prev = bin;
                m_head = bin;
            } else {
                while (head->m_next) {
                    if (bin->m_depth > ((DrawableView*)head->m_next)->m_depth)
                        break;
                    head = (DrawableView*)head->m_next;
                }
                bin->m_next = head->m_next;
                if (head->m_next)
                    head->m_next->m_prev = bin;
                bin->m_prev = head;
                head->m_next = bin;
            }
        } else {
            m_head = bin;
        }
    }

    unsigned char unknown00[8];
    DrawableView* m_head;
    unsigned char unknown0c[76];
    SystemView* m_systems;
    unsigned char unknown5c[16];
    OtherView* m_others;
};

bool CParticleSystemManager::IsDrawEnabled() const {
    return !g_config.noParticles;
}

bool CParticleSystemManager::IsVisible(CDrawContext&) const {
    return true;
}

void CParticleSystemManager::Draw(CDrawContext& context) {
    g_pContext = &context;
    context.m_camera->GetXAxis(g_XAxis);
    context.m_camera->GetYAxis(g_YAxis);
    context.m_camera->GetZAxis(g_ZAxis);
    g_WorldToCamera = *context.m_worldToCamera;
    g_pFog = context.m_fog;
    for (SystemView* system = m_systems; system; system = system->m_next) {
        if (system->IsVisible(context))
            AddBin(system);
    }
    for (OtherView* other = m_others; other; other = other->m_next) {
        if (other->IsVisible(context))
            AddBin(other);
    }
    for (CRenderBin* bin = m_head; bin; bin = bin->m_next)
        g_pRenderList->Render(*bin, 0);
    g_bPaused = true;
}
