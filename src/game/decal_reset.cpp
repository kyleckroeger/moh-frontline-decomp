// A fragment of decal.cpp (0x80083424): CBulletDecalManager::
// InitBulletDecalFromCollision (a decal's type from the contact surface, a
// frame built on the contact normal (flipped when asked) with a random spin
// about it, and four corners of a randomly scaled quad 0.005 above the
// surface transformed into the world), AddBulletDecal (recycles the oldest
// decal past 512 and links the new one into its texture bin) and Reset (two
// scene-node words cleared, the manager added to the scene again, no decal
// active and every texture bin emptied). The file name is this project's;
// the original record is decal.cpp. The classes, functions and g_scene are
// named by the mangled symbols (CRenderBinData by CRenderBin's RTTI record);
// the members are inferred views (as in decal_dtor.cpp and decal_link.cpp).
// Each class declares its destructor (defined in decal_dtor.cpp) first so
// its table is not emitted here. The inline fabs is MSL's; SetXYZ and the
// CVector3 Cross helper are inferred inlines.
class CDmaTag;
class CDmaPacket;
class CRenderList;
class CBullet;
class CCollision;
class CDrawContext;
class CLight;
class CPlayerObject;
class CStaticObject;
struct AIDoodadView;
enum EClsnId {};

#undef fabs
inline double fabs(double x) { return __fabs(x); }

union CVector3Data {
    double pair[2];
    struct {
        float x, y, z, w;
    } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.c.x = x;
        d.c.y = y;
        d.c.z = z;
    }
    void Cross(const CVector3& a, const CVector3& b) {
        d.c.x = a.d.c.y * b.d.c.z - a.d.c.z * b.d.c.y;
        d.c.y = a.d.c.z * b.d.c.x - a.d.c.x * b.d.c.z;
        d.c.z = a.d.c.x * b.d.c.y - a.d.c.y * b.d.c.x;
    }
    CVector3& operator*=(float f) {
        d.c.x *= f;
        d.c.y *= f;
        d.c.z *= f;
        return *this;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void BuildRotZ(float);
    void Multiply(const CMatrix&, const CMatrix&);
    void TransformPoint(CVector3&, CVector3) const;
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

float MathFunRandomReal(float, float);
float MathFunRandomRealSigned(float, float);

/* inferred: decal.crp holds six (key, value) pairs per decal type: the
   name (pair 0, see Init), then width, height, the scale range and
   the rotation range (degrees, converted to radians by Init). */
struct DecalParam {
    int key;
    float value;
};

struct DecalResourceData {
    int data00;
    int m_count;
    DecalParam m_params[6]; /* six pairs per decal type */
};

struct TLTResourceView {
    int data00;
    int data04;
    DecalResourceData* data;
};

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
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

/* ISceneNode's virtuals in table order (slots 20 to 216). They are the weak
   inline defaults of Moh2.cpp in the original; their bodies are not repeated
   here, so they are declared without one. */
class ISceneNode : public IObserver {
public:
    enum EVolumeType {};
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CRenderBin;

/* CRenderBin's non-polymorphic base, named by CRenderBin's RTTI record; the
   members are inferred (CRenderBin::Init walks children from +8 through the
   siblings at +4, IsUsed tests +16, +20 is the render priority, 3 by
   default). */
struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. Init's and IsUsed's bodies are not part of this view, so they
   are declared without one, which also keeps CRenderBin's table out of this
   unit. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    CUcodeRenderBin() {}
    virtual ~CUcodeRenderBin() {}
    virtual void Link(CDmaTag*, CDmaPacket&);
};


struct SurfaceInfo {
    unsigned char unknown00[12];
    short m_type : 11;
    short unknown0c : 5;
};

struct SClsnContact {
    CVector3 m_normal;
    CVector3 m_position;
    unsigned char unknown20[4];
    SurfaceInfo* m_surface;
};

class CCollision {
public:
    unsigned char unknown00[24];
    SClsnContact* m_contact;
};

class CSurfaceType {
public:
    static int GetDecalIndex(int);
};

/* Only Load and the 64-byte size are part of this view. */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[64];
};

/* Inferred: a decal's four vertices (position, colour, texture coordinates)
   and the next decal of its bin. */
struct SDecalVertex {
    float x, y, z;
    unsigned char r, g, b, a;
    float u, v;
};

inline void SetXYZ(SDecalVertex& v, float x, float y, float z) {
    v.x = x;
    v.y = y;
    v.z = z;
}

struct BulletDecal {
    SDecalVertex m_vertices[4];
    BulletDecal* m_next;
    int m_bin;
};

class CBulletDecalTextureBin : public CUcodeRenderBin {
public:
    virtual ~CBulletDecalTextureBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    int m_count;
    BulletDecal* m_decals;
    CTexture m_texture;
    int unknown68;
};

