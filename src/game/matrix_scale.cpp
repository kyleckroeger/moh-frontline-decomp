// A fragment of matrix.cpp (0x8008fc70): CMatrix::Scale multiplies the first
// three columns of each of the four rows by the factor. CMatrix is named by
// the mangled symbol; the element layout is inferred. The rest of the file is
// not part of this unit.
class CMatrix {
public:
    void Scale(float);

    float m[4][4];
};

void CMatrix::Scale(float scale) {
    for (int i = 0; i < 4; i++) {
        m[i][0] *= scale;
        m[i][1] *= scale;
        m[i][2] *= scale;
    }
}
