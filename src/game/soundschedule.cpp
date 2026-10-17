// SoundSchedule, the last functions of the file: Init (clears the schedule
// and links the 100 registration records into the free list), the
// destructor and constructor and the static construction of the records
// (with the records' empty inline constructor, emitted here). SoundSchedule,
// SoundScheduleRegistrationRecord, BSObject and the statics are named by the
// symbols; the record (144 bytes) and schedule layouts and the free-list
// inline are inferred. The free list's head is defined in another unit.
class BSObject;

class SoundScheduleRegistrationRecord {
public:
    SoundScheduleRegistrationRecord() {}

    unsigned char unknown00[24];
    BSObject* object;
    int endEvent;
    unsigned char aemsEvent[80];
    int aemsHandle;
    unsigned char unknown74[24];
    SoundScheduleRegistrationRecord* next;
};

static SoundScheduleRegistrationRecord g_AllRecords[100];
extern SoundScheduleRegistrationRecord* g_FreeRecordList;

static inline void LinkFreeRecords() {
    SoundScheduleRegistrationRecord* record = g_AllRecords;
    for (unsigned int i = 0; i < 99; i++, record++)
        record->next = record + 1;
    record->next = 0;
    g_FreeRecordList = g_AllRecords;
}

class SoundSchedule {
public:
    SoundSchedule();
    ~SoundSchedule();
    void Init();

    int m_count;
    SoundScheduleRegistrationRecord* m_records;
};

void SoundSchedule::Init() {
    m_records = 0;
    m_count = 0;
    LinkFreeRecords();
}

SoundSchedule::~SoundSchedule() {
}

SoundSchedule::SoundSchedule() {
    m_records = 0;
    m_count = 0;
}
