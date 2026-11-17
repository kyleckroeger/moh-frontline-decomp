// The end of debug_prof.cpp (0x800f4608): the static initialisation of the
// profiler's data. The 31 profile bins and their 31 history bins are built
// through their constructors (__construct_array), the graph cursor is
// converted to a float and the selected and graphed bins start at the top
// bin, the two debug render bins are built through the inline CRenderBinData
// and CRenderBin constructors (priority 9; destructors registered), and the
// profile data is initialised through CProfileInitClass's constructor. The
// file's .bss and .sbss data are defined here. The globals, SProfileBin,
// SProfHistBin, CProfileInitClass, CDbgStatsRenderBin and CProfHistRenderBin
// are named by the symbols; the element and object sizes come from the
// symbols and the array construction, while the members, the 9 priority and
// the contents of the opaque objects are inferred. The bin constructors are
// defined elsewhere in the file (SProfileBin's and CProfileInitClass's are
// weak there); the render bins declare Render (defined elsewhere) first so
// their global tables stay elsewhere, and CRenderBin's inline destructor is a
// weak duplicate. The rest of the file is in other units.
class CDmaTag;
class CDmaPacket;

class CRenderBinData {
public:
    CRenderBinData() : m_prev(0), m_next(0), m_children(0), m_lastTag(0), m_firstTag(0), m_priority(3) {}

    int m_prev;
    int m_next;
    int m_children;
    int m_lastTag;
    int m_firstTag;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CDbgStatsRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CDbgStatsRenderBin() { m_priority = 9; }
    virtual ~CDbgStatsRenderBin();
};

class CProfHistRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CProfHistRenderBin() { m_priority = 9; }
    virtual ~CProfHistRenderBin();
};

struct SProfileBin {
    SProfileBin();

    unsigned char unknown00[24];
};

struct SProfHistBin {
    SProfHistBin();

    unsigned char unknown00[7212];
};

class CProfileInitClass {
public:
    CProfileInitClass();
};

struct DebugStatsView {
    unsigned char unknown00[1200];
};

struct DebugProfTimersView {
    unsigned char unknown00[832];
};

static DebugStatsView debugStats;
SProfileBin g_ProfileBins[31];
static SProfHistBin g_ProfileBinsHistory[31];
static int g_screenWidth;
static int g_screenHeight;
static int currentIndex;
bool g_bDebugStatsDrawProfData;
int g_HistoryCursor;
int g_GraphCursor;
float g_fGraphCursor = g_GraphCursor;
int g_ProfileBinTOP;
int g_ProfileBinSelect = g_ProfileBinTOP;
int g_ProfileBinGraph = g_ProfileBinTOP;
DebugProfTimersView g_pDebugProfTimers;
static CDbgStatsRenderBin g_DbgStatsRenderBin;
static CProfHistRenderBin g_ProfHistRenderBin;
static CProfileInitClass g_ProfileDataInit;
