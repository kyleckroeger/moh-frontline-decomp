// A fragment of compartment.cpp (0x80075ea8): the CCompartment destructor,
// copy constructor and default constructor. The destructor, when the
// compartment has a header, unlinks the header's simple and alpha render bin
// runs from g_CptSimpleBin's and g_CptSimpleAlphaBin's child lists (the run's
// first bin's previous link and its last bin's next link are joined, the
// list head moved past the run when it starts there) and marks the
// compartment's texture swap row (g_TextureSwapCache, row byte at +0x8000)
// as free, then the collision object and the scene-node bases are
// destroyed. The constructors run the scene-node base constructors (table
// pointers, eight cleared words) and set the compartment's table pointer;
// the copy then copies the collision object at +40 (CCDBObject's copy
// constructor), four words and a byte and clears the byte at +97, and the
// default one builds the collision object from no database and sets -1,
// three cleared words, 1 and 0. The file name is this project's; the
// original record is compartment.cpp. The classes, functions and globals are
// named by the mangled symbols; the members and their names, the bin record
// (112 bytes, links at +0/+4), the parent bin view (children at +8), the
// header's bin arrays (+8/+12, +16/+20) and the unlink inline are inferred
// views, and only the virtuals these functions need are declared. The bin
// lists and the cache are file-local in the original and declared extern
// here. CCompartment declares one of its own virtual functions (defined
// elsewhere; the result type is not known) first so its global virtual table
// is not emitted here. ISceneNode's destructor is inline and its table weak
// in the original, so the compiler's copies are weak duplicates, linked to
// the original copies.
class CDB;
class CDrawContext;
class CCollision;

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

/* inferred: a 16-byte vector copied as two doubles (as in cdbobject_ctor.cpp) */
struct CVector3View {
    double pair[2];
};

class IVolume {
public:
    virtual ~IVolume();
};

class CCDBObject : public IVolume {
public:
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual ~CCDBObject();
    CCDBObject(const CCDBObject&);
    CCDBObject(CDB*);

    CDB* m_db;
    CVector3View data08;
    CVector3View data18;
};

/* inferred: a compartment render bin (112 bytes), chained through +0/+4 */
struct CCptBin {
    CCptBin* m_prev;
    CCptBin* m_next;
    unsigned char unknown08[104];
};

/* inferred: the parent render bin view (children list at +8) */
struct CCptBinList {
    int data00;
    CCptBin* m_next;
    CCptBin* m_children;
};

/* inferred: the compartment header's two bin arrays */
class CPTHeader {
public:
    unsigned char unknown00[8];
    CCptBin* m_bins;
    int m_binCount;
    CCptBin* m_alphaBins;
    int m_alphaBinCount;
};

extern CCptBinList g_CptSimpleBin;
extern CCptBinList g_CptSimpleAlphaBin;
/* inferred: the texture swap cache (layout as in compartment_texswap.cpp) */
class CTextureSwapCache {
public:
    unsigned short m_entries[32][512];
    unsigned char m_rows[32];
};

extern CTextureSwapCache g_TextureSwapCache;

static inline void UnlinkBins(CCptBinList& list, CCptBin* first, int count) {
    CCptBin* last = &first[count - 1];
    if (list.m_children == first)
        list.m_children = last->m_next;
    if (first->m_prev)
        first->m_prev->m_next = last->m_next;
    if (last->m_next)
        last->m_next->m_prev = first->m_prev;
    last->m_next = 0;
    first->m_prev = 0;
}

class CCompartment : public ISceneNode {
public:
    virtual void Draw(CDrawContext&); /* result type not known */
    virtual ~CCompartment();
    CCompartment(const CCompartment&);
    CCompartment();

    unsigned char unknown24[4];
    CCDBObject m_cdb;
    int data50;
    int data54;
    CPTHeader* data58;
    int data5c;
    bool data60;
    bool data61;
};

CCompartment::~CCompartment() {
    if (data58) {
        if (data58->m_bins)
            UnlinkBins(g_CptSimpleBin, data58->m_bins, data58->m_binCount);
        if (data58->m_alphaBins)
            UnlinkBins(g_CptSimpleAlphaBin, data58->m_alphaBins, data58->m_alphaBinCount);
        g_TextureSwapCache.m_rows[data50] = 1;
    }
}

CCompartment::CCompartment(const CCompartment& other)
    : m_cdb(other.m_cdb), data50(other.data50), data54(other.data54), data58(other.data58), data5c(other.data5c),
      data60(other.data60), data61(false) {
}

CCompartment::CCompartment()
    : m_cdb(0), data50(-1), data54(0), data58(0), data5c(0), data60(true), data61(false) {
}
