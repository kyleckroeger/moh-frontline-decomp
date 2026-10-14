// A fragment of lightvolman.cpp (0x80078df8): CLightVolumeManager, which
// keeps up to four active light volumes ordered by priority (the first one
// lights the scene; the list is full when the fourth slot is used). When the
// first volume changes (RemoveVolume, AddVolume), the manager fades from the
// previous one over 10 time units (a fade already under way back to the new
// first volume is reversed); Update runs the fade down; Reset, the destructor
// and the constructor clear it. GetVolume before it (blending both volumes'
// lights into the file's g_BlendLights while fading) is not part of this unit.
// CLightVolumeManager, BPDLightVolume and BPDLight are named by the symbols;
// the members and the layouts are inferred.
struct BPDLight {
    int type;
    unsigned char unknown04[12];
    float direction[3];
    float color[3];
    float intensity;
    unsigned char unknown2c[4];
};

struct BPDLightVolume {
    unsigned char unknown00[32];
    int count;
    BPDLight* lights;
    int priority;
    unsigned char unknown2c[4];
};

class CLight {
public:
    static BPDLightVolume* GetDefaultLightVolume();
};

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);
    void Update(float);
    void Reset();
    ~CLightVolumeManager();
    CLightVolumeManager();

    BPDLightVolume* m_volumes[4];
    BPDLightVolume* m_previous;
    float m_fade;
};

void CLightVolumeManager::RemoveVolume(BPDLightVolume* volume) {
    BPDLightVolume* first = m_volumes[0];
    for (int i = 0; i < 4; i++) {
        if (m_volumes[i] == volume) {
            m_volumes[i] = 0;
            for (int j = i + 1; j < 4; j++) {
                if (!m_volumes[j])
                    break;
                m_volumes[j - 1] = m_volumes[j];
                m_volumes[j] = 0;
            }
            if (first != m_volumes[0]) {
                if (m_previous && m_previous == m_volumes[0])
                    m_fade = 10.0f - m_fade;
                else
                    m_fade = 10.0f;
                m_previous = first ? first : CLight::GetDefaultLightVolume();
            }
            return;
        }
    }
}

void CLightVolumeManager::AddVolume(BPDLightVolume* volume) {
    if (m_volumes[3])
        return;
    BPDLightVolume* first = m_volumes[0];
    for (int i = 0; i < 4; i++) {
        if (!m_volumes[i]) {
            m_volumes[i] = volume;
            break;
        }
        if (m_volumes[i]->priority < volume->priority) {
            BPDLightVolume* moved = m_volumes[i];
            for (int j = i + 1; j < 4; j++) {
                BPDLightVolume* next = m_volumes[j];
                m_volumes[j] = moved;
                moved = next;
            }
            m_volumes[i] = volume;
            break;
        }
    }
    if (first != m_volumes[0]) {
        if (m_previous && m_previous == m_volumes[0])
            m_fade = 10.0f - m_fade;
        else
            m_fade = 10.0f;
        m_previous = first ? first : CLight::GetDefaultLightVolume();
    }
}

void CLightVolumeManager::Update(float time) {
    if (m_previous) {
        m_fade -= time;
        if (m_fade <= 0.0f)
            m_previous = 0;
    }
}

void CLightVolumeManager::Reset() {
    m_volumes[0] = 0;
    m_volumes[1] = 0;
    m_volumes[2] = 0;
    m_volumes[3] = 0;
    m_previous = 0;
    m_fade = 0.0f;
}

CLightVolumeManager::~CLightVolumeManager() {
    Reset();
}

CLightVolumeManager::CLightVolumeManager() {
    Reset();
}
