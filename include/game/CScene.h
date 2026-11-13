// CScene: the scene (every node, the player, camera and world, the collision sweep), shared by
// every file that uses it (PortReadiness step 3.3).
//
// Evidence:
// - No virtuals (no table). g_scene (Moh2.cpp, .bss) is 0x148 bytes.
// - Out of line in the original (scene.cpp, units scene*, two drafts): every member below but
//   the nested types. The *_now functions take ISceneNode*, the deferred ones (Add, SetCamera,
//   ...) queue a command (fast_vec<CScene::SModCommand>).
// - Field offsets: the matched views and the layout lint (research/layout/classes.tsv).
//   The containers' types are in the original's mangled names: dwi::list<ISceneNode::SExtent>
//   (SweepExtents), dwi::fast_vec<CLight*>, <CCharacterShadow*>, <CScene::UNodePair>,
//   <CScene::SExclusionEntry>, <CScene::SModCommand> and dwi::vector<CCollision> (their
//   destructors; the order is scene_dtor's).
//
// Inferred, not in any mangled name: the return types (the matched views'), the field names
// and pointer types, UNodePair's members (scene_fast.cpp's), SExclusionEntry's (the views'
// common names; one view reads +8 as an int) and SModCommand's (scene.cpp's, with its
// constructor). The containers are typed when game/DwiContainers.h was included first (the
// weak-copy fragments scene_fastvec_pairs/commands keep their own fast_vec view, which the
// header would redefine) and byte ranges otherwise; the extent lists also need
// game/ISceneNode.h (ISceneNode::SExtent). VisitAllNodesInVolume takes ISceneNode's nested
// EVolumeType, so it is declared only where game/ISceneNode.h is included (files still on
// their own ISceneNode view can't name it).
#ifndef GAME_CSCENE_H
#define GAME_CSCENE_H

class ISceneNode;
class IVolume;
class CPlayerObject;
class CCamera;
class CWorldObject;
class CStaticObject;
class CAnimObject;
class CLight;
class CVector3;
class CViewport;
class CScreen;
class CRenderList;
class CFog;
class CCharacterShadow;
class CCollision;
namespace dwi {
template <class T> class IVisitor;
}

class CScene {
public:
    // A pair of nodes the sweep found overlapping, ordered; key compares the two pointers.
    union UNodePair {
        struct {
            ISceneNode* first;
            ISceneNode* second;
            signed char order;
        } nodes;
        unsigned long long key;
        unsigned char data[16];
    };
    // An excluded pair (ExcludeCollision), the two nodes in address order.
    struct SExclusionEntry {
        const ISceneNode* m_first;
        const ISceneNode* m_second;
        float m_value;
    };
    // A deferred scene change (Add, Remove, SetCamera, ...), run after the update.
    struct SModCommand {
        SModCommand(void (CScene::*f)(ISceneNode*), ISceneNode* n) : function(f), node(n) {}

        void (CScene::*function)(ISceneNode*);
        ISceneNode* node;
    };

    CScene();
    ~CScene();
    void Reset();
    void Shutdown();
    void Update(float);
    void Draw(CViewport&, CScreen&, CRenderList&, CFog&);
    void Add(ISceneNode&);
    void Remove(ISceneNode&);
    void AddPlayer(CPlayerObject*);
    void SetCamera(CCamera*);
    void SetWorld(CWorldObject*);
    void AddNode_now(ISceneNode*);
    void RemoveNode_now(ISceneNode*);
    void AddPlayer_now(ISceneNode*);
    void SetCamera_now(ISceneNode*);
    void SetWorld_now(ISceneNode*);
    void AddNodeExtents(ISceneNode&);
    void RemoveNodeExtents(ISceneNode&);
    void SweepAllExtents();
    bool DetectFastCollisions(float);
    int DetectSlowCollisions(float, float&);
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);
    void IncludeCollision(const ISceneNode&, const ISceneNode&);
    bool IsNodeInScene(const ISceneNode*) const;
    CPlayerObject* GetPlayer(int) const;
    CPlayerObject* FindClosestPlayer(CVector3);
    bool VisitAllStaticObjects(dwi::IVisitor<ISceneNode>&);
#ifdef GAME_ISCENENODE_H
    bool VisitAllNodesInVolume(dwi::IVisitor<ISceneNode>&, const IVolume&, ISceneNode::EVolumeType, bool);
#endif

    ISceneNode* m_head;
    ISceneNode* m_tail;
    CPlayerObject* m_players[4];
    ISceneNode* m_world;
    ISceneNode* m_camera;
    CStaticObject* m_staticHead;
    CStaticObject* m_staticTail;
    CAnimObject* m_animHead;
    CAnimObject* m_animTail;
    CLight* m_lightHead;
    CLight* m_lightTail;
    int m_nodeCount;
    int m_staticCount;
    int m_animCount;
    int m_lightCount;
    bool m_updating;
    int m_playerCount;
#ifdef GAME_DWICONTAINERS_H
    dwi::fast_vec<CLight*> m_lights;
    dwi::fast_vec<CCharacterShadow*> m_shadows;
#ifdef GAME_ISCENENODE_H
    dwi::list<ISceneNode::SExtent> m_extentsX;
    dwi::list<ISceneNode::SExtent> m_extentsY;
#else
    unsigned char m_extentsX[0x2c];    // dwi::list<ISceneNode::SExtent> (needs game/ISceneNode.h)
    unsigned char m_extentsY[0x2c];
#endif
    dwi::fast_vec<UNodePair> m_pairsX;
    dwi::fast_vec<UNodePair> m_pairsY;
    dwi::vector<CCollision> m_collisions;
    dwi::fast_vec<SExclusionEntry> m_exclusions;
    dwi::fast_vec<SModCommand> m_commands;
#else
    unsigned char m_lights[0x18];      // dwi::fast_vec<CLight*> (typed with game/DwiContainers.h)
    unsigned char m_shadows[0x18];     // dwi::fast_vec<CCharacterShadow*>
    unsigned char m_extentsX[0x2c];    // dwi::list<ISceneNode::SExtent>
    unsigned char m_extentsY[0x2c];
    unsigned char m_pairsX[0x18];      // dwi::fast_vec<CScene::UNodePair>
    unsigned char m_pairsY[0x18];
    unsigned char m_collisions[0x10];  // dwi::vector<CCollision>
    unsigned char m_exclusions[0x18];  // dwi::fast_vec<CScene::SExclusionEntry>
    unsigned char m_commands[0x18];    // dwi::fast_vec<CScene::SModCommand>
#endif
};

#endif
