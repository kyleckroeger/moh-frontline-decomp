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

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    void Sub(float ax, float ay, float az) {
        x -= ax;
        y -= ay;
        z -= az;
    }
    void Add(float ax, float ay, float az) {
        x += ax;
        y += ay;
        z += az;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
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
class CPlayerObject;

/* inferred: the script object's user data and its team byte */
struct BSDataView {
    unsigned char unknown00[53];
    unsigned char m_team;
};

struct BSUserView {
    unsigned char unknown00[4];
    BSDataView* m_data;
};

struct BSObject {
    unsigned char unknown00[8];
    BSUserView* m_user;
};

inline CVector3 Lerp(const CVector3& a, const CVector3& b, float t) {
    float dx = (b.x - a.x) * t;
    float dy = (b.y - a.y) * t;
    float dz = (b.z - a.z) * t;
    return CVector3(a.x + dx, a.y + dy, a.z + dz);
}

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

class CCameraView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
};

class CPlayerObject {
public:
    unsigned char unknown0000[1892];
    unsigned long m_controller;
    unsigned char unknown0768[104];
    CCameraView m_camera;
    unsigned char unknown07d4[668];
    UserInterface m_ui;
    unsigned char unknown0a71[2111];
    int m_team;
};

/* inferred: the shell's team-play flag */
class CShellMenu {
public:
    unsigned char unknown0000[5172];
    int m_teamPlay;
    unsigned char unknown1438[1444];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[24];
    ISceneNode* m_worldObject;
    unsigned char unknown01c[300];
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
    virtual bool Visit(T&) = 0;
};
}

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[12];
    float m_t;
    unsigned char unknown10[8];
    struct ContactView* m_contacts;
    unsigned char unknown1c[4];
};

/* inferred: a surface's packed type fields (+12) */
struct SurfaceView {
    unsigned char unknown00[12];
    signed int m_surface : 11;
    signed int m_texSwap : 11;
    unsigned int m_rest : 10;
};

struct ContactView {
    unsigned char unknown00[36];
    SurfaceView* m_surface;
};

class CSurfaceType {
public:
    static bool IsShootThru(int);
};

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.x = m_end.x - m_start.x;
        m_dir.y = m_end.y - m_start.y;
        m_dir.z = m_end.z - m_start.z;
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class IVolume {
public:
    virtual ~IVolume();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber(CVector3, CVector3);
    CVolFiber(const CLine3&);
    virtual ~CVolFiber();

    unsigned char unknown04[12];
    CLine3 m_line;
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
    CVector3 GetCenter() const;

    unsigned char unknown04[28];
} __attribute__((aligned(8)));

class CImpact : public dwi::IVisitor<ISceneNode> {
public:
    virtual bool Visit(ISceneNode&);
    virtual ~CImpact();
    bool DamageClosestObject();

    unsigned char unknown04[12];
    CVolSphere m_sphere;
    unsigned char unknown30[4];
    float m_damage;
    unsigned char unknown38[8];
    ISceneNode* m_owner;
    ISceneNode* m_target;
    float m_closest;
    unsigned char unknown4c[4];
    CVector3 m_position;
};

CImpact::~CImpact() {
}

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


bool CImpact::Visit(ISceneNode& node) {
    if (&node == m_owner)
        return false;
    if (node.GetCollisionId() == -1)
        return false;
    if (node.GetScriptObject()) {
        if (node.AsSoldierObject() && m_owner->AsSoldierObject()) {
            unsigned char nodeTeam = 255;
            unsigned char ownerTeam = 255;
            BSObject* script = node.GetScriptObject();
            if (script && script->m_user)
                nodeTeam = script->m_user->m_data->m_team;
            script = m_owner->GetScriptObject();
            if (script && script->m_user)
                ownerTeam = script->m_user->m_data->m_team;
            if (nodeTeam != 255 && ownerTeam != 255 && (ownerTeam == nodeTeam || nodeTeam == 0))
                return false;
        }
        CVector3 forward;
        CVector3 origin;
        if (m_owner->AsPlayerObject()) {
            int team = node.GetScriptObject()->m_user->m_data->m_team;
            if (node.AsSoldierObject() && team == 0)
                return false;
            if (node.AsPlayerObject() && g_Shell.m_teamPlay == 0) {
                int nodeTeam = node.AsPlayerObject()->m_team;
                if (m_owner->AsPlayerObject()->m_team == nodeTeam)
                    return false;
            }
            CCameraView* camera = &m_owner->AsPlayerObject()->m_camera;
            camera->GetForward(forward);
            camera->GetPosition(origin);
        } else {
            m_owner->GetForward(forward);
            m_owner->GetPosition(origin);
            origin.Add(0.0f, 0.0f, 0.75f);
        }
        CVector3 target;
        CVector3 delta;
        if (node.AsSoldierObject() || node.AsPlayerObject()) {
            if (g_bInMultiplayerMode) {
                if (node.AsPlayerObject()) {
                    node.AsPlayerObject()->m_camera.GetPosition(target);
                    target.Sub(0.0f, 0.0f, 0.5f);
                } else {
                    return false;
                }
            } else {
                node.GetPosition(target);
                target.Add(0.0f, 0.0f, 0.75f);
            }
            delta.x = target.x - origin.x;
            delta.y = target.y - origin.y;
            delta.z = target.z - origin.z;
            CVector3 direction(delta);
            direction.Normalize();
            if (direction.x * forward.x + direction.y * forward.y + direction.z * forward.z < 0.85f)
                return false;
        } else {
            const CVector3 start(origin);
            CVector3 end(forward);
            end.Normalize();
            end.x *= 1.275f;
            end.y *= 1.275f;
            end.z *= 1.275f;
            end.x += start.x;
            end.y += start.y;
            end.z += start.z;
            CVolFiber fiber(start, end);
            IVolume* volume = (IVolume*)node.GetWorldBoundingVolume((ISceneNode::EVolumeType)0);
            CCollision collision;
            if (volume->TestCollision(fiber, collision, false) == 2) {
                target = Lerp(start, end, collision.m_t);
                delta.x = target.x - origin.x;
                delta.y = target.y - origin.y;
                delta.z = target.z - origin.z;
            } else {
                return false;
            }
        }
        CLine3 line(origin, target);
        CVolFiber fiber(line);
        {
            IVolume* world = (IVolume*)g_scene.m_worldObject->GetWorldBoundingVolume((ISceneNode::EVolumeType)0);
            CCollision collision;
            if (world->TestCollision(fiber, collision, true) == 2 && collision.m_contacts->m_surface->m_surface >= 0 &&
                !CSurfaceType::IsShootThru(collision.m_contacts->m_surface->m_surface))
                return false;
        }
        float zz = delta.z * delta.z;
        float length = zz + (delta.x * delta.x + delta.y * delta.y);
        if (sqrtf(length) < m_closest) {
            m_closest = sqrtf(length);
            m_target = &node;
            m_position = target;
        }
    }
    return false;
}
