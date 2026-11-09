class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CDrawContext;
class CCollision;
class IVolume;

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(int) const;
    virtual const IVolume* GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(void*) const;
    virtual void GetRightward(void*) const;
    virtual void GetForward(void*) const;
    virtual void GetUpward(void*) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;

    ISceneNode* m_link4;
    ISceneNode* m_link8;
    unsigned char unknown0c[28];
    ISceneNode* m_nextVisible;
};

class CScreen {
public:
    virtual void s08();
    virtual void s0c();
    virtual void s10();
    virtual void s14();
    virtual void s18();
    virtual void s1c();
    virtual void s20();
    virtual void s24();
    virtual void s28();
    virtual void s2c();
    virtual void s30();
    virtual void s34();
    virtual void s38();
    virtual void s3c();
    virtual void s40();
    virtual void s44();
    virtual const CMatrix& GetTMViewToScreen();
};

class CViewport {
public:
    const CMatrix& GetTMClipToView() const;

    unsigned char unknown00[24];
    int m_mode;
};

class CRenderList;

class CPlane {
public:
    void Transform(const CMatrix&);

    float m_normal[4];
    float m_d;
    unsigned char unknown14[12];
};

class CFog {
public:
    CPlane m_near;
    CPlane m_far;
    unsigned char unknown40[4];
    float m_nearDistance;
    float m_farDistance;
};

class CCamera;

class CFrustum {
public:
    void Set(const CCamera&, CScreen&, CViewport&);
    void Transform(const CMatrix&);
};

class CCamera : public ISceneNode {
public:
    unsigned char unknown30[16];
    CMatrix m_tm;
    CMatrix m_tm128;
    CMatrix m_tm192;
    unsigned char unknown100[80];
    CFrustum m_frustum;

    const CMatrix& GetTM128() const { return m_tm128; }
    const CMatrix& GetTM192() const { return m_tm192; }
};

namespace dwi {
template <class T>
class fast_vec {
public:
    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    void push_back(const T& x) { m_data[m_size++] = x; }
    void clear() { m_size = 0; }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CDrawContext {
public:
    CDrawContext(const CMatrix&, const CMatrix&, const CMatrix&, const CMatrix&, int);

    unsigned char unknown00[212];
    CCamera* m_camera;
    CFog* m_fog;
    unsigned char unknownd8[20];
    dwi::fast_vec<ISceneNode*>* m_visible;
    dwi::fast_vec<ISceneNode*>* m_list104;
    unsigned char unknownf8[44];
} __attribute__((aligned(16)));

class CWorldObject {
public:
    void DrawShadows(CDrawContext&);
};

class CScene {
public:
    struct SModCommand {
        void (CScene::*m_fn)();
        int m_unused;
    };

    void Draw(CViewport&, CScreen&, CRenderList&, CFog&);

    ISceneNode* m_firstNode;
    unsigned char unknown004[20];
    CWorldObject* m_world;
    CCamera* m_camera;
    unsigned char unknown020[16];
    ISceneNode* m_firstVisible;
    unsigned char unknown034[20];
    bool m_drawing;
    unsigned char unknown049[7];
    dwi::fast_vec<ISceneNode*> m_visible;
    dwi::fast_vec<ISceneNode*> m_list104;
    unsigned char unknown080[176];
    dwi::fast_vec<SModCommand> m_commands;
};

void CScene::Draw(CViewport& viewport, CScreen& screen, CRenderList& list, CFog& fog) {
    CCamera* camera = m_camera;
    const CMatrix& screenTM = screen.GetTMViewToScreen();
    CDrawContext context(camera->m_tm128, camera->m_tm192, viewport.GetTMClipToView(), screenTM, viewport.m_mode);
    camera = m_camera;
    context.m_camera = camera;
    context.m_fog = &fog;
    context.m_visible = &m_visible;
    context.m_list104 = &m_list104;
    camera->m_frustum.Set(*camera, screen, viewport);
    camera->m_frustum.Transform(camera->m_tm);
    camera = m_camera;
    CMatrix tm;
    camera->GetTMLocalToWorld(tm);
    fog.m_near.m_normal[0] = 0.0f;
    fog.m_near.m_normal[1] = 1.0f;
    fog.m_near.m_normal[2] = 0.0f;
    fog.m_near.m_d = fog.m_nearDistance;
    fog.m_near.Transform(tm);
    fog.m_far.m_normal[0] = 0.0f;
    fog.m_far.m_normal[1] = -1.0f;
    fog.m_far.m_normal[2] = 0.0f;
    fog.m_far.m_d = fog.m_farDistance;
    fog.m_far.Transform(tm);
    m_visible.clear();
    m_list104.clear();
    for (ISceneNode* node = m_firstVisible; node; node = node->m_nextVisible) {
        if (node->IsVisible(context))
            m_visible.push_back(node);
    }
    m_drawing = true;
    for (ISceneNode* node = m_firstNode; node; node = node->m_link8) {
        if (node->IsDrawEnabled() && node->IsVisible(context))
            node->Draw(context);
    }
    m_world->DrawShadows(context);
    m_drawing = false;
    for (SModCommand* cmd = m_commands.begin(); cmd != m_commands.end(); cmd++) {
        SModCommand c = *cmd;
        (this->*c.m_fn)();
    }
    m_commands.clear();
}
