// The end of staticmesh.cpp (0x800f32a0): the weak offsetPtr<T>
// instantiations for the static mesh file's records, each rebasing a non-null
// pointer by the file base, and the static initialisation of the file's
// matrices (each inline constructor initialises the matrix class once) and of
// the two simple mesh bins (inline CRenderBinData, CRenderBin and
// CUcodeRenderBin constructors; priority 3 and 7; destructors registered).
// The template and its instantiations are named by the mangled symbols (as in
// propdat_offsetptr.cpp); the explicit instantiations reproduce the order of
// the weak copies and the record types are left incomplete. The file's
// globals are named by the symbols; their types, the bin constructors
// (default, and with a priority) and the members are inferred (the light parameters are
// opaque). The rest of the file is not part of this unit. CMshSimpleBin
// declares Link (defined elsewhere) first so its global table stays
// elsewhere; the inline base destructors are weak duplicates.
class CDmaTag;
class CDmaPacket;

#include "game/CVector3.h"

#include "game/CMatrix.h"


class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CUcodeRenderBin() {}
};

class CMshSimpleBin : public CUcodeRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    CMshSimpleBin() {}
    CMshSimpleBin(int priority) { m_priority = priority; }
    virtual ~CMshSimpleBin();

    bool m_used;
};

struct LightParamView {
    unsigned char unknown00[96];
};

static CMatrix g_ActualWorldToCameraMatrix;
static CMatrix g_WorldToCameraMatrix;
static CMatrix g_ModelToScreenMatrix;
static CMatrix g_ModelToClipMatrix;
static CMatrix g_ClipToScreenMatrix;
static CVector3 g_CamPos;
LightParamView g_LightParam;
static CMshSimpleBin g_MshSimpleBin;
static CMshSimpleBin g_MshSimpleAlphaBin(7);

struct StaticMesh;
struct MSHVertex_GC;
struct MSHAttachNode;
struct MSHNode;
struct MSHChunk;
class CMSHMatBin;

template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

template void offsetPtr<StaticMesh>(StaticMesh*&, int);
template void offsetPtr<StaticMesh*>(StaticMesh**&, int);
template void offsetPtr<MSHVertex_GC>(MSHVertex_GC*&, int);
template void offsetPtr<unsigned char>(unsigned char*&, int);
template void offsetPtr<MSHAttachNode>(MSHAttachNode*&, int);
template void offsetPtr<MSHNode>(MSHNode*&, int);
template void offsetPtr<MSHChunk>(MSHChunk*&, int);
template void offsetPtr<CMSHMatBin>(CMSHMatBin*&, int);
