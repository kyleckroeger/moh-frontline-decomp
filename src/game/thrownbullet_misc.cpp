// A fragment of thrownbullet.cpp (0x800d1d88): CThrownBullet's CheckEnemies
// (once per throw, for the grenade-like types: the first live German soldier,
// not the thrower, that the grenade is 1.3-1.9 above and in front of within a
// side band (narrower when his weapon's flag at +74 is set) gets event 163;
// otherwise, for a grenade lying still, the nearest soldier within 3 or 6 gets
// event 172 or 173; every other live soldier within 6 then gets event 174 and
// counts the friends of the chosen one, and the chosen one's event carries
// the warning record), ApplyForceTo
// (the velocity at +240 set to the force's direction times the weapon
// record's first value and 1/512 of the scale, then copied to +256; the
// CVector3 view and MSL-style inline sqrtf are those of player_weapon.cpp),
// Halt (zeroes the velocity at +240 and the vector at +256, last component
// first),
// HandleBulletCollision (a projectile hit sets bit 0x10 of the flag byte at
// +504 to the inverse of bit 0x08), SetDetonationTime (stores the time at
// +144 when the weapon record's byte at +74 allows it), Explode (the same
// flag update), GetScriptBulletType (maps the thrown type at +164 to the
// script's bullet type, warning on an unknown type) and IsVisible (the world
// volume's visibility test, IVolume's slot +68), then Draw: in multiplayer a
// player's throw is hidden until its detonation time (+144) is within the
// weapon record's delay (+72) minus MP_THROWNBULLET_DRAW_DELAY; the thrown
// type picks the static object (grenade, stick grenade, knife, wrench or
// bazooka round, warning on an unknown type), whose mesh (+48) is drawn with
// the rotation (+288..+296) applied before the object's matrix (+80), set as
// the context's matrix (+192), using the light volume when the mesh asks for
// it (mesh slot +20, the object's slot +288). The classes and functions
// are named by the mangled symbols; the members, the weapon-record and
// volume views and the flag names are inferred, as in thrownbullet.cpp, and
// CBullet declares placeholder virtuals up to AsProjectile (+320, named by
// __vt__7CBullet). CThrownBullet declares MarkForDestruction (defined
// elsewhere) first so its table is not emitted here. The type numbers are
// not named. The second loop of CheckEnemies is an inferred inline that
// shares the soldier and target variables by reference. The rest of the file
// is not part of this unit.
class CCollision;
class CProjectileBullet;

void DebugMsg(const char*, ...);

namespace std {
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
}

