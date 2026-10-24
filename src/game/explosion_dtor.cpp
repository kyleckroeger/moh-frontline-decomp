// A fragment of explosion.cpp (0x800cedd4): CExplosion::Visit, the destructor
// (its virtual table pointer, the sphere volume at +16 destroyed, then the
// inline dwi::IVisitor<ISceneNode> destructor, and the object freed when asked)
// and the constructor (the sphere built at the position with the radius
// through an inferred inline constructor; strength, a second value, the bullet
// type, the source node and a flag kept; a noise made at the position (type
// 52, six times the strength); every scene node in the sphere visited; the
// scene's world object told about the explosion when there is one; the
// constants are items of the file's .sdata2 pool). Visit pushes, damages and
// alerts every visited node in range (see the function). The file is compiled
// with RTTI and deferred inlining (functions emitted in reverse source order;
// the destructor's place before Visit in the source also gives the original
// order of the RTTI name strings: CExplosion's, then the visitor's), so this
// unit owns CExplosion's virtual table and RTTI records. The file name is
// this project's; the original record is explosion.cpp. The classes, the
// template and the functions are named by the mangled symbols (the argument
// and member names are inferred); the visitor's table lists its destructor
// and a pure Visit, the members before the sphere are not known, and only the
// virtuals the code needs are declared. The visitor's and IVolume's
// destructors are inline and their tables weak in the original, so the
// compiler's copies are weak duplicates, linked to the original copies.
struct BSObject;
class CVector3;
class CPlayerObject;
class CSoldierObject;
class IMovingSceneNode;
class CBullet;

class ISceneNode {
public:
    enum EVolumeType {};
    virtual void unknown008();
    virtual void unknown012();
    virtual void unknown016();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown032();
    virtual void unknown036();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown052();
    virtual void unknown056();
    virtual void unknown060();
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void unknown076();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown092();
    virtual BSObject* GetScriptObject() const;
    virtual void unknown100();
    virtual void unknown104();
    virtual IMovingSceneNode* AsMovingNode();
    virtual void unknown112();
    virtual void unknown116();
    virtual void unknown120();
    virtual void* AsHierObject();
    virtual void unknown128();
    virtual void unknown132();
    virtual void unknown136();
    virtual void unknown140();
    virtual void unknown144();
    virtual CSoldierObject* AsSoldierObject();
    virtual void unknown152();
    virtual CPlayerObject* AsPlayerObject();
    virtual void unknown160();
    virtual void unknown164();
    virtual void unknown168();
    virtual void unknown172();
    virtual void unknown176();
    virtual void unknown180();
    virtual void unknown184();
    virtual CBullet* AsBullet();
    virtual void unknown192();
    virtual void* AsCollisionVolume();
};
class CAIFilterObject;
class CWorldObject;
class CExplosion;
enum EScriptBulletType {};

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

/* inferred: a 16-byte vector (four floats overlaid with two doubles, copied
   as two doubles) */
class CVector3 {
public:
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

void MakeNoise(void*, CAIFilterObject*, const CVector3&, int, float, bool, int);

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor() {}
    virtual bool Visit(T&) = 0;
};
}

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    float GetRadius() const;
    CVector3 GetCenter() const;
    CVolSphere(CVector3 center, float radius) : m_radius(radius), m_center(center) {}
    virtual IVolume* Create() const;
    virtual ~CVolSphere();

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
};

class CWorldObject {
public:
    void HandleExplosion(const CExplosion&);
};

class CScene {
public:
    void VisitAllNodesInVolume(dwi::IVisitor<ISceneNode>&, const IVolume&, ISceneNode::EVolumeType, bool);

    unsigned char unknown000[24];
    CWorldObject* m_worldObject;
};

/* inferred: the hit event's block (BS_STRUCT_HitByBulletInfo_struct) */
struct BS_STRUCT_HitByBulletInfo_struct {
    BSObject* m_source;
    int m_damage;
    int m_front;
    int m_type;
    int m_location;
    int m_unknown14;
};

class UserInterface {
public:
    void AddHitDirection(unsigned int);
};

class CPlayerObject : public ISceneNode {
public:
    void SetMPShotLocation(BS_STRUCT_HitByBulletInfo_struct*);
    /* inferred accessor for the flag bit */
    unsigned char IsNoExplosionPush() const { return m_noExplosionPush; }

    unsigned char unknown004[914];
    unsigned char unknown396 : 6;
    unsigned char m_noExplosionPush : 1;
    unsigned char unknown396b : 1;
    unsigned char unknown397[1892 - 919];
    unsigned long m_controller;
    unsigned char unknown768[2672 - 1896];
    UserInterface m_ui;
};

class CSoldierObject {
public:
    unsigned char unknown0000[12641];
    bool m_noExplosionPush;
};

