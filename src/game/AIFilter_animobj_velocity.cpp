// A fragment of AIFilter.cpp (0x8005b480): the weak
// CAnimObject::GetWorldLinearVelocity, which returns the object's velocity
// vector (at +9160). CAnimObject and CVector3 are named by the mangled
// symbols; the member offset is from the code and the vector is the
// double-pair view (its copies move doubleword pairs). The function is a weak
// copy of a header inline emitted in this file, so it is defined
// __declspec(weak). The rest of the file is not part of this unit.


#include "game/CVector3.h"


class CAnimObject {
public:
    CVector3 GetWorldLinearVelocity() const;

    unsigned char unknown0000[9160];
    CVector3 m_velocity;
};

__declspec(weak) CVector3 CAnimObject::GetWorldLinearVelocity() const {
    return m_velocity;
}
