// A fragment of scene.cpp (0x800db458): CScene::VisitAllNodesInVolume walks
// the scene's node list (optionally skipping world objects), tests each node's
// world bounding volume of the given type against the query volume (or, for a
// node without one, its position), and hands every node that is not reported
// as no collision (a result other than 1, as in AIFilter.cpp) to the visitor;
// it stops and returns true as soon as the visitor does.
// VisitAllStaticObjects hands every static object (the scene's static list)
// to the visitor the same way. IncludeCollision orders the two nodes by
// address, finds their entry in the scene's collision-exclusion vector (+280)
// and, when there is one, sets bit 0x10 of the flag byte at +916 of each node
// that is a player, then erases the entry (the later entries copied down).
// ExcludeCollision, when one node is a player and the other a hierarchy
// object with collision id 23 that is flagged (bits 0x40, 0x20 or 0x04 of
// +480) or in a state other than 0 or 1 (+260), sends the player's script
// event 43 with a 24-byte event block (value 0x7fffffff, 0 with the
// invincibility cheat, the first byte of g_cheats); it then adds the ordered
// pair with the priority as a float unless the pair is already excluded. The
// flag and state helpers and the event-block and cheats views are inferred.
// The exclusion search and match test are inline helpers whose names
// (FindExclusion, Matches) are inferred, as are the fast_vec begin, end and
// erase members. The file name is this
// project's. CScene, ISceneNode, IVolume, CCollision and dwi::IVisitor are
// named by the mangled symbols; the views (node list head and link, the
// visitor's bool result, the int collision result) and the parameter names
// are inferred. The rest of the file is not part of this unit.
class CCollision;
class CMatrix;
class CDrawContext;
class CTriangle;
class CPlane;
class CLine3;
class CWorldVolume;
class CAnimatedVolume;
class CVolFiber;
class CCDBObject;
class CVolCapsule;
class CVolSphere;
class CVolBox;
class CBullet;
class CPlayerObject;
class CHierObject;
class BSObject;
enum EClsnId {};

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

/* inferred view: the first byte of g_cheats (20 bytes) */
struct CheatsView {
    bool m_invincible;
    unsigned char unknown01[19];
};

extern CheatsView g_cheats;

/* inferred view: the event block sent with event 43 */
struct ExclusionEventView {
    int m_value0;
    int m_value4;
    int m_value8;
    int m_valuec;
    unsigned char unknown10[8];
};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CCollision {
public:
    CCollision();
    ~CCollision();
    unsigned char unknown00[32];
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};


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
    virtual const IVolume* GetWorldBoundingVolume(EVolumeType) const;
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
    virtual const CHierObject* AsHierObject() const;
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
    virtual void* AsLight();
    virtual const void* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;

    ISceneNode* m_link4;
    ISceneNode* m_link8;
};


namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor();
    virtual bool Visit(T&) = 0;
};
}

/* inferred view: static objects are chained through +44 */
class CStaticObject : public ISceneNode {
public:
    unsigned char unknown00c[32];
    CStaticObject* m_nextStatic;
};

/* inferred view: a state at +260 and flag bits at +480 */
class CHierObject : public ISceneNode {
public:
    unsigned char unknown00c[248];
    int m_state;
    unsigned char unknown108[216];
    unsigned char m_flags1e0a : 1;
    bool m_flag1e0a : 1;
    bool m_flag1e0b : 1;
    unsigned char m_flags1e0b : 2;
    bool m_flag1e0c : 1;
    unsigned char m_flags1e0c : 2;

    /* inferred helpers */
    bool IsFlagged() const { return m_flag1e0a || m_flag1e0b || m_flag1e0c; }
    bool IsMoving() const { return m_state != 0 && m_state != 1; }
};

/* inferred view: the player flag word at +916 */
class CPlayerObject {
public:
    unsigned char unknown000[916];
    unsigned char m_flags394 : 3;
    unsigned char m_excluded : 1;
    unsigned char m_flags394b : 4;
};

namespace dwi {
template <class T>
class fast_vec {
public:
    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    void erase(T* it) {
        T* last = end();
        for (T* next = it + 1; next != last; next++, it++)
            *it = *next;
        m_size--;
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

}

class CScene {
public:
    struct SExclusionEntry {
        const ISceneNode* m_first;
        const ISceneNode* m_second;
        float m_value;