// Two doubles: CVector3's copies move it as a doubleword pair (the view of
// player_weapon.cpp).
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3 operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    float LengthSq() const { return x * x + y * y + z * z; }
    void Normalize() {
        float length = std::sqrtf(LengthSq());
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


class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void BuildRotX(float);
    void RotateY(float);
    void RotateZ(float);
    void Multiply(const CMatrix&, const CMatrix&);
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

struct BPDLightVolume;

/* inferred: the draw context's matrix pointer at +192 */
class CDrawContext {
public:
    unsigned char unknown000[192];
    const CMatrix* m_matrix;
};

class CStaticMesh {
public:
    virtual void vf008();
    virtual void Draw(CDrawContext&, int);
    virtual void vf010();
    virtual bool UsesLightVolume();
    static void SetLightVolume(BPDLightVolume*);
};

/* inferred: a static object's mesh at +48 */
struct StaticObjectView {
    unsigned char unknown00[48];
    CStaticMesh* m_mesh;
};

extern StaticObjectView* g_pStaticObjectGrenade;
extern StaticObjectView* g_pStaticObjectStickGrenade;
extern StaticObjectView* g_pStaticObjectKnife;
extern StaticObjectView* g_pStaticObjectWrench;
extern StaticObjectView* g_pStaticObjectBazooka;
extern bool g_bInMultiplayerMode;
extern float MP_THROWNBULLET_DRAW_DELAY;

struct BSObject;
class CSoldierObject;
class CAIDoodadView;

class ISceneNode {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual BSObject* GetScriptObject() const;
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual CSoldierObject* AsSoldierObject();
    virtual void vf098();
    virtual void* AsPlayerObject();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual CAIDoodadView* GetAIDoodad();
};

enum aistatus_var {};

class CAIStatus {
public:
    int GetVarValue(aistatus_var) const;
};

/* inferred: the AI object behind a doodad (team at +20, status at +436) */
struct AIObjectView {
    unsigned char unknown000[20];
    unsigned char m_team;
    unsigned char unknown015[415];
    CAIStatus m_status;
};

struct AIDoodadInfoView {
    unsigned char unknown00[8];
    AIObjectView* m_object;
};

class CAIDoodadView {
public:
    unsigned char unknown00[4];
    AIDoodadInfoView* m_info;
};

/* inferred: the weapon's properties pointer (+640) and the flag at +74 */
struct WeaponPropertiesView {
    unsigned char unknown00[74];
    short m_flag4a;
};

struct WeaponView {
    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
};

class CSoldierObject : public ISceneNode {
public:
    unsigned char unknown0004[16396];
    WeaponView* m_weapons[25];
    int m_currentWeapon;
};

/* BSGO_Basic's three words precede its table pointer */
class BSGO_Basic {
public:
    BSObject* m_object;
    BSGO_Basic* m_next;
    int m_field8;

    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

extern BSGO_Basic* g_pGermanList;
bool BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CThrownBullet;

/* inferred: the grenade warning's event block */
struct GrenadeWarningInfo {
    BSObject* m_firedBy;
    CThrownBullet* m_bullet;
    int m_count;
};

/* inferred: whether two AI teams are enemies */
inline bool AreEnemies(unsigned char team, unsigned char other) {
    switch (team) {
    case 1:
        switch (other) {
        case 0:
        case 1:
            return false;
        case 2:
            return true;
        }
        break;
    case 2:
        switch (other) {
        case 0:
        case 2:
            return false;
        case 1:
            return true;
        }
        break;
    case 0:
        switch (other) {
        case 0:
            return false;
        case 1:
        case 2:
            return true;
        }
        break;
    }
    return false;
}

class CBullet {
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
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual CProjectileBullet* AsProjectile();
};

class IVolume {
public:
    virtual ~IVolume();
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
    virtual bool TestVisibility(CDrawContext&) const;
};

/* inferred: an 80-byte bounding volume */
class ThrownVolumeView : public IVolume {
public:
    unsigned char unknown04[76];
};

/* inferred: the weapon record's detonation-time switch at +74 */
struct ThrownWeaponView {
    float forceScale;
    unsigned char unknown04[68];
    short m_drawDelay;
    signed char timedDetonation;
};

class CThrownBullet {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual BPDLightVolume* GetLightVolume();
    void Draw(CDrawContext&);
    void CheckEnemies();
    void ApplyForceTo(CVector3, float);
    void Halt();
    void HandleBulletCollision(CBullet*, const CCollision&);
    void SetDetonationTime(float);
    void Explode();
    int GetScriptBulletType() const;
    bool IsVisible(CDrawContext&) const;

    unsigned char unknown004[44];
    CVector3 m_position;
    unsigned char unknown040[16];
    CMatrix m_tm;
    float m_detonationTime;
    unsigned char unknown094[16];
    unsigned int m_type;
    ThrownWeaponView* m_weapon;
    ISceneNode* m_owner;
    unsigned char unknown0b0[64];
    CVector3 m_velocity;
    CVector3 m_vector100;
    unsigned char unknown110[16];
    float m_rotationX;
    float m_rotationY;
    float m_rotationZ;
    unsigned char unknown12c[36];
    ThrownVolumeView m_localVolume;
    ThrownVolumeView m_worldVolume;
    unsigned char unknown1f0[8];
    unsigned char m_flags1f8 : 3;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flag04 : 1;
    unsigned char m_warned : 1;
    unsigned char m_flag01 : 1;
};

inline void WarnOthers(CThrownBullet* self, BSGO_Basic* best, GrenadeWarningInfo& info, CVector3& delta, CSoldierObject*& soldier, AIObjectView*& target) {
    BSGO_Basic* other;
    AIDoodadInfoView* doodad;
    target = best->GetSceneNode()->GetAIDoodad()->m_info->m_object;
    for (other = g_pGermanList; other; other = other->m_next) {
        soldier = other->GetSceneNode()->AsSoldierObject();
        if (self->m_owner && soldier == self->m_owner->AsSoldierObject())
            continue;
        if (other == best)
            continue;
        doodad = soldier->GetAIDoodad()->m_info;
        if (doodad->m_object->m_status.GetVarValue((aistatus_var)0) <= 0)
            continue;
        soldier->GetPosition(delta);
        delta.x = self->m_position.x - delta.x;
        delta.y = self->m_position.y - delta.y;
        delta.z = self->m_position.z - delta.z;
        if (delta.x * delta.x + delta.y * delta.y < 36.0f) {
            BSObjectTriggerEvent(other->m_object, 174, &info, 0, true);
            if (!AreEnemies(target->m_team, doodad->m_object->m_team))
                info.m_count++;
        }
    }
}

void CThrownBullet::CheckEnemies() {
    BSGO_Basic* node;
    CSoldierObject* soldier;
    float bestDistance;
    BSGO_Basic* best;
    int event;
    bestDistance = 0.0f;
    best = 0;
    event = -1;
    if (m_warned || (int)m_type == 0 || m_type - 1 <= 1 || m_type - 5 <= 5 || (int)m_type == 8)
        return;
    CVector3 delta;
    CVector3 direction;
    for (node = g_pGermanList; node; node = node->m_next) {
        soldier = node->GetSceneNode()->AsSoldierObject();
        if (m_owner && soldier == m_owner->AsSoldierObject())
            continue;
        if (soldier->GetAIDoodad()->m_info->m_object->m_status.GetVarValue((aistatus_var)0) <= 0)
            continue;
        soldier->GetPosition(delta);
        delta.x = m_position.x - delta.x;
        delta.y = m_position.y - delta.y;
        delta.z = m_position.z - delta.z;
        float speed = std::sqrtf(m_velocity.LengthSq());
        if (delta.z > 1.3f && delta.z < 1.9f) {
            soldier->GetForward(direction);
            float facing = direction.x * delta.x + direction.y * delta.y + direction.z * delta.z;
            if (facing < 1.0f && facing > 0.0f) {
                float low = -0.1f;
                float high = 0.5f;
                WeaponView* weapon = soldier->m_weapons[soldier->m_currentWeapon];
                if (weapon && weapon->m_properties->m_flag4a) {
                    low = -0.3f;
                    high = 0.2f;
                }
                soldier->GetRightward(direction);
                float side = direction.x * delta.x + direction.y * delta.y + direction.z * delta.z;
                if (side < high && side > low) {
                    m_warned = 1;
                    best = node;
                    event = 163;
                    break;
                }
            }
        } else if (delta.z < 0.05f && speed < 0.05f) {
            float distance = delta.x * delta.x + delta.y * delta.y;
            if (distance < 9.0f) {
                if (!best || distance < bestDistance) {
                    m_warned = 1;
                    bestDistance = distance;
                    best = node;
                    event = 172;
                }
            } else if (distance < 36.0f) {
                if (!best || distance < bestDistance) {
                    m_warned = 1;
                    bestDistance = distance;
                    best = node;
                    event = 173;
                }
            }
        }
    }
    if (best) {
        GrenadeWarningInfo info;
        if (m_owner)
            info.m_firedBy = m_owner->GetScriptObject();
        else
            info.m_firedBy = 0;
        info.m_bullet = this;
        info.m_count = 0;
        AIObjectView* target;
        WarnOthers(this, best, info, delta, soldier, target);
        BSObjectTriggerEvent(best->m_object, (unsigned short)event, &info, 0, true);
    }
}

void CThrownBullet::ApplyForceTo(CVector3 force, float scale) {
    float strength = scale / 512.0f;
    m_velocity = force;
    m_velocity.Normalize();
    m_velocity *= m_weapon->forceScale * strength;
    m_vector100 = m_velocity;
}

void CThrownBullet::Halt() {
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    m_vector100.x = m_vector100.y = m_vector100.z = 0.0f;
}

void CThrownBullet::HandleBulletCollision(CBullet* bullet, const CCollision&) {
    if (bullet->AsProjectile())
        m_flag10 = !m_flag08;
}

void CThrownBullet::SetDetonationTime(float time) {
    if (m_weapon->timedDetonation)
        m_detonationTime = time;
}

void CThrownBullet::Explode() {
    m_flag10 = !m_flag08;
}

int CThrownBullet::GetScriptBulletType() const {
    switch (m_type) {
    case 3:
        return 3;
    case 0:
    case 8:
    case 10:
        return 21;
    case 1:
        return 23;
    case 2:
        return 22;
    case 4:
        return 24;
    case 5:
        return 24;
    case 9:
        return 24;
    case 6:
        return 26;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in GetScriptBulletType.  Type %d\n", m_type);
        return 3;
    }
}

bool CThrownBullet::IsVisible(CDrawContext& context) const {
    return m_worldVolume.TestVisibility(context);
}

void CThrownBullet::Draw(CDrawContext& context) {
    if (g_bInMultiplayerMode && m_owner && m_owner->AsPlayerObject()) {
        if (m_detonationTime > (float)m_weapon->m_drawDelay - MP_THROWNBULLET_DRAW_DELAY)
            return;
    }
    StaticObjectView* object = 0;
    switch (m_type) {
    case 3:
        object = g_pStaticObjectGrenade;
        break;
    case 4:
        object = g_pStaticObjectStickGrenade;
        break;
    case 5:
        object = g_pStaticObjectKnife;
        break;
    case 9:
        object = g_pStaticObjectWrench;
        break;
    case 0:
    case 1:
    case 2:
    case 8:
    case 10:
        object = g_pStaticObjectBazooka;
        break;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in Draw.  Type %d\n", m_type);
        break;
    }
    if (object) {
        CMatrix world;
        CMatrix rotation;
        rotation.BuildRotX(m_rotationX);
        rotation.RotateY(m_rotationY);
        rotation.RotateZ(m_rotationZ);
        world.Multiply(rotation, m_tm);
        CStaticMesh* mesh = object->m_mesh;
        if (mesh) {
            context.m_matrix = &world;
            if (mesh->UsesLightVolume())
                CStaticMesh::SetLightVolume(GetLightVolume());
            mesh->Draw(context, 0);
        }
    }
}
