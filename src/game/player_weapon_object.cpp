// CPlayerWeaponObject's constructor (0x800a8d30: the base constructor, the
// matrix member, zeroed words and -1 at +9108), then the static
// initialisation of player_weapon_object.cpp (0x800a8db8): the
// camera-to-clip matrix's inline constructor initialises the matrix class
// once. The rest of the file is in other units. g_Cam2clip and CMatrix are
// named by the symbols; the object size comes from the symbol.
class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

// Inferred views: CAnimObject (its constructor, its first two virtuals as in
// the table - the destructor gives the constructor's cleanup entry - and the
// three words at +36..+44 the derived constructor clears), and
// CPlayerWeaponObject's words by offset. The derived class has a matrix member (its inline constructor
// runs here) whose offset is not known; it is placed in the unexplained gap.
class CAnimObject {
public:
    CAnimObject();
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();

    unsigned char unknown004[32];
    int m_unknown24;
    int m_unknown28;
    int m_unknown2c;
};

class CPlayerWeaponObject : public CAnimObject {
public:
    CPlayerWeaponObject();
    virtual ~CPlayerWeaponObject();

    unsigned char unknown0030[9040];
    int m_unknown2380;
    int m_unknown2384;
    int m_unknown2388;
    int m_unknown238c;
    int m_unknown2390;
    int m_unknown2394;
    unsigned char unknown2398[8];
    CMatrix m_matrix;
    unsigned char unknown23e0[3332];
    int m_unknown30e4;
    int m_unknown30e8;
    unsigned char unknown30ec[4];
    int m_unknown30f0;
    int m_unknown30f4;
};

CPlayerWeaponObject::CPlayerWeaponObject() {
    m_unknown24 = 0;
    m_unknown28 = 0;
    m_unknown2c = 0;
    m_unknown30e4 = 0;
    m_unknown2380 = 0;
    m_unknown2384 = 0;
    m_unknown2388 = 0;
    m_unknown238c = 0;
    m_unknown2390 = 0;
    m_unknown30e8 = 0;
    m_unknown30f0 = 0;
    m_unknown30f4 = 0;
    m_unknown2394 = -1;
}

static CMatrix g_Cam2clip;
