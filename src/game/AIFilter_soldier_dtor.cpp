// A fragment of AIFilter.cpp (0x8005ba1c): CAIFilterSoldierObject's
// UpdateManualUpdateVars (the current weapon's bullet speed, 1.0 without one,
// and its inverse; a weapon class from the weapon id through a switch, 9
// without a weapon), SetOneTimeVars (the script objects at +12/+16, then
// AI tuning values from the soldier's script properties: a scale, two
// three-way choices of constants, three shorts scaled by 1/16, byte and short
// copies; then UpdateFromGameObject) and its placement operator new; then the
// CAIFilterSoldierObject destructor (its virtual
// table pointer, the AI object at +8 deleted through its virtual destructor,
// then CAIFilterObject's table pointer). The filter has no delete call: it is
// constructed in place in its owner's memory (see AITankDoodad.cpp), so its
// operator delete is an inferred empty inline. The file name is this
// project's; the original record is AIFilter.cpp. The classes and functions
// are named by the mangled symbols; the members follow AIFilter.cpp's view (whose first word is the table pointer),
// and only the virtuals the destructor needs are declared. CAIFilterObject's
// destructor is defined later in the same original file
// (AIFilter_caifilterobject_dtor.cpp) and inlined here; this fragment cannot
// hold it, so the view declares it inline. Each class declares ChooseTarget
// (defined elsewhere; the result type is not known) before its destructor so
// the global virtual tables are not emitted here. The soldier, doodad,
// weapon, script and AI-object views are inferred (members by offset); the
// constants are .sdata2 pool entries.
class CAIObject {
public:
    virtual ~CAIObject();
};

class CWeapon {
public:
    float GetBulletSpeed() const;

    unsigned char unknown000[664];
    unsigned int m_weaponId;
};

/* Inferred: the soldier's script properties (members by offset). */
struct SoldierPropsView {
    unsigned char unknown00[53];
    unsigned char unknown35;
    unsigned char unknown36[2];
    short unknown38;
    unsigned char unknown3a[54];
    unsigned char unknown70;
    unsigned char unknown71;
    unsigned char unknown72;
    unsigned char unknown73;
    unsigned char unknown74[1];
    unsigned char unknown75;
    unsigned char unknown76[3];
    unsigned char unknown79;
    unsigned char unknown7a[4];
    short unknown7e;
    short unknown80;
    short unknown82;
};

struct ScriptDataView {
    unsigned char unknown00[4];
    SoldierPropsView* props;
};

struct ScriptObjectView {
    unsigned char unknown00[8];
    ScriptDataView* data;
    void* unknown0c;
};

/* Inferred: scene-node views with the table slots used here. */
class CSceneObjectView {
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
    virtual ScriptObjectView* GetScriptObject() const;
};

class CSoldierView : public CSceneObjectView {
public:
    unsigned char unknown0004[16396];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};

class CSoldierDoodadView {
public:
    virtual void unknown008();
    virtual CSceneObjectView* unknown00c();

    unsigned char unknown004[76];
    CSoldierView* m_soldier;
};

/* Inferred: the soldier AI object's members by offset. */
class CAISoldierView {
public:
    unsigned char unknown000[20];
    unsigned char m_unknown014;
    unsigned char m_unknown015;
    short m_unknown016;
    unsigned char unknown018[160];
    float m_bulletSpeed;
    float m_bulletTime;
    unsigned char unknown0c0[304];
    float m_unknown1f0;
    float m_unknown1f4;
    float m_unknown1f8;
    float m_unknown1fc;
    unsigned char unknown200[4];
    float m_unknown204;
    float m_unknown208;
    unsigned char unknown20c[4];
    float m_unknown210;
    int m_unknown214;
    unsigned char unknown218[1296];
    int m_weaponType;
    unsigned char unknown72c[60];
    float m_unknown768;
    float m_unknown76c;
};


class CAIFilterObject {
public:
    virtual void ChooseTarget(float); /* result type not known */
    virtual ~CAIFilterObject() {}

