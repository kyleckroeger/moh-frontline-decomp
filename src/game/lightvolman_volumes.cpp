// A fragment of lightvolman.cpp (0x80078a3c): CLightVolumeManager, which
// keeps up to four active light volumes ordered by priority (the first one
// lights the scene; the list is full when the fourth slot is used). GetVolume
// returns the first volume (or the default one) unless a fade is under way:
// then the file's four g_BlendLights are cleared and filled from the previous
// volume scaled by fade/10 (the ambient light in slot 0, the others in order),
// the current volume's lights are added scaled by 1 - fade/10 (a light
// already set is accumulated, its direction blended toward the new one and
// renormalised) and g_BlendLightVolume is returned. When the first volume
// changes (RemoveVolume, AddVolume), the manager fades from the previous one
// over 10 time units (a fade already under way back to the new first volume
// is reversed); Update runs the fade down; Reset, the destructor and the
// constructor clear it. CLightVolumeManager, BPDLightVolume, BPDLight and the
// blend globals are named by the symbols; the members, the layouts and the
// direction-blend helper are inferred.
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

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y ;
    }
    return x;
}

extern BPDLight g_BlendLights[4];
extern BPDLightVolume g_BlendLightVolume;

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

static inline void BlendDirection(float* dst, const float* src, float t) {
    float ox;
    float z;
    float y;
    float x;
    float oy;
    float oz;
    x = src[0] - (ox = dst[0]);
    y = src[1] - (oy = dst[1]);
    z = src[2] - (oz = dst[2]);
    x *= t;
    y *= t;
    z *= t;
    x += ox;
    y += oy;
    z += oz;
    float length = sqrtf(x * x + y * y + z * z);
    if (length != 0.0f) {
        float inv = 1.0f / length;
        x *= inv;
        y *= inv;
        z *= inv;
    }
    dst[0] = x;
    dst[1] = y;
    dst[2] = z;
}

BPDLightVolume* CLightVolumeManager::GetVolume() {
    if (!m_previous) {
        if (m_volumes[0])
            return m_volumes[0];
        return CLight::GetDefaultLightVolume();
    }
    for (int k = 0; k < 4; k++) {
        g_BlendLights[k].intensity = 0.0f;
        g_BlendLights[k].color[0] = 0.0f;
        g_BlendLights[k].color[1] = 0.0f;
        g_BlendLights[k].color[2] = 0.0f;
        g_BlendLights[k].direction[0] = 0.0f;
        g_BlendLights[k].direction[1] = 0.0f;
        g_BlendLights[k].direction[2] = 0.0f;
    }
    BPDLight* blend = &g_BlendLights[1];
    BPDLight* light = m_previous->lights;
    int i;
    for (i = 0; i < m_previous->count; i++, light++) {
        float scale = light->intensity * m_fade / 10.0f;
        if (light->type == 1) {
            g_BlendLights[0].intensity = 1.0f;
            g_BlendLights[0].color[0] = light->color[0] * scale;
            g_BlendLights[0].color[1] = light->color[1] * scale;
            g_BlendLights[0].color[2] = light->color[2] * scale;
        } else {
            blend->intensity = 1.0f;
            blend->color[0] = light->color[0] * scale;
            blend->color[1] = light->color[1] * scale;
            blend->color[2] = light->color[2] * scale;
            blend->direction[0] = light->direction[0];
            blend->direction[1] = light->direction[1];
            blend->direction[2] = light->direction[2];
            blend++;
        }
    }
    BPDLightVolume* volume = m_volumes[0];
    if (!volume)
        volume = CLight::GetDefaultLightVolume();
    blend = &g_BlendLights[1];
    light = volume->lights;
    for (i = 0; i < volume->count; i++, light++) {
        float scale = light->intensity * (1.0f - m_fade / 10.0f);
        if (light->type == 1) {
            if (g_BlendLights[0].intensity) {
                g_BlendLights[0].color[0] = light->color[0] * scale + g_BlendLights[0].color[0];
                g_BlendLights[0].color[1] = light->color[1] * scale + g_BlendLights[0].color[1];
                g_BlendLights[0].color[2] = light->color[2] * scale + g_BlendLights[0].color[2];
            } else {
                g_BlendLights[0].intensity = 1.0f;
                g_BlendLights[0].color[0] = light->color[0] * scale;
                g_BlendLights[0].color[1] = light->color[1] * scale;
                g_BlendLights[0].color[2] = light->color[2] * scale;
            }
        } else {
            if (blend->intensity) {
                blend->color[0] = light->color[0] * scale + blend->color[0];
                blend->color[1] = light->color[1] * scale + blend->color[1];
                blend->color[2] = light->color[2] * scale + blend->color[2];
                BlendDirection(blend->direction, light->direction, scale);
            } else {
                blend->intensity = 1.0f;
                blend->color[0] = light->color[0] * scale;
                blend->color[1] = light->color[1] * scale;
                blend->color[2] = light->color[2] * scale;
                blend->direction[0] = light->direction[0];
                blend->direction[1] = light->direction[1];
                blend->direction[2] = light->direction[2];
            }
            blend++;
        }
    }
    return &g_BlendLightVolume;
}

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
