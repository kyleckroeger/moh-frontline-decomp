// late2c (2026-11-13): this draft scores 9 only through 'UpdateStatus(copy = object, ...)', the declined chained-copy form;
// the plain call scores 24 (object r30, g_scene r29, data r28, German pointer r29; orig g_scene r30, data r29, object r28,
// German pointer r31 = i's). Inert on the plain form: object/user locals hoisted or reused, per-loop locals (46), reference
// parameter (both spellings), the inline's locals in ForceUpdate's order (88).
// A fragment of proxtrig.cpp (0x80045f84): CheckLeafsTriggers tests the point
// against each trigger listed by the BSP leaf, setting one bit per entry: an
// entry with its top bit set indexes a light volume, inside when the point is
// on the front of every plane; otherwise it indexes the trigger objects and,
// by the two flags, the trigger is tested (IsPointInTrigger) only when its core
// has the type and pointers the flags select (type 5 with the CRC 0x2070a66d
// in one case). The previous mask and leaf are kept in the proximity data.
// The function, structs and globals are named by the symbols; the members
// follow propdat_bsp.cpp where known and are otherwise inferred from offsets,
// as are the flag names and the light-volume test (an inferred inline
// helper). ForceUpdateProximityTriggerStatus (0x8004618c) updates an object's
// proximity data unless the object has none or its script object's value at
// +16 is -1: when the object's position is still inside the cached leaf's
// bounds the leaf is checked again, otherwise the property BSP tree is walked
// from g_pPBSPHead, alternating between the y and x splits, to the leaf to
// check; then the enter and leave messages are sent. The node and object
// views, the leaf bounds and the inline tests are inferred; the order of the
// coordinate locals gives the original's loads. The rest of the file is in
// other units.
#include "game/CVector3.h"


struct PropPlane4 {
    float x, y, z, d;
};

struct BPDLightVolume {
    float field00;
    float field04;
    float field08;
    float field0c;
    float field10;
    float field14;
    int planeCount;
    PropPlane4* planes;
    unsigned char unknown20[16];
};

struct MOH_core_Struct {
    unsigned char unknown00[12];
    int field0c;
    unsigned char unknown10[56];
    int field48;
    unsigned char unknown4c[12];
    unsigned long* field58;
    unsigned long* field5c;
    unsigned long* field60;
    unsigned long* field64;
    unsigned long* field68;
    void* field6c;
};

struct BSObject;

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    BSObject* bsObject;
};

struct _PropBSPLeaf {
    unsigned int triggerCount;
    unsigned char unknown04[8];
    float minX;
    float maxX;
    float minY;
    float maxY;
    unsigned char unknown1c[4];
    unsigned short* triggers;
};

struct _ProximityData {
    unsigned int previousTriggers;
    unsigned int triggers;
    _PropBSPLeaf* previousLeaf;
    _PropBSPLeaf* leaf;
};

extern BPDLightVolume* g_pLightVolumes;
extern TriggerObject_struct g_pTriggerObjects[2000];

bool IsPointInTrigger(const CVector3&, const TriggerObject_struct*);

inline bool IsPointInLightVolume(const BPDLightVolume& volume, const CVector3& point) {
    PropPlane4* plane = volume.planes;
    for (int i = 0; i < volume.planeCount; i++, plane++) {
        if (plane->x * point.x + plane->y * point.y + plane->z * point.z + plane->d < 0.0)
            return false;
    }
    return true;
}

void CheckLeafsTriggers(const _PropBSPLeaf* leaf, const CVector3& point, _ProximityData* data, bool a, bool b) {
    unsigned int bit = 1;
    unsigned int inside = 0;
    for (unsigned int i = 0; i < leaf->triggerCount; i++) {
        unsigned short entry = leaf->triggers[i];
        unsigned int index = entry & 0x7fff;
        if ((entry >> 15) & 1) {
            if (IsPointInLightVolume(g_pLightVolumes[index], point))
                inside |= bit;
        } else if (a && !b) {
            TriggerObject_struct* trigger = &g_pTriggerObjects[index];
            MOH_core_Struct* core = trigger->core;
            if (core->field0c != 0 && (core->field60 || core->field64 || core->field6c) && IsPointInTrigger(point, trigger))
                inside |= bit;
        } else if (!b) {
            TriggerObject_struct* trigger = &g_pTriggerObjects[index];
            MOH_core_Struct* core = trigger->core;
            int type = core->field0c;
            if (type == 0 || core->field58 || core->field5c || core->field68) {
                if (IsPointInTrigger(point, trigger))
                    inside |= bit;
            } else if (type == 5) {
                if (core->field48 == 0x2070a66d && IsPointInTrigger(point, trigger))
                    inside |= bit;
            }
        }
        bit <<= 1;
    }
    data->previousTriggers = data->triggers;
    data->triggers = inside;
    data->previousLeaf = data->leaf;
    data->leaf = (_PropBSPLeaf*)leaf;
}

struct _PropBSPNode;

union PBSPNodeOrLeaf {
    _PropBSPNode* node;
    _PropBSPLeaf* leaf;
};

struct _PropBSPNode {
    float split;
    PBSPNodeOrLeaf front;
    PBSPNodeOrLeaf back;
    bool frontIsLeaf;
    bool backIsLeaf;
};

