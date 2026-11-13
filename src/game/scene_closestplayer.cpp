// A fragment of scene.cpp (0x800da540): CScene::FindClosestPlayer returns the
// player (the table at +8, its count at +76) whose position, through its
// GetPosition (virtual slot +64), is nearest the given point, or none. The
// class and function are named by the symbols; the scene and player views,
// the vector's double-pair view and its inline operations (the libc inline
// sqrtf for the length) are inferred. The rest of the file is in other units.
#include <math.h>



#include "game/CVector3.h"

#include "game/CPlayerObject.h"


#include "game/CScene.h"


CPlayerObject* CScene::FindClosestPlayer(CVector3 point) {
    float best = 3.4028235e+38f;
    CPlayerObject* closest = 0;
    for (int i = 0; i < m_playerCount; i++) {
        CVector3 position;
        m_players[i]->GetPosition(position);
        position -= point;
        float distance = position.Length();
        if (distance < best) {
            best = distance;
            closest = m_players[i];
        }
    }
    return closest;
}
