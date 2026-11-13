extern "C" {
#include <stdio.h>
#include <string.h>
}

#include "game/CColor.h"


extern "C" void FONT_getrecta(int, const char*, float*, float*, float*, float*);

class CFont {
public:
    void SetColor(CColor);
    void GetScale(float&, float&);
    void SetScale(float, float);
    void DrawText(float, float, const char*);

    unsigned char unknown00[32];
    int m_font;
};

struct _spritepolyvert {
    unsigned char unknown00[8];
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

#include "game/CScreen.h"


struct ViewportView {
    float scaleX;
    float scaleY;
    float offsetX;
    unsigned char unknown0c[4];
    float width;
    float height;
};

class CWeapon {
public:
    unsigned char unknown000[664];
    int m_type;
};

#include "game/CPlayerObject.h"


#include "game/CScene.h"


extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern CScene g_scene;
extern CScreenFieldMode g_screen;
extern ViewportView g_viewport;
extern _spritepolyvert TempVerts[4];
extern _spritepolyvert AmmoSpriteVerts[4];

class UserInterface {
public:
    void DrawBulletStats();
    static CSprite* ammoSprite;
    static CFont* bulletText;

    unsigned char unknown000[1984];
    float m_alpha;
    unsigned char unknown7c4[8];
    float m_ammoFade;
    unsigned char unknown7d0[72];
    int m_player;
};

void UserInterface::DrawBulletStats() {
    int type;
    CPlayerObject* player = g_scene.GetPlayer(m_player);
    if (!player->GetCurrentWeapon() || player->m_usingMountedWeapon)
        return;
    type = player->GetCurrentWeapon()->m_type;
    bool showCount = true;
    if (type == 37)
        showCount = false;
    CScreen* screen = CScreen::GetCurrent();
    int width = screen->GetWidth();
    int height = screen->GetHeight();
    float viewWidth = g_viewport.width;
    float viewHeight = g_viewport.height;
    float scale = 1.0f;
    if (g_bInMultiplayerMode && g_NumPlayers > 2)
        scale = 0.75f;
    if (showCount) {
        unsigned short clip = player->GetCurrentWeaponClipAmoCount();
        unsigned short reserve = player->GetCurrentWeaponReserveAmoCount();
        bulletText->SetColor(CColor(128, 128, 128, 128.0f * m_ammoFade * m_alpha));
        float scaleX;
        float scaleY;
        bulletText->GetScale(scaleX, scaleY);
        bulletText->SetScale(scale, scale);
        char text[80];
        if (type != 33 && type != 34) {
            sprintf(text, "%d", clip);
            float x0;
            float y0;
            float x1;
            float y1;
            FONT_getrecta(bulletText->m_font, text, &x0, &y0, &x1, &y1);
            x1 *= screen->GetWidthScale();
            int x = 506.0f * g_screen.m_widthScale - x1;
            if (g_bInMultiplayerMode) {
                int ax = x * viewWidth;
                int bx = width * g_viewport.scaleX;
                int ay = viewHeight * (400.0f * g_screen.m_heightScale);
                int by = height * g_viewport.scaleY;
                bulletText->DrawText(ax + bx, ay - 10 + by, text);
            } else
                bulletText->DrawText(x, (int)(400.0f * g_screen.m_heightScale), text);
        }
        if (type == 33 || type == 34)
            sprintf(text, "%d", reserve + clip);
        else
            sprintf(text, "%d", reserve);
        int x = 554.0f * g_screen.m_widthScale;
        if (g_bInMultiplayerMode) {
            int px = x * viewWidth + width * g_viewport.scaleX;
            int py = (int)(viewHeight * (400.0f * g_screen.m_heightScale)) + (int)(height * g_viewport.scaleY) - 10;
            bulletText->DrawText(px, py, text);
        } else
            bulletText->DrawText(x, (int)(400.0f * g_screen.m_heightScale), text);
        bulletText->SetScale(scaleX, scaleY);
    }
    ammoSprite = player->GetCurrentBulletSprite();
    if (!ammoSprite)
        return;
    memcpy(TempVerts, AmmoSpriteVerts, sizeof(TempVerts));
    _spritepolyvert* vert = TempVerts;
    for (int i = 0; i < 4; i++, vert++) {
        vert->color.a = m_alpha * (TempVerts->color.a * m_ammoFade);
        vert->x *= g_screen.m_widthScale;
        vert->y *= g_screen.m_heightScale;
    }
    if (g_bInMultiplayerMode) {
        int w = scale * (TempVerts[1].x - TempVerts[0].x);
        int h = scale * (TempVerts[3].y - TempVerts[0].y);
        float centerX = viewWidth * (0.5f * (TempVerts[0].x + TempVerts[1].x)) + width * g_viewport.scaleX;
        float centerY = viewHeight * (0.5f * (TempVerts[0].y + TempVerts[3].y)) + height * g_viewport.scaleY;
        float left = centerX - 0.5f * w;
        float top = centerY - 0.5f * h - 5.0f;
        TempVerts[0].x = TempVerts[3].x = left;
        TempVerts[0].y = TempVerts[1].y = top;
        TempVerts[1].x = TempVerts[2].x = left + w;
        TempVerts[2].y = TempVerts[3].y = top + h - 5.0f;
    }
    ammoSprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
}
