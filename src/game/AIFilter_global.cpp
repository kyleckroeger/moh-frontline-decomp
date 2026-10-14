// A fragment of AIFilter.cpp (0x8005db14): CAIFilterGlobal's empty
// UpdateToGlobals, UpdateFromGlobals (the frame count, and the time advanced
// by the elapsed frames at 60 per second) and the destructor. The file name
// is this project's; the original record is AIFilter.cpp. The class and
// functions are named by the symbols; the members are inferred, and the
// constant is a .sdata2 pool entry. CreateSplinePathManager and
// PropagateSoundToAllObjects before them are not part of this unit.
class CAIFilterGlobal {
public:
    void UpdateToGlobals();
    void UpdateFromGlobals(float);
    ~CAIFilterGlobal();

    int m_frame;
    float m_time;
};

int GetFrameCount();

void CAIFilterGlobal::UpdateToGlobals() {
}

void CAIFilterGlobal::UpdateFromGlobals(float frames) {
    m_frame = GetFrameCount();
    m_time += 0.016666668f * frames;
}

CAIFilterGlobal::~CAIFilterGlobal() {
}
