// A fragment of static_obj.cpp (0x800ae9bc):
// CStaticObject::BeginPathMotionUpdate records the starting position (+368)
// and value (+504) of a path motion, flags the motion (bit 0x40 of +480) and
// updates its destination and speed. The class and functions are named by
// the mangled symbols; the members and flag names are inferred, the CVector3
// view assigns through a double pair as in player_weapon.cpp, and
// CStaticObject declares Draw (defined elsewhere) first so its table is not
// emitted here. The rest of the file is not part of this unit.
class CCollision;
class CStaticObject;
class ISceneNode;
class BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void DestroyDumpObject(CStaticObject*);

class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
};

class CScene {
public:
    void Remove(ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

/* inferred: the first byte of g_cheats (20 bytes) */
struct CheatsView {
    bool m_invincible;
    unsigned char unknown01[19];
};

extern CheatsView g_cheats;

union CVector3Pair {
    double pair[2];
    float v[4];
};

/* inferred: CVector3 assignments move it as a doubleword pair */
class CVector3 {
public:
    CVector3 operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CPlayerObject;

class ISubject {
public:
    virtual void MarkForDestruction(int);
};

class ISceneNodeView : public ISubject {
public:
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
    virtual BSObject* GetScriptObject() const;
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
    virtual CPlayerObject* AsPlayerObject();
};

/* inferred: the object's script owner, its table at +12 */
class StaticOwnerView {
public:
    unsigned char unknown00[12];

    virtual void Destroy();

    unsigned char unknown10[4];
    int m_index;
};

struct OwnerRefView {
    unsigned char unknown00[12];
    StaticOwnerView* m_owner;
    int m_index;
};

/* inferred: the collision-damage event block */
struct CollisionDamageEvent {
    OwnerRefView* m_owner;
    int m_value04;
    int m_value08;
    int m_value0c;
    int m_value10;
};

class CStaticObject : public ISceneNodeView {
public:
    virtual void Draw(); /* defined elsewhere; keeps the table out */
    virtual void Destroy();
    virtual void MarkForDestruction(int);
    void BeginPathMotionUpdate(float);
    void UpdatePathMotionDestAndSpeed();
    CVector3 GetPathPosition() const { return m_position; }

    unsigned char unknown004[108];
    CVector3 m_position;
    unsigned char unknown080[88];
    float m_damage;
    unsigned char unknown0dc[4];
    float m_value0e0;
    unsigned char unknown0e4[52];
    OwnerRefView* m_ownerRef;
    unsigned char unknown11c[84];
    CVector3 m_pathStart;
    unsigned char unknown180[96];
    bool m_flag80 : 1;
    bool m_pathMotion : 1;
    unsigned char m_flags1e0 : 5;
    bool m_damageable : 1;
    unsigned char m_flags1e1a : 1;
    bool m_dumped : 1;
    unsigned char m_flags1e1b : 6;
    unsigned char unknown1e2[22];
    float m_pathDamage;
};

class CCollision {
public:
    /* inferred: the node that is not the given one */
    ISceneNodeView* GetOther(const ISceneNodeView* node) const {
        ISceneNodeView* other = m_nodeA;
        if (other == node)
            other = m_nodeB;
        return other;
    }

    ISceneNodeView* m_nodeA;
    ISceneNodeView* m_nodeB;
};

void CStaticObject::BeginPathMotionUpdate(float) {
    m_pathStart = GetPathPosition();
    m_pathDamage = m_damage;
    m_pathMotion = true;
    UpdatePathMotionDestAndSpeed();
}