class CBulletDecalManager : public ISceneNode, public CUcodeRenderBin {
public:
    virtual ~CBulletDecalManager();
    virtual void Draw(CDrawContext&);
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    void InitBulletDecalFromCollision(BulletDecal*, const SClsnContact*, bool);
    void AddBulletDecal(const CCollision&, bool);
    void Reset();

    CBulletDecalTextureBin m_bins[13];
    BulletDecal* m_decals;
    int m_active;
    TLTResourceView* m_resource;
};


/* Only Add and the object's size are part of this view. */
class CScene {
public:
    void Add(ISceneNode&);

    unsigned char unknown00[0x148];
};

extern CScene g_scene;

void CBulletDecalManager::InitBulletDecalFromCollision(BulletDecal* decal, const SClsnContact* contact, bool flag) {
    decal->m_bin = CSurfaceType::GetDecalIndex(contact->m_surface->m_type);
    CVector3 normal = contact->m_normal;
    int first = decal->m_bin * 6;
    float width = m_resource->data->m_params[first + 1].value;
    float height = m_resource->data->m_params[first + 2].value;
    float minScale = m_resource->data->m_params[first + 3].value;
    float maxScale = m_resource->data->m_params[first + 4].value;
    float angle = m_resource->data->m_params[first + 5].value;
    if (flag)
        normal *= -1.0f;
    CVector3 right;
    CVector3 front;
    CVector3 p;
    CVector3 out;
    if (fabs(normal.d.c.z) < fabs(normal.d.c.x)) {
        front.Cross(normal, CVector3(0.0f, 0.0f, -1.0f));
        right.Cross(front, normal);
    } else {
        front.Cross(normal, CVector3(-1.0f, 0.0f, 0.0f));
        right.Cross(front, normal);
    }
    CMatrix matrix;
    CMatrix rotation;
    matrix.SetRight(right);
    matrix.SetFront(front);
    matrix.SetUp(normal);
    matrix.SetPos(contact->m_position);
    rotation.BuildRotZ(MathFunRandomRealSigned(-angle, angle));
    matrix.Multiply(rotation, matrix);
    float scale = MathFunRandomReal(minScale, maxScale);
    float x = width * scale;
    float y = height * scale;
    SDecalVertex* v = decal->m_vertices;
    v[0].x = -x;
    v[0].y = y;
    v[0].z = 0.005f;
    v[1].x = x;
    v[1].y = y;
    v[1].z = 0.005f;
    v[2].x = x;
    v[2].y = -y;
    v[2].z = 0.005f;
    v[3].x = -x;
    v[3].y = -y;
    v[3].z = 0.005f;
    p.d.c.x = v[0].x;
    p.d.c.y = v[0].y;
    p.d.c.z = v[0].z;
    matrix.TransformPoint(out, p);
    SetXYZ(v[0], out.d.c.x, out.d.c.y, out.d.c.z);
    p.d.c.x = v[1].x;
    p.d.c.y = v[1].y;
    p.d.c.z = v[1].z;
    matrix.TransformPoint(out, p);
    SetXYZ(v[1], out.d.c.x, out.d.c.y, out.d.c.z);
    p.d.c.x = v[2].x;
    p.d.c.y = v[2].y;
    p.d.c.z = v[2].z;
    matrix.TransformPoint(out, p);
    SetXYZ(v[2], out.d.c.x, out.d.c.y, out.d.c.z);
    p.d.c.x = v[3].x;
    p.d.c.y = v[3].y;
    p.d.c.z = v[3].z;
    matrix.TransformPoint(out, p);
    SetXYZ(v[3], out.d.c.x, out.d.c.y, out.d.c.z);
}

void CBulletDecalManager::AddBulletDecal(const CCollision& collision, bool flag) {
    SClsnContact* contact = collision.m_contact;
    if (contact) {
        BulletDecal* decal;
        if (m_active >= 512) {
            decal = &m_decals[m_active & 511];
            CBulletDecalTextureBin& bin = m_bins[decal->m_bin];
            decal = &m_decals[m_active & 511];
            bin.m_count--;
            BulletDecal* d = bin.m_decals;
            if (decal == d) {
                bin.m_decals = 0;
            } else {
                for (; d;) {
                    BulletDecal* next = d->m_next;
                    if (next == decal) {
                        d->m_next = next->m_next;
                        break;
                    }
                    d = next;
                }
            }
        } else {
            decal = &m_decals[m_active];
        }
        m_active++;
        InitBulletDecalFromCollision(decal, contact, flag);
        int index = CSurfaceType::GetDecalIndex(contact->m_surface->m_type);
        decal->m_next = m_bins[index].m_decals;
        m_bins[index].m_decals = decal;
        m_bins[index].m_count++;
    }
}

void CBulletDecalManager::Reset() {
    data08 = 0;
    data04 = 0;
    g_scene.Add(*this);
    m_active = 0;
    for (CBulletDecalTextureBin* bin = (CBulletDecalTextureBin*)m_children; bin; bin = (CBulletDecalTextureBin*)bin->m_next) {
        bin->m_decals = 0;
        bin->m_count = 0;
    }
}

