// A fragment of scene.cpp (0x800dc5e0): CScene::Shutdown removes nodes until
// the first node pointer is empty. CScene and ISceneNode are named by the
// mangled symbols; the first member is an inferred view, and CScene is a
// non-virtual view (see scene.cpp). Reset before this and the rest of the
// file are not part of this unit.
class ISceneNode;

#include "game/CScene.h"


void CScene::Shutdown() {
    while (m_head)
        Remove(*m_head);
}
