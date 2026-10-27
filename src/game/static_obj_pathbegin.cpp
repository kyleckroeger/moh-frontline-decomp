// A fragment of static_obj.cpp (0x800ae8c0):
// CStaticObject::AttemptCollisionDamage, for a damageable object with damage
// to deal, fires script event 192 on the other node with a 20-byte block
// (owner reference, 1 or 0 under the invincibility cheat for a player, 1, 5,
// 9); CStaticObject::BeginPathMotionUpdate records the starting position (+368)
// and value (+504) of a path motion, flags the motion (bit 0x40 of +480) and
// updates its destination and speed; CStaticObject::UpdatePathMotionDestAndSpeed
// steps the path traversal to its next point, sets the target (+488) and
// adjusts the speed by path state (state 3 keeps a gap to a leader node). The class and functions are named by
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

/* inferred: an AI position (x, y, z) built from a vector */
class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}
    CAIFilterRealPosition(CVector3 v) : x(v.x), y(v.y), z(v.z) {}
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CAIFilterObject;

class CAISplinePathTraversal {
public:
    void GetFinalPointForward(CAIFilterRealPosition*);
    void GetNextPointForward(const CAIFilterRealPosition&, CAIFilterRealPosition*, CAIFilterObject*);

    int m_state;
    unsigned char unknown04[12];
    int m_value10;
    unsigned char unknown14[20];
};

class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

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
    virtual void GetPosition(CAIFilterRealPosition&) const;
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
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
    void AttemptCollisionDamage(const CCollision&);
    void BeginPathMotionUpdate(float);
    void UpdatePathMotionDestAndSpeed();
    CVector3 GetPathPosition() const { return m_position; }

    unsigned char unknown004[108];
    CVector3 m_position;
    unsigned char unknown080[48];
    CAISplinePathTraversal m_traversal;
    float m_damage;
    float m_pathMaxSpeed;
    float m_value0e0;
    float m_pathAccel;
    unsigned char unknown0e8[4];
    ISceneNodeView* m_pathLeader;
    float m_pathGap;
    unsigned char unknown0f4[16];
    int m_pathState;
    unsigned char unknown108[16];
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
    unsigned char unknown1e2[6];
    CVector3 m_pathTarget;
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

void CStaticObject::AttemptCollisionDamage(const CCollision& collision) {
    if (!m_damageable)
        return;
    ISceneNodeView* other;
    ISceneNodeView* node;
    float zero = 0.0f;
    if (m_value0e0 == zero && m_damage == zero)
        return;
    other = node = collision.GetOther(this);
    CollisionDamageEvent event;
    event.m_owner = m_ownerRef;
    event.m_value04 = 1;
    event.m_value10 = 9;
    event.m_value08 = 1;
    event.m_value0c = 5;
    if (g_cheats.m_invincible && node->AsPlayerObject())
        event.m_value04 = 0;
    if (other->GetScriptObject())
        BSObjectTriggerEvent(other->GetScriptObject(), 192, &event, 0, true);
}

void CStaticObject::BeginPathMotionUpdate(float) {
    m_pathStart = GetPathPosition();
    m_pathDamage = m_damage;
    m_pathMotion = true;
    UpdatePathMotionDestAndSpeed();
}

/* inferred helper */
inline float Dot3(float a0, float a1, float b0, float b1, float c0, float c1) { return a0 * a1 + b0 * b1 + c0 * c1; }

void CStaticObject::UpdatePathMotionDestAndSpeed() {
    CAIFilterRealPosition here(m_pathStart);
    CAIFilterRealPosition next;
    switch (m_pathState) {
    case 0:
    case 1:
    default:
        if (m_damage <= 0.0f) {
            if (m_pathState == 6)
                m_pathState = 1;
            else
                m_pathState = 0;
            m_damage = 0.0f;
            m_value0e0 = 0.0f;
            m_pathMotion = false;
        }
    case 2:
    case 3: {
        CAIFilterRealPosition final;
        m_traversal.GetNextPointForward(here, &next, 0);
        m_traversal.GetFinalPointForward(&final);
        float distance = here.GetDistanceSquaredXYZReal(final);
        if (distance < 400.0f) {
            float time = m_damage / (2.0f * m_pathAccel);
            float stop = 0.001 * (time * time);
            if (m_traversal.m_state == 7 || distance < stop * stop) {
                m_pathState = 4;
                here.GetDistanceSquaredXYZReal(next);
                m_traversal.GetFinalPointForward(&next);
            }
        }
        m_pathTarget = *(CVector3*)&next;
        break;
    }
    case 4: {
        m_traversal.GetFinalPointForward(&next);
        if (here.GetDistanceSquaredXYZReal(next) < 4.0f) {
            TriggerScriptEvent(55, 0, false);
            m_damage = 0.0f;
            m_value0e0 = 0.0f;
            m_traversal.m_value10 = 0;
            m_pathState = 0;
            m_pathMotion = false;
        }
        m_pathTarget = *(CVector3*)&next;
        break;
    }
    }
    switch (m_pathState) {
    case 2:
        break;
    case 3: {
        CAIFilterRealPosition leader;
        m_pathLeader->GetPosition(leader);
        float dx = next.x - here.x;
        float dy = next.y - here.y;
        float dz = next.z - here.z;
        float tx = leader.x - here.x;
        float ty = leader.y - here.y;
        float tz = leader.z - here.z;
        float along = dx * tx + dy * ty + dz * tz;
        float length = Dot3(dx, dx, dy, dy, dz, dz);
        length = sqrtf(length);
        if (length != 0.0f) {
            float scale = 1.0f / length;
            dx *= scale;
            dy *= scale;
            dz *= scale;
        }
        dx *= along;
        dy *= along;
        dz *= along;
        leader.x = here.x + dx;
        leader.y = here.y + dy;
        leader.z = here.z + dz;
        float gap = leader.GetDistanceSquaredXYZReal(here);
        if (along > 0.0f)
            gap = gap + m_pathGap;
        else
            gap = -gap + m_pathGap;
        if (g_Shell.Get_currentMission() == 4 && g_Shell.Get_currentStage() == 2 && !g_bInMultiplayerMode) {
            if (gap > 25.0f)
                m_value0e0 = m_pathMaxSpeed;
            else if (gap < -25.0f)
                m_value0e0 = -0.6f * m_pathMaxSpeed;
            else
                m_value0e0 = 0.2f * m_pathMaxSpeed;
        } else {
            if (gap > 25.0f)
                m_value0e0 = m_pathMaxSpeed;
            else if (gap < -25.0f)
                m_value0e0 = 0.0f;
            else
                m_value0e0 = 0.6f * m_pathMaxSpeed;
        }
        break;
    }
    case 4:
        m_value0e0 = 0.0100100115f;
        break;
    case 5:
    case 6:
        m_value0e0 = 0.0f;
        break;
    }
}