        /* inferred: whether this is the exclusion of the given (address-ordered) pair */
        bool Matches(const ISceneNode* first, const ISceneNode* second) const {
            bool match = m_first == first && m_second == second;
            return match;
        }
    };

    void IncludeCollision(const ISceneNode&, const ISceneNode&);
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);
    SExclusionEntry* FindExclusion(const ISceneNode* first, const ISceneNode* second) {
        SExclusionEntry* entry = m_exclusions.begin();
        SExclusionEntry* last = m_exclusions.end();
        for (; entry != last; entry++) {
            if (entry->Matches(first, second))
                break;
        }
        return entry;
    }
    bool VisitAllNodesInVolume(dwi::IVisitor<ISceneNode>&, const IVolume&, ISceneNode::EVolumeType, bool);
    bool VisitAllStaticObjects(dwi::IVisitor<ISceneNode>&);

    ISceneNode* m_firstNode;
    unsigned char unknown004[28];
    CStaticObject* m_firstStatic;
    unsigned char unknown024[244];
    dwi::fast_vec<SExclusionEntry> m_exclusions;
};

void CScene::ExcludeCollision(const ISceneNode& a, const ISceneNode& b, int priority) {
    const ISceneNode* player;
    const ISceneNode* first;
    const ISceneNode* second;
    float value;
    if (&a < &b) {
        first = &a;
        second = &b;
    } else {
        first = &b;
        second = &a;
    }
    value = priority;
    player = (const ISceneNode*)(a.AsPlayerObject() ? a.AsPlayerObject() : b.AsPlayerObject());
    if (player) {
        const CHierObject* other = b.AsHierObject() ? b.AsHierObject() : a.AsHierObject();
        if (other && other->GetCollisionId() == 23) {
            if (other->IsFlagged() || other->IsMoving()) {
                ExclusionEventView* event = (ExclusionEventView*)BSEventGetEventMemoryBlock(24);
                event->m_valuec = 0;
                event->m_value4 = 0x7fffffff;
                event->m_value0 = 0;
                event->m_value8 = 0;
                if (g_cheats.m_invincible)
                    event->m_value4 = 0;
                BSObjectTriggerEvent(player->GetScriptObject(), 43, event, 0, false);
            }
        }
    }
    SExclusionEntry* entry = FindExclusion(first, second);
    if (entry == m_exclusions.end()) {
        SExclusionEntry* added = &m_exclusions.m_data[m_exclusions.m_size++];
        added->m_first = first;
        added->m_second = second;
        added->m_value = value;
    }
}

void CScene::IncludeCollision(const ISceneNode& a, const ISceneNode& b) {
    const ISceneNode* first;
    const ISceneNode* second;
    if (&a < &b) {
        first = &a;
        second = &b;
    } else {
        first = &b;
        second = &a;
    }
    SExclusionEntry* entry = FindExclusion(first, second);
    if (entry != m_exclusions.end()) {
        CPlayerObject* player0 = (CPlayerObject*)entry->m_first->AsPlayerObject();
        CPlayerObject* player1 = (CPlayerObject*)entry->m_second->AsPlayerObject();
        if (player0)
            player0->m_excluded = 1;
        if (player1)
            player1->m_excluded = 1;
        m_exclusions.erase(entry);
    }
}

bool CScene::VisitAllStaticObjects(dwi::IVisitor<ISceneNode>& visitor) {
    for (CStaticObject* object = m_firstStatic; object; object = object->m_nextStatic) {
        if (visitor.Visit(*object))
            return true;
    }
    return false;
}

bool CScene::VisitAllNodesInVolume(dwi::IVisitor<ISceneNode>& visitor, const IVolume& volume,
                                   ISceneNode::EVolumeType type, bool skipWorld) {
    for (ISceneNode* node = m_firstNode; node; node = node->m_link8) {
        if (skipWorld && node->AsWorldObject())
            continue;
        CCollision collision;
        const IVolume* bounds = node->GetWorldBoundingVolume(type);
        bool hit;
        if (bounds) {
            hit = volume.TestCollision(*bounds, collision, false) != 1;
        } else {
            CVector3 position;
            node->GetPosition(position);
            hit = volume.TestCollision(position, collision, false) != 1;
        }
        if (hit && visitor.Visit(*node))
            return true;
    }
    return false;
}
