// A fragment of static_obj.cpp (0x800aee38): CStaticObject::SetAttachedLight
// swaps the attached light's subscription when the light changes (cancelling
// the old one and subscribing to the new light so the pointer is cleared when
// it is marked for destruction), then, with a light attached, stores the
// offset and places the light at the object's local-to-world matrix
// pre-translated by it. CStaticObject::HandleBulletCollision: a projectile hit
// on an object with a script object fills a 24-byte script event block as in
// anim_obj_bullet.cpp (shooter's script object, damage, side hit from the
// bullet's direction against the object's right and forward axes, type, body
// part of the first contact) and fires script event 43, first deflecting the
// bullet when the flag at +484 asks for it (a bullet the object fired itself
// is only deflected); a thrown bullet is deflected when the object threw it,
// otherwise it explodes unless it is of type 3 or 4 and the object has no
// owner node, or the owner's script state is not positive. The classes and
// functions are named by the symbols; the members, virtual slots, the event
// block and the CMatrix and CVector3 views (as in Soldier_object_attach.cpp and
// anim_obj_bullet.cpp) are inferred. The rest of the file is in other units.
class ISubject;
class IObserver;
enum ESubjectEvent {};

void SECB_NullifyPointerOnMarked(IObserver&, ISubject**, ESubjectEvent);

class CPublisher {
public:
    static void CancelSubscription(IObserver&, ISubject**);
    static void AddSubscription(IObserver&, ISubject**, void (*)(IObserver&, ISubject**, ESubjectEvent));
};

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
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
    void PreTranslate(CVector3);
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

struct BSObject;
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


class CProjectileBullet;
class CCollision;

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
    virtual class CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void vf144();
    virtual void vf148();
    virtual void Deflect(const CCollision&);
};

class CThrownBullet : public CBullet {
public:
    void Explode();

    unsigned char unknown004[160];
    int m_type;
};

/* inferred: a contact of a collision: its position at +16 and body part at +36 */
struct ContactView {
    unsigned char unknown00[16];
    CVector3 m_position;
    unsigned char unknown20[4];
    int m_part;
};

/* inferred: the collision's contact list at +24 */
class CCollision {
public:
    unsigned char unknown00[24];
    ContactView* m_contacts;
};

/* inferred: the bullet event block (24 bytes) passed with script event 43 */
struct BulletHitEvent {
    BSObject* m_shooter;
    int m_damage;
    int m_side;
    int m_type;
    int m_part;
};

/* inferred: the light's transform setter (slot +232) */
class CLight {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual void vf60();
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual void vfcc();
    virtual void vfd0();
    virtual void vfd4();
    virtual void vfd8();
    virtual void vfdc();
    virtual void vfe0();
    virtual void vfe4();
    virtual void SetTMLocalToWorld(const CMatrix&);
};

/* inferred: a script object's class (table at +12; its second slot gives the state, an int first) */
class BSClassView {
public:
    unsigned char unknown00[12];

    virtual void vf08();
    virtual int* GetState();
};

struct BSObject {
    unsigned char unknown00[12];
    BSClassView* m_class;
};

class CStaticObject {
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
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
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
    virtual void vf130();
    virtual void vf134();
    virtual void vf138();
    virtual void vf13c();
    virtual void vf140();
    virtual void vf144();
    virtual ISceneNode* GetOwnerNode();

    void SetAttachedLight(CLight*, CVector3);
    void HandleBulletCollision(CBullet*, const CCollision&);

    unsigned char unknown004[276];
    BSObject* m_script;
    unsigned char unknown11c[200];
    int m_flags;
    unsigned char unknown1e8[100];
    CLight* m_light;
    CVector3 m_lightOffset;
};

void CStaticObject::SetAttachedLight(CLight* light, CVector3 offset) {
    if (light != m_light) {
        if (&m_light)
            CPublisher::CancelSubscription(*(IObserver*)this, (ISubject**)&m_light);
        m_light = light;
        if (light)
            CPublisher::AddSubscription(*(IObserver*)this, (ISubject**)&m_light, SECB_NullifyPointerOnMarked);
    }
    if (m_light) {
        m_lightOffset = offset;
        CMatrix matrix;
        GetTMLocalToWorld(matrix);
        matrix.PreTranslate(m_lightOffset);
        m_light->SetTMLocalToWorld(matrix);
    }
}

void CStaticObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    CProjectileBullet* projectile = bullet->AsProjectile();
    CThrownBullet* thrown = bullet->AsThrown();
    if (projectile) {
        if (m_script) {
            ISceneNode* shooter = bullet->GetFiredBy();
            if (shooter == (ISceneNode*)this) {
                bullet->Deflect(collision);
            } else {
                if (m_flags & 1)
                    bullet->Deflect(collision);
                BulletHitEvent* event = (BulletHitEvent*)BSEventGetEventMemoryBlock(24);
                event->m_type = bullet->GetScriptBulletType();
                event->m_damage = (int)bullet->GetDamage();
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
                event->m_part = collision.m_contacts->m_part;
                BSObjectTriggerEvent(m_script, 43, event, 0, false);
            }
        }
    } else if (thrown) {
        if (thrown->GetFiredBy() == (ISceneNode*)this) {
            thrown->Deflect(collision);
        } else if ((thrown->m_type != 3 && thrown->m_type != 4) || GetOwnerNode()) {
            if (GetOwnerNode() && *((BSObject*)GetOwnerNode()->GetScriptObject())->m_class->GetState() <= 0)
                return;
            thrown->Explode();
        }
    }
}
