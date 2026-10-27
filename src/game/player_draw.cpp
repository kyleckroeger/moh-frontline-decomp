// A fragment of player.cpp (0x800a5180): CPlayerObject::Draw, the player's
// screen overlay for the viewport that draws it (context player == index).
// It runs the pop-up message loop and the multiplayer frag count, then the
// crosshair (unless the weapon is zoomed with a scope sprite) at the centre
// of the viewport, then by the weapon's scope type: the plain scope masks
// (left/right bars in split screen), the four-quadrant scopes 15/16/30 with
// their cross hairs and insets, and the muzzle flash of the first-person
// weapon. The class layouts are views inferred from the offsets used.
// Matching notes: the viewport corners are converted in the order left,
// right, top, bottom; midX and span (= right - left) are named function-level
// locals (the span's /4 reuses it in the insets); the declaration order sets
// the callee-saved registers.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};

struct CColor {
    CColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : m_r(r), m_g(g), m_b(b), m_a(a) {}

    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
    unsigned char m_a;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void Draw(float, float, CColor);
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

class CDrawContext {
public:
    unsigned char unknown000[276];
    int m_player;
};

class CScreen {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual int GetHeight();
    virtual int GetWidth();
};

extern CScreen* g_pScreen;

/* inferred: the current viewport's corners (fractions of the screen) and scale */
struct ViewportView {
    float m_left;
    float m_top;
    float m_right;
    float m_bottom;
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown18[88];
};

extern ViewportView g_viewport;

class Pause {
public:
    bool bIsGamePaused();

    unsigned char unknown00[152];
};

extern Pause g_Pause;

class PopUpMessageHandler {
public:
    void PopUpMessageHandler_Loop();
};

extern PopUpMessageHandler* g_pPopUpMessageHandler[];

class UserInterface {
public:
    void DrawFragCount(bool);
};

class CShellMenu {
public:
    unsigned char GetCrosshairStatus();

    unsigned char unknown0000[5216];
    struct {
        unsigned char m_crosshair;
        unsigned char unknown01[27];
    } m_controllers[4];
    unsigned char unknown1530[1196];
};

extern CShellMenu g_Shell;

/* inferred: the cheat flags */
struct CheatsView {
    unsigned char unknown00[2];
    bool m_zoomCheat;
    unsigned char unknown03[8];
    bool m_noMuzzleFlash;
    unsigned char unknown0c[8];
};

extern CheatsView g_cheats;
extern bool g_bInMultiplayerMode;
extern int g_bMPEndLevel;
extern int g_NumPlayers;

class CWeapon {
public:
    bool GetMuzzleFlashStatus();

    unsigned char unknown000[644];
    short m_clipAmmo;
    unsigned char unknown286[18];
    int m_scopeType;
    unsigned char unknown29c[36];
    unsigned char m_2c0_80 : 1;
    unsigned char m_2c0_rest : 7;
};

class CPlayerWeaponObject {
public:
    int GetZoomType() const;

    unsigned char unknown0000[12516];
    CWeapon* m_weapon;
    CSprite* m_scopeSprite;
    CSprite* m_scopeSprite2;
    CSprite* m_crosshair;
    unsigned char unknown30f4[184];
    int m_zoomType;
};

inline int CPlayerWeaponObject::GetZoomType() const { return g_cheats.m_zoomCheat ? 3 : m_zoomType; }

class ISceneNode {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void Draw(CDrawContext&);
};

extern ISceneNode* g_pStaticObjectMuzzleFlashPlayer;

class CPlayerObject : public ISceneNode {
public:
    virtual ~CPlayerObject();
    void Draw(CDrawContext&);
    CWeapon* CurrentWeapon() const {
        if (m_usingMountedWeapon)
            return m_mountedWeapon;
        if (m_currentWeapon >= 0)
            return m_weapons[m_currentWeapon]->m_weapon;
        return 0;
    }
    /* inferred inline flag accessor */
    bool IsDisabled() const { return m_397_40; }
    CPlayerWeaponObject* CurrentPlayerWeapon() const {
        if (m_usingMountedWeapon)
            return 0;
        if (m_currentWeapon >= 0)
            return m_weapons[m_currentWeapon];
        return 0;
    }

