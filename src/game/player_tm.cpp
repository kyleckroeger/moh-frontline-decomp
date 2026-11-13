// A fragment of player.cpp (0x800a0530): CPlayerObject's transform wrappers, from
// GetTMLocalToWorld to GetTMLocalToWorld, each forwarding to the object's transform
// (a CMatrix at +128). The file
// name is this project's; the original record is player.cpp and the functions
// around these are not reconstructed. CPlayerObject and CMatrix's methods are named by
// the mangled symbols; CPlayerObject is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
#include "game/CVector3.h"

#include "game/CMatrix.h"


#include "game/CPlayerObject.h"


void CPlayerObject::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}
