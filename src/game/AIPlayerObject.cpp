// AIPlayerObject.cpp: CAIPlayerObject, the AI-side view of the player. The
// whole file: the weak IsPlayer, IsCrouching, Init, Update, PreUpdate, the
// destructor (its table pointer, then CAIObject's destructor, and the object
// freed when asked) and the constructor (CAIObject's constructor, its table
// pointer, the owner kept at +8), with the class's virtual table and RTTI
// record. The class names come from the mangled symbols, the table's slot
// order from __vt__15CAIPlayerObject. Only the members these functions touch
// are declared, at their offsets (their names are not original). IsPlayer is
// inline in the original (a weak symbol), so it is defined __declspec(weak);
// the destructor is the key function. The health value is rounded up to
// fifteenths of the maximum.
#include "game/CPlayerObject.h"


#include "game/CScene.h"


extern CScene g_scene;

class CAIObject {
public:
    CAIObject();
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;
    virtual bool IsPlayer() const;
    void Init();

    unsigned char unknown0004[4];
    void* m_owner;
    unsigned char unknown000c[8];
    unsigned char unknown0014;
    unsigned char unknown0015;
    unsigned char unknown0016[414];
    int health;
    int unknown01b8;
    int unknown01bc;
    int unknown01c0;
    int unknown01c4;
    int unknown01c8;
    unsigned char unknown01cc[8];
    int unknown01d4;
    unsigned char unknown01d8[60];
    int unknown0214;
    unsigned char unknown0218[1312];
    float currentHealth;
    float maxHealth;
};

class CAIPlayerObject : public CAIObject {
public:
    virtual ~CAIPlayerObject();
    virtual bool IsPlayer() const;
    virtual bool IsCrouching() const;
    void Init();
    virtual void Update(float);
    virtual void PreUpdate();
    CAIPlayerObject(void*);
};

__declspec(weak) bool CAIPlayerObject::IsPlayer() const {
    return true;
}

bool CAIPlayerObject::IsCrouching() const {
    return g_scene.GetPlayer(0)->IsPlayerCrouching();
}

void CAIPlayerObject::Init() {
    CAIObject::Init();
    health = 15.0f * (currentHealth / maxHealth) + 0.999f;
    unknown01b8 = unknown0015;
    unknown01bc = unknown0214;
    unknown01c0 = 4;
    unknown01c4 = 1;
    unknown01c8 = unknown0014;
    unknown01d4 = 0;
}

void CAIPlayerObject::Update(float) {
    health = 15.0f * (currentHealth / maxHealth) + 0.999f;
    unknown01c0 = 4;
    unknown01c4 = 1;
}

void CAIPlayerObject::PreUpdate() {
}

CAIPlayerObject::~CAIPlayerObject() {
}

CAIPlayerObject::CAIPlayerObject(void* owner) {
    m_owner = owner;
}
