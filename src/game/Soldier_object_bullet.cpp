// A fragment of Soldier_object.cpp (0x800aaea0):
// CSoldierObject::HandleBulletCollision. A projectile fills a 24-byte script
// event block as in anim_obj_bullet.cpp (type, damage, shooter's script
// object, the side hit, the body part of the first contact) plus a hit count:
// a type-1 bullet starts a 0.25 timer if none runs, and while it runs the
// soldier's hit counter is incremented into the event (else the count is 1).
// The damage is scaled by part (0.5 for limbs, 1 for the torso, 3 for the
// head); a head shot may also knock the helmet off (with the soldier's
// percentage chance): a thrown object of the helmet type is created at the
// head, sent off with the bullet's velocity scaled by a random 0.002..0.01
// and a random upward speed of 0.005..0.065, and, when the helmet type stops
// bullets, the damage is further scaled by the soldier's helmet factor. Two
// cheats set the damage of player soldiers to 1000 or zero it for non-head
// hits. Script event 43 fires; a player shooter's hit and shot location of a
// player soldier's first two hits are counted in g_Shell; an impact sound
// (15 when a helmet stopped it, else 14) plays at the contact and a blood
// particle system is placed there, oriented to the contact normal. A thrown
// bullet explodes instead. The classes and functions are named by the
// symbols; the members, the event block, the views, the helmet inline (as
// EjectHelmet in Soldier_object_helmet.cpp, but from a bullet) and the
// vector helpers are inferred. The rest of the file is in other units.
extern "C" int stricmp(const char*, const char*);

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

float MathFunRandomReal(float, float);
long long MathFunRandomI64(long long, long long);

class CVector3 {
public:
    CVector3() {}
    CVector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
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

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Ident();
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

struct BSObject;
struct TriggerObject_struct;
class CPlayerObject;

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void PlayImpactSound(unsigned short, unsigned short, CVector3, bool);

enum EShotLocations {};

class CShellMenu {
public:
    void Set_hits(unsigned int, bool);
    void Set_shotLocation(unsigned int, EShotLocations, bool);

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;

struct CheatsView {
    unsigned char unknown00[12];
    bool m_bigDamage;
    bool m_headShotsOnly;
};

extern CheatsView g_cheats;

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual void Render();
    virtual void Init();
    virtual void Link();
    virtual int IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    virtual ~CParticleSystem();
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
    virtual void SetLocalToWorld(const CMatrix&);
    static CParticleSystem* Create(unsigned long);
};

class CThrownBullet {
public:
    void Explode();
};

class CProjectileBullet;

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
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
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
    virtual void vf094();
    virtual void vf098();
    virtual CPlayerObject* AsPlayerObject();
};

class CBullet {
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
    virtual void vf040();
    virtual void vf044();
    virtual void GetForward(CVector3&) const;
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
    virtual void vf120();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&) const;
};
class CThrownObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void BeginUpdate(float);
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
    virtual void SetTMLocalToWorld(const CMatrix&);

    void SetVelocity(CVector3&);

    unsigned char unknown004[760];
    int m_helmetState;
};
CThrownObject* CreateSpecialThrownObject(TriggerObject_struct*, int, int);

class CStaticObjectFactory {
public:
    static bool DoesHelmetTypeStopBullets(int);
};

/* inferred: the script object's data chain to the soldier's data */
struct SoldierDataView {
    unsigned char unknown00[53];
    unsigned char m_kind;
    unsigned char unknown36[18];
    int m_helmetType;
};

struct ScriptOwnerView {
    unsigned char unknown00[4];
    SoldierDataView* m_data;
};

struct BSObject {
    unsigned char unknown00[8];
    ScriptOwnerView* m_owner;
};

/* inferred: a contact of a collision: normal, position, body part */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_position;
    unsigned char unknown20[4];
    int m_part;
};

class CCollision {
public:
    unsigned char unknown00[24];
    ContactView* m_contacts;
    ContactView* GetContact() const { return m_contacts; }
};

