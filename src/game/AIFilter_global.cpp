// A fragment of AIFilter.cpp (0x8005d8b4): CAIFilterGlobal's (static)
// PropagateSoundToAllObjects (the recent sounds older than 1 s are dropped; a
// sound from a source already heard at the same or a higher priority is
// ignored, a lower one replaced; the list is trimmed to 14 by dropping the
// oldest, the sound is passed to every AI object and recorded, and the next
// propagation time set 1 s ahead), the empty UpdateToGlobals,
// UpdateFromGlobals (the frame count, and the time advanced by the elapsed
// frames at 60 per second) and the destructor. The file name is this
// project's; the original record is AIFilter.cpp. The classes, functions and
// objects are named by the symbols; the members, the recent-sound record
// (source, priority, time; built first, compared and appended from memory)
// and the vector's inline accessors (push_back takes the record by value) are
// inferred (the global object's view has its real size, so it is not
// small-data addressed), and the constants are .sdata2 pool entries. CreateSplinePathManager
// before them is not part of this unit.
/* Inferred: a sound's source and priority. */
struct CAIFilterSoundInfo {
    int source;
    unsigned char unknown04[20];
    unsigned short priority;
};

class CAIObject {
public:
    static CAIObject* GetGlobalListStart();
    static void PropagateSoundToAllObjects(const CAIFilterSoundInfo&, CAIObject*);
};

struct NoiseInfo {
    unsigned int source;
    unsigned short priority;
    float time;
};

namespace dwi {
template <class T>
class fast_vec {
public:
    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    int size() const { return m_size; }
    void erase(T* item) {
        T* next = item + 1;
        T* last = end();
        while (next != last) {
            *item = *next;
            item++;
            next++;
        }
        m_size--;
    }
    void push_back(T item) {
        m_data[m_size++] = item;
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CAIFilterGlobal {
public:
    static void PropagateSoundToAllObjects(const CAIFilterSoundInfo&);
    void UpdateToGlobals();
    void UpdateFromGlobals(float);
    ~CAIFilterGlobal();

    static float s_rNextSoundPropagateTime;

    int m_frame;
    float m_time;
    unsigned char unknown08[8];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;
extern dwi::fast_vec<NoiseInfo> recent_sounds;
int GetFrameCount();

void CAIFilterGlobal::PropagateSoundToAllObjects(const CAIFilterSoundInfo& info) {
    NoiseInfo noise;
    noise.source = info.source;
    float now = g_aigAIFilterGlobalObject.m_time;
    noise.priority = info.priority;
    noise.time = now;
    NoiseInfo* it = recent_sounds.begin();
    while (it != recent_sounds.end()) {
        if (now - it->time > 1.0f)
            recent_sounds.erase(it);
        else
            it++;
    }
    bool skip = false;
    for (it = recent_sounds.begin(); it != recent_sounds.end(); it++) {
        if (it->source == noise.source)
            break;
    }
    if (it != recent_sounds.end()) {
        if (noise.priority <= it->priority)
            skip = true;
        else
            recent_sounds.erase(it);
    }
    if (!skip) {
        while (recent_sounds.size() >= 15) {
            NoiseInfo* oldest = recent_sounds.begin();
            for (it = oldest + 1; it != recent_sounds.end(); it++) {
                if (it->time < oldest->time)
                    oldest = it;
            }
            recent_sounds.erase(oldest);
        }
        CAIObject* list = CAIObject::GetGlobalListStart();
        CAIObject::PropagateSoundToAllObjects(info, list);
        recent_sounds.push_back(noise);
        s_rNextSoundPropagateTime = 1.0f + g_aigAIFilterGlobalObject.m_time;
    }
}

void CAIFilterGlobal::UpdateToGlobals() {
}

void CAIFilterGlobal::UpdateFromGlobals(float frames) {
    m_frame = GetFrameCount();
    m_time += 0.016666668f * frames;
}

CAIFilterGlobal::~CAIFilterGlobal() {
}
