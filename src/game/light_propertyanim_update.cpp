/* Inferred: an animated light's record (frame count, per-frame colours and intensities). */
struct MOH_animatedLight_Struct {
    unsigned char unknown00[112];
    unsigned char mode;
    unsigned char unknown71;
    short frameCount;
    unsigned char (*colors)[4];
    float* intensities;
};

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
};

class CLight : public ISceneNode {
public:
    virtual void MarkForDestruction(int);

    unsigned char unknown004[172];
    float m_color[4];
    unsigned char unknown0c0[16];
};

class CPropertyAnimLight : public CLight {
public:
    void BeginUpdate(float);

    MOH_animatedLight_Struct* m_data;
    short m_frame;
    char m_reverse : 1;
};

void CPropertyAnimLight::BeginUpdate(float delta) {
    int frames = (int)delta;
    float intensity;
    switch (m_data->mode) {
    case 0:
        m_frame += frames;
        if (m_frame >= m_data->frameCount) {
            m_frame = m_data->frameCount - 1;
            MarkForDestruction(0);
        }
        break;
    case 1:
        m_frame += frames;
        while (m_frame >= m_data->frameCount)
            m_frame = m_frame - m_data->frameCount;
        break;
    case 2:
        for (int i = frames; i != 0; i--) {
            if (m_reverse) {
                m_frame--;
                if (m_frame <= 0) {
                    m_frame = 1;
                    m_reverse = 0;
                }
            } else {
                m_frame++;
                if (m_frame >= m_data->frameCount) {
                    m_frame = m_data->frameCount - 2;
                    m_reverse = 1;
                }
            }
        }
        break;
    }
    unsigned char* color = m_data->colors[m_frame];
    float r;
    float g;
    float b;
    float a;
    r = color[0];
    g = color[1];
    b = color[2];
    intensity = m_data->intensities[m_frame];
    a = color[3];
    r *= intensity;
    g *= intensity;
    b *= intensity;
    a *= intensity;
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
    m_color[3] = a;
}