class IMovingSceneNode {
public:
    virtual void unknown008();
    virtual void unknown012();
    virtual void unknown016();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown032();
    virtual void unknown036();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown052();
    virtual void unknown056();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown072();
    virtual void unknown076();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown092();
    virtual void unknown096();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown112();
    virtual void unknown116();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown132();
    virtual void unknown136();
    virtual void unknown140();
    virtual void unknown144();
    virtual void unknown148();
    virtual void unknown152();
    virtual void unknown156();
    virtual void unknown160();
    virtual void unknown164();
    virtual void unknown168();
    virtual void unknown172();
    virtual void unknown176();
    virtual void unknown180();
    virtual void unknown184();
    virtual void unknown188();
    virtual void unknown192();
    virtual void unknown196();
    virtual void unknown200();
    virtual void unknown204();
    virtual void unknown208();
    virtual void unknown212();
    virtual void unknown216();
    virtual void unknown220();
    virtual void unknown224();
    virtual void ApplyForceTo(CVector3, float);
};

class CThrownBullet {
public:
    void Explode();
};

class CBullet {
public:
    virtual void unknown008();
    virtual void unknown012();
    virtual void unknown016();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown032();
    virtual void unknown036();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown052();
    virtual void unknown056();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown072();
    virtual void unknown076();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown092();
    virtual void unknown096();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown112();
    virtual void unknown116();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown132();
    virtual void unknown136();
    virtual void unknown140();
    virtual void unknown144();
    virtual void unknown148();
    virtual void unknown152();
    virtual void unknown156();
    virtual void unknown160();
    virtual void unknown164();
    virtual void unknown168();
    virtual void unknown172();
    virtual void unknown176();
    virtual void unknown180();
    virtual void unknown184();
    virtual void unknown188();
    virtual void unknown192();
    virtual void unknown196();
    virtual void unknown200();
    virtual void unknown204();
    virtual void unknown208();
    virtual void unknown212();
    virtual void unknown216();
    virtual void unknown220();
    virtual void unknown224();
    virtual void unknown228();
    virtual void unknown232();
    virtual void unknown236();
    virtual void unknown240();
    virtual void unknown244();
    virtual void unknown248();
    virtual void unknown252();
    virtual void unknown256();
    virtual void unknown260();
    virtual void unknown264();
    virtual void unknown268();
    virtual void unknown272();
    virtual void unknown276();
    virtual void unknown280();
    virtual void unknown284();
    virtual void unknown288();
    virtual void unknown292();
    virtual void unknown296();
    virtual void unknown300();
    virtual void unknown304();
    virtual void unknown308();
    virtual void unknown312();
    virtual CThrownBullet* AsThrown();
};

class CInputManager {
public:
    void SetFeedback(unsigned long, long, long, float, float);

    unsigned char unknown00[64];
};

extern CInputManager g_inputMgr;

/* inferred: the cheat flags read here */
struct CheatsView {
    unsigned char m_invulnerable;
    unsigned char unknown01[13];
    unsigned char m_oneHitKills;
    unsigned char unknown0f[5];
};

extern CheatsView g_cheats;
extern bool g_bInMultiplayerMode;
extern float g_fScriptExplosionHeightScaleOnPlayer;
extern float g_fScriptExplosionVelocityScaleOnPlayer;
void* BSEventGetEventMemoryBlock(unsigned int);
bool BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
extern "C" int rand();

class CExplosion : public dwi::IVisitor<ISceneNode> {
public:
    virtual bool Visit(ISceneNode&);
    virtual ~CExplosion();
    bool CheckForObstructions(ISceneNode&);
    CExplosion(CScene&, CVector3, float, float, float, EScriptBulletType, ISceneNode*, bool);

    unsigned char unknown04[12];
    CVolSphere m_sphere;
    float data30;
    float data34;
    EScriptBulletType m_type;
    ISceneNode* m_source;
    bool data40;
};

CExplosion::CExplosion(CScene& scene, CVector3 position, float radius, float strength, float value34,
                       EScriptBulletType type, ISceneNode* source, bool flag)
    : m_sphere(position, radius), data30(strength), data34(value34), m_type(type), m_source(source),
      data40(flag) {
    MakeNoise(0, 0, position, 52, strength * 6.0, true, 5);
    scene.VisitAllNodesInVolume(*this, m_sphere, (ISceneNode::EVolumeType)0, true);
    if (scene.m_worldObject)
        scene.m_worldObject->HandleExplosion(*this);
}

CExplosion::~CExplosion() {
}

