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
// then CAnimObject's) and InitAI (initialises the AI doodad for this
// soldier). The classes, functions and globals are named by the mangled
// symbols; the members, the attachment, skeleton and node-search views are
// inferred, the base view declares placeholder virtuals up to DetachObject
// (+312, named by __vt__14CSoldierObject), and CSoldierObject declares
// DetachObject (defined elsewhere) first so its table is not emitted here. The search loop is an
// inferred inline helper. The rest of the file is not part of this unit.
class ISceneNode;
class CWeapon;
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

    unsigned char unknown00[144];
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

class CAnimObject {
public:
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
    virtual void DetachObject(char*);

    unsigned char unknown0004[3296];
    SkeletonView* m_skeleton;
    unsigned char unknown0ce8[5784];
    void* m_localBox;
    void* m_localAnimated;
    void* m_worldBox;
    void* m_worldAnimated;
    unsigned char unknown2390[3408];
};

class CAnimMorph {
public:
    ~CAnimMorph();

    unsigned char unknown0000[3744];
};

class CVolSphere {
public:
    ~CVolSphere();

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

    CAISoldierDoodad m_doodad;
    CAnimMorph m_head;
    CWeapon* m_weapons[9];
    CStaticObject* m_attachments[8];
    unsigned int m_attachTypes[8];
    unsigned char unknown4074[680];
    bool m_faceStateSet;
    unsigned char unknown431d;
    unsigned short m_faceState;
    CVolSphere m_localVolume1;
    CVolSphere m_worldVolume1;
    CVolSphere m_localVolume2;
    CVolSphere m_worldVolume2;
    unsigned char unknown4380[48];
    CCharacterShadow m_shadow;
    unsigned char unknown43e0[172];
    CWeapon* m_otherWeapon;
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
