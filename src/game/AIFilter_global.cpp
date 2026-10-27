// A fragment of AIFilter.cpp (0x8005d760): CAIFilterGlobal's
// CreateSplinePathManager (a CAISplinePathManager for the given number of
// poly paths: each path's point offset relocated by the data base, its points
// byte-swapped in place and the spline generated, between the manager's
// generate-buffer allocation and release), the (static)
// PropagateSoundToAllObjects (the recent sounds older than 1 s are dropped; a
// sound from a source already heard at the same or a higher priority is
// ignored, a lower one replaced; the list is trimmed to 14 by dropping the
// oldest, the sound is passed to every AI object and recorded, and the next
// propagation time set 1 s ahead), the empty UpdateToGlobals,
// UpdateFromGlobals (the frame count, and the time advanced by the elapsed
// frames at 60 per second) and the destructor. The file name is this
// project's; the original record is AIFilter.cpp. The classes, functions and
// objects are named by the symbols; the members, the poly path record, the
// recent-sound record (source, priority, time; built first, compared and
// appended from memory) and the vector's inline accessors (push_back takes the
// record by value) are inferred (the global object's view has its real size,
// so it is not small-data addressed), as is the byte-swap helper (as in
// propdat_core.cpp); the constants are .sdata2 pool entries.
struct _aifilter_simple_v3 {
    float x;
    float y;
    float z;
};

/* Inferred: a poly path record (id, point count, point offset). */
struct BPDPolyPath {
    unsigned long id;
    int count;
    _aifilter_simple_v3* points;
};

class CAISplinePathManager {
public:
    CAISplinePathManager(unsigned long);
    void AllocateGenerateBuffers();
    void FreeGenerateBuffers();
    void GenerateSplinePath(unsigned long, unsigned long, unsigned long, const _aifilter_simple_v3*);

    unsigned char unknown00[24];
};

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
    void CreateSplinePathManager(void*, BPDPolyPath*, int);
    static void PropagateSoundToAllObjects(const CAIFilterSoundInfo&);
    void UpdateToGlobals();
    void UpdateFromGlobals(float);
    ~CAIFilterGlobal();

    static float s_rNextSoundPropagateTime;

    int m_frame;
    float m_time;
    unsigned char unknown08[4];
    CAISplinePathManager* m_splines;
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;
extern dwi::fast_vec<NoiseInfo> recent_sounds;
int GetFrameCount();

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void SwapPoint(_aifilter_simple_v3* point) {
    ChangeEndian(*reinterpret_cast<int*>(&point->x));
    ChangeEndian(*reinterpret_cast<int*>(&point->y));
    ChangeEndian(*reinterpret_cast<int*>(&point->z));
}

void CAIFilterGlobal::CreateSplinePathManager(void* base, BPDPolyPath* paths, int count) {
    if (count) {
        m_splines = new CAISplinePathManager(count);
        m_splines->AllocateGenerateBuffers();
        int i;
        BPDPolyPath* p = paths;
        for (i = 0; i < count; i++, p++) {
            p->points = (_aifilter_simple_v3*)((char*)base + (int)p->points);
            for (int j = 0; j < p->count; j++) {
                SwapPoint(&p->points[j]);
            }
            m_splines->GenerateSplinePath(i, p->id, p->count, p->points);
        }
        m_splines->FreeGenerateBuffers();
    }
}

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
