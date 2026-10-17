// A fragment of Soldier_object.cpp (0x800adeac): CSoldierObject's
// SetFaceAnimationState (stores the face state at +17182 and flags it as set
// at +17180), RemoveWeapons (detaches the hand objects "lt_hand" and
// "rt_hand", removes each of the nine weapons from g_scene when it is in the
// scene and returns it to g_WeaponFactory, then the other weapon at +17548)
// and CleanUp (frees the animated volumes at +9092/+9100 and the box volumes
// at +9088/+9096, removes the weapons, frees the static object attached at
// the "head" node when it is of attachment type 1, and cleans up the AI
// doodad at +12512), the destructor (the character shadow, the four
// bounding spheres, the head morph and the AI doodad destroyed in reverse,
// then CAnimObject's), InitAI (initialises the AI doodad for this soldier)
// and Init (the soldier's state, volumes, aim matrix and defaults; see the
// function). The classes, functions and globals are named by the mangled
// symbols; the members, the attachment, skeleton and node-search views are
// inferred, the base view declares placeholder virtuals up to DetachObject
// (+312, named by __vt__14CSoldierObject), and CSoldierObject declares
// DetachObject (defined elsewhere) first so its table is not emitted here.
// The search loop and the matrix's SetAxes and SetTranslation are inferred
// inline helpers (their by-value argument copies take the original's stack
// slots: an inline's temporaries are placed by inline depth). The rest of
// the file is not part of this unit.
class ISceneNode;
class CWeapon;
struct CharFrame_t;
enum EMoodType {};
enum EClsnId {};
enum EWeaponTypes {};

union CVector3Pair {
    double pair[2];
    float v[4];
};

/* inferred: CVector3 copies move it as a doubleword pair (player_weapon.cpp) */
class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    /* inferred helper */
    void SetAxes(const CVector3& right, CVector3 front, CVector3 up) {
        SetRight(right);
        SetFront(front);
        SetUp(up);
    }
    void SetTranslation(const CVector3& p) { SetPos(p); }

    float m[4][4];
} __attribute__((aligned(16)));

class CVolSphere;
class CVolBox {
public:
    void SetBasis(CVector3, CVector3, CVector3);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);
};

void* AllocateAnimatedVolume();
void* AllocateBoxVolume();
extern "C" int rand();
extern "C" void* memset(void*, int, unsigned long);
class CStaticObject;

extern "C" int stricmp(const char*, const char*);
void FreeAnimatedVolume(void*);
void FreeBoxVolume(void*);

class CScene {
public:
    bool IsNodeInScene(const ISceneNode*) const;
    void Remove(ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CWeaponFactory {
public:
    void DestroyWeapon(CWeapon*);
    CWeapon* CreateWeapon(EWeaponTypes, ISceneNode*, int);

    unsigned char unknown00[28];
};

extern CWeaponFactory g_WeaponFactory;

class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
};

class CSoldierObject;

class CAISoldierDoodad {
public:
    ~CAISoldierDoodad();
    void CleanUp();
    void Init(CSoldierObject*);

    unsigned char unknown00[84];
};

/* inferred: a skeleton node (24 bytes), its name first */
struct SkeletonNodeView {
    char m_name[16];
    int m_bone;
    unsigned char unknown14[4];
};

struct SkeletonView {
    unsigned char unknown00[16];
    unsigned int m_count;
    SkeletonNodeView* m_nodes;

    /* inferred: the index of the node with the given name, or -1 */
    unsigned int Find(const char* name) const {
        for (unsigned int i = 0; i < m_count; i++) {
            if (stricmp(m_nodes[i].m_name, name) == 0)
                return i;
        }
        return (unsigned int)-1;
    }
};

class CAnimObject;

/* inferred: the soldier's animated sphere volume, its owner at +1968 */
class CAnimatedSphereView {
public:
    unsigned char unknown000[1968];
    CAnimObject* m_owner;
};

class CAnimObject {
public:
    void Init();
    virtual void unknown008();
    virtual ~CAnimObject();
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
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void SetCollisionId(EClsnId);
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
    virtual void DetachObject(char*);

