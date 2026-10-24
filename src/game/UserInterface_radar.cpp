// A fragment of UserInterface.cpp (0x800bb1ac): UserInterface::DrawRadar
// draws the radar dots (unless the player's radar option is off in
// multiplayer): one dot per living player, coloured by team (alpha from the
// HUD alpha and health fade), the player's own at the radar centre (92, 384
// in viewport units, 16 up) and every other one at its direction relative to
// the player's forward and rightward vectors (atan2 of the dot products) and
// at a radius of 34 scaled by its distance up to 50 (two thirds with more than
// two players). The classes, functions and globals are named by the mangled
// symbols; the screen's virtual slots are unnamed placeholders, and the
// members and the vector, colour, vertex, viewport, shell and player views are
// inferred from offsets (CVector3's inline Dot, Length and Normalize are
// inferred). The constants are entries of the file's .sdata2 pool. The rest of
// the file is in other units.
extern "C" {
#include <math.h>
#include <string.h>
}

class CVector3 {
public:
    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            x *= scale;
            y *= scale;
            z *= scale;
        }
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
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
};

struct ViewportView {
    float scaleX;
    float scaleY;
    float offsetX;
    unsigned char unknown0c[4];
    float width;
    float height;
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
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;

    unsigned char unknown004[915];
    unsigned char flag397_7 : 1;
    unsigned char m_dead : 1;
    unsigned char flag397_low : 6;
    unsigned char unknown398[972];
    unsigned long m_controller;
    unsigned char unknown768[2888];
    int m_team;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

struct ShellPlayerOptions {
    unsigned char unknown00[3];
    bool radar;
    unsigned char unknown04[24];
};

struct ShellView {
    unsigned char unknown0000[5216];
    ShellPlayerOptions options[4];
};

float MathFunAtan2F(float, float);

extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern CScene g_scene;
extern ShellView g_Shell;
extern ViewportView g_viewport;
extern CColor teamColors[4];
extern _spritepolyvert TempVerts[4];
extern _spritepolyvert DotSpriteVerts[4];

class UserInterface {
public:
    void DrawRadar();
    static CSprite* dotSprite;

    unsigned char unknown000[1984];
    float m_alpha;
    unsigned char unknown7c4[4];
    float m_healthFade;
    unsigned char unknown7cc[76];
    int m_player;
};

void UserInterface::DrawRadar() {
    CPlayerObject* other;
    CPlayerObject* player = g_scene.GetPlayer(m_player);
    int i;
    if (g_bInMultiplayerMode && !g_Shell.options[player->m_controller].radar)
        return;
    CScreen* screen = CScreen::GetCurrent();
    int width = screen->vf28();
    int height = screen->vf24();
    float viewWidth = g_viewport.width;
    float viewHeight = g_viewport.height;
    CVector3 offset;
    CVector3 position;
    CVector3 forward;
    CVector3 right;
    player->GetForward(forward);
    player->GetPosition(position);
    player->GetRightward(right);
    float centerX = 92.0f * viewWidth;
    float centerY = 384.0f * viewHeight;
    for (i = 0; i < g_NumPlayers; i++) {
        other = g_scene.GetPlayer(i);
        if (other->m_dead)
            continue;
        float x = width * g_viewport.scaleX + centerX;
        float y = height * g_viewport.scaleY + centerY - 16.0f;
        int team = other->m_team;
        memcpy(TempVerts, DotSpriteVerts, sizeof(TempVerts));
        for (int j = 0; j < 4; j++) {
            if (other == player) {
                TempVerts[j].x += x;
                TempVerts[j].y += y;
            } else {
                other->GetPosition(offset);
                offset.x -= position.x;
                offset.y -= position.y;
                offset.z -= position.z;
                float distance = offset.Length();
                offset.Normalize();
                float radius;
                if (distance >= 50.0f)
                    radius = 34.0f;
                else
                    radius = 34.0f * (distance / 50.0f);
                float fd = forward.Dot(offset);
                float rd = right.Dot(offset);
                float angle = MathFunAtan2F(fd, rd);
                if (g_NumPlayers > 2) {
                    float c = cos(angle);
                    offset.x = 0.6666f * (radius * c);
                    float s = sin(angle);
                    offset.y = 0.6666f * (radius * s);
                } else {
                    float c = cos(angle);
                    offset.x = radius * c;
                    float s = sin(angle);
                    offset.y = radius * s;
                }
                TempVerts[j].x += x + offset.x;
                TempVerts[j].y += y + offset.y;
            }
            TempVerts[j].color.r = teamColors[team].r;
            TempVerts[j].color.g = teamColors[team].g;
            TempVerts[j].color.b = teamColors[team].b;
            TempVerts[j].color.a = m_alpha * (m_healthFade * teamColors[team].a);
        }
        dotSprite->DrawPoly(TempVerts, 4, (CSprite::EPolyPrimType)5);
    }
}