bool CExplosion::Visit(ISceneNode& node) {
    if (node.GetScriptObject()) {
        if (CheckForObstructions(node))
            return false;
        float range = 0.8f * m_sphere.GetRadius();
        BS_STRUCT_HitByBulletInfo_struct* info = (BS_STRUCT_HitByBulletInfo_struct*)BSEventGetEventMemoryBlock(24);
        CVector3 delta;
        CVector3 forward;
        node.GetForward(forward);
        node.GetPosition(delta);
        const CVector3& center = m_sphere.GetCenter();
        delta.d.v[0] = delta.d.v[0] - center.d.v[0];
        delta.d.v[1] = delta.d.v[1] - center.d.v[1];
        delta.d.v[2] = delta.d.v[2] - center.d.v[2];
        float falloff = 1.0f - delta.Length() / m_sphere.GetRadius();
        if (falloff < 0.5f)
            falloff = 0.5f;
        info->m_damage = data34 * falloff;
        info->m_type = m_type;
        if (m_source)
            info->m_source = m_source->GetScriptObject();
        else
            info->m_source = 0;
        if (forward.d.v[0] * delta.d.v[0] + forward.d.v[1] * delta.d.v[1] + forward.d.v[2] * delta.d.v[2] <= 0.0f)
            info->m_front = 0;
        else
            info->m_front = 1;
        if (node.AsPlayerObject()) {
            if (g_cheats.m_invulnerable)
                info->m_damage = 0;
            else if (g_cheats.m_oneHitKills)
                info->m_damage = 1000;
            if (g_bInMultiplayerMode && info->m_source) {
                if (info->m_front == 1)
                    info->m_location = 10;
                else
                    info->m_location = 9;
                node.AsPlayerObject()->SetMPShotLocation(info);
            }
        }
        if (node.AsMovingNode() && delta.Length() < range) {
            int noPush = 0;
            CPlayerObject* player = node.AsPlayerObject();
            if (player)
                noPush = player->IsNoExplosionPush();
            CSoldierObject* soldier = node.AsSoldierObject();
            if ((!soldier || !soldier->m_noExplosionPush) && !noPush) {
                IMovingSceneNode* moving = node.AsMovingNode();
                delta.Normalize();
                if (player && data40) {
                    delta.d.v[2] = g_fScriptExplosionHeightScaleOnPlayer * (0.5f * falloff + 0.5f);
                    moving->ApplyForceTo(delta, g_fScriptExplosionVelocityScaleOnPlayer * (30.0f * falloff + 20.0f));
                } else {
                    delta.d.v[2] = 0.5f * falloff + 0.5f;
                    moving->ApplyForceTo(delta, 30.0f * falloff + 20.0f);
                }
            }
            if (!BSObjectTriggerEvent(node.GetScriptObject(), 176, info, 0, false)) {
                BS_STRUCT_HitByBulletInfo_struct* copy =
                    (BS_STRUCT_HitByBulletInfo_struct*)BSEventGetEventMemoryBlock(24);
                if (copy) {
                    *copy = *info;
                    if (!g_bInMultiplayerMode)
                        copy->m_type = 5;
                    if (info->m_front == 1)
                        copy->m_location = 10;
                    else
                        copy->m_location = 9;
                    BSObjectTriggerEvent(node.GetScriptObject(), 43, copy, 0, false);
                }
            }
        } else if (node.AsHierObject()) {
            info->m_location = 0;
            BSObjectTriggerEvent(node.GetScriptObject(), 176, info, 0, false);
        } else if (node.AsCollisionVolume()) {
            BSObjectTriggerEvent(node.GetScriptObject(), 43, info, 0, false);
        } else {
            if (!g_bInMultiplayerMode)
                info->m_type = 5;
            info->m_location = rand() % 21;
            BSObjectTriggerEvent(node.GetScriptObject(), 43, info, 0, false);
        }
        CPlayerObject* player = node.AsPlayerObject();
        if (player) {
            CVector3 playerForward;
            player->GetForward(playerForward);
            CVector3 playerRight;
            player->GetRightward(playerRight);
            float ahead = playerForward.d.v[0] * delta.d.v[0] + playerForward.d.v[1] * delta.d.v[1] + playerForward.d.v[2] * delta.d.v[2];
            float side = playerRight.d.v[0] * delta.d.v[0] + playerRight.d.v[1] * delta.d.v[1] + playerRight.d.v[2] * delta.d.v[2];
            if (side < -0.7071f)
                player->m_ui.AddHitDirection(2);
            else if (side > 0.7071f)
                player->m_ui.AddHitDirection(4);
            else if (ahead < 0.0f)
                player->m_ui.AddHitDirection(1);
            else
                player->m_ui.AddHitDirection(3);
            g_inputMgr.SetFeedback(player->m_controller, 2, 0, falloff, 1.0f);
        }
    } else {
        CBullet* bullet = node.AsBullet();
        if (bullet) {
            CThrownBullet* thrown = bullet->AsThrown();
            if (thrown) {
                if (CheckForObstructions(node))
                    return false;
                thrown->Explode();
            }
        }
    }
    return false;
}

