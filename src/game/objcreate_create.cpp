// A fragment of objcreate.cpp (0x8004029c): CreateObject (creates the
// object for a trigger by its core's type: players by CRC, else
// destructables, collision volumes, Germans, particle systems (kept only
// with a zero value at +136) and lights (kept only when enabled); a unique
// prop is reported), UpdateObjectCreation (destroys queued objects at least
// two frames old, counts the queued German creations that have waited more
// than six frames, has that many far soldiers destroyed, creates the queued
// Germans until one fails, then advances the frame counter) and
// ShutdownObjectCreators (destroys every queued object and clears the box,
// German and tank lists). The functions, classes and globals are named by
// the symbols (the queues, counters and frame counter are file-local in the
// original and declared extern here); the trigger, queue-entry and object
// views and the CRC-switch and destroy helpers are inferred. The rest of the
// file is not part of this unit.
void DebugMsg(const char*, ...);
class BSObject;
struct TriggerObject_struct;

void BSSendMessage(unsigned short, BSObject*, TriggerObject_struct*, BSObject*, void*);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void NonVisDestructFarthestSoldiers(int);

struct TriggerCoreView {
    unsigned char unknown00[12];
    unsigned int m_type;
    unsigned char unknown10[56];
    int m_crc;
    unsigned char unknown4c[36];
    bool m_enabled;
    unsigned char unknown71[23];
    float m_value88;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* m_core;
    BSObject* m_script;
};

struct ScriptObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* m_trigger;
};

/* inferred: an object queued for delayed destruction, its table at +12 */
class DelayedObjectView {
public:
    ScriptObjectView* m_script;
    unsigned char unknown04[8];

    virtual void Destroy();
};

/* inferred: a delayed destruction and the frame it was queued in */
struct DelayDestroyEntry {
    DelayedObjectView* m_object;
    unsigned int m_frame;
};

/* inferred: a queued German creation and the frames it has waited */
struct GermanCreationEntry {
    TriggerObject_struct* m_trigger;
    int m_index;
    int m_wait;
};

extern DelayDestroyEntry g_pDelayDestroyList[10];
extern int g_delayDestroyHead;
extern int g_delayDestroyTail;
extern unsigned int g_frameCounter;
extern GermanCreationEntry g_pGermanCreationQueue[15];
extern int g_iGermanCreationQueueHeadIndex;
extern int g_iGermanCreationQueueTailIndex;
extern int g_iNumGermansQueuedThisFrame;
extern void* g_pBoxList;
extern void* g_pGermanList;
extern void* g_pTankList;

/* inferred: destroys the oldest delayed object */
inline void DestroyDelayedObject() {
    DelayedObjectView* object = g_pDelayDestroyList[g_delayDestroyHead].m_object;
    BSSendMessage(441, (BSObject*)object->m_script, object->m_script->m_trigger, (BSObject*)object->m_script, object->m_script);
    object->Destroy();
    g_pDelayDestroyList[g_delayDestroyHead].m_frame = 0;
    g_pDelayDestroyList[g_delayDestroyHead].m_object = 0;
    g_delayDestroyHead++;
    if (g_delayDestroyHead >= 10)
        g_delayDestroyHead = 0;
}

void* CreatePlayer(TriggerObject_struct*, int);
void* CreateDestructable(TriggerObject_struct*, int);
void* CreateCollisionVolume(TriggerObject_struct*, int);
void* CreateGerman(TriggerObject_struct*, int, bool);

class CParticleSystem {
public:
    static void* Create(void*);
};

class CLight {
public:
    static void* Create(void*);
};

/* inferred: the player creation types by CRC */
inline void* CreateUnique(TriggerObject_struct* trigger, int index, void* data) {
    switch (trigger->m_core->m_crc) {
    case 0x271bd574:
    case (int)0xfd092133:
        return CreatePlayer(trigger, (int)data);
    }
    return CreateDestructable(trigger, index);
}

void* CreateObject(TriggerObject_struct* trigger, int index, void* data) {
    void* object = 0;
    TriggerCoreView* core = trigger->m_core;
    switch (core->m_type) {
    case 2:
        object = CreateUnique(trigger, index, data);
        break;
    case 8:
        object = CreateCollisionVolume(trigger, index);
        break;
    case 3:
        object = CreateGerman(trigger, index, true);
        break;
    case 4:
        object = CreateDestructable(trigger, index);
        break;
    case 9:
        object = CParticleSystem::Create(core);
        if (trigger->m_core->m_value88 != 0.0f)
            object = 0;
        break;
    case 14:
        object = CLight::Create(core);
        if (!trigger->m_core->m_enabled)
            object = 0;
        break;
    case 0:
        DebugMsg("How is a UNIQUE_PROP trying to create something?\n");
        break;
    }
    return object;
}

void UpdateObjectCreation() {
    while (g_delayDestroyHead != g_delayDestroyTail && g_frameCounter - g_pDelayDestroyList[g_delayDestroyHead].m_frame >= 2)
        DestroyDelayedObject();
    int waited = 0;
    for (int i = g_iGermanCreationQueueHeadIndex; i != g_iGermanCreationQueueTailIndex; i = i + 1 >= 15 ? 0 : i + 1) {
        g_pGermanCreationQueue[i].m_wait++;
        if (g_pGermanCreationQueue[i].m_wait > 6)
            waited++;
    }
    NonVisDestructFarthestSoldiers(g_iNumGermansQueuedThisFrame + waited);
    g_iNumGermansQueuedThisFrame = 0;
    while (g_iGermanCreationQueueHeadIndex != g_iGermanCreationQueueTailIndex) {
        void* german = CreateGerman(g_pGermanCreationQueue[g_iGermanCreationQueueHeadIndex].m_trigger, g_pGermanCreationQueue[g_iGermanCreationQueueHeadIndex].m_index, false);
        if (!german)
            break;
        BSObjectTriggerEvent(g_pGermanCreationQueue[g_iGermanCreationQueueHeadIndex].m_trigger->m_script, 243, german, 0, false);
        g_pGermanCreationQueue[g_iGermanCreationQueueHeadIndex].m_trigger = 0;
        g_pGermanCreationQueue[g_iGermanCreationQueueHeadIndex].m_index = -1;
        g_pGermanCreationQueue[g_iGermanCreationQueueHeadIndex].m_wait = 0;
        g_iGermanCreationQueueHeadIndex++;
        g_iGermanCreationQueueHeadIndex = g_iGermanCreationQueueHeadIndex >= 15 ? 0 : g_iGermanCreationQueueHeadIndex;
    }
    g_frameCounter++;
}

void ShutdownObjectCreators() {
    while (g_delayDestroyHead != g_delayDestroyTail)
        DestroyDelayedObject();
    g_pBoxList = 0;
    g_pGermanList = 0;
    g_pTankList = 0;
}
