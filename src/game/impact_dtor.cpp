// A fragment of impact.cpp (0x800d0164): CImpact::DamageClosestObject sends
// the impact's target (+68) a damage event (script event 43): a 24-byte event
// block holding the owner's (+64) script object, the damage (+52; doubled
// when the target faces away from the sphere's centre, which also marks the
// hit as from behind), event type 25, and, for a target that is not a
// player, whether it was hit from the front. A player target gets no damage
// while cheats are on, a hit direction for its interface (left, right, front
// or behind from the target's right and forward vectors) and controller
// feedback, as does a player owner; a player or soldier target also starts
// an AEMS sound event at the impact's position (+80) with the listener's
// azimuth. Then the CImpact destructor (its virtual table
// pointer, the sphere volume at +16 destroyed, then the inline
// dwi::IVisitor<ISceneNode> destructor, and the object freed when asked). The
// file name is this project's; the original record is impact.cpp. The classes,
// the template and the functions are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode;
// the visitor's table lists its destructor and a pure Visit, the impact's
// members are inferred from offsets, as are the event block, player, cheat
// flag and input manager views; the constants are entries of the file's
// .sdata2 pool. CImpact declares its Visit override (defined elsewhere) first
// so its global virtual table is not emitted here. The visitor's destructor is
// inline and its table weak in the original, so the compiler's copies are
// weak duplicates, linked to the original copies.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
struct BSObject;
class CPlayerObject;

class ISceneNode {
public:
    enum EVolumeType {};
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual void* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class UserInterface {
public:
    void AddHitDirection(unsigned int);
};

class CPlayerObject {
public:
    unsigned char unknown0000[1892];
    unsigned long m_controller;
    unsigned char unknown0768[776];
    UserInterface m_ui;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

class CInputManager {
public:
    void SetFeedback(unsigned long, long, long, float, float);

    unsigned char unknown00[64];
};

struct CheatsView {
    bool enabled;
    unsigned char unknown01[15];
};

extern CScene g_scene;
extern CInputManager g_inputMgr;
extern CheatsView g_cheats;

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
unsigned short GetAzimuth(CVector3, const CPlayerObject*);
void AEMS_BeginEvent(void*);

struct DamageEventView {
    BSObject* source;
    int damage;
    int fromBehind;
    int type;
    int front;
    int unknown14;
};

class ISceneNode;

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor() {}
    virtual void Visit(T&) = 0;
};
}

class IVolume {
public:
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
    CVector3 GetCenter() const;

    unsigned char unknown04[28];
} __attribute__((aligned(8)));

class CImpact : public dwi::IVisitor<ISceneNode> {
public:
    virtual void Visit(ISceneNode&);
    virtual ~CImpact();
    bool DamageClosestObject();

    unsigned char unknown04[12];
    CVolSphere m_sphere;
    unsigned char unknown30[4];
    float m_damage;
    unsigned char unknown38[8];
    ISceneNode* m_owner;
    ISceneNode* m_target;
    unsigned char unknown48[8];
    CVector3 m_position;
};

bool CImpact::DamageClosestObject() {
    DamageEventView* event = (DamageEventView*)BSEventGetEventMemoryBlock(sizeof(DamageEventView));
    CVector3 delta;
    CVector3 forward;
    m_target->GetForward(forward);
    m_target->GetPosition(delta);
    const CVector3& center = m_sphere.GetCenter();
    delta.x -= center.x;
    delta.y -= center.y;
    delta.z -= center.z;
    event->type = 25;
    if (m_owner)
        event->source = m_owner->GetScriptObject();
    else
        event->source = 0;
    if (forward.x * delta.x + forward.y * delta.y + forward.z * delta.z <= 0.0f) {
        event->damage = m_damage;
        event->fromBehind = 0;
    } else {
        event->damage = 2.0f * m_damage;
        event->fromBehind = 1;
    }
    if (g_cheats.enabled && m_target->AsPlayerObject())
        event->damage = 0;
    if (m_target->AsPlayerObject()) {
        CPlayerObject* player = m_target->AsPlayerObject();
        CVector3 right;
        m_target->GetRightward(right);
        float ahead = forward.x * delta.x + forward.y * delta.y + forward.z * delta.z;
        float side = right.x * delta.x + right.y * delta.y + right.z * delta.z;
        if (side < -0.7071f)
            player->m_ui.AddHitDirection(2);
        else if (side > 0.7071f)
            player->m_ui.AddHitDirection(4);
        else if (ahead < 0.0f)
            player->m_ui.AddHitDirection(1);
        else
            player->m_ui.AddHitDirection(3);
    } else {
        if (event->fromBehind == 1)
            event->front = 0;
        else
            event->front = 1;
    }
    if (m_target->AsPlayerObject())
        g_inputMgr.SetFeedback(m_target->AsPlayerObject()->m_controller, 12, 0, 1.0f, 0.1f);
    if (m_owner->AsPlayerObject())
        g_inputMgr.SetFeedback(m_owner->AsPlayerObject()->m_controller, 12, 0, 1.0f, 0.1f);
    if (m_target->AsPlayerObject() || m_target->AsSoldierObject()) {
        int sound[5];
        sound[0] = 3;
        sound[1] = 3;
        sound[2] = 0;
        CPlayerObject* listener = g_scene.GetPlayer(0);
        sound[3] = GetAzimuth(m_position, listener);
        sound[4] = 1;
        AEMS_BeginEvent(sound);
    }
    BSObjectTriggerEvent(m_target->GetScriptObject(), 43, event, 0, false);
    return true;
}

CImpact::~CImpact() {
}