    unsigned char unknown004[912];
    unsigned char m_394_80 : 1;
    unsigned char m_394_40 : 1;
    unsigned char m_394_20 : 1;
    unsigned char m_394_10 : 1;
    unsigned char m_394_08 : 1;
    unsigned char m_394_rest : 3;
    unsigned char unknown395;
    unsigned char m_usingMountedWeapon : 1;
    unsigned char m_396_40 : 1;
    unsigned char m_396_rest : 6;
    unsigned char m_397_80 : 1;
    unsigned char m_397_40 : 1;
    unsigned char m_397_rest : 6;
    unsigned char unknown398[196];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[200];
    CWeapon* m_lastWeapon;
    CWeapon* m_mountedWeapon;
    unsigned char unknown5e4[384];
    unsigned long m_controller;
    int m_index;
    unsigned char unknown76c[772];
    UserInterface m_ui;
};

void CPlayerObject::Draw(CDrawContext& context) {
    CWeapon* weapon;
    CPlayerWeaponObject* playerWeapon;
    int midX;
    int span;
    bool showCrosshair;
    bool scope;
    int width;
    CSprite* crosshair;
    int height;
    int left;
    int right;
    int top;
    int bottom;
    if (context.m_player != m_index)
        return;
    if (!g_Pause.bIsGamePaused() && !g_bMPEndLevel)
        g_pPopUpMessageHandler[m_index]->PopUpMessageHandler_Loop();
    if (g_bInMultiplayerMode && m_397_40) {
        if (!g_bMPEndLevel && !g_Pause.bIsGamePaused())
            m_ui.DrawFragCount(false);
    }
    weapon = CurrentWeapon();
    playerWeapon = CurrentPlayerWeapon();
    scope = false;
    m_396_40 = 0;
    if (m_394_08 && weapon && playerWeapon && (!weapon->m_2c0_80 || (unsigned int)weapon->m_clipAmmo == 0)) {
        m_396_40 = playerWeapon->GetZoomType() == 3;
        scope = playerWeapon->m_scopeSprite != 0;
    }
    if (!m_396_40 || !scope) {
        crosshair = 0;
        if (playerWeapon)
            crosshair = playerWeapon->m_crosshair;
        else if (m_usingMountedWeapon)
            crosshair = 0;
        if (g_bInMultiplayerMode)
            showCrosshair = g_Shell.m_controllers[m_controller].m_crosshair != 0;
        else
            showCrosshair = g_Shell.GetCrosshairStatus() != 0;
        if (crosshair && !IsDisabled() && !weapon->m_2c0_80 && !m_397_80 && (m_394_08 || showCrosshair)) {
            width = g_pScreen->GetWidth();
            height = g_pScreen->GetHeight();
            int x = (int)((float)width * ((g_viewport.m_left + g_viewport.m_right) / 2.0f));
            int y = (int)((float)height * ((g_viewport.m_top + g_viewport.m_bottom) / 2.0f));
            crosshair->Draw((float)(x - 16), (float)(y - 8), CColor(128, 128, 128, 128));
        }
    }
    if (scope) {
        width = g_pScreen->GetWidth();
        height = g_pScreen->GetHeight();
        left = (int)((float)width * g_viewport.m_left);
        right = (int)((float)width * g_viewport.m_right);
        top = (int)((float)height * g_viewport.m_top);
        bottom = (int)((float)height * g_viewport.m_bottom);
        switch (weapon->m_scopeType) {
        default:
            if (g_NumPlayers == 2) {
                int quarter = (right - left) / 4;
                _spritepolyvert verts[4] = {
                    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                };
                verts[0].x = left;
                verts[0].y = top;
                verts[1].x = left + quarter;
                verts[1].y = top;
                verts[2].x = left + quarter;
                verts[2].y = bottom;
                verts[3].x = left;
                verts[3].y = bottom;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
                verts[0].x = right - quarter;
                verts[1].x = right;
                verts[2].x = right;
                verts[3].x = right - quarter;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
                verts[0].x = left + quarter;
                verts[1].x = right - quarter;
                verts[2].x = right - quarter;
                verts[3].x = left + quarter;
                verts[1].u = 1.0f;
                verts[2].u = 1.0f;
                verts[2].v = 1.0f;
                verts[3].v = 1.0f;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            } else {
                _spritepolyvert verts[4] = {
                    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                };
                verts[0].x = left;
                verts[0].y = top;
                verts[1].x = right;
                verts[1].y = top;
                verts[2].x = right;
                verts[2].y = bottom;
                verts[3].x = left;
                verts[3].y = bottom;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            }
            break;
        case 15:
        case 16:
        case 30: {
            int edge;
            if (g_NumPlayers == 2)
                edge = (int)(33.0f * g_viewport.m_scaleX + (float)((right - left) / 4));
            else
                edge = (int)(66.0f * g_viewport.m_scaleX);
            _spritepolyvert verts[4] = {
                { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
            };
            verts[0].x = left;
            verts[0].y = top;
            verts[1].x = left + edge;
            verts[1].y = top;
            verts[2].x = left + edge;
            verts[2].y = bottom;
            verts[3].x = left;
            verts[3].y = bottom;
            playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            verts[0].x = right - edge;
            verts[1].x = right;
            verts[2].x = right;
            verts[3].x = right - edge;
            playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            span = right - left;
            midX = left + (span >> 1);
            int midY = top + ((bottom - top) >> 1);
            verts[0].x = left + edge;
            verts[1].x = midX;
            verts[2].x = midX;
            verts[2].y = midY;
            verts[3].x = left + edge;
            verts[3].y = midY;
            verts[1].u = 1.0f;
            verts[2].u = 1.0f;
            verts[2].v = 1.0f;
            verts[3].v = 1.0f;
            playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            verts[0].y = midY;
            verts[1].y = midY;
            verts[2].y = bottom;
            verts[3].y = bottom;
            verts[0].u = 1.0f;
            verts[1].u = 1.0f;
            verts[1].v = 1.0f;
            verts[2].u = 0.0f;
            verts[3].v = 0.0f;
            playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            verts[0].x = midX;
            verts[1].x = right - edge;
            verts[2].x = right - edge;
            verts[3].x = midX;
            verts[0].v = 1.0f;
            verts[1].v = 0.0f;
            verts[2].v = 0.0f;
            verts[3].v = 1.0f;
            playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            verts[0].y = top;
            verts[1].y = top;
            verts[2].y = midY;
            verts[3].y = midY;
            verts[1].u = 0.0f;
            verts[0].v = 0.0f;
            verts[1].u = 0.0f;
            verts[2].v = 1.0f;
            verts[3].u = 1.0f;
            playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            if (weapon->m_scopeType == 15) {
                verts[1].x = midX + 1;
                verts[2].x = midX + 1;
                verts[2].y = bottom;
                verts[3].y = bottom;
                verts[0].u = 0.0f;
                verts[0].v = 0.0f;
                verts[2].v = 0.0f;
                verts[3].u = 0.0f;
                verts[3].v = 0.0f;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
                verts[0].x = left;
                verts[0].y = midY;
                verts[1].x = right;
                verts[1].y = midY;
                verts[2].x = right;
                verts[2].y = midY + 1;
                verts[3].x = left;
                verts[3].y = midY + 1;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            } else if (weapon->m_scopeType == 30) {
                int insetLeft;
                int insetRight;
                if (g_NumPlayers == 2) {
                    insetLeft = (int)(54.0f * g_viewport.m_scaleX + (float)(span / 4));
                    insetRight = (int)(54.0f * g_viewport.m_scaleX + (float)(span / 4));
                } else {
                    insetLeft = (int)(108.0f * g_viewport.m_scaleX);
                    insetRight = (int)(108.0f * g_viewport.m_scaleX);
                }
                int insetTop = (int)(82.0f * g_viewport.m_scaleY);
                _spritepolyvert reticle[4] = {
                    { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                    { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0x80808080 },
                };
                int insetBottom = (int)(68.0f * g_viewport.m_scaleY);
                reticle[0].x = left + insetLeft;
                reticle[0].y = top + insetTop;
                reticle[1].x = right - insetRight;
                reticle[1].y = top + insetTop;
                reticle[2].x = right - insetRight;
                reticle[2].y = bottom - insetBottom;
                reticle[3].x = left + insetLeft;
                reticle[3].y = bottom - insetBottom;
                playerWeapon->m_scopeSprite2->DrawPoly(reticle, 4, (CSprite::EPolyPrimType)5);
            } else {
                verts[0].y = bottom;
                verts[1].x = midX - 1;
                verts[1].y = bottom;
                verts[2].x = midX - 1;
                verts[0].u = 0.0f;
                verts[0].v = 0.0f;
                verts[2].v = 0.0f;
                verts[3].u = 0.0f;
                verts[3].v = 0.0f;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
                verts[1].x = midX + 1;
                verts[2].x = midX + 1;
                playerWeapon->m_scopeSprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
            }
            break;
        }
        }
    }
    if (m_usingMountedWeapon) {
        if (m_mountedWeapon != m_lastWeapon && m_mountedWeapon->GetMuzzleFlashStatus() && !g_cheats.m_noMuzzleFlash && g_pStaticObjectMuzzleFlashPlayer)
            g_pStaticObjectMuzzleFlashPlayer->Draw(context);
    }
}
