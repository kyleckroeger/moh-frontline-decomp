// The end of decal.cpp (0x80083ec0): the static initialisation of the file's
// decal list (512 bullet decals built through their constructor by
// __construct_array) and of its identity matrix (the inline constructor
// initialises the matrix class once, then the identity is set), followed by
// the weak BulletDecal constructor emitted after it, which sets the colour of
// each of the decal's four vertices to (0, 0, 0, 128), and the weak
// this-adjusting thunks of CBulletDecalManager's table for its render-bin base
// at +36 (IsUsed, Link, destructor). The rest of the file is in other units.
// BulletDecal, g_DecalList and g_Ident are named by the symbols; the object
// sizes come from the symbols, the vertex layout, its colour view and the
// identity-matrix constructor are inferred. The manager's virtual functions
// are named by its table; their declaration order (IsUsed first, as the key
// function that emits the table here) is inferred, and IsUsed itself is placed
// by decal_link.cpp (a file duplicate, like the table).
/* Inferred: a plain colour (the vertex is constructor-less). */
struct ColorView {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    void Set(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) {
        r = ar;
        g = ag;
        b = ab;
        a = aa;
    }
};

#include "game/CMatrix.h"


/* Inferred: a decal vertex (position, colour, texture coordinates). */
struct DecalVertexView {
    float position[3];
    ColorView color;
    float uv[2];
};

class BulletDecal {
public:
    BulletDecal() {
        m_verts[0].color.Set(0, 0, 0, 128);
        m_verts[1].color.Set(0, 0, 0, 128);
        m_verts[2].color.Set(0, 0, 0, 128);
        m_verts[3].color.Set(0, 0, 0, 128);
    }

    DecalVertexView m_verts[4];
    unsigned char unknown60[8];
};

static BulletDecal g_DecalList[512];
static CMatrix g_Ident(true);

class CDmaTag;
class CDmaPacket;
class CDrawContext;
class CCollision;
class CBullet;
class CLight;
class CVector3;
struct BSObject;







#include "game/ISceneNode.h"


class CRenderBin {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    int data04[7];
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual ~CUcodeRenderBin();
};

/* Inferred: only the bases and the member IsUsed reads. */
class CBulletDecalManager : public ISceneNode, public CUcodeRenderBin {
public:
    virtual bool IsUsed();
    virtual ~CBulletDecalManager();
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual void Link(CDmaTag*, CDmaPacket&);

    unsigned char unknown44[1408];
    int m_active;
};

bool CBulletDecalManager::IsUsed() {
    return m_active > 0;
}
