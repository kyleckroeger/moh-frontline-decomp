// A fragment of soundschedule.cpp (0x80046bc8): SoundSchedule::Update refreshes
// every scheduled sound that is not delayed: for a node sound the azimuth and
// distance to the listener (the closest player in multiplayer, else player 0)
// from the node's position, and for a moving static object the speed parameter
// (scaled to 220, with a separate formula in mission 4 stage 2); for a fixed
// sound the same azimuth and distance from its stored position. It then counts
// down the delays and updates the AEMS events under the sound critical section,
// and returns finished records to the free list (triggering the object's end
// event and releasing a soldier's voice). UpdateSound
// copies new AEMS event data into the scheduled sound with the given handle
// (found by an inline search of the schedule); RemoveSound removes the sound
// with the given handle (triggering its object's end event, optionally ending
// the AEMS event, releasing a soldier's voice and returning the record to the
// free list); AddSound takes a free record (when none is left it reports the
// full queue and triggers the object's end event), fills it in and puts it at
// the head of the schedule, and for a soldier's voice first removes the
// soldier's current voice sound (RemoveSound inlined) and records the new
// handle; Shutdown ends every scheduled sound, returns the records and clears
// the schedule and the free list. The unit owns the file's .rodata block (the
// header's hash-table primes, then AddSound's messages). The file name is this
// project's; the original record is soundschedule.cpp. SoundSchedule,
// SoundScheduleRegistrationRecord, ISceneNode, CSoldierObject, CVector3,
// BSObject, EBSEventEnum and the free list are named by the symbols; the
// record (144 bytes) and the soldier view are inferred (the voice handle at
// +17312, as in bsbifunc), as is the inline record search. AddSound's zero is
// a .sdata2 pool entry, as are Update's float constants. Update's helpers
// (NodeAzimuth, NodeDistance, PositionAzimuth, PositionDistance, ObjectSpeed),
// the static object, scene and shell views are inferred; the helpers' grouping
// follows the original's stack-slot layout. Init after these is not part of
// this unit (Init is in scratch/ssw, 7 lines off).
class BSObject;
enum EBSEventEnum {};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void AEMS_EndEvent(void*, int);
void DebugMsg(const char*, ...);
extern "C" void* memcpy(void*, const void*, unsigned long);
extern "C" {
#include <math.h>
}

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

class CSoldierObject {
public:
    unsigned char unknown0000[17312];
    int m_voice;
};

class CStaticObject;

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual int GetLocalBoundingVolume(int) const;
    virtual int GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(void*) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(void*) const;
    virtual int IsDrawEnabled() const;
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(int);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(void*, const void*);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual CSoldierObject* AsSoldierObject();
};

class CPlayerObject : public ISceneNode {
};

class CStaticObject {
public:
    bool IsMoving() const { return m_state != 0 && m_state != 1; }
    float GetSpeed() const {
        return sqrtf(m_velocity[0] * m_velocity[0] + m_velocity[1] * m_velocity[1] + m_velocity[2] * m_velocity[2]);
    }

    unsigned char unknown000[128];
    float m_scale;
    unsigned char unknown084[84];
    float m_speed;
    float m_maxSpeed;
    unsigned char unknown0e0[36];
    int m_state;
    unsigned char unknown108[88];
    float m_velocity[3];
};

class CScene {
public:
    unsigned char unknown0000[16];

    CPlayerObject* GetPlayer(int) const;
    CPlayerObject* FindClosestPlayer(CVector3);
};

class CShellMenu {
public:
    unsigned char unknown0000[16];

    unsigned int Get_currentStage();
    unsigned int Get_currentMission();
};

extern CScene g_scene;
extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;
unsigned short GetAzimuth(CVector3, const CPlayerObject*);
int AEMS_UpdateEvent(void*, int);
extern "C" void SNDSYS_entercritical();
extern "C" void SNDSYS_leavecritical();

class SoundScheduleRegistrationRecord {
public:
    CVector3 position;
    int type;
    ISceneNode* node;
    BSObject* object;
    int endEvent;
    int aemsEvent[20];
    int aemsHandle;
    short azimuthParam;
    short distanceParam;
    short speedParam;
    short size;
    short done;
    short delay;
    float distanceScale;
    CSoldierObject* soldier;
    bool voice;
    SoundScheduleRegistrationRecord* next;
};

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

// File-local in the original; declared extern so this fragment can refer to it.
extern SoundScheduleRegistrationRecord* g_FreeRecordList;

