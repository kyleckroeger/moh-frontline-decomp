// A fragment of player.cpp (0x8009d5f8): CPlayerObject::TransitionFOV starts
// moving the field of view (+1068) towards a new one, unless it is there
// already: the target is kept (+1076) and the rate (+1080) is the given speed,
// negated when the target is below the current field of view. The class and
// function are named by the symbols; the members are inferred from offsets.
// The rest of the file is in other units.
class CPlayerObject {
public:
    void TransitionFOV(float, float);

    unsigned char unknown000[1068];
    float m_fov;
    unsigned char unknown430[4];
    float m_targetFov;
    float m_fovRate;
};

void CPlayerObject::TransitionFOV(float target, float speed) {
    if (m_fov == target)
        return;
    m_targetFov = target;
    if (m_targetFov < m_fov)
        m_fovRate = -speed;
    else
        m_fovRate = speed;
}
