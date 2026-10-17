// A fragment of UserInterface.cpp (0x800bbea0): UserInterface::DrawCompass
// draws the compass and health HUD while the health panel is visible. Outside
// multiplayer it rotates the compass dial about its centre (92, 384) by the
// shown compass angle; in a multiplayer viewport it draws the plain circle
// placed and scaled into the viewport instead. It re-reads the player's health
// (re-showing the panel for 5 s and flashing it below 25, for 4.5 s
// otherwise), draws the compass edge (or, in multiplayer, the ring of a living
// player), then the eight-segment health meter coloured red, yellow or green,
// hiding the empty segments and stretching the partial one. The classes,
// functions and globals are named by the symbols; the members, the vertex
// view (texture coordinates, position, colour) and the inventory views are
// inferred. One loop sets the meter vertices for both modes, which the
// compiler unswitches.
extern "C" {
#include <math.h>
#include <string.h>
}

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    unsigned char unknown10[4];
    CColor color;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
};

class CScreen {
public:
    static CScreen* GetCurrent();
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual int vf24();
    virtual int vf28();

    unsigned char unknown04[36];
    float m_scaleX;
    float m_scaleY;
};

struct ViewportView {
    float scaleX;
    float scaleY;
    float offsetX;
    unsigned char unknown0c[4];
    float width;
    float height;
};

class CInventoryItem {
public:
    unsigned char unknown00[12];
    virtual void vf08();
    virtual unsigned int* GetId();
};

class CInventory {
public:
    unsigned char unknown00[12];
    CInventoryItem* m_item;
};

class CPlayerObject {
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
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual CInventory* GetInventory();

    unsigned char unknown004[915];
    unsigned char flag397_7 : 1;
    unsigned char m_dead : 1;
    unsigned char flag397_low : 6;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

void MathFunRotateAboutY(float*, float*, float);

extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern CScene g_scene;
extern CScreen g_screen;
extern ViewportView g_viewport;
extern _spritepolyvert TempVerts[24];
extern _spritepolyvert CompassVerts[24];

class UserInterface {
public:
    void DrawCompass();
    static CSprite* healthSprite;
    static CSprite* compassSprite;
    static CSprite* compassEdgeSprite;
    static CSprite* circleSprite;

