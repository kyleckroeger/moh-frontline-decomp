// A fragment of shadow.cpp (0x80081454): CCharacterShadow::Draw (unless the
// configuration's switch at +2 disables shadows or drawing is off: copies the
// owner's (+100) world box, fades the alpha (+108, from +104) by the camera
// distance over g_FadeDistance, widens the box by g_ShadowScale, sets its
// height to the projection distance and drops its centre to the projection
// height, fades again between the z-delta limits and, when the box is
// visible, appends the shadow to the draw context's list at +244), then
// CCharacterShadow's destructor and constructor. The constructor sets up the shadow's box volume (+16), draw
// flag (+96) and two values (+100: 0, +104: 1, +108: 0) and, the first time,
// loads the shadow image (the level's resource 11 names it at +404; resource
// 13 may give another at +532 when +524 is set, and the fade distance,
// alpha, projection distance, scale, projection height and z delta range at
// +540.. in steps of 8) and makes it the compartments' shadow texture. The
// classes, functions and globals are named by the symbols (the globals are
// file-local in the original and declared extern here, except g_texture);
// the members, the resource, configuration, owner, camera and list views
// and the meaning of the first values are inferred (the configuration view's
// size only keeps it out of the small-data area). CCharacterShadow has no base class (its RTTI
// record lists none); the vtable follows __vt__16CCharacterShadow. InitClass,
// after them, is empty, and the file's static initialiser is empty too (it
// constructs g_texture, whose inline constructor does nothing; the source is
// named shadow.cpp because the initialiser's name comes from it, and the unit
// defines g_texture, file-local as in the image). The rest of the file is in
// other units.
class CDrawContext;
struct ShapeFile;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

namespace std {
extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual bool TestVisibility(CDrawContext&) const;
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    CVolBox& operator=(const CVolBox&);
    float GetWidth() const;
    float GetDepth() const;
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);

    unsigned char unknown04[8];
    float m_value0c;
    unsigned char unknown10[48];
    CVector3 m_center;
} __attribute__((aligned(8)));

/* inferred: the owner's world bounding volume (ISceneNode slot +56) */
class ShadowOwnerView {
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
    virtual const CVolBox* GetWorldBoundingVolume(int) const;
};

/* inferred: the camera's position (ISceneNode slot +64) */
class ShadowCameraView {
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
    virtual void GetPosition(CVector3&) const;
};

/* inferred: the draw context's camera (+212) and shadow list (+244) */
struct ShadowListView {
    void** items;
    int unknown4;
    int count;
};

class CDrawContext {
public:
    unsigned char unknown000[212];
    ShadowCameraView* m_camera;
    unsigned char unknown0d8[28];
    ShadowListView* m_shadows;
};

/* inferred: the configuration's shadow switch at +2 */
struct ShadowConfigView {
    unsigned char unknown0[2];
    bool m_noShadows;
    unsigned char unknown3[61];
};

extern ShadowConfigView g_config;

class CTexture {
public:
    CTexture() {}
    void Set(ShapeFile*, bool);

    unsigned char unknown00[64];
};

class CCompartment {
public:
    static void SetShadowTexture(CTexture&);
};

/* inferred: the level resources read here */
struct ShadowResourceView {
    unsigned char unknown000[404];
    const char* m_imageName;
    unsigned char unknown198[116];
    int m_hasImage;
    unsigned char unknown210[4];
    const char* m_image;
    unsigned char unknown218[4];
    float m_fadeDistance;
    unsigned char unknown220[4];
    float m_alpha;
    unsigned char unknown228[4];
    float m_projectionDistance;
    unsigned char unknown230[4];
    float m_scale;
    unsigned char unknown238[4];
    float m_projectionHeight;
    unsigned char unknown240[4];
    float m_zDeltaMin;
    unsigned char unknown248[4];
    float m_zDeltaMax;
};

struct TLTResourceView {
    unsigned char unknown00[8];
    ShadowResourceView* data;
};

TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
ShapeFile* TLT_LoadFileFromLevelBigFile(const char*, int*);

extern ShapeFile* g_pImageData;
extern float g_FadeDistance;
extern float g_ShadowAlpha;
extern float g_ProjectionDistance;
extern float g_ShadowScale;
extern float g_ProjectionHeight;
extern float g_zDeltaMin;
extern float g_zDeltaMax;
static CTexture g_texture;

class CCharacterShadow {
public:
    CCharacterShadow();
    virtual ~CCharacterShadow();
    static void InitClass();
    virtual void Draw(CDrawContext&);
    virtual bool IsDrawEnabled() const { return m_drawEnabled; }

    unsigned char unknown04[12];
    CVolBox m_box;
    bool m_drawEnabled;
    unsigned char unknown61[3];
    ShadowOwnerView* m_owner;
    float m_value68;
    float m_value6C;
};

void CCharacterShadow::Draw(CDrawContext& context) {
    if (g_config.m_noShadows)
        return;
    if (!IsDrawEnabled())
        return;
    const CVolBox* volume = m_owner->GetWorldBoundingVolume(3);
    m_box = *volume;
    CVector3 centre = m_box.m_center;
    CVector3 camera;
    context.m_camera->GetPosition(camera);
    float z = centre.z;
    float dx = centre.x - camera.x;
    float dy = centre.y - camera.y;
    float dz = z - camera.z;
    float distance = std::sqrtf(dx * dx + dy * dy + dz * dz);
    if (distance >= g_FadeDistance)
        return;
    m_value6C = m_value68 * (g_ShadowAlpha * (1.0f - distance / g_FadeDistance));
    float bottom = m_box.m_value0c;
    m_box.SetWidth(g_ShadowScale * m_box.GetWidth());
    m_box.SetDepth(g_ShadowScale * m_box.GetDepth());
    m_box.SetHeight(g_ProjectionDistance);
    centre.z = g_ProjectionHeight + ((z - bottom) - 0.5f * g_ProjectionDistance);
    m_box.SetCenter(centre);
    float delta = (centre.z - 0.5f * g_ProjectionDistance) - camera.z;
    if (delta > g_zDeltaMax)
        return;
    if (delta > g_zDeltaMin)
        m_value6C *= (g_zDeltaMax - delta) / (g_zDeltaMax - g_zDeltaMin);
    if (m_box.TestVisibility(context)) {
        ShadowListView* list = context.m_shadows;
        list->items[list->count++] = this;
    }
}

CCharacterShadow::~CCharacterShadow() {
}

CCharacterShadow::CCharacterShadow() {
    m_drawEnabled = true;
    m_owner = 0;
    m_value68 = 1.0f;
    m_value6C = 0.0f;
    if (!g_pImageData) {
        g_pImageData = TLT_LoadFileFromLevelBigFile(TLT_FindNextResourceByType((TLTResourceID)11, 0)->data->m_imageName, 0);
        TLTResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)13, 0);
        if (resource->data->m_hasImage) {
            g_pImageData = TLT_LoadFileFromLevelBigFile(resource->data->m_image, 0);
            g_FadeDistance = resource->data->m_fadeDistance;
            g_ShadowAlpha = resource->data->m_alpha;
            g_ProjectionDistance = resource->data->m_projectionDistance;
            g_ShadowScale = resource->data->m_scale;
            g_ProjectionHeight = resource->data->m_projectionHeight;
            g_zDeltaMin = resource->data->m_zDeltaMin;
            g_zDeltaMax = resource->data->m_zDeltaMax;
        }
        if (g_pImageData) {
            g_texture.Set(g_pImageData, false);
            CCompartment::SetShadowTexture(g_texture);
        }
    }
}

void CCharacterShadow::InitClass() {
}
