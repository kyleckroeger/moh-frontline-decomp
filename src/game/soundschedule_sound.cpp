// A fragment of soundschedule.cpp (0x80047310): SoundSchedule::UpdateSound
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
// a .sdata2 pool entry. Update before these and Init after them are not part
// of this unit (Init is in scratch/ssw, 7 lines off).
class BSObject;
enum EBSEventEnum {};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void AEMS_EndEvent(void*, int);
void DebugMsg(const char*, ...);
extern "C" void* memcpy(void*, const void*, unsigned long);

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
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual CSoldierObject* AsSoldierObject();
};

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
    void Shutdown();
    void UpdateSound(int, void*, int);
    void RemoveSound(int, bool);
    void AddSound(ESoundScheduleObjectType, ISceneNode*, CVector3*, void*, int, int, int, int, int,
                  BSObject*, EBSEventEnum, float, bool, int);

    int m_count;
    SoundScheduleRegistrationRecord* m_records;
};

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
