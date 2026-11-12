// A fragment of matrix.cpp (0x8008fc70): CMatrix::Scale multiplies the first
// three columns of each of the four rows by the factor, or by a vector's
// coordinates. CMatrix and CVector3 are named by the mangled symbols; the
// element layout is inferred. The rest of the file is not part of this unit.
#include "game/CVector3.h"

#include "game/CMatrix.h"


void CMatrix::Scale(float scale) {
    for (int i = 0; i < 4; i++) {
        ((float(*)[4])&right)[i][0] *= scale;
        ((float(*)[4])&right)[i][1] *= scale;
        ((float(*)[4])&right)[i][2] *= scale;
    }
}

void CMatrix::Scale(CVector3 scale) {
    for (int i = 0; i < 4; i++) {
        ((float(*)[4])&right)[i][0] *= scale.x;
        ((float(*)[4])&right)[i][1] *= scale.y;
        ((float(*)[4])&right)[i][2] *= scale.z;
    }
}
