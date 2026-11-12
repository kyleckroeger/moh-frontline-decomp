// A fragment of proxtrig.cpp (0x800451ec): TriggerEnterAndLeaveMessages
// sends the enter and leave messages of the triggers an object's proximity
// data gained or lost. With the same leaf as before (or no previous leaf) it
// walks the bits of the changed trigger mask; otherwise it merges the sorted
// trigger lists of the current and previous leaves. Each entry's top bit
// selects a light volume (entered or left through the object's moving scene
// node); otherwise the trigger's core either sends its (restore) enter or
// leave message list to the trigger's script object, and the climbing
// trigger (type 5, crc 0x2070a66d) toggles the player's climbing state, or,
// for a type-0 core with id 0x4d078f23, acquires the core's four acquire
// shorts in the scene's world (a const SAcquireSet, built from the .rodata
// zero template). The function, classes, BSSendMessage, g_pTriggerObjects,
// g_pLightVolumes and g_scene are named by the symbols; the inline helpers,
// the views and the member names are inferred. Matching keys: the light test
// written '(entry & 0x8000) >> 15', the send loops 'for (i = 0; i < count;
// i++, messages++)', the const acquire set (it gives the original's single
// unfolded core load and the template copy order) and the mixed core access
// spellings in EnterTrigger.
class BSObject;
class CBullet;
class CCollision;
class CDrawContext;
class CLight;
class CMatrix;
class CPlayerObject;
class CStaticObject;
struct AIDoodadView;
struct EClsnIdView;

#include "game/CVector3.h"

struct BPDLightVolume {
    unsigned char data[48];
};

class IMovingSceneNode;

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
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(int);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual IMovingSceneNode* AsMovingNode();
    virtual const IMovingSceneNode* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
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
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual int GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual int GetLightVolume();
};

class CPlayerObject : public IMovingSceneNode {
public:
    void SetClimbingState(bool);
};

class CWorldObject {
public:
    struct SAcquireSet {
        int a;
        int b;
        int c;
        int d;
    };
    void Acquire(const SAcquireSet&, bool);
};

class CScene {
public:
    unsigned char unknown00[24];
    CWorldObject* m_world;
    unsigned char unknown1c[4];

    void Acquire(const CWorldObject::SAcquireSet& set) { m_world->Acquire(set, true); }
};

extern CScene g_scene;

struct _PropBSPLeaf {
    unsigned int count;
    unsigned char unknown04[28];
    unsigned short* triggers;
};

struct ProximityDataView {
    unsigned int previousTriggers;
    unsigned int triggers;
    _PropBSPLeaf* previousLeaf;
    _PropBSPLeaf* leaf;
};

class BSGO_Basic {
public:
    BSObject* object;
    unsigned char unknown04[8];

    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual ProximityDataView* GetProximityData();
};

struct TriggerMessagesView {
    unsigned int count;
    unsigned int messages[1];
};

struct MOH_core_Struct {
    int id;
    unsigned char unknown04[8];
    int type;
    unsigned char unknown10[36];
    short acquire[4];
    unsigned char unknown3c[12];
    int crc;
    unsigned char unknown4c[12];
    TriggerMessagesView* enter;
    TriggerMessagesView* leave;
    TriggerMessagesView* restoreEnter;
    TriggerMessagesView* restoreLeave;
};

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    BSObject* bsObject;
};

extern BPDLightVolume* g_pLightVolumes;
extern TriggerObject_struct g_pTriggerObjects[2000];

void BSSendMessage(unsigned short, BSObject*, TriggerObject_struct*, BSObject*, void*);

