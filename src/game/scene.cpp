// CScene's deferred modifications: while the scene is being updated, adding
// or removing a node, setting the camera or world and adding a player are
// queued as commands (a pointer to the matching _now member function and the
// node) and run later; otherwise they run at once. CScene, ISceneNode,
// SModCommand and fast_vec are named by the mangled symbols; the scene, the
// command and the vector layouts are inferred, and CScene is a non-virtual
// view. The member-function pointers are the file's .data constants.
// Compiled with deferred inlining, so the functions appear in reverse.
class ISceneNode;
class CCamera;
class CWorldObject;
class CPlayerObject;

#include "game/DwiContainers.h"

class CScene {
public:
    struct SModCommand {
        SModCommand(void (CScene::*f)(ISceneNode*), ISceneNode* n) : function(f), node(n) {}

        void (CScene::*function)(ISceneNode*);
        ISceneNode* node;
    };

    void Remove(ISceneNode&);
    void Add(ISceneNode&);
    void SetCamera(CCamera*);
    void SetWorld(CWorldObject*);
    void AddPlayer(CPlayerObject*);
    void RemoveNode_now(ISceneNode*);
    void AddNode_now(ISceneNode*);
    void SetCamera_now(ISceneNode*);
    void SetWorld_now(ISceneNode*);
    void AddPlayer_now(ISceneNode*);

    unsigned char unknown000[72];
    bool m_updating;
    unsigned char unknown049[231];
    dwi::fast_vec<SModCommand> m_commands;
};

void CScene::AddPlayer(CPlayerObject* player) {
    if (m_updating) {
        SModCommand command(&CScene::AddPlayer_now, (ISceneNode*)player);
        m_commands.push_back(command);
    } else {
        AddPlayer_now((ISceneNode*)player);
    }
}

void CScene::SetWorld(CWorldObject* world) {
    if (m_updating) {
        SModCommand command(&CScene::SetWorld_now, (ISceneNode*)world);
        m_commands.push_back(command);
    } else {
        SetWorld_now((ISceneNode*)world);
    }
}

void CScene::SetCamera(CCamera* camera) {
    if (m_updating) {
        SModCommand command(&CScene::SetCamera_now, (ISceneNode*)camera);
        m_commands.push_back(command);
    } else {
        SetCamera_now((ISceneNode*)camera);
    }
}

void CScene::Add(ISceneNode& node) {
    if (m_updating) {
        SModCommand command(&CScene::AddNode_now, &node);
        m_commands.push_back(command);
    } else {
        AddNode_now(&node);
    }
}

void CScene::Remove(ISceneNode& node) {
    if (m_updating) {
        SModCommand command(&CScene::RemoveNode_now, &node);
        m_commands.push_back(command);
    } else {
        RemoveNode_now(&node);
    }
}
