// A fragment of projectilebullet.cpp (0x800d0de8): CProjectileBullet::Draw
// (nothing when the cheat flag at g_cheats+11 is set; in multiplayer a
// player's bullet is hidden from the other viewports while it is younger than
// MP_PROJBULLET_DRAW_DELAY; nothing for a zero width; otherwise a tracer from
// the previous position moved along the world fiber's line by the offset at
// +184 to that point plus the velocity, scaled by 0.09 * (width - 0.5) +
// 0.455, goes to g_pRenderList with g_BulletBin, the draw context kept in
// g_pContext). The classes, functions and globals are named by the mangled
// symbols; the members, the bullet info, tracer, cheats, player and context
// views are inferred, and only the virtual slots this function uses are named
// (AsPlayerObject at +156 and GetVelocity at +324, the rest placeholders).
// CVector3 is the double-pair view of projectilebullet_update.cpp. The rest
// of the file is in other units.
class CDrawContext;
class CPlayerObject;
class CRenderBin;

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) {
        pair[0] = o.pair[0];
        pair[1] = o.pair[1];
    }
    CVector3(float ax, float ay, float az) {
        v[0] = ax;
        v[1] = ay;
        v[2] = az;
    }

    void operator*=(float s) {
        v[0] *= s;
        v[1] *= s;
        v[2] *= s;
    }

    union {
        double pair[2];
        float v[4];
    };
} __attribute__((aligned(8)));


class CLine3 {
public:
    CVector3 GetDirection() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
};

class CVolFiber {
public:
    const CLine3& GetLine() const;

    void* vtable;
    unsigned char unknown04[76];
};

struct CheatsView {
    unsigned char unknown00[11];
    bool flag0b;
    unsigned char unknown0c[8];
};

extern CheatsView g_cheats;
extern bool g_bInMultiplayerMode;
extern float MP_PROJBULLET_DRAW_DELAY;

class CRenderList {
public:
    void Render(CRenderBin&, void*);
};

extern CRenderList* g_pRenderList;
extern CDrawContext* g_pContext;
extern CRenderBin g_BulletBin;

struct CDrawContextView {
    int GetViewport() const { return m_viewport; }
    unsigned char unknown000[276];
    int m_viewport;
};

struct CPlayerView {
    int GetViewport() const { return m_viewport; }
    unsigned char unknown000[1896];
    int m_viewport;
};


class ISceneNode {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual CPlayerView* AsPlayerObject();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual void unknown140();
    virtual void GetVelocity(CVector3&);
};

struct BulletInfo {
    unsigned char unknown00[52];
    short m_life;
    unsigned char unknown36[2];
    float m_width;
};

struct TracerData {
    CVector3 start;
    CVector3 unknown10;
    CVector3 end;
    CVector3 unknown30;
    float width;
};

class CProjectileBullet : public ISceneNode {
public:
    void Draw(CDrawContext&);

    unsigned char unknown004[44];
    CVector3 m_position;
    CVector3 m_prevPosition;
    CVector3 m_matrix[4];
    float m_life;
    float m_value94;
    unsigned char unknown098[16];
    BulletInfo* m_info;
    ISceneNode* m_owner;
    unsigned char unknown0b0[4];
    float m_speed;
    float m_offset;
    unsigned char unknown0bc[4];
    CVolFiber m_fiberA;
    CVolFiber m_fiberB;
};

void CProjectileBullet::Draw(CDrawContext& context) {
    if (g_cheats.flag0b)
        return;
    if (g_bInMultiplayerMode && m_owner && m_owner->AsPlayerObject() &&
        ((CDrawContextView&)context).GetViewport() != m_owner->AsPlayerObject()->GetViewport() &&
        m_life > (float)m_info->m_life - MP_PROJBULLET_DRAW_DELAY)
        return;
    if (m_info->m_width == 0.0f)
        return;
    TracerData data;
    CVector3 velocity;
    GetVelocity(velocity);
    velocity *= 0.09f * (m_info->m_width - 0.5f) + 0.455f;
    float offset = m_offset;
    const CVector3& dir = m_fiberB.GetLine().GetDirection();
    float x = offset * dir.v[0];
    float y = offset * dir.v[1];
    float z = offset * dir.v[2];
    CVector3 start(m_prevPosition.v[0] + x, m_prevPosition.v[1] + y, m_prevPosition.v[2] + z);
    CVector3 end(start.v[0] + velocity.v[0], start.v[1] + velocity.v[1], start.v[2] + velocity.v[2]);
    data.width = m_info->m_width;
    data.start = start;
    data.end = end;
    g_pContext = &context;
    g_pRenderList->Render(g_BulletBin, &data);
}
