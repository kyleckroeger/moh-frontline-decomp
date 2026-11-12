// The end of player.cpp: CPlayerObject's weak AI-doodad accessor, identity
// casts, world linear velocity (the velocity at +600) and Destroy (deletes
// the object through its virtual destructor), the weak MathFunClamp<float>
// instance, and the static initialisation of the file's camera offsets
// (standing, single- and multiplayer crouching; the current offset starts as
// the standing one). The names come from the mangled symbols; the members,
// the vector constructor and the result types are inferred, and the offset
// constants are entries of the file's .sdata2 pool. The accessors are inline
// in the original (weak symbols), so they are defined __declspec(weak); the
// template is defined __declspec(weak) and instantiated explicitly.
// CPlayerObject declares two of its virtuals (defined elsewhere) so that its
// table stays elsewhere. The rest of the file is not part of this unit.
class CAIDoodad;

/* Inferred: vectors are copied as two doubles. */


#include "game/CVector3.h"


class CPlayerObject {
public:
    virtual void MarkForDestruction(int);
    virtual ~CPlayerObject();
    const CAIDoodad* GetAIDoodad() const;
    const CPlayerObject* AsPlayerObject() const;
    CPlayerObject* AsPlayerObject();
    CVector3 GetWorldLinearVelocity() const;
    void Destroy();

    unsigned char unknown004[32];
    unsigned char m_aiDoodad[4];
    unsigned char unknown028[560];
    CVector3 m_velocity;
};

__declspec(weak) const CAIDoodad* CPlayerObject::GetAIDoodad() const {
    return (const CAIDoodad*)m_aiDoodad;
}

__declspec(weak) const CPlayerObject* CPlayerObject::AsPlayerObject() const {
    return this;
}

__declspec(weak) CPlayerObject* CPlayerObject::AsPlayerObject() {
    return this;
}

__declspec(weak) CVector3 CPlayerObject::GetWorldLinearVelocity() const {
    return m_velocity;
}

__declspec(weak) void CPlayerObject::Destroy() {
    delete this;
}

template <class T> __declspec(weak) T MathFunClamp(T value, T low, T high) {
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

template float MathFunClamp<float>(float, float, float);

static CVector3 CAMERA_STAND_OFFSET(0.0f, 0.0f, 1.2f);
static CVector3 CAMERA_CROUCH_OFFSET_SP(0.0f, 0.0f, 0.1f);
static CVector3 CAMERA_CROUCH_OFFSET_MP(0.0f, 0.0f, 0.8f);
static CVector3 g_CameraOffset = CAMERA_STAND_OFFSET;