inline void EnterTrigger(BSGO_Basic* object, unsigned short entry, bool restore) {
    unsigned int count = 0;
    unsigned int* messages = 0;
    unsigned int index = entry & 0x7FFF;
    unsigned int i;
    TriggerObject_struct* trigger;
    if ((entry & 0x8000) >> 15) {
        IMovingSceneNode* node = object->GetSceneNode()->AsMovingNode();
        if (node)
            node->EnterLightVolume(&g_pLightVolumes[index]);
        return;
    }
    trigger = &g_pTriggerObjects[index];
    MOH_core_Struct* core = g_pTriggerObjects[index].core;
    if (core->type) {
        if (restore && core->restoreEnter) {
            count = g_pTriggerObjects[index].core->restoreEnter->count;
            messages = g_pTriggerObjects[index].core->restoreEnter->messages;
        } else if (!restore && core->enter) {
            count = core->enter->count;
            messages = core->enter->messages;
        }
        for (i = 0; i < count; i++, messages++)
            BSSendMessage(*messages, g_pTriggerObjects[index].bsObject, trigger, object->object, 0);
        if (trigger->core->type == 5 && trigger->core->crc == 0x2070a66d && !restore)
            ((CPlayerObject*)object->GetSceneNode())->SetClimbingState(true);
    } else {
        switch (core->id) {
        case 0x4d078f23:
            if (!restore) {
                const CWorldObject::SAcquireSet set = {g_pTriggerObjects[index].core->acquire[0], g_pTriggerObjects[index].core->acquire[1], g_pTriggerObjects[index].core->acquire[2], g_pTriggerObjects[index].core->acquire[3]};
                g_scene.Acquire(set);
            }
            break;
        }
    }
}
inline void LeaveTrigger(BSGO_Basic* object, unsigned short entry, bool restore) {
    unsigned int count = 0;
    unsigned int* messages = 0;
    unsigned int index = entry & 0x7FFF;
    unsigned int i;
    TriggerObject_struct* trigger;
    if ((entry & 0x8000) >> 15) {
        IMovingSceneNode* node = object->GetSceneNode()->AsMovingNode();
        if (node)
            node->ExitLightVolume(&g_pLightVolumes[index]);
        return;
    }
    if (!g_pTriggerObjects[index].core->type)
    return;
    if (restore && g_pTriggerObjects[index].core->restoreLeave) {
        count = g_pTriggerObjects[index].core->restoreLeave->count;
        messages = g_pTriggerObjects[index].core->restoreLeave->messages;
    } else if (!restore && g_pTriggerObjects[index].core->leave) {
        count = g_pTriggerObjects[index].core->leave->count;
        messages = g_pTriggerObjects[index].core->leave->messages;
    }
    trigger = &g_pTriggerObjects[index];
    for (i = 0; i < count; i++, messages++)
        BSSendMessage(*messages, g_pTriggerObjects[index].bsObject, trigger, object->object, 0);
    if (trigger->core->type == 5 && trigger->core->crc == 0x2070a66d && !restore)
        ((CPlayerObject*)object->GetSceneNode())->SetClimbingState(false);
}

void TriggerEnterAndLeaveMessages(BSGO_Basic* object, bool restore) {
    ProximityDataView* proximity = object->GetProximityData();
    if (proximity->leaf == proximity->previousLeaf || !proximity->previousLeaf) {
        unsigned int changed = proximity->triggers ^ proximity->previousTriggers;
        int entered = changed & proximity->triggers;
        int left = changed & proximity->previousTriggers;
        int i;
        for (i = 0; entered; entered >>= 1, i++) {
            if (entered & 1)
                EnterTrigger(object, proximity->leaf->triggers[i], restore);
        }
        for (i = 0; left; left >>= 1, i++) {
            if (left & 1)
                LeaveTrigger(object, proximity->leaf->triggers[i], restore);
        }
    } else {
        unsigned short* current;
        unsigned short* previous;
        unsigned short* currentEnd;
        unsigned short* previousEnd;
        unsigned int currentBit;
        unsigned int previousBit;
        unsigned int currentMask;
        unsigned int previousMask;
        current = proximity->leaf->triggers;
        previous = proximity->previousLeaf->triggers;
        currentMask = proximity->triggers;
        previousMask = proximity->previousTriggers;
        currentEnd = proximity->leaf->triggers + proximity->leaf->count;
        previousEnd = proximity->previousLeaf->triggers + proximity->previousLeaf->count;
        currentBit = 1;
        previousBit = 1;
        for (;;) {
            if (current == currentEnd || previous == previousEnd)
                break;
            if (*current < *previous) {
                if (currentBit & currentMask)
                    EnterTrigger(object, *current, restore);
                currentBit <<= 1;
                current++;
            } else if (*current > *previous) {
                if (previousBit & previousMask)
                    LeaveTrigger(object, *previous, restore);
                previousBit <<= 1;
                previous++;
            } else {
                unsigned int inCurrent = currentBit & currentMask;
                unsigned int inPrevious = previousBit & previousMask;
                if (inCurrent && !inPrevious)
                    EnterTrigger(object, *current, restore);
                else if (inPrevious && !inCurrent)
                    LeaveTrigger(object, *previous, restore);
                currentBit <<= 1;
                previousBit <<= 1;
                current++;
                previous++;
            }
        }
        for (; current != currentEnd; current++) {
            if (currentMask & currentBit)
                EnterTrigger(object, *current, restore);
            currentBit <<= 1;
        }
        for (; previous != previousEnd; previous++) {
            if (previousMask & previousBit)
                LeaveTrigger(object, *previous, restore);
            previousBit <<= 1;
        }
    }
}