extern _PropBSPNode* g_pPBSPHead;

#include "game/ISceneNode.h"


struct BSObjectView {
    unsigned char unknown00[16];
    int m_value10;
};

class BSGO_Basic {
public:
    BSObjectView* m_object;
    BSGO_Basic* m_next;
    unsigned char unknown08[4];

    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual _ProximityData* GetProximityData();
};

void TriggerEnterAndLeaveMessages(BSGO_Basic*, bool);

inline bool IsInLeaf(const _PropBSPLeaf* leaf, const CVector3& pos) {
    if (leaf && pos.x <= leaf->maxX && pos.x >= leaf->minX && pos.y <= leaf->maxY && pos.y >= leaf->minY)
        return true;
    return false;
}

inline bool IsInFront(const _PropBSPNode* node, float y, float x, bool axis) {
    if (axis) {
        if (y > node->split)
            return true;
        return false;
    }
    if (x > node->split)
        return true;
    return false;
}

void ForceUpdateProximityTriggerStatus(BSGO_Basic* object, bool flag) {
    _ProximityData* data = object->GetProximityData();
    if (!data)
        return;
    if (object->m_object && object->m_object->m_value10 == -1)
        return;
    CVector3 pos;
    object->GetSceneNode()->GetPosition(pos);
    if (IsInLeaf(data->leaf, pos)) {
        CheckLeafsTriggers(data->leaf, pos, data, true, flag);
        TriggerEnterAndLeaveMessages(object, true);
        return;
    }
    float y = pos.y;
    bool axis = true;
    float x = pos.x;
    _PropBSPNode* node = g_pPBSPHead;
    for (;;) {
        if (IsInFront(node, y, x, axis)) {
            if (node->backIsLeaf) {
                CheckLeafsTriggers(node->back.leaf, pos, data, true, flag);
                break;
            }
            node = node->back.node;
        } else {
            if (node->frontIsLeaf) {
                CheckLeafsTriggers(node->front.leaf, pos, data, true, flag);
                break;
            }
            node = node->front.node;
        }
        axis = axis == false;
    }
    TriggerEnterAndLeaveMessages(object, true);
}

struct CTriggerUser {
    unsigned char unknown000[0x1e0];
    unsigned char bit7 : 1;
    unsigned char bit6 : 1;
    unsigned char bit5 : 1;
    unsigned char bit4 : 1;
    unsigned char bit3 : 1;
    unsigned char bit2 : 1;
};

inline bool UsesTriggers(const CTriggerUser* user) {
    bool result = false;
    if (user->bit6 || user->bit5 || user->bit2)
        result = true;
    return result;
}

inline void UpdateStatus(BSGO_Basic* object, bool a, bool b) {
    _ProximityData* data = object->GetProximityData();
    if (!data)
        return;
    if (object->m_object && object->m_object->m_value10 == -1)
        return;
    CVector3 pos;
    object->GetSceneNode()->GetPosition(pos);
    if (IsInLeaf(data->leaf, pos)) {
        CheckLeafsTriggers(data->leaf, pos, data, a, b);
        TriggerEnterAndLeaveMessages(object, a);
        return;
    }
    float y = pos.y;
    bool axis = true;
    float x = pos.x;
    _PropBSPNode* node = g_pPBSPHead;
    for (;;) {
        if (IsInFront(node, y, x, axis)) {
            if (node->backIsLeaf) {
                CheckLeafsTriggers(node->back.leaf, pos, data, a, b);
                break;
            }
            node = node->back.node;
        } else {
            if (node->frontIsLeaf) {
                CheckLeafsTriggers(node->front.leaf, pos, data, a, b);
                break;
            }
            node = node->front.node;
        }
        axis = axis == false;
    }
    TriggerEnterAndLeaveMessages(object, a);
}

struct CPlayerView {
    unsigned char unknown000[0x398];
    unsigned char active : 1;
    unsigned char unknown399[0x770 - 0x399];
    BSGO_Basic m_object;
};

#include "game/CScene.h"


extern CScene g_scene;
extern int g_PopUpMsgTimer;
extern BSGO_Basic* g_pGermanList;
extern BSGO_Basic* g_pBoxList;
extern BSGO_Basic* g_pTankList;

void UpdateProximityTriggerStatus() {
    int i;
    BSGO_Basic* copy;
    BSGO_Basic* user;
    if (g_PopUpMsgTimer > 0)
        g_PopUpMsgTimer--;
    for (i = 0; i < g_scene.m_playerCount; i++) {
        CPlayerView* player = (CPlayerView*)g_scene.GetPlayer(i);
        if (player && player->active) {
            BSGO_Basic* object = &player->m_object;
            UpdateStatus(copy = object, false, false);
        }
    }
    for (user = g_pGermanList; user; user = user->m_next)
        UpdateStatus(user, true, false);
    for (user = g_pBoxList; user; user = user->m_next) {
        CTriggerUser* t = (CTriggerUser*)user->GetSceneNode()->AsStaticObject();
        if (t && UsesTriggers(t))
            UpdateStatus(user, true, false);
    }
    for (user = g_pTankList; user; user = user->m_next) {
        CTriggerUser* t = (CTriggerUser*)user->GetSceneNode()->AsHierObject();
        if (t && UsesTriggers(t))
            UpdateStatus(user, true, false);
    }
}
