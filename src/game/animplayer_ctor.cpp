// A fragment of animplayer.cpp (0x8010ac30): CAnimatedPlayer's destructor
// (CSoldierObject's, then freed when asked) and constructor (CSoldierObject's
// constructor, then the word at +0x44b0 cleared; its meaning is unknown).
// The file name is this project's; the original record is animplayer.cpp and
// the rest of the file is in other units. CAnimatedPlayer and CSoldierObject
// are named by the mangled symbols; the views are inferred, and
// CAnimatedPlayer declares Draw (defined elsewhere) first so its virtual
// table is not emitted here.
class CDrawContext;

class CSoldierObject {
public:
    CSoldierObject();
    virtual ~CSoldierObject();
};

class CAnimatedPlayer : public CSoldierObject {
public:
    CAnimatedPlayer();
    virtual void Draw(CDrawContext&);
    virtual ~CAnimatedPlayer();

    unsigned char unknown0004[0x44ac];
    int m_value44b0;
};

CAnimatedPlayer::~CAnimatedPlayer() {}

CAnimatedPlayer::CAnimatedPlayer() {
    m_value44b0 = 0;
}
