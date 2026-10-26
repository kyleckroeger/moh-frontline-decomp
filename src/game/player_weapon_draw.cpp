// A fragment of player_weapon_object.cpp (0x800a80b4):
// CPlayerWeaponObject::Draw (first-person weapon view: muzzle-flash sprite
// projected through the camera, a weapon-specific perspective matrix, then the
// skinned weapon model drawn with the weapon scale). Class names come from the
// mangled symbols; members and the context/camera/screen views are inferred.
// The rest of the file is not part of this unit.
extern "C" double tan(double);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    void Set(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CVector4 {
public:
    float x;
    float y;
    float z;
    float w;
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    void Transform(CVector4&, CVector3) const;
    void Perspective(float, float, float, float);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

/* inferred: the camera's near and far distances */
struct CameraView {
    unsigned char unknown00[36];
    float m_near;
    float m_far;
};

/* inferred: the draw context starts with its combined matrix */
class CDrawContext : public CMatrix {
public:
    unsigned char unknown040[128];
    CMatrix* m_localToWorld;
    CMatrix* m_worldToView;
    CMatrix* m_viewToScreen;
    unsigned char unknown0cc[8];
    CameraView* m_camera;
    unsigned char unknown0d8[8];
    CVector3 m_scale;
    unsigned char unknown0f0[36];
    int m_viewport;
};

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

class CTexture;

class CAnimated {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void Draw(CDrawContext&);
    void BindSleeveTexture(CTexture*);
};

class CAnimSkinned : public CAnimated {
public:
    void UpdateSkeletonMatrices(CMatrix*, CVector3*, CVector3*);
};

class CPlayerObject {
public:
    unsigned char unknown000[916];
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flag20 : 1;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flag04 : 1;
    unsigned char m_flag02 : 1;
    unsigned char m_flag01 : 1;
    unsigned char unknown395;
    unsigned char m_flags396a : 1;
    unsigned char m_flag396 : 1;
    unsigned char m_flags396b : 6;
    unsigned char unknown397[149];
    float m_fov;
    unsigned char unknown430[12];
    CSprite* m_flashSprite;
    unsigned char unknown440[808];
    int m_viewport;
    unsigned char unknown76c[2820];
    unsigned char m_dead;
};

class CWeapon {
public:
    unsigned char GetMuzzleFlashStatus();

    unsigned char unknown000[644];
    short m_reserve;
    unsigned char unknown286[58];
    unsigned char m_loaded : 1;
    unsigned char m_flags2c0 : 7;
};

class CScreen {
public:
    int GetWidth();
    int GetHeight();

    unsigned char unknown00[40];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown30[16];
};

struct ViewportView {
    float scaleX;
    float scaleY;
    float offsetX;
    unsigned char unknown0c[4];
    float width;
    float height;
};

struct ConfigView {
    unsigned char unknown00[3];
    bool m_noWeapon;
    unsigned char unknown04;
    bool m_noWeapon2;
    unsigned char unknown06[14];
};

struct CheatsView {
    unsigned char unknown00[2];
    bool m_cheat2;
    unsigned char unknown03[8];
    bool m_noFlash;
    unsigned char unknown0c[8];
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

void MathFunRotateAboutY(float*, float*, float);

extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern ConfigView g_config;
extern CheatsView g_cheats;
extern CScreen g_screen;
extern ViewportView g_viewport;
extern CScene g_scene;
extern const float DEFAULT_FOV;
/* file statics of player_weapon_object.cpp (other units) */
extern CMatrix g_Cam2clip;
extern float g_fNearMultiplier;

class CCamera {
public:
    static const float ASPECT_4x3;
};

class CPlayerWeaponObject {
public:
    void Draw(CDrawContext&);

    unsigned char unknown0000[48];
    CAnimSkinned m_animated;
    unsigned char unknown0034[3852];
    CMatrix m_matrices[80];
    CMatrix m_matrix2340;
    unsigned char unknown2380[3424];
    CPlayerObject* m_player;
    CWeapon* m_weapon;
    void* m_unknown30e8;
    unsigned char unknown30ec[44];
    CVector3 m_scale;
    CVector3 m_flashPosition;
    unsigned char unknown3138[72];
    float m_zoom;
    unsigned char unknown3184[20];
    float m_flashScale;
    float m_flashAngle;
    unsigned char unknown31a0[12];
    int m_mode;
    int m_delay;
    unsigned char unknown31b4[36];
    CTexture* m_sleeve;
};

void CPlayerWeaponObject::Draw(CDrawContext& context) {
    if (context.m_viewport != m_player->m_viewport)
        return;
    if (m_delay > 0) {
        m_delay--;
        return;
    }
    if (g_bInMultiplayerMode && m_sleeve)
        m_animated.BindSleeveTexture(m_sleeve);
    if (g_config.m_noWeapon || g_config.m_noWeapon2)
        return;
    float far; float near; float aspect;
    CMatrix saved;
    CMatrix proj;
    CMatrix* viewToScreen = context.m_viewToScreen;
    saved = context;
    g_Cam2clip = *context.m_viewToScreen;
    float fov;
    if (m_player->m_fov > 0.0f)
        fov = m_zoom * (0.017453292f * m_player->m_fov);
    else
        fov = m_zoom * (0.017453292f * DEFAULT_FOV);
    if (m_weapon->GetMuzzleFlashStatus() && !m_player->m_flag396 && !g_cheats.m_noFlash) {
        float width = g_screen.GetWidth();
        float height = g_screen.GetHeight();
        float x = width * g_viewport.scaleX;
        float y = height * g_viewport.scaleY;
        CVector4 clip;
        g_Cam2clip.Transform(clip, m_flashPosition);
        clip.x /= m_flashPosition.z;
        clip.y /= m_flashPosition.z;
        clip.x *= -320.0f;
        clip.y *= 224.0f;
        clip.y = g_NumPlayers == 2 ? 0.5f * clip.y : clip.y;
        x += (4.0f + (320.0f + clip.x)) * g_viewport.width;
        float top = (224.0f + clip.y) * g_viewport.height + y;
        float size = m_flashScale;
        if (g_bInMultiplayerMode)
            size *= 0.5f;
        size = -96.0f * size;
        float neg = -size;
        _spritepolyvert verts[4] = {
            { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0x80808055 },
            { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0x80808055 },
            { 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0x80808055 },
            { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0x80808055 },
        };
        verts[0].x = size;
        verts[0].y = size;
        verts[1].x = neg;
        verts[1].y = size;
        verts[2].x = neg;
        verts[2].y = neg;
        verts[3].x = size;
        verts[3].y = neg;
        _spritepolyvert* v = verts;
        for (int i = 0; i < 4; i++) {
            MathFunRotateAboutY(&v->x, &v->y, m_flashAngle);
            v->x *= g_screen.m_scaleX;
            v->x += x;
            v->y *= g_screen.m_scaleY;
            v->y += top;
            v++;
        }
        m_player->m_flashSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
    }
    if (g_NumPlayers != 2) {
        far = context.m_camera->m_far;
        near = context.m_camera->m_near;
        aspect = tan(fov * CCamera::ASPECT_4x3);
        proj.Perspective(tan(fov), aspect, g_fNearMultiplier * near, far);
    } else {
        float far = context.m_camera->m_far;
        float near = context.m_camera->m_near;
        float aspect = tan(CCamera::ASPECT_4x3 * (0.75f * fov));
        proj.Perspective(tan(1.3333334f * fov), aspect, g_fNearMultiplier * near, far);
    }
    context.m_viewToScreen = &proj;
    context.Multiply(*context.m_worldToView, *context.m_viewToScreen);
    context.m_localToWorld = &m_matrix2340;
    m_animated.UpdateSkeletonMatrices(m_matrices, 0, 0);
    CVector3 scale = context.m_scale;
    bool draw = true;
    context.m_scale.Set(m_scale.x, m_scale.y, m_scale.z);
    if (m_player->m_flag08) {
        int mode;
        if (g_cheats.m_cheat2)
            mode = 3;
        else
            mode = m_mode;
        if (mode == 3) {
            if (!m_weapon->m_loaded || (unsigned int)m_weapon->m_reserve == 0)
                draw = false;
        } else if (m_unknown30e8) {
            if (!m_weapon->m_loaded || (unsigned int)m_weapon->m_reserve == 0)
                draw = false;
        }
    }
    if (!g_scene.GetPlayer(0)->m_dead && draw)
        m_animated.Draw(context);
    context.m_viewToScreen = viewToScreen;
    (CMatrix&)context = saved;
    context.m_scale = scale;
}
