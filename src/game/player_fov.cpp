// A fragment of player.cpp (0x8009d5f8): CPlayerObject::TransitionFOV starts
// moving the field of view (+1068) towards a new one, unless it is there
// already: the target is kept (+1076) and the rate (+1080) is the given speed,
// negated when the target is below the current field of view.
// UpdateDeathSequence runs the death timer (+952, out of 180): outside
// multiplayer it turns the view by cosine-shaped yaw and pitch steps, in
// multiplayer it blends the yaw and pitch from their starting values (+4796,
// +4800) towards the killer's (+1992) look direction; when the timer runs out
// it triggers script event 120, otherwise (single player) it fades the master
// volume. The class and functions are named by the symbols; the members are
// inferred from offsets, the virtual functions before TriggerScriptEvent are
// placeholders in ISceneNode's order (vtable not emitted here), and the Lerp
// helper is an inferred inline. The rest of the file is in other units.
extern "C" {
double cos(double);
double atan2(double, double);
}



#include "game/CVector3.h"

#include "game/CMatrix.h"


class CSoldierObject {
public:
    void GetLookMatrix(CMatrix&);
};

extern bool g_bInMultiplayerMode;
void SoundSetMasterVolume(int);

#include "game/CPlayerObject.h"


void CPlayerObject::TransitionFOV(float target, float speed) {
    if (m_fov == target)
        return;
    m_targetFov = target;
    if (m_targetFov < m_fov)
        m_fovSpeed = -speed;
    else
        m_fovSpeed = speed;
}

static inline float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void CPlayerObject::UpdateDeathSequence(float dt) {
    float time = m_deathTime / 180.0f;
    float left = 1.0f - time;
    float yaw = dt * (0.05f * (float)cos(3.1415927f * (2.5f * left)));
    float pitch = dt * ((0.035f * (float)cos(3.1415927f / (0.5f + left))) / (2.0f * left + 1.0f));
    m_deathTime -= dt;
    if (!g_bInMultiplayerMode) {
        m_yaw += yaw;
        m_pitch -= pitch;
    } else {
        CMatrix look;
        m_soldier->GetLookMatrix(look);
        CVector3 front = look.front;
        float angle = atan2(front.x, front.y);
        m_yaw = Lerp(m_startYaw, angle, left);
        m_pitch = Lerp(m_startPitch, -3.1415927f * front.z, left);
    }
    if (m_deathTime <= 0.0f) {
        m_deathTime = 0.0f;
        TriggerScriptEvent(120, 0, false);
    } else if (!g_bInMultiplayerMode) {
        SoundSetMasterVolume((int)(127.0f * time));
    }
}
