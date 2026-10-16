// A fragment of bsbifunc.cpp (0x8002351c): the built-in script function
// GiveEnemyGrenadeWeapon, which gives the soldier a grenade weapon (slot 3;
// its type chosen from the grenade source at the stack top) and switches to
// it in the hand the current weapon's properties give (bit 0x80 of the slot
// flips the hand). The switch is an inline CSoldierObject helper (inferred,
// as are its name and the names of the virtual functions it calls at +0x134
// and +0x138 and of the source's virtual at +0x134): it updates the current
// weapon, stores the new slot, picks the "lt_hand" or "rt_hand" bone (with
// its CRC), detaches the weapon from the bone (from both in multiplayer),
// attaches the new weapon, refreshes the AI filter outside multiplayer and
// sets the bullet emitter; with no current weapon it only stores the slot
// (the detach under a second test is dead code, as in the original). The
// file name is this project's; the original record is bsbifunc.cpp. The
// soldier, weapon and properties views are inferred non-virtual views apart
// from the placeholder virtual slots. The rest of the file is in other units.
extern bool g_bInMultiplayerMode;

class CAIFilterSoldierObject {
public:
    void UpdateManualUpdateVars();
};

struct CWeaponPropertiesView {
    unsigned char unknown00[74];
    short hand;
};

class CWeapon {
public:
    void Update(float, bool);

    unsigned char unknown000[640];
    CWeaponPropertiesView* m_properties;
};

class CStaticObject;

class CSoldierObject {
public:
    virtual void Function008();
    virtual void Function00c();
    virtual void Function010();
    virtual void Function014();
    virtual void Function018();
    virtual void Function01c();
    virtual void Function020();
    virtual void Function024();
    virtual void Function028();
    virtual void Function02c();
    virtual void Function030();
    virtual void Function034();
    virtual void Function038();
    virtual void Function03c();
    virtual void Function040();
    virtual void Function044();
    virtual void Function048();
    virtual void Function04c();
    virtual void Function050();
    virtual void Function054();
    virtual void Function058();
    virtual void Function05c();
    virtual void Function060();
    virtual void Function064();
    virtual void Function068();
    virtual void Function06c();
    virtual void Function070();
    virtual void Function074();
    virtual void Function078();
    virtual void Function07c();
    virtual void Function080();
    virtual void Function084();
    virtual void Function088();
    virtual void Function08c();
    virtual void Function090();
    virtual void Function094();
    virtual void Function098();
    virtual void Function09c();
    virtual void Function0a0();
    virtual void Function0a4();
    virtual void Function0a8();
    virtual void Function0ac();
    virtual void Function0b0();
    virtual void Function0b4();
    virtual void Function0b8();
    virtual void Function0bc();
    virtual void Function0c0();
    virtual void Function0c4();
    virtual void Function0c8();
    virtual void Function0cc();
    virtual void Function0d0();
    virtual void Function0d4();
    virtual void Function0d8();
    virtual void Function0dc();
    virtual void Function0e0();
    virtual void Function0e4();
    virtual void Function0e8();
    virtual void Function0ec();
    virtual void Function0f0();
    virtual void Function0f4();
    virtual void Function0f8();
    virtual void Function0fc();
    virtual void Function100();
    virtual void Function104();
    virtual void Function108();
    virtual void Function10c();
    virtual void Function110();
    virtual void Function114();
    virtual void Function118();
    virtual void Function11c();
    virtual void Function120();
    virtual void Function124();
    virtual void Function128();
    virtual void Function12c();
    virtual void Function130();
    virtual void AttachWeapon(CWeapon*, const char*, int);
    virtual void DetachWeapon(const char*);
    void AddWeapon(int, int);
    void SetBulletEmitter(CStaticObject*, char*);