    unsigned char unknown0004[3296];
    SkeletonView* m_skeleton;
    unsigned char unknown0ce8[24];
    unsigned char m_frame[5760];
    void* m_localBox;
    void* m_localAnimated;
    void* m_worldBox;
    void* m_worldAnimated;
    unsigned char unknown2390[8];
    void* m_animatedVolume;
    unsigned char unknown239c[3396];
};

class CAnimMorph {
public:
    ~CAnimMorph();
    void Init(CAnimObject*, unsigned short, CharFrame_t*);

    unsigned char unknown0000[3568];
};

class CVolSphere {
public:
    ~CVolSphere();
    void SetRadius(float);
    void SetCenter(CVector3);

    unsigned char unknown00[32];
};

class CCharacterShadow {
public:
    ~CCharacterShadow();

    unsigned char unknown00[16];
};

class CSoldierObject : public CAnimObject {
public:
    virtual void DetachObject(char*); /* defined elsewhere; keeps the table out */
    virtual ~CSoldierObject();
    int SetFaceAnimationState(unsigned short, long);
    void RemoveWeapons();
    void CleanUp();
    void InitAI();
    void Init();
    void SetMood(EMoodType);

    CAISoldierDoodad m_doodad;
    unsigned char unknown3134[12];
    float m_value3140;
    float m_value3144;
    float m_value3148;
    int m_value314c;
    float m_value3150;
    int m_value3154;
    unsigned char unknown3158[4];
    int m_value315c;
    bool m_flag3160;
    bool m_flag3161;
    unsigned char unknown3162[14];
    CAnimMorph m_head;
    CMatrix m_aimMatrix;
    CVector3 m_aimOffset;
    unsigned char unknown3fb0[16];
    bool m_helmetAttached;
    void* m_helmet;
    unsigned char unknown3fc8[72];
    CWeapon* m_weapons[9];
    CStaticObject* m_attachments[8];
    unsigned int m_attachTypes[8];
    int m_currentWeapon;
    unsigned char unknown4078[520];
    unsigned char m_value4280;
    unsigned char unknown4281[79];
    int m_value42d0;
    int m_value42d4;
    int m_value42d8;
    float m_value42dc;
    float m_value42e0;
    float m_value42e4;
    float m_value42e8;
    float m_value42ec;
    float m_value42f0;
    unsigned char unknown42f4[40];
    bool m_faceStateSet;
    unsigned char unknown431d;
    unsigned short m_faceState;
    CVolSphere m_localVolume1;
    CVolSphere m_worldVolume1;
    CVolSphere m_localVolume2;
    CVolSphere m_worldVolume2;
    int m_value43a0;
    int m_value43a4;
    int m_value43a8;
    float m_value43ac;
    bool m_crouchTimerOn;
    unsigned char unknown43b1[11];
    float m_value43bc;
    unsigned char unknown43c0;
    unsigned char m_value43c1;
    unsigned char unknown43c2[2];
    int m_value43c4;
    int m_value43c8;
    unsigned char unknown43cc[4];
    CCharacterShadow m_shadow;
    unsigned char unknown43e0[84];
    CSoldierObject* m_self;
    float m_fadeAmount;
    unsigned char unknown443c[68];
    int m_mood;
    bool m_drawFlash;
    void* m_attached;
    CWeapon* m_otherWeapon;
    float m_timer;
    int m_timerState;
    bool m_flag4498;
    unsigned char unknown4499[7];
    float m_value44a0;
    float m_value44a4;
    float m_value44a8;
};

int CSoldierObject::SetFaceAnimationState(unsigned short state, long) {
    m_faceState = state;
    m_faceStateSet = true;
    return 0;
}

void CSoldierObject::RemoveWeapons() {
    DetachObject("lt_hand");
    DetachObject("rt_hand");
    for (int i = 0; i < 9; i++) {
        if (m_weapons[i]) {
            if (g_scene.IsNodeInScene((ISceneNode*)m_weapons[i]))
                g_scene.Remove(*(ISceneNode*)m_weapons[i]);
            g_WeaponFactory.DestroyWeapon(m_weapons[i]);
            m_weapons[i] = 0;
        }
    }
    if (m_otherWeapon) {
        g_WeaponFactory.DestroyWeapon(m_otherWeapon);
        m_otherWeapon = 0;
    }
}

void CSoldierObject::CleanUp() {
    if (m_localAnimated)
        FreeAnimatedVolume(m_localAnimated);
    if (m_worldAnimated)
        FreeAnimatedVolume(m_worldAnimated);
    if (m_localBox)
        FreeBoxVolume(m_localBox);
    if (m_worldBox)
        FreeBoxVolume(m_worldBox);
    RemoveWeapons();
    unsigned int head = m_skeleton->Find("head");
    if (head != (unsigned int)-1 && m_attachTypes[head]) {
        switch (m_attachTypes[head]) {
        case 2:
            break;
        case 1:
            CStaticObjectFactory::DeallocateStaticObject(m_attachments[head]);
            break;
        }
    }
    m_doodad.CleanUp();
}

CSoldierObject::~CSoldierObject() {
}

void CSoldierObject::InitAI() {
    m_doodad.Init(this);
}

void CSoldierObject::Init() {
    m_helmet = 0;
    CAnimObject::Init();
    m_head.Init(this, 0, (CharFrame_t*)m_frame);
    m_value42d0 = -1;
    m_value42d4 = m_value42d8 = -1;
    m_value42dc = m_value42e0 = 0.0f;
    m_value42e4 = m_value42e8 = 0.0f;
    m_value42ec = m_value42f0 = 0.0f;
    SetMood((EMoodType)0);
    int roll = rand() % 100;
    if (roll < 25)
        m_mood = 13;
    else if (roll < 50)
        m_mood = 6;
    else if (roll < 75)
        m_mood = 7;
    else
        m_mood = 5;
    m_faceStateSet = false;
    SetCollisionId((EClsnId)2);
    m_currentWeapon = 0;
    memset(m_attachments, 0, sizeof(m_attachments));
    memset(m_attachTypes, 0, sizeof(m_attachTypes));
    memset(m_weapons, 0, sizeof(m_weapons));
    CVolSphere* sphere = (CVolSphere*)AllocateAnimatedVolume();
    sphere->SetRadius(1.2f);
    sphere->SetCenter(CVector3(0.0f, 0.0f, 0.9f));
    ((CAnimatedSphereView*)sphere)->m_owner = this;
    m_animatedVolume = sphere;
    m_localAnimated = sphere;
    m_worldAnimated = AllocateAnimatedVolume();
    m_localVolume1.SetRadius(0.25f);
    m_localVolume1.SetCenter(CVector3(0.0f, 0.0f, 0.55f));
    m_localVolume2.SetRadius(1.0f);
    m_localVolume2.SetCenter(CVector3(0.0f, 0.0f, 0.9f));
    CVolBox* box = (CVolBox*)AllocateBoxVolume();
    box->SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
    box->SetWidth(0.75f);
    box->SetHeight(1.6f);
    box->SetDepth(0.6f);
    box->SetCenter(CVector3(0.0f, 0.0f, 0.8f));
    m_localBox = box;
    m_worldBox = AllocateBoxVolume();
    CVector3 right;
    CVector3 front;
    CVector3 up;
    GetRightward(right);
    GetForward(front);
    GetUpward(up);
    m_aimMatrix.SetAxes(right, front, up);
    CVector3 pos;
    GetPosition(pos);
    right *= m_aimOffset.x;
    front *= m_aimOffset.y;
    up *= m_aimOffset.z;
    pos += right;
    pos += front;
    pos += up;
    m_aimMatrix.SetTranslation(pos);
    m_helmetAttached = false;
    m_value4280 = 0;
    m_value43a0 = -1;
    m_value43a4 = 1;
    m_value43a8 = 40;
    m_value43ac = 0.75f;
    m_crouchTimerOn = false;
    m_value43bc = 0.0f;
    m_self = this;
    m_fadeAmount = 1.0f;
    m_value43c1 = 0;
    m_value43c4 = 0;
    m_value43c8 = 0;
    m_value315c = 1;
    m_value3150 = 0.0f;
    m_value3140 = 0.0f;
    m_value3144 = 0.0f;
    m_value3148 = 0.0f;
    m_value314c = 0;
    m_value3154 = 0;
    m_flag3160 = false;
    m_flag3161 = false;
    m_drawFlash = false;
    m_attached = 0;
    m_otherWeapon = g_WeaponFactory.CreateWeapon((EWeaponTypes)26, (ISceneNode*)this, 999);
    m_timer = 0.0f;
    m_timerState = 0;
    m_flag4498 = false;
    m_value44a0 = m_value44a4 = m_value44a8 = 0.0f;
}