class SoundSchedule {
public:
    enum ESoundScheduleObjectType {};

    SoundScheduleRegistrationRecord* Find(int handle) {
        for (SoundScheduleRegistrationRecord* record = m_records; record; record = record->next) {
            if (record->aemsHandle == handle)
                return record;
        }
        return 0;
    }
    void Update(int);
    void Shutdown();
    void UpdateSound(int, void*, int);
    void RemoveSound(int, bool);
    void AddSound(ESoundScheduleObjectType, ISceneNode*, CVector3*, void*, int, int, int, int, int,
                  BSObject*, EBSEventEnum, float, bool, int);

    int m_count;
    SoundScheduleRegistrationRecord* m_records;
};

static inline int NodeAzimuth(SoundScheduleRegistrationRecord* record) {
    CVector3 position;
    record->node->GetPosition(position);
    if (g_bInMultiplayerMode) {
        CVector3 nodePosition;
        record->node->GetPosition(nodePosition);
        return GetAzimuth(nodePosition, g_scene.FindClosestPlayer(nodePosition));
    }
    return GetAzimuth(position, g_scene.GetPlayer(0));
}
static inline float NodeDistance(SoundScheduleRegistrationRecord* record) {
    CVector3 delta;
    record->node->GetPosition(delta);
    CVector3 listener;
    if (g_bInMultiplayerMode)
        {
        CVector3 nodePosition;
        record->node->GetPosition(nodePosition);
        g_scene.FindClosestPlayer(nodePosition)->GetPosition(listener);
    }
    else
        g_scene.GetPlayer(0)->GetPosition(listener);
    delta.d.v[0] -= listener.d.v[0];
    delta.d.v[1] -= listener.d.v[1];
    delta.d.v[2] -= listener.d.v[2];
    return sqrtf(delta.d.v[0] * delta.d.v[0] + delta.d.v[1] * delta.d.v[1] + delta.d.v[2] * delta.d.v[2]);
}
static inline int PositionAzimuth(const CVector3& position) {
    if (g_bInMultiplayerMode)
        return GetAzimuth(position, g_scene.FindClosestPlayer(position));
    return GetAzimuth(position, g_scene.GetPlayer(0));
}
static inline float PositionDistance(const CVector3& position) {
    CVector3 listener;
    if (g_bInMultiplayerMode)
        g_scene.FindClosestPlayer(position)->GetPosition(listener);
    else
        g_scene.GetPlayer(0)->GetPosition(listener);
    listener.d.v[0] -= position.d.v[0];
    listener.d.v[1] -= position.d.v[1];
    listener.d.v[2] -= position.d.v[2];
    return sqrtf(listener.d.v[0] * listener.d.v[0] + listener.d.v[1] * listener.d.v[1] + listener.d.v[2] * listener.d.v[2]);
}
static inline unsigned int ObjectSpeed(CStaticObject* object) {
    unsigned int speed;
    if (g_Shell.Get_currentMission() == 4 && g_Shell.Get_currentStage() == 2 && !g_bInMultiplayerMode) {
        if (object->IsMoving()) {
            speed = (0.16f + object->m_speed) * (220.0f / (0.16f + object->m_maxSpeed));
        } else {
            float length = object->GetSpeed();
            speed = length * (220.0f * object->m_scale);
        }
    } else {
        if (object->IsMoving()) {
            speed = object->m_speed * (220.0f / object->m_maxSpeed);
        } else {
            float length = object->GetSpeed();
            speed = length * (220.0f * object->m_scale);
        }
    }
    return speed;
}
void SoundSchedule::Update(int frames) {
    SoundScheduleRegistrationRecord* record;
    for (record = m_records; record; record = record->next) {
        if (record->delay != 0)
            continue;
        if (record->type == 0) {
            record->aemsEvent[record->azimuthParam] = NodeAzimuth(record);
            float distance = NodeDistance(record);
            record->aemsEvent[record->distanceParam] = (int)(256.0f * (record->distanceScale * distance));
            CStaticObject* object = record->node->AsStaticObject();
            if (object && record->speedParam != -1) {
                record->aemsEvent[record->speedParam] = ObjectSpeed(object);
            }
        } else {
            record->aemsEvent[record->azimuthParam] = PositionAzimuth(record->position);
            float distance = PositionDistance(record->position);
            record->aemsEvent[record->distanceParam] = (int)(256.0f * (record->distanceScale * distance));
        }
    }
    SNDSYS_entercritical();
    for (record = m_records; record; record = record->next) {
        if (record->delay != 0) {
            record->delay -= frames;
            if (record->delay < 0)
                record->delay = 0;
        } else if (AEMS_UpdateEvent(record->aemsEvent, record->aemsHandle) < 0) {
            record->done = 1;
        }
    }
    SNDSYS_leavecritical();
    SoundScheduleRegistrationRecord* previous = 0;
    record = m_records;
    while (record) {
        SoundScheduleRegistrationRecord* next = record->next;
        if (record->done) {
            if (record->object && record->endEvent != -1)
                BSObjectTriggerEvent(record->object, record->endEvent, 0, 0, false);
            if (record->soldier)
                record->soldier->m_voice = -1;
            record->next = g_FreeRecordList;
            g_FreeRecordList = record;
            m_count--;
            if (previous)
                previous->next = next;
            else
                m_records = next;
        } else {
            previous = record;
        }
        record = next;
    }
}

