// A fragment of Pause.cpp (0x800dd758): Pause::PauseShutdown (releases the
// controller lock and shuts the studio's siren down), Pause::UnpauseGame
// (releases the lock, resumes music and ambient sound, clears the paused flag
// and the cheat-sequence index), Pause::bIsGamePaused and Pause::PauseDraw
// (while paused: the studio screen and, unless hidden, up to 14 objectives
// with a completed/pending icon and, for text types below 50, their text in
// white, 24 units apart from y 100; in states 3-5 only the studio screen;
// the objective drawing is an inferred inline helper). The file name is
// this project's; the original record is Pause.cpp (Pause.cpp and
// Pause_tail.cpp hold other parts). Pause, IStudio and the globals are named
// by the mangled symbols; the members are inferred and Pause is a non-virtual
// view.
// The pause screen, the functions at the start of the file: state changes
// (pausing and resuming music and ambient sound, the master volume), the
// objective list and the pause/unpause entry points. Pause, PauseState,
// IStudio and CShellMenu are named by the mangled symbols; the members, the
// objective list and the objective-text table views are inferred. Pause is a
// non-virtual view.
enum PauseState {};

class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFont {
public:
    void SetColor(CColor);
    void DrawText(float, float, const char*);
};

struct _spritepolyvert {
    unsigned char unknown00[12];
    float y;
    unsigned char unknown10[8];
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

class IStudio {
public:
    void iSirenDrawProcessFnc(unsigned long);
    void iSirenActivateScreen(unsigned short, unsigned short);
    void iSirenShutDown();
};

class CShellMenu {
public:
    void CompletedAnotherObjective();

    unsigned char data[16];
};

extern CShellMenu g_Shell;

void MUSIC_Pause(bool);
void AEMS_Pause(bool);
void SoundSetMasterVolume(int);

struct ObjectiveView {
    unsigned int text;
    unsigned char type;
    bool completed;
    unsigned char unknown06[2];
};

struct ObjectiveListView {
    int count;
    ObjectiveView objectives[100];
};

struct ObjectiveTextView {
    unsigned int text;
    unsigned char type;
    unsigned char unknown05[3];
};

struct ObjectiveTextTableView {
    ObjectiveTextView* entries;
    unsigned char unknown04[8];
    int count;
};

extern ObjectiveListView theObjectiveList;
extern ObjectiveTextTableView umTable;
extern int g_PauseScreenLockedController;
extern int g_cheatSequenceIndex;

class Pause {
public:
    void SetState(PauseState);
    void PauseShutdown();
    void UnpauseGame();
    bool bIsGamePaused();
    void PauseDraw(unsigned long);
    void DrawObjectives();

    CFont* m_font;
    CSprite* m_completedSprite;
    CSprite* m_pendingSprite;
    _spritepolyvert m_vertices[4];
    bool m_paused;
    unsigned char unknown6d[3];
    IStudio m_studio;
    unsigned char unknown74[20];
    PauseState m_state;
    float m_volume;
    float m_volumeFade;
    bool m_hideObjectives;
};

void Pause::SetState(PauseState state);

bool GetObjectiveStatus(unsigned int index);

void SetObjectiveStatus(unsigned int index, bool completed);

void AddObjective(unsigned int index, unsigned int id);

void Pause::PauseShutdown() {
    g_PauseScreenLockedController = -1;
    m_studio.iSirenShutDown();
}

void Pause::UnpauseGame() {
    g_PauseScreenLockedController = -1;
    MUSIC_Pause(false);
    AEMS_Pause(false);
    m_paused = false;
    g_cheatSequenceIndex = 0;
}

bool Pause::bIsGamePaused() {
    return m_paused;
}

inline void Pause::DrawObjectives() {
    unsigned short y = 100;
    for (unsigned int i = 0; i < 100; i++) {
        if (y < 424) {
            if (m_font && theObjectiveList.objectives[i].text) {
                m_vertices[0].y = y + 1;
                m_vertices[1].y = y + 1;
                m_vertices[2].y = y + 24;
                m_vertices[3].y = y + 24;
                if (theObjectiveList.objectives[i].completed)
                    m_completedSprite->DrawPoly(m_vertices, 4, (CSprite::EPolyPrimType)5);
                else
                    m_pendingSprite->DrawPoly(m_vertices, 4, (CSprite::EPolyPrimType)5);
                if (theObjectiveList.objectives[i].type < 50) {
                    m_font->SetColor(CColor(255, 255, 255, 255));
                    m_font->DrawText(56.0f, y + 6, (const char*)theObjectiveList.objectives[i].text);
                }
            }
            y += 24;
        }
    }
}

void Pause::PauseDraw(unsigned long time) {
    if (m_paused) {
        m_studio.iSirenDrawProcessFnc(time);
        if (!m_hideObjectives) {
            DrawObjectives();
        }
    } else if (m_state >= 3 && m_state < 6) {
        m_studio.iSirenDrawProcessFnc(time);
    }
}
