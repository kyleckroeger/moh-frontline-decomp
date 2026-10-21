// A fragment of UserInterface.cpp (0x800beb24): UserInterface::Draw updates the
// HUD and, from the second frame on (a guarded function static), draws it: with
// the letterbox on, the letterbox quads (copied from LetterBoxVerts into
// TempVerts and scaled to the screen) through the black sprite; otherwise,
// unless the player hides the HUD or the pause menu is in states 3-5, the
// bullet stats, compass, hit status and weapon name, and in multiplayer the
// frag count instead while paused (for the other controllers), while the
// scores are shown or requested, or else the full HUD with the viewport edge,
// radar and score. The classes, functions and globals are named by the
// symbols; the members, the HUD flag bits and the vertex layout are inferred.
extern "C" void* memcpy(void*, const void*, unsigned long);

struct _spritepolyvert {
    unsigned char unknown00[8];
    float x;
    float y;
    unsigned char unknown10[8];
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

class CPlayerWeaponObject {
public:
    unsigned char unknown0000[12724];
    char m_name[1];
};

class CPlayerObject {
public:
    CPlayerWeaponObject* GetCurrentPlayerWeapon() const;

    unsigned char unknown000[919];
    unsigned char m_hideHud : 1;
    unsigned char m_showScores : 1;
    unsigned char unknown398[972];
    int m_controller;
};

class CScene {
public:
    unsigned char unknown00[64];
    CPlayerObject* GetPlayer(int) const;
};

class Pause {
public:
    bool bIsGamePaused();

    unsigned char unknown00[136];
    int m_state;
};

class CInputManager {
public:
    unsigned char unknown00[64];
    int GetValue(unsigned long, unsigned long);
};

class CScreen {
public:
    unsigned char unknown00[40];
    float m_scaleX;
    float m_scaleY;
};

extern CScene g_scene;
extern Pause g_Pause;
extern CInputManager g_inputMgr;
extern CScreen g_screen;
extern bool g_bInMultiplayerMode;
extern int g_bMPEndLevel;
extern int g_PauseScreenLockedController;
extern _spritepolyvert LetterBoxVerts[8];
extern _spritepolyvert TempVerts[24];

class UserInterface {
public:
    void Draw(float);
    void Update(float);
    void DrawFragCount(bool);
    void DrawViewportEdge();
    void DrawBulletStats();
    void DrawCompass();
    void DrawHitStatus();
    void DrawWeaponName(const char*);
    void DrawRadar();
    void DrawScore();

    static CSprite* blackSprite;

    unsigned char unknown000[2048];
    bool m_letterBox;
    unsigned char unknown801[23];
    int m_player;
};

void UserInterface::Draw(float time) {
    CPlayerObject* player = g_scene.GetPlayer(m_player);
    static bool firstframe = true;
    Update(time);
    if (!firstframe) {
        if (!m_letterBox) {
            int state = g_Pause.m_state;
            if (!player->m_hideHud && (state < 3 || state >= 6)) {
                if (g_bInMultiplayerMode) {
                    CPlayerObject* local = g_scene.GetPlayer(m_player);
                    if (!g_bMPEndLevel) {
                        if (g_Pause.bIsGamePaused()) {
                            if (g_PauseScreenLockedController != local->m_controller)
                                DrawFragCount(false);
                        } else if (local->m_showScores || g_inputMgr.GetValue(local->m_controller, 23)) {
                            DrawFragCount(false);
                            DrawViewportEdge();
                        } else {
                            DrawBulletStats();
                            DrawCompass();
                            DrawHitStatus();
                            CPlayerWeaponObject* weapon = player->GetCurrentPlayerWeapon();
                            if (weapon)
                                DrawWeaponName(weapon->m_name);
                            DrawViewportEdge();
                            DrawRadar();
                            DrawScore();
                        }
                    }
                } else {
                    DrawBulletStats();
                    DrawCompass();
                    DrawHitStatus();
                    CPlayerWeaponObject* weapon = player->GetCurrentPlayerWeapon();
                    if (weapon)
                        DrawWeaponName(weapon->m_name);
                }
            }
        } else {
            memcpy(TempVerts, LetterBoxVerts, sizeof(LetterBoxVerts));
            for (int i = 0; i < 24; i++) {
                TempVerts[i].x *= g_screen.m_scaleX;
                TempVerts[i].y *= g_screen.m_scaleY;
            }
            blackSprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
            blackSprite->DrawPoly(TempVerts + 4, 4, (CSprite::EPolyPrimType)5);
        }
    }
    firstframe = false;
}
