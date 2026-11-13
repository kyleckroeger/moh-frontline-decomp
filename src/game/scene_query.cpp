// A fragment of scene.cpp (0x800d8214): CScene::IsNodeInScene (a node is in
// the scene if it is the first node or the second scene pointer, or if its
// own link pointers at +8 or +4 are set), GetPlayer (the player pointers
// from +8) and DetectSlowCollisions: it destroys the previous collisions,
// sweeps the extents, then walks the two sorted node-pair lists (+216, +240)
// in step; for each pair found in both that involves no bullet it tests a
// collision (pass-through disabled when a player is involved), keeps it when
// the test returns 2 and stops with 1 when it returns 3. The OSGetTick calls
// are the remains of timing code. CScene, ISceneNode, CCollision,
// CCDBObject, UNodePair and the functions are named by the symbols; the
// members, the pair layout (two nodes and an order byte over the 64-bit key)
// and the InvolvesBullet helper are inferred (CScene is non-virtual; see
// scene.cpp; ISceneNode's virtuals follow __vt__10ISceneNode). Deferred
// inlining lists the functions in reverse. The collision iterator's
// destructor, which this compiles as a weak copy, is discarded as a duplicate
// of the one in scene_veciter_dtor.cpp. The rest of the file is not part of
// this unit.

enum EClsnId {};
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CPlayerObject;

extern "C" long OSGetTick();

class ISceneNode;

class CCollision {
public:
    CCollision();
    CCollision(const CCollision&);
    ~CCollision();
    int Test(float&);

    ISceneNode* m_node0;
    ISceneNode* m_node1;
    float m_time;
    unsigned char unknown0c[4];
    unsigned char m_line : 1;
    unsigned char unknown10 : 1;
    unsigned char m_order : 2;
    unsigned char unknown10b : 4;
    unsigned char unknown11[15];
};

class CCDBObject {
public:
    static void DisablePassThru(bool);
};

#include "game/DwiContainers.h"

class CCollision;
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
    virtual void* AsLight();
    virtual const void* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;

    ISceneNode* m_link4;
    ISceneNode* m_link8;
};

class CScene {
public:
    union UNodePair {
        struct {
            ISceneNode* first;
            ISceneNode* second;
            signed char order;
        } nodes;
        unsigned long long key;
        unsigned char data[16];
    };

    /* inferred inline helper */
    static bool InvolvesBullet(const UNodePair& p) { return p.nodes.first->AsBullet() || p.nodes.second->AsBullet(); }
    bool IsNodeInScene(const ISceneNode*) const;
    CPlayerObject* GetPlayer(int) const;
    int DetectSlowCollisions(float, float&);
    void SweepAllExtents();

    ISceneNode* m_firstNode;
    ISceneNode* m_node4;
    CPlayerObject* m_players[4];
    unsigned char unknown018[192];
    dwi::fast_vec<UNodePair> m_pairs216;
    dwi::fast_vec<UNodePair> m_pairs240;
    dwi::vector<CCollision> m_collisions;
};

int CScene::DetectSlowCollisions(float dt, float& time) {
    UNodePair* aEnd;
    UNodePair* bEnd;
    long ticks = 0;
    int result;
    UNodePair* a;
    UNodePair* b;

    OSGetTick();
    m_collisions.clear();
    SweepAllExtents();
    OSGetTick();
    result = 0;
    a = m_pairs216.m_data;
    aEnd = m_pairs216.m_data + m_pairs216.m_size;
    b = m_pairs240.m_data;
    bEnd = m_pairs240.m_data + m_pairs240.m_size;
    while (a != aEnd && b != bEnd) {
        if (a->key == b->key) {
            if (!InvolvesBullet(*a)) {
                CCollision collision;
                collision.m_node0 = a->nodes.first;
                collision.m_node1 = a->nodes.second;
                collision.m_time = dt;
                collision.m_order = a->nodes.order > 1 ? a->nodes.order - 1 : 0;
                bool passThru = false;
                if (collision.m_node0->AsPlayerObject() || collision.m_node1->AsPlayerObject()) {
                    CCDBObject::DisablePassThru(true);
                    passThru = true;
                }
                ticks -= OSGetTick();
                int status = collision.Test(time);
                ticks += OSGetTick();
                if (passThru)
                    CCDBObject::DisablePassThru(false);
                switch (status) {
                case 1:
                    break;
                case 2:
                    m_collisions.push_back(collision);
                    break;
                case 3:
                    result = 1;
                    goto done;
                }
            }
            a++;
            b++;
        } else {
            if (a->key < b->key)
                a++;
            if (b->key < a->key)
                b++;
        }
    }
done:
    OSGetTick();
    OSGetTick();
    return result;
}

CPlayerObject* CScene::GetPlayer(int index) const {
    return m_players[index];
}

bool CScene::IsNodeInScene(const ISceneNode* node) const {
    if (!node)
        return false;
    if (m_firstNode == node)
        return true;
    if (m_node4 == node)
        return true;
    if (node->m_link8)
        return true;
    return node->m_link4 != 0;
}
