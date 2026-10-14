// A fragment of soundschedule.cpp (0x80047310): SoundSchedule::UpdateSound
// copies new AEMS event data into the scheduled sound with the given handle
// (found by an inline search of the schedule), and RemoveSound removes the
// sound with the given handle: it triggers the object's end event, optionally
// ends the AEMS event, clears a soldier's voice handle (when the sound is that
// soldier's voice) and returns the record to the free list. The file name is
// this project's; the original record is soundschedule.cpp. SoundSchedule,
// SoundScheduleRegistrationRecord, CSoldierObject, BSObject and the free list
// are named by the symbols; the record (144 bytes) and the soldier view are
// inferred (the voice handle at +17312, as in bsbifunc), as is the search.
// Update before these and AddSound after them are not part of this unit.
class BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void AEMS_EndEvent(void*, int);
extern "C" void* memcpy(void*, const void*, unsigned long);

class CSoldierObject {
public:
    unsigned char unknown0000[17312];
    int m_voice;
};

class SoundScheduleRegistrationRecord {
public:
    unsigned char unknown00[24];
    BSObject* object;
    int endEvent;
    unsigned char aemsEvent[80];
    int aemsHandle;
    unsigned char unknown74[16];
    CSoldierObject* soldier;
    bool voice;
    SoundScheduleRegistrationRecord* next;
};

// File-local in the original; declared extern so this fragment can refer to it.
extern SoundScheduleRegistrationRecord* g_FreeRecordList;

class SoundSchedule {
public:
    SoundScheduleRegistrationRecord* Find(int handle) {
        for (SoundScheduleRegistrationRecord* record = m_records; record; record = record->next) {
            if (record->aemsHandle == handle)
                return record;
        }
        return 0;
    }
    void UpdateSound(int, void*, int);
    void RemoveSound(int, bool);

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
