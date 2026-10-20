// CAISoldierDoodad, the fire, update, clean-up and initialisation functions in
// the middle of the file: the AI doodad that drives a soldier through a
// CAIFilterSoldierObject placed in the soldier's memory. Both fire functions
// take the filter's target guess position, finish a pending reload of the
// soldier's current weapon (20 rounds added), fire (the MMG at the target, or
// the current weapon from the bullet emitter's position towards the target)
// and, when a shot was fired, set bit 0 of the AI soldier's flags (+432). The
// class names come from the mangled symbols. This is an inferred, non-virtual
// view: only the members these functions touch are declared, at their offsets
// (their names are not original), and the virtual table is not reproduced; the
// reload, weapon-slot and translation helpers are inferred inlines (the
// translation returned by value gives the original's dead stack copy). The
// scene-node function before these and the destructor and constructor after
// them are not reconstructed here.
class CStaticObject;
struct BSObject;

struct CVector3Pair {
    double a;
    double b;
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& o) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&o;
        return *this;
    }

    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CAIFilterRealPosition : public CVector3 {
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    CVector3 GetTranslation() const { return *(const CVector3*)m[3]; }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CWeapon {
public:
    void DoneReloading();
    void AddAmo(short);

    unsigned char unknown000[704];
    unsigned char m_reloading : 1;
    unsigned char unknown2c0_1 : 7;
};

class CSoldierObject {
public:
    bool FireMMG(CVector3&, CStaticObject*);
    bool FireWeapon(int, CVector3&, CVector3&, float);
    void GetBulletEmitterMatrix(CMatrix&);
    CWeapon* GetCurrentWeapon() { return m_weapons[m_currentWeapon]; }
    CWeapon* GetWeapon(int i) { return m_weapons[i]; }
    bool IsReloading() {
        CWeapon* weapon = GetCurrentWeapon();
        unsigned char reloading = false;
        if (weapon)
            reloading = weapon->m_reloading;
        return reloading;
    }
    void FinishReloading(int i) {
        if (GetWeapon(i)) {
            GetWeapon(i)->DoneReloading();
            GetWeapon(i)->AddAmo(20);
        }
    }

    unsigned char unknown0000[16400];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};


int BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAISoldierObject {
public:
    void Init();

    unsigned char unknown000[432];
    unsigned int m_flags;
};

class CAIFilterObject {
public:
    void LinkGameObject(void*);
    void GetTargetGuessPosition(CAIFilterRealPosition&);
};

class CAIFilterSoldierObject : public CAIFilterObject {
public:
    CAIFilterSoldierObject();
    virtual ~CAIFilterSoldierObject();
    void* operator new(unsigned long, void*);
    void Update(float);
    void PreUpdate();
    void SetOneTimeVars();
    void UpdateManualUpdateVars();

    unsigned char unknown04[4];
    CAISoldierObject* m_soldierObject;
    BSObject* m_script;
    unsigned char unknown10[4];
};

class CAIDoodad {
public:
    void Init();

    unsigned char unknown00[4];
};

class CAISoldierDoodad : public CAIDoodad {
public:
    void FireMMGAtCurrentTarget(CStaticObject*);
    void FireAtCurrentTarget(float);
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CSoldierObject*);

    CAIFilterSoldierObject* m_filter;
    int unknown08;
    int unknown0c;
    unsigned char unknown10[64];
    CSoldierObject* m_soldier;
};

void CAISoldierDoodad::FireMMGAtCurrentTarget(CStaticObject* gun) {
    CAIFilterSoldierObject* filter = m_filter;
    CAIFilterRealPosition target;

    filter->GetTargetGuessPosition(target);
    if (m_soldier->IsReloading())
        m_soldier->FinishReloading(m_soldier->m_currentWeapon);
    if (m_soldier->FireMMG(target, gun))
        filter->m_soldierObject->m_flags |= 1;
}

void CAISoldierDoodad::FireAtCurrentTarget(float spread) {
    CAIFilterSoldierObject* filter = m_filter;
    CAIFilterRealPosition target;

    filter->GetTargetGuessPosition(target);
    if (m_soldier->IsReloading())
        m_soldier->FinishReloading(m_soldier->m_currentWeapon);
    CMatrix emitter;
    m_soldier->GetBulletEmitterMatrix(emitter);
    CVector3 origin;
    origin = emitter.GetTranslation();
    CVector3 direction;
    direction.x = target.x - origin.x;
    direction.y = target.y - origin.y;
    direction.z = target.z - origin.z;
    if (m_soldier->FireWeapon(m_soldier->m_currentWeapon, origin, direction, spread))
        filter->m_soldierObject->m_flags |= 1;
}

void CAISoldierDoodad::Update(float time) {
    CAIFilterSoldierObject* filter = m_filter;

    filter->Update(time);
    if ((unknown08 & 1) && !(unknown0c & 1))
        BSObjectTriggerEvent(filter->m_script, 124, 0, 0, false);
}

void CAISoldierDoodad::PreUpdate() {
    m_filter->PreUpdate();
}

void CAISoldierDoodad::CleanUp() {
    delete m_filter;
}

void CAISoldierDoodad::Init(CSoldierObject* soldier) {
    m_filter = new ((char*)soldier + 17584) CAIFilterSoldierObject;
    m_filter->LinkGameObject(this);
    m_soldier = soldier;
    CAIDoodad::Init();
    CAIFilterSoldierObject* filter = m_filter;
    filter->SetOneTimeVars();
    filter->UpdateManualUpdateVars();
    filter->m_soldierObject->Init();
}