/* inferred: the bullet event block (24 bytes) passed with script event 43 */
struct BulletHitEvent {
    BSObject* m_shooter;
    int m_damage;
    int m_side;
    int m_type;
    int m_part;
    int m_hitCount;
};

/* inferred: a skeleton node (24 bytes): name, bone, own matrix */
struct SkeletonNodeView {
    char m_name[16];
    int m_bone;
    CMatrix* m_matrix;
};

struct SkeletonView {
    unsigned char unknown00[16];
    unsigned int m_count;
    SkeletonNodeView* m_nodes;

    unsigned int Find(const char* name) const {
        for (unsigned int i = 0; i < m_count; i++) {
            if (stricmp(m_nodes[i].m_name, name) == 0)
                return i;
        }
        return (unsigned int)-1;
    }
};

class CStaticMesh;

class CSoldierObject {
public:
    virtual void MarkForDestruction(int);
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

    void HandleBulletCollision(CBullet*, const CCollision&);

    void RemoveAttachment(const char* name) {
        SkeletonView* skeleton = GetSkeleton();
        unsigned int i;
        for (i = 0; i < skeleton->m_count; i++) {
            if (stricmp(skeleton->m_nodes[i].m_name, name) == 0)
                break;
        }
        if (i != skeleton->m_count) {
            m_attachments[i] = 0;
            m_attachTypes[i] = 0;
        }
    }

    /* inferred: knocks the helmet off with the bullet's velocity; whether
       the helmet type stops bullets */
    bool KnockOffHelmet(CBullet* bullet) {
        bool stops = true;
        SkeletonView* skeleton = GetSkeleton();
        unsigned int node = skeleton->Find("head");
        if (node == (unsigned int)-1)
            return false;
        if (!m_attachTypes[node])
            return false;
        int type = m_script->m_owner->m_data->m_helmetType;
        CThrownObject* helmet = CreateSpecialThrownObject(0, type, -1);
        if (!helmet)
            return false;
        RemoveAttachment("head");
        if (!CStaticObjectFactory::DoesHelmetTypeStopBullets(type)) {
            helmet->m_helmetState = 2;
            stops = false;
        } else {
            helmet->m_helmetState = 1;
        }
        CMatrix matrix;
        matrix.Multiply(m_bones[skeleton->m_nodes[node].m_bone], m_matrix);
        matrix.Multiply(m_handMatrices[node], matrix);
        helmet->SetTMLocalToWorld(matrix);
        CVector3 velocity;
        bullet->GetVelocity(velocity);
        float scale = MathFunRandomReal(0.002f, 0.01f);
        velocity.x *= scale;
        velocity.y *= scale;
        velocity.z *= scale;
        velocity.z = MathFunRandomReal(0.005f, 0.065f);
        helmet->SetVelocity(velocity);
        helmet->BeginUpdate(1.0f);
        return stops;
    }

    unsigned char unknown0004[3296];
    SkeletonView* m_skeleton;
    SkeletonView* GetSkeleton() const { return m_skeleton; }
    unsigned char unknown0ce8[600];
    CMatrix m_bones[80];
    CMatrix m_matrix;
    unsigned char unknown2380[16];
    BSObject* m_script;
    unsigned char unknown2394[7328];
    CStaticMesh* m_attachments[8];
    unsigned int m_attachTypes[8];
    unsigned char unknown4074[12];
    CMatrix m_handMatrices[8];
    unsigned char unknown4280[296];
    int m_helmetChance;
    float m_helmetDamageScale;
    unsigned char unknown43b0[224];
    float m_hitTimer;
    int m_hitCount;
};

void CSoldierObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    CProjectileBullet* projectile = bullet->AsProjectile();
    CThrownBullet* thrown = bullet->AsThrown();
    if (projectile) {
        BulletHitEvent* event = (BulletHitEvent*)BSEventGetEventMemoryBlock(24);
        event->m_type = bullet->GetScriptBulletType();
        event->m_damage = (int)bullet->GetDamage();
        if (event->m_type == 1 && m_hitTimer == 0.0f)
            m_hitTimer = 0.25f;
        if (m_hitTimer > 0.0f)
            event->m_hitCount = ++m_hitCount;
        else
            event->m_hitCount = 1;
        ISceneNode* shooter = bullet->GetFiredBy();
        if (shooter)
            event->m_shooter = shooter->GetScriptObject();
        else
            event->m_shooter = 0;
        CVector3 direction;
        bullet->GetForward(direction);
        CVector3 forward;
        GetForward(forward);
        CVector3 right;
        GetRightward(right);
        float front = forward.x * direction.x + forward.y * direction.y + forward.z * direction.z;
        float side = right.x * direction.x + right.y * direction.y + right.z * direction.z;
        if (side < -0.7071f)
            event->m_side = 3;
        else if (side > 0.7071f)
            event->m_side = 2;
        else if (front < 0.0f)
            event->m_side = 0;
        else
            event->m_side = 1;
        ContactView* contact = collision.GetContact();
        bool stopped = false;
        event->m_part = contact->m_part;
        switch (event->m_part) {
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            event->m_damage = (int)(0.5f * event->m_damage);
            break;
        case 0:
        case 1: {
            float scale = 1.0f;
            if (MathFunRandomI64(0, 100) < m_helmetChance) {
                stopped = KnockOffHelmet(bullet);
                if (stopped)
                    scale = m_helmetDamageScale;
            }
            event->m_damage = (int)(scale * (3.0f * event->m_damage));
            break;
        }
        case 2:
        case 9:
        case 10:
        case 11:
        case 12:
            event->m_damage = (int)(1.0f * event->m_damage);
            break;
        }
        if (g_cheats.m_bigDamage && m_script->m_owner->m_data->m_kind == 2)
            event->m_damage = 1000;
        if (g_cheats.m_headShotsOnly && event->m_part != 0 && event->m_part != 1)
            event->m_damage = 0;
        BSObjectTriggerEvent(m_script, 43, event, 0, false);
        if (shooter && shooter->AsPlayerObject() && m_script->m_owner->m_data->m_kind == 2 && event->m_hitCount < 2) {
            g_Shell.Set_hits(1, false);
            switch (event->m_part) {
            case 3:
            case 4:
            case 5:
                g_Shell.Set_shotLocation(1, (EShotLocations)1, false);
                break;
            case 6:
            case 7:
            case 8:
                g_Shell.Set_shotLocation(1, (EShotLocations)2, false);
                break;
            case 13:
            case 14:
            case 15:
                g_Shell.Set_shotLocation(1, (EShotLocations)4, false);
                break;
            case 16:
            case 17:
            case 18:
                g_Shell.Set_shotLocation(1, (EShotLocations)5, false);
                break;
            case 19:
            case 20:
                g_Shell.Set_shotLocation(1, (EShotLocations)6, false);
                break;
            case 0:
            case 1:
                g_Shell.Set_shotLocation(1, (EShotLocations)0, false);
                break;
            case 2:
            case 9:
            case 10:
            case 11:
            case 12:
                g_Shell.Set_shotLocation(1, (EShotLocations)3, false);
                break;
            }
        }
        int sound = 14;
        if (stopped)
            sound = 15;
        PlayImpactSound(sound, 1, contact->m_position, false);
        CParticleSystem* blood = CParticleSystem::Create(0x6da45fe8);
        if (blood) {
            CMatrix matrix;
            matrix.Ident();
            CVector3 right;
            CVector3 front;
            if ((float)__fabs(contact->m_normal.Dot(CVector3(1.0f, 0.0f, 0.0f))) <= 0.9999f) {
                front.Cross(contact->m_normal, CVector3(1.0f, 0.0f, 0.0f));
                front.Normalize();
                right.Cross(front, contact->m_normal);
                right.Normalize();
            } else {
                right.Cross(contact->m_normal, CVector3(0.0f, 0.0f, 1.0f));
                right.Normalize();
                front.Cross(contact->m_normal, right);
                front.Normalize();
            }
            matrix.SetRight(right);
            matrix.SetFront(front);
            matrix.SetUp(contact->m_normal);
            matrix.SetPos(contact->m_position);
            blood->SetLocalToWorld(matrix);
        }
    } else if (thrown) {
        thrown->Explode();
    }
}
