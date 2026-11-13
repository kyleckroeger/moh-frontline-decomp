// CAIPlayerObject, the functions at the start of the file: the AI-side view
// of the player. The class names come from the mangled symbols. This is an
// inferred, non-virtual view: only the members these functions touch are
// declared, at their offsets (their names are not original), and the class's
// virtual table is not reproduced, so the destructor and constructor that
// store it are not reconstructed here. IsPlayer is inline in the original (a
// weak symbol), so it is defined __declspec(weak). The health value is
// rounded up to fifteenths of the maximum.
class CPlayerObject {
public:
    bool IsPlayerCrouching() const;
};

#include "game/CScene.h"


extern CScene g_scene;

class CAIObject {
public:
    void Init();

    unsigned char unknown0000[20];
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
    bool IsPlayer() const;
    bool IsCrouching() const;
    void Init();
    void Update(float);
    void PreUpdate();
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