    void* m_gameObject;
    CAIObject* m_aiObject;
};

class CAIFilterSoldierObject : public CAIFilterObject {
public:
    virtual void ChooseTarget(float); /* result type not known */
    virtual ~CAIFilterSoldierObject();
    void UpdateFromGameObject();
    void UpdateManualUpdateVars();
    void SetOneTimeVars();
    static void* operator new(unsigned long, void*);

    static void operator delete(void*) {}

    ScriptObjectView* m_script;
    void* m_scriptData;
};

void CAIFilterSoldierObject::UpdateManualUpdateVars() {
    CSoldierDoodadView* doodad = (CSoldierDoodadView*)m_gameObject;
    CAISoldierView* ai = (CAISoldierView*)m_aiObject;
    CWeapon* weapon = doodad->m_soldier->m_weapons[doodad->m_soldier->m_currentWeapon];
    float speed = 1.0f;
    if (weapon)
        speed = weapon->GetBulletSpeed();
    ai->m_bulletSpeed = speed;
    ai->m_bulletTime = 1.0f / ai->m_bulletSpeed;
    weapon = doodad->m_soldier->m_weapons[doodad->m_soldier->m_currentWeapon];
    int type;
    if (!weapon) {
        type = 9;
    } else {
        switch (weapon->m_weaponId) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 14:
        case 15:
        case 16:
        case 20:
        case 21:
        case 24:
        case 25:
        case 26:
        case 27:
        case 29:
        case 35:
        case 36:
        case 37:
        case 39:
        default:
            type = 1;
            break;
        case 10:
        case 11:
        case 12:
        case 13:
        case 28:
            type = 2;
            break;
        case 30:
        case 31:
        case 32:
            type = 5;
            break;
        case 17:
        case 18:
        case 19:
        case 22:
        case 23:
            type = 4;
            break;
        case 33:
        case 34:
        case 40:
            type = 6;
            break;
        case 38:
            type = 8;
            break;
        }
    }
    ai->m_weaponType = type;
}

void CAIFilterSoldierObject::SetOneTimeVars() {
    CSoldierDoodadView* doodad = (CSoldierDoodadView*)m_gameObject;
    CAISoldierView* ai = (CAISoldierView*)m_aiObject;
    m_script = doodad->unknown00c()->GetScriptObject();
    m_scriptData = m_script->unknown0c;
    SoldierPropsView* props = doodad->m_soldier->GetScriptObject()->data->props;
    if (props->unknown79 == 0)
        ai->m_unknown1f0 = 0.1f;
    else
        ai->m_unknown1f0 = 0.1f * props->unknown79;
    switch (props->unknown75) {
    case 0:
        ai->m_unknown1f8 = 0.5f;
        break;
    case 1:
        ai->m_unknown1f8 = 1.0f;
        break;
    case 2:
        ai->m_unknown1f8 = 2.0f;
        break;
    }
    switch (props->unknown73) {
    case 0:
        ai->m_unknown1f4 = 0.05235988f;
        ai->m_unknown1fc = 1.25f;
        break;
    case 1:
        ai->m_unknown1f4 = 0.034906585f;
        ai->m_unknown1fc = 0.75f;
        break;
    case 2:
        ai->m_unknown1f4 = 0.017453292f;
        ai->m_unknown1fc = 0.333f;
        break;
    }
    ai->m_unknown204 = 0.0625f * props->unknown7e;
    ai->m_unknown208 = 0.0625f * props->unknown80;
    ai->m_unknown210 = 0.0625f * props->unknown82;
    ai->m_unknown214 = props->unknown72;
    ai->m_unknown014 = props->unknown35;
    ai->m_unknown015 = 4;
    ai->m_unknown016 = props->unknown38;
    ai->m_unknown76c = props->unknown70;
    ai->m_unknown768 = ai->m_unknown76c;
    UpdateFromGameObject();
}

void* CAIFilterSoldierObject::operator new(unsigned long, void* place) {
    return place;
}

CAIFilterSoldierObject::~CAIFilterSoldierObject() {
    delete m_aiObject;
}
