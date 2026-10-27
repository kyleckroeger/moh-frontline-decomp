// A fragment of bsbifunc.cpp (0x800238e4): script built-ins that create a
// script impact (a CImpact on the stack that hits the scene nodes in its
// sphere) or a script explosion (a CExplosion constructed and destroyed on the stack,
// bullet type 21, at the calling node's position or the script trigger's,
// moved by the given offset), set the ambient track's location to the script
// trigger's position (or clear it), switch the player's muzzle fire, and set
// the ambient track's volume (the argument in thousandths; 1000.0f is an
// entry of the file's .sdata2 pool). Each reads its arguments below the
// script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around
// these are not reconstructed. The functions, classes and globals are named
// by the mangled symbols; ISceneNode is declared with its virtual functions
// in the order of __vt__10ISceneNode (GetPosition at +64); CExplosion is a
// view of 80 bytes, 16-byte aligned (inferred from the stack frame), and the
// vector view (four floats, 8-byte aligned, with an inline setter), trigger,
// script-object and built-in record views are inferred.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;

    void Set(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
} __attribute__((aligned(8)));
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;

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
    virtual int GetScriptObject() const;
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
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};


class IVolume;

namespace dwi {
template <class T>
class IVisitor {
public:
    IVisitor() {}
    virtual ~IVisitor();
    virtual bool Visit(T&) = 0;
};
}

class CScene {
public:
    bool VisitAllNodesInVolume(dwi::IVisitor<ISceneNode>&, const IVolume&, ISceneNode::EVolumeType, bool);

    unsigned char unknown000[328];
};

enum EScriptBulletType {};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    CVolSphere(const CVector3& center, float radius) : m_radius(radius), m_center(center) {}
    virtual ~CVolSphere();

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
} __attribute__((aligned(8)));

class CImpact : public dwi::IVisitor<ISceneNode> {
public:
    CImpact(CVector3 center, float radius, float damage, float unknown34, float unknown38, EScriptBulletType type,
            ISceneNode* source)
        : m_sphere(center, radius), m_damage(damage), m_unknown34(unknown34), m_unknown38(unknown38), m_type(type),
          m_source(source), m_closest(0), m_closestDistance(9999.0f) {
        Apply();
    }
    void Apply();
    virtual bool Visit(ISceneNode&);
    virtual ~CImpact();
    void DamageClosestObject();

    unsigned char unknown04[12];
    CVolSphere m_sphere;
    float m_damage;
    float m_unknown34;
    float m_unknown38;
    EScriptBulletType m_type;
    ISceneNode* m_source;
    ISceneNode* m_closest;
    float m_closestDistance;
    unsigned char unknown4c[8];
} __attribute__((aligned(16)));

class CExplosion {
public:
    CExplosion(CScene&, CVector3, float, float, float, EScriptBulletType, ISceneNode*, bool);
    ~CExplosion();

    unsigned char unknown00[80];
} __attribute__((aligned(16)));

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

extern BSObjectView* g_pBSObject;
extern CScene g_scene;

void AmbientTrack_SetLocation(const CVector3*);
void SetMuzzleStatusForPlayer(bool);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void AmbientTrack_Volume(float);

// Inferred: the impact is applied by its constructor: it visits every scene
// node in its sphere and damages the closest one found (the original's EH
// table only destroys the sphere member and the visitor base, i.e. the calls
// run while the object is being constructed).
inline void CImpact::Apply() {
    g_scene.VisitAllNodesInVolume(*this, m_sphere, (ISceneNode::EVolumeType)0, true);
    if (m_closest)
        DamageClosestObject();
}

void BIFunc_CreateImpact(int** stack, void* object) {
    ISceneNode* node = (ISceneNode*)object;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float radius = *(float*)(*stack - (count - 1));
    float damage = *(float*)(*stack - (count - 2));
    float dx = *(float*)(*stack - (count - 3));
    float dy = *(float*)(*stack - (count - 4));
    float dz = *(float*)(*stack - (count - 5));
    float unknown6 = *(float*)(*stack - (count - 6));
    float unknown7 = *(float*)(*stack - (count - 7));
    CVector3 position;
    if (object) {
        node->GetPosition(position);
    } else {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        position.Set(core->x, core->y, core->z);
    }
    position.x += dx;
    position.y += dy;
    position.z += dz;
    CImpact impact(position, radius, damage, unknown6, unknown7, (EScriptBulletType)21, node);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_CreateExplosion(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float radius = *(float*)(*stack - (count - 1));
    float damage = *(float*)(*stack - (count - 2));
    float dx = *(float*)(*stack - (count - 3));
    float dy = *(float*)(*stack - (count - 4));
    float dz = *(float*)(*stack - (count - 5));
    float force = *(float*)(*stack - (count - 6));
    CVector3 position;
    if (object) {
        ((ISceneNode*)object)->GetPosition(position);
    } else {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        position.Set(core->x, core->y, core->z);
    }
    position.x += dx;
    position.y += dy;
    position.z += dz;
    CExplosion explosion(g_scene, position, radius, damage, force, (EScriptBulletType)21, 0, true);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AmbientTrack_Azimuth(int** stack, void*) {
    if (*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1))) {
        CVector3 position;
        TriggerCoreView* core = g_pBSObject->trigger->core;
        position.Set(core->x, core->y, core->z);
        AmbientTrack_SetLocation(&position);
    } else {
        AmbientTrack_SetLocation(0);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetMuzzleFireForPlayer(int** stack, void*) {
    SetMuzzleStatusForPlayer(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AmbientTrack_Volume(int** stack, void*) {
    AmbientTrack_Volume(*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) / 1000.0f);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
