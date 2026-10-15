// A fragment of matrix.cpp (0x8008fc70): CMatrix::Scale multiplies the first
// three columns of each of the four rows by the factor, or by a vector's
// coordinates. CMatrix and CVector3 are named by the mangled symbols; the
// element layout is inferred. The rest of the file is not part of this unit.
class CVector3 {
public:
    float x;
    float y;
    float z;
};

class CMatrix {
public:
    void Scale(float);
    void Scale(CVector3);

    float m[4][4];
};

void CMatrix::Scale(float scale) {
    for (int i = 0; i < 4; i++) {
        m[i][0] *= scale;
        m[i][1] *= scale;
        m[i][2] *= scale;
    }
}

void CMatrix::Scale(CVector3 scale) {
    for (int i = 0; i < 4; i++) {
        m[i][0] *= scale.x;
        m[i][1] *= scale.y;
        m[i][2] *= scale.z;
    }
}
