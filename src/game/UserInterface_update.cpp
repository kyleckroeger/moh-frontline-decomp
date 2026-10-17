// A fragment of UserInterface.cpp (0x800bb6f4): UserInterface::Update advances
// the HUD's pulse phase (one turn per second of 1/59.94 s steps), fades the
// whole HUD in, re-shows the health, ammo and weapon panels for 4.5 s when the
// item, ammo counts or weapon change, and (outside the pause) counts the
// health panel down or pulses it while the health warning is on, with a
// controller rumble each second. It then turns the compass, fades the ammo
// panel the same way, decays the four hit indicators and the weapon panel,
// counts down the multiplayer message time and remembers the values it
// compared. The classes and functions are named by the symbols; the members,
// the player's inventory slot and the inventory views are inferred.
extern "C" double cos(double);

template <class T> T MathFunClamp(T, T, T);

class CPlayerObject;

class CInventoryItem {
public:
    unsigned char unknown00[12];
    virtual void vf08();
    virtual int* GetId();
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

    int GetCurrentWeaponReserveAmoCount() const;
    int GetCurrentWeaponClipAmoCount() const;
    void* GetCurrentWeapon() const;

    unsigned char unknown004[1888];
    unsigned long m_controller;
};

class CScene {
public:
    unsigned char unknown00[64];
    CPlayerObject* GetPlayer(int) const;
};

class Pause {
public:
    unsigned char unknown00[64];
    bool bIsGamePaused();
};

class CInputManager {
public:
    unsigned char unknown00[64];
    void SetFeedback(unsigned long, long, long, float, float);
};

extern CScene g_scene;
extern Pause g_Pause;
extern CInputManager g_inputMgr;
extern bool g_bInMultiplayerMode;

class UserInterface {
public:
    void Update(float);
    void UpdateCompassRotation(CPlayerObject*, float);

    unsigned char unknown000[1960];
    float m_hits[4];
    unsigned char unknown7b8[8];
    float m_alpha;
    float m_fadeIn;
    float m_healthFade;
    float m_ammoFade;
    float m_weaponFade;
    int m_healthMode;
    int m_ammoMode;
    float m_healthTime;
    float m_ammoTime;
    float m_weaponTime;
    float m_phase;
    unsigned char unknown7ec[4];
    bool m_alwaysShow;
    unsigned char unknown7f1[3];
    float m_rumbleTime;
    unsigned char unknown7f8[12];
    int m_reserve;
    int m_clip;
    void* m_weapon;
    int m_itemId;
    unsigned char unknown814[4];
    int m_player;
    unsigned char unknown81c[4];
    float m_messageTime;
};

void UserInterface::Update(float time) {
    CPlayerObject* player = g_scene.GetPlayer(m_player);
    m_phase += 0.016683351f * (6.2831855f * time);
    if (m_phase > 6.2831855f)
        m_phase -= 6.2831855f;
    if (m_fadeIn > 0.0f)
        m_fadeIn -= 0.016683351f * time;
    if (m_fadeIn >= 1.0f)
        m_alpha = 0.0f;
    else if (m_fadeIn > 0.0f)
        m_alpha = 1.0f - m_fadeIn;
    else
        m_alpha = 1.0f;
    if (m_alwaysShow) {
        m_healthTime = 1.0f;
        m_ammoTime = 1.0f;
    }
    CInventory* inventory = player->GetInventory();
    int* item = 0;
    if (inventory)
        item = inventory->m_item->GetId();
    int itemId;
    if (item)
        itemId = *item;
    else
        itemId = 0;
    int reserve = player->GetCurrentWeaponReserveAmoCount();
    int clip = player->GetCurrentWeaponClipAmoCount();
    if (player->GetCurrentWeapon() != m_weapon) {
        m_ammoTime = 4.5f;
        m_weaponTime = 4.5f;
    }
    if (reserve != m_reserve || clip != m_clip)
        m_ammoTime = 4.5f;
    if (itemId != m_itemId)
        m_healthTime = 4.5f;
    if (!g_Pause.bIsGamePaused()) {
        float step = 0.016683351f * time;
        if (m_healthMode == 0) {
            if (!m_alwaysShow) {
                m_healthTime = MathFunClamp<float>(m_healthTime - step, 0.0f, 4.5f);
                m_healthFade = m_healthTime > 1.0f ? 1.0f : m_healthTime;
                m_healthFade *= m_alpha;
            } else {
                m_healthFade = m_alpha;
            }
        } else if (m_healthMode == 1) {
            m_healthTime -= step;
            if (m_healthTime <= 0.0f) {
                m_healthTime = 0.0f;
                m_healthFade = m_alpha;
            } else {
                float c = cos(m_phase);
                m_healthFade = m_alpha * (0.5f * c + 0.5f);
            }
            m_rumbleTime -= step;
            if (m_rumbleTime <= 0.0f) {
                g_inputMgr.SetFeedback(g_scene.GetPlayer(m_player)->m_controller, 9, 0, 0.0125f, 0.25f);
                m_rumbleTime = 1.0f;
            }
        }
    }
    UpdateCompassRotation(player, time);
    float step = 0.016683351f * time;
    if (m_ammoMode == 0) {
        if (!m_alwaysShow) {
            m_ammoTime = MathFunClamp<float>(m_ammoTime - step, 0.0f, 4.5f);
            m_ammoFade = m_ammoTime > 1.0f ? 1.0f : m_ammoTime;
            m_ammoFade *= m_alpha;
        } else {
            m_ammoFade = m_alpha;
        }
    } else if (m_ammoMode == 1) {
        m_ammoTime = 1.0f;
        float c = cos(m_phase);
        m_ammoFade = m_alpha * (0.5f * c + 0.5f);
    }
    for (int i = 0; i < 4; i++) {
        if (m_hits[i] > 0.0f) {
            m_hits[i] -= step;
            if (m_hits[i] < 0.0f)
                m_hits[i] = 0.0f;
        }
    }
    m_weaponTime = MathFunClamp<float>(m_weaponTime - step, 0.0f, 4.5f);
    m_weaponFade = m_weaponTime > 1.0f ? 1.0f : m_weaponTime;
    m_weaponFade *= m_alpha;
    if (g_bInMultiplayerMode && m_messageTime > 0.0f)
        m_messageTime -= step;
    m_itemId = itemId;
    m_reserve = player->GetCurrentWeaponReserveAmoCount();
    m_clip = player->GetCurrentWeaponClipAmoCount();
    m_weapon = player->GetCurrentWeapon();
}
