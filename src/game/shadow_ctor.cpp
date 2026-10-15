// A fragment of shadow.cpp (0x800816d8): CCharacterShadow's destructor and
// constructor. The constructor sets up the shadow's box volume (+16), draw
// flag (+96) and two values (+100: 0, +104: 1, +108: 0) and, the first time,
// loads the shadow image (the level's resource 11 names it at +404; resource
// 13 may give another at +532 when +524 is set, and the fade distance,
// alpha, projection distance, scale, projection height and z delta range at
// +540.. in steps of 8) and makes it the compartments' shadow texture. The
// classes, functions and globals are named by the symbols (the globals are
// file-local in the original and declared extern here; g_texture's size is
// its symbol's); the members, the resource views and the meaning of the
// first values are inferred. CCharacterShadow has no base class (its RTTI
// record lists none); the vtable follows __vt__16CCharacterShadow. The rest
// of the file is in other units.
class CDrawContext;
struct ShapeFile;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();

    unsigned char unknown04[76];
};

class CTexture {
public:
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
extern CTexture g_texture;

class CCharacterShadow {
public:
    CCharacterShadow();
    virtual ~CCharacterShadow();
    virtual void Draw(CDrawContext&);
    virtual bool IsDrawEnabled() const { return m_drawEnabled; }

    unsigned char unknown04[12];
    CVolBox m_box;
    bool m_drawEnabled;
    unsigned char unknown61[3];
    int m_value64;
    float m_value68;
    float m_value6C;
};

CCharacterShadow::~CCharacterShadow() {
}

CCharacterShadow::CCharacterShadow() {
    m_drawEnabled = true;
    m_value64 = 0;
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