void SoundSchedule::UpdateSound(int handle, void* data, int size) {
    SoundScheduleRegistrationRecord* record = Find(handle);
    if (record)
        memcpy(record->aemsEvent, data, size);
}

void SoundSchedule::RemoveSound(int handle, bool end) {
    SoundScheduleRegistrationRecord* record = m_records;
    SoundScheduleRegistrationRecord* previous = 0;
    while (record) {
        SoundScheduleRegistrationRecord* next = record->next;
        if (record->aemsHandle == handle) {
            if (record->object && record->endEvent != -1)
                BSObjectTriggerEvent(record->object, record->endEvent, 0, 0, false);
            if (end)
                AEMS_EndEvent(record->aemsEvent, handle);
            if (record->soldier && record->voice)
                record->soldier->m_voice = -1;
            record->next = g_FreeRecordList;
            g_FreeRecordList = record;
            m_count--;
            if (previous)
                previous->next = next;
            else
                m_records = next;
            return;
        }
        previous = record;
        record = next;
    }
}

void SoundSchedule::AddSound(ESoundScheduleObjectType type, ISceneNode* node, CVector3* position,
                             void* data, int size, int handle, int a, int b, int c, BSObject* object,
                             EBSEventEnum event, float f, bool voice, int d) {
    SoundScheduleRegistrationRecord* record = 0;
    if (g_FreeRecordList) {
        record = g_FreeRecordList;
        g_FreeRecordList = record->next;
    }
    if (!record) {
        DebugMsg("*************************************************************\n");
        DebugMsg("The Sound Schedule queue is full!\n");
        DebugMsg("the sound you tried to play is being dropped\n");
        DebugMsg("some investigation should be done as to why the queue is full\n");
        DebugMsg("the schedular size is currently set to %d", 100);
        DebugMsg("*************************************************************\n");
        if (object && event != -1)
            BSObjectTriggerEvent(object, event, 0, 0, false);
        return;
    }
    record->type = type;
    record->node = node;
    if (position) {
        record->position = *position;
    } else {
        record->position.d.v[0] = 0.0f;
        record->position.d.v[1] = 0.0f;
        record->position.d.v[2] = 0.0f;
    }
    memcpy(record->aemsEvent, data, size);
    record->size = size;
    record->aemsHandle = handle;
    record->azimuthParam = a;
    record->distanceParam = b;
    record->speedParam = c;
    record->object = object;
    record->endEvent = event;
    record->done = 0;
    record->distanceScale = f;
    record->delay = d;
    record->soldier = node ? node->AsSoldierObject() : 0;
    record->voice = voice;
    record->next = m_records;
    m_records = record;
    m_count++;
    if (voice && record->soldier) {
        if (record->soldier->m_voice != -1)
            RemoveSound(record->soldier->m_voice, true);
        record->soldier->m_voice = handle;
    }
}

void SoundSchedule::Shutdown() {
    SoundScheduleRegistrationRecord* record = m_records;
    while (record) {
        SoundScheduleRegistrationRecord* next = record->next;
        if (record->object && record->endEvent != -1)
            BSObjectTriggerEvent(record->object, record->endEvent, 0, 0, false);
        AEMS_EndEvent(record->aemsEvent, record->aemsHandle);
        record->next = g_FreeRecordList;
        g_FreeRecordList = record;
        record = next;
        m_count--;
    }
    m_records = 0;
    m_count = 0;
    g_FreeRecordList = 0;
}