    inline void SwitchWeapon(int slot) {
        CWeapon* current = m_weapons[m_currentWeapon];
        const char* bone;
        int crc;

        if (current) {
            current->Update(1.0f, false);
        } else {
            if (current) {
                if ((((slot >> 7) + current->m_properties->hand) & 1) == 0)
                    DetachWeapon("lt_hand");
                else
                    DetachWeapon("rt_hand");
            }
            m_currentWeapon = slot & ~0x80;
            return;
        }
        m_currentWeapon = slot & ~0x80;
        if ((((slot >> 7) + m_weapons[m_currentWeapon]->m_properties->hand) & 1) == 0) {
            bone = "lt_hand";
            crc = 0x4C83C769;
        } else {
            bone = "rt_hand";
            crc = 0xD58A96D3;
        }
        if (g_bInMultiplayerMode) {
            DetachWeapon("lt_hand");
            DetachWeapon("rt_hand");
        } else {
            DetachWeapon(bone);
        }
        AttachWeapon(m_weapons[m_currentWeapon], bone, crc);
        if (!g_bInMultiplayerMode)
            m_aiFilter->UpdateManualUpdateVars();
        SetBulletEmitter((CStaticObject*)m_weapons[m_currentWeapon], (char*)bone);
    }

    unsigned char unknown0004[12512];
    CAIFilterSoldierObject* m_aiFilter;
    unsigned char unknown30e8[3880];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};

class CGrenadeSourceView {
public:
    virtual void Function008();
    virtual void Function00c();
    virtual void Function010();
    virtual void Function014();
    virtual void Function018();
    virtual void Function01c();
    virtual void Function020();
    virtual void Function024();
    virtual void Function028();
    virtual void Function02c();
    virtual void Function030();
    virtual void Function034();
    virtual void Function038();
    virtual void Function03c();
    virtual void Function040();
    virtual void Function044();
    virtual void Function048();
    virtual void Function04c();
    virtual void Function050();
    virtual void Function054();
    virtual void Function058();
    virtual void Function05c();
    virtual void Function060();
    virtual void Function064();
    virtual void Function068();
    virtual void Function06c();
    virtual void Function070();
    virtual void Function074();
    virtual void Function078();
    virtual void Function07c();
    virtual void Function080();
    virtual void Function084();
    virtual void Function088();
    virtual void Function08c();
    virtual void Function090();
    virtual void Function094();
    virtual void Function098();
    virtual void Function09c();
    virtual void Function0a0();
    virtual void Function0a4();
    virtual void Function0a8();
    virtual void Function0ac();
    virtual void Function0b0();
    virtual void Function0b4();
    virtual void Function0b8();
    virtual void Function0bc();
    virtual void Function0c0();
    virtual void Function0c4();
    virtual void Function0c8();
    virtual void Function0cc();
    virtual void Function0d0();
    virtual void Function0d4();
    virtual void Function0d8();
    virtual void Function0dc();
    virtual void Function0e0();
    virtual void Function0e4();
    virtual void Function0e8();
    virtual void Function0ec();
    virtual void Function0f0();
    virtual void Function0f4();
    virtual void Function0f8();
    virtual void Function0fc();
    virtual void Function100();
    virtual void Function104();
    virtual void Function108();
    virtual void Function10c();
    virtual void Function110();
    virtual void Function114();
    virtual void Function118();
    virtual void Function11c();
    virtual void Function120();
    virtual void Function124();
    virtual void Function128();
    virtual void Function12c();
    virtual void Function130();
    virtual int GetGrenadeType();
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_GiveEnemyGrenadeWeapon(int** stack, void* object) {
    CSoldierObject* soldier = (CSoldierObject*)object;
    int type = (*(CGrenadeSourceView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)))->GetGrenadeType();
    int crc;

    switch (type) {
    case 3:
        crc = 0xA55DDA2D;
        break;
    case 24:
        crc = 0x83674487;
        break;
    default:
        crc = 0xA55DDA2D;
        break;
    }
    soldier->AddWeapon(3, crc);
    if (soldier->m_weapons[soldier->m_currentWeapon]->m_properties->hand == 0)
        soldier->SwitchWeapon(3);
    else
        soldier->SwitchWeapon(0x83);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
