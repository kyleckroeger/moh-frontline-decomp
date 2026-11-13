// A fragment of scene.cpp (0x800d9e24): the weak CmpNodePairs, which orders
// two node pairs by their 64-bit key (unsigned less-than), and the static
// SweepExtents (0x800d9e48), which walks a sorted extent list keeping the
// nodes whose extents are open in a scratch-pad vector: at a start extent it
// pairs the node with every active node whose collision ids have a non-zero
// entry in the collision table and that is not excluded, then makes it
// active; at an end extent it removes it. The list is walked with an iterator
// object (dwi::list::iterator, as Rising Sun's dwi containers), which gives
// the original's register order. CScene, UNodePair, SExtent, the dwi
// containers and the functions are named by the mangled symbols; members,
// locals and the inline helpers are inferred, except fast_vec::reserve(T*, int)
// (adopting the scratch pad; the original's weak copy is at 0x800770a4). CmpNodePairs is a weak copy of
// a header inline emitted in this file, so it is defined __declspec(weak); the
// fast_vec destructor's original copy is placed by scene_fastvec_node. The
// rest of the file is not part of this unit.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CPlayerObject;

extern "C" void MEM_free(void*);
extern void* SPR_BEGIN;
extern const signed char g_theCollisionTable[];

class ISceneNode {
public:
    enum EVolumeType {};
    struct SExtent {
        ISceneNode* node;
        bool start;
        unsigned char unknown5[11];
    };
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

#include "game/DwiContainers.h"

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
    struct SExclusionEntry {
        ISceneNode* first;
        ISceneNode* second;
        int unknown8;

        bool Matches(ISceneNode* a, ISceneNode* b) const { return first == a && second == b; }
    };
};

static inline signed char GetCollisionOrder(int a, int b) {
    int index;

    if (a == -1 || b == -1)
        return 0;
    if (a <= b)
        index = a + b * (b + 1) / 2;
    else
        index = b + a * (a + 1) / 2;
    return g_theCollisionTable[index];
}

static inline bool IsExcluded(dwi::fast_vec<CScene::SExclusionEntry>& exclusions, ISceneNode* a, ISceneNode* b) {
    ISceneNode* hi;
    ISceneNode* lo;
    if (a < b) {
        lo = a;
        hi = b;
    } else {
        lo = b;
        hi = a;
    }
    CScene::SExclusionEntry* e;
    for (e = exclusions.begin(); e != exclusions.end(); e++) {
        if (e->Matches(lo, hi))
            break;
    }
    return e != exclusions.end();
}

static void SweepExtents(dwi::list<ISceneNode::SExtent>& extents, dwi::fast_vec<CScene::UNodePair>& pairs,
                         dwi::fast_vec<CScene::SExclusionEntry>& exclusions) {
    dwi::fast_vec<ISceneNode*> active;
    dwi::list<ISceneNode::SExtent>::iterator node;
    ISceneNode* sceneNode;
    ISceneNode** it;
    active.reserve((ISceneNode**)SPR_BEGIN, 250);
    for (node = extents.begin(); node != extents.end(); ++node) {
        sceneNode = (*node).node;
        if (sceneNode->GetCollisionId() == -1)
            continue;
        if ((*node).start) {
            for (it = active.begin(); it != active.end(); it++) {
                ISceneNode* other;
                int order = GetCollisionOrder((*it)->GetCollisionId(), sceneNode->GetCollisionId());
                if (order) {
                    other = *it;
                    if (!IsExcluded(exclusions, other, sceneNode)) {
                        CScene::UNodePair pair;
                        if (other < sceneNode) {
                            pair.nodes.first = other;
                            pair.nodes.second = sceneNode;
                        } else {
                            pair.nodes.first = sceneNode;
                            pair.nodes.second = other;
                        }
                        pair.nodes.order = order;
                        pairs.push_back(pair);
                    }
                }
            }
            active.push_back(sceneNode);
        } else {
            active.erase(active.find(sceneNode));
        }
    }
}

__declspec(weak) bool CmpNodePairs(const CScene::UNodePair& a, const CScene::UNodePair& b) {
    return a.key < b.key;
}