    unsigned char unknown000[1980];
    unsigned int m_health;
    float m_alpha;
    unsigned char unknown7c4[4];
    float m_healthFade;
    unsigned char unknown7cc[8];
    int m_healthMode;
    unsigned char unknown7d8[4];
    float m_healthTime;
    unsigned char unknown7e0[28];
    float m_compassAngle;
    unsigned char unknown800[24];
    int m_player;
};

void UserInterface::DrawCompass() {
    CPlayerObject* player = g_scene.GetPlayer(m_player);
    if (m_healthFade * m_alpha <= 0.0f)
        return;
    CScreen* screen = CScreen::GetCurrent();
    int width = screen->vf28();
    int height = screen->vf24();
    float viewWidth = g_viewport.width;
    float viewHeight = g_viewport.height;
    float scale = 1.0f;
    if (g_bInMultiplayerMode && g_NumPlayers > 2)
        scale = 0.75f;
    if (!g_bInMultiplayerMode) {
        memcpy(TempVerts, CompassVerts, sizeof(TempVerts));
        for (int i = 0; i < 24; i++) {
            TempVerts[i].x -= 92.0f;
            TempVerts[i].y -= 384.0f;
            MathFunRotateAboutY(&TempVerts[i].x, &TempVerts[i].y, m_compassAngle);
            TempVerts[i].x += 92.0f;
            TempVerts[i].y += 384.0f;
            TempVerts[i].x *= g_screen.m_scaleX;
            TempVerts[i].y *= g_screen.m_scaleY;
            TempVerts[i].color.a = 128.0f * m_healthFade * m_alpha;
        }
        compassSprite->DrawPoly(TempVerts, 24, (CSprite::EPolyPrimType)3);
    } else {
        memcpy(TempVerts, CompassVerts, sizeof(TempVerts));
        for (int i = 0; i < 24; i++) {
            float x = TempVerts[i].x - 92.0f;
            TempVerts[i].x = 92.0f * viewWidth + (0.95f * (scale * x) + width * g_viewport.scaleX);
            float y = TempVerts[i].y - 384.0f;
            TempVerts[i].y = 384.0f * viewHeight + (0.95f * (scale * y) + height * g_viewport.scaleY) - 16.0f;
            TempVerts[i].color.a = 64;
        }
        circleSprite->DrawPoly(TempVerts, 24, (CSprite::EPolyPrimType)3);
    }
    unsigned int health = 0;
    CInventory* inventory = player->GetInventory();
    if (inventory) {
        unsigned int* item = inventory->m_item->GetId();
        if (item)
            health = *item;
        if (m_health != health) {
            if (health < 25) {
                m_healthTime = 5.0f;
                m_healthMode = 1;
            } else {
                m_healthTime = 4.5f;
                m_healthMode = 0;
            }
        }
    }
    m_health = health;
    if (!g_bInMultiplayerMode) {
        memcpy(TempVerts, CompassVerts, sizeof(TempVerts));
        for (int i = 0; i < 24; i++) {
            TempVerts[i].x *= g_screen.m_scaleX;
            TempVerts[i].y *= g_screen.m_scaleY;
            TempVerts[i].color.a = 128.0f * m_healthFade * m_alpha;
        }
        compassEdgeSprite->DrawPoly(TempVerts, 24, (CSprite::EPolyPrimType)3);
    } else if (!player->m_dead) {
        memcpy(TempVerts, CompassVerts, sizeof(TempVerts));
        for (int i = 0; i < 24; i++) {
            TempVerts[i].x = 92.0f * viewWidth + (scale * (TempVerts[i].x - 92.0f) + width * g_viewport.scaleX);
            TempVerts[i].y = 384.0f * viewHeight + (scale * (TempVerts[i].y - 384.0f) + height * g_viewport.scaleY) - 16.0f;
            TempVerts[i].color.a = 128.0f * m_healthFade * m_alpha;
        }
        healthSprite->DrawPoly(TempVerts, 24, (CSprite::EPolyPrimType)3);
    }
    memcpy(TempVerts, CompassVerts, sizeof(TempVerts));
    unsigned char r;
    unsigned char g;
    unsigned char b;
    if (health <= 33) {
        r = 128;
        g = 0;
        b = 0;
    } else if (health <= 67) {
        r = 128;
        g = 128;
        b = 0;
    } else {
        r = 0;
        g = 128;
        b = 0;
    }
    for (int i = 0; i < 24; i++) {
        if (!g_bInMultiplayerMode) {
            TempVerts[i].x *= g_screen.m_scaleX;
            TempVerts[i].y *= g_screen.m_scaleY;
        } else {
            TempVerts[i].x = 92.0f * viewWidth + (scale * (TempVerts[i].x - 92.0f) + width * g_viewport.scaleX);
            TempVerts[i].y = 384.0f * viewHeight + (scale * (TempVerts[i].y - 384.0f) + height * g_viewport.scaleY) - 16.0f;
        }
        TempVerts[i].color.r = r;
        TempVerts[i].color.g = g;
        TempVerts[i].color.b = b;
        TempVerts[i].color.a = 128.0f * m_healthFade * m_alpha;
    }
    if (health < 100 && health != 0) {
        float segments = health / 12.5f;
        int full = (float)floor(segments);
        float part = segments - full;
        int empty = 8 - full;
        if (empty > 0) {
            for (int s = 1; s < empty; s++) {
                TempVerts[(s - 1) * 3].color.a = 0;
                TempVerts[(s - 1) * 3 + 1].color.a = 0;
                TempVerts[(s - 1) * 3 + 2].color.a = 0;
            }
            float rest = 1.0f - part;
            int last = empty - 1;
            switch (last) {
            case 0:
            case 7:
                TempVerts[last * 3 + 1].x += 64.0f * scale * rest;
                TempVerts[last * 3 + 1].u += 0.5f * rest;
                break;
            case 1:
            case 2:
                TempVerts[last * 3 + 1].y += 64.0f * scale * rest;
                TempVerts[last * 3 + 1].v += 0.5f * rest;
                break;
            case 3:
            case 4:
                TempVerts[last * 3 + 1].x -= 64.0f * scale * rest;
                TempVerts[last * 3 + 1].u -= 0.5f * rest;
                break;
            case 5:
            case 6:
                TempVerts[last * 3 + 1].y -= 64.0f * scale * rest;
                TempVerts[last * 3 + 1].v -= 0.5f * rest;
                break;
            }
        }
    }
    if (health != 0)
        healthSprite->DrawPoly(TempVerts, 24, (CSprite::EPolyPrimType)3);
}
