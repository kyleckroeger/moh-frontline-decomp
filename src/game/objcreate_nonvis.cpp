// A fragment of objcreate.cpp (0x8003bf88): NonVisDestructFarthestSoldiers.
// When creating 'count' more soldiers would pass the limit from commondata.crp,
// it reports the limit, measures the squared distance from the player to every
// German not already marked for non-visible destruction, then repeatedly picks
// the farthest one still nearer than the last pick and sends its script object
// event 45 (counting the soldiers that accept), warning when it runs out of
// candidates. Its strings are reached from the base of the file's .rodata block,
// which starts with the header's prime list and the RTTI type names of
// BSGO_Player and BSGO_Basic; so BSGO_Player::Destroy (the key function of its
// table, landed in objcreate_volumes) is compiled here again as a file
// duplicate to create those names first, and the block is pooled. The file is
// compiled with deferred inlining, so the functions are written in reverse
// image order. BSGO_Basic::Destroy is the class's empty inline default (weak in
// the original), so BSGO_Basic's type name follows BSGO_Player's as in the
// original. The classes, functions and globals are named by the symbols;
// ISceneNode's table follows AIFilter_visit.cpp, the German list links (+4) and
// the scene-node slot of BSGO_Basic follow objcreate_destroy.cpp, the
// non-visible-destruction flag (+0x3160) follows bsbifunc_melee.cpp, and
// CScene's size is inferred.
void DebugMsg(const char*, ...);
#include "game/CVector3.h"

// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

class BSObject;
class CMatrix;
class CDrawContext;
class CCollision;



/* ISceneNode's table in slot order up to the script object (+0x60), as AIFilter_visit.cpp. */
#include "game/ISceneNode.h"


class BSGO_Basic {
public:
    BSObject* m_object;
    BSGO_Basic* m_next;
    BSGO_Basic* m_prev;

    virtual void Destroy() {}
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData(); /* result type not known */
    virtual void* GetEmbeddedHitReactionInfo(); /* result type not known */

};

#include "game/CPlayerObject.h"


void DestroyBSObject(BSObject*);

/* inferred: the player script object (see player_ctor.cpp), its player at +44 */
class BSGO_Player : public BSGO_Basic {
public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData();

    unsigned char unknown10[28];
    CPlayerObject* m_player;
};

class CScene {
public:
    ISceneNode* GetPlayer(int) const;
    unsigned char unknown[0x4850];
};
extern CScene g_scene;
bool BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

struct NonVisDestructView {
    unsigned char unknown0000[12640];
    bool nonVisDestruct;
};

extern BSGO_Basic* g_pGermanList;
extern int g_maxActiveSoldiers;
extern int g_numActiveSoldiers;

void BSGO_Player::Destroy() {
    m_player->CleanUp();
    DestroyBSObject(m_object);
    m_object = 0;
    m_player = 0;
}

void NonVisDestructFarthestSoldiers(int count) {
    count -= g_maxActiveSoldiers - g_numActiveSoldiers;
    if (count <= 0)
        return;
    DebugMsg("-----The maximum number of soldiers in existance has been reached.\n");
    DebugMsg("-----This number is currently %d.  It is controled by commondata.crp\n", g_maxActiveSoldiers);
    DebugMsg("-----Attempting to remove %d soldiers\n", count);

    CVector3 playerPos;
    g_scene.GetPlayer(0)->GetPosition(playerPos);

    float distance[128];
    ISceneNode* nodes[128];
    int found = 0;
    for (BSGO_Basic* g = g_pGermanList; g; g = g->m_next, found++) {
        ISceneNode* node = g->GetSceneNode();
        if (((NonVisDestructView*)node)->nonVisDestruct) {
            found--;
            continue;
        }
        CVector3 pos;
        node->GetPosition(pos);
        pos -= playerPos;
        nodes[found] = node;
        distance[found] = pos.LengthSq();
    }

    float limit = 1.0e35f;
    int removed = 0;
    while (removed < count) {
        float best = 0.0f;
        ISceneNode* farthest = 0;
        for (int i = 0; i < found; i++) {
            if (distance[i] > best && distance[i] < limit) {
                farthest = nodes[i];
                best = distance[i];
            }
        }
        limit = best;
        if (farthest) {
            if (BSObjectTriggerEvent(farthest->GetScriptObject(), 45, 0, 0, true)) {
                removed++;
                DebugMsg("-----I found a suitable candidate.\n");
            }
        } else {
            DebugMsg("WARNING:WARNING: I tried to NonVisDestruct %d soldiers because there\n", count);
            DebugMsg("WARNING:WARNING: are too many in existance, but I was only able to\n");
            DebugMsg("WARNING:WARNING: NonVisDestruct %d soldiers.  I'll try again later\n", removed);
            return;
        }
    }
}
