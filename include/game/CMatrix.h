// CMatrix: the game's 4x4 matrix, shared by every file that uses it (PortReadiness step 3.3).
//
// Layout: four CVector3 rows, 64 bytes, 16-byte aligned (91 of the 148 matched views say
// so, and 13 matched units lose their match without it: the original realigns the stack
// for local matrices; s_TempMat is 16-aligned). player_ctor.cpp's CPlayerObject view puts
// matrices at 8-aligned offsets (m_matrix518...) and loses its match with it: that view's
// layout, not this alignment, is the open question (research/notes/shared-headers.md). The rows
// are right (+0), front (+0x10), up (+0x20) and pos (+0x30), named after the original's
// SetRight/SetFront/SetUp/SetPos (classes.tsv: every matched view that names its rows puts
// them at these offsets). The fourth float of each row is the CVector3's w.
//
// Matched views also spell the storage float m[4][4], float m[16], or the rows forward /
// position / m_right / m_front / m_up / m_pos; MWCC 1.3.2 can't reach an anonymous
// struct's members, so one spelling has to win and moving a file to this header rewrites
// the others (research/tools/try_header.py). A fragment whose own view differs keeps it
// behind a guard until it is re-matched with this one; see docs/PortReadiness.md.
#ifndef GAME_CMATRIX_H
#define GAME_CMATRIX_H

#include "game/CVector3.h"

class CVector4;

class CMatrix {
public:
    // The original has a weak __defctor__7CMatrixFv (0x80095b40): the default constructor
    // takes a defaulted argument, and with it the body is only the class-init check.
    // Inferred: the argument asks for the identity (a non-zero argument calls Ident() in
    // the matched constructors that pass one); its type is not in any mangled name (int,
    // as in 13 of the 16 matched views that declare one).
    CMatrix(int identity = 0) {
        if (!s_ClassInit)
            InitClass();
        if (identity)
            Ident();
    }
    // Inferred: the original keeps no copy constructor, and matched code copies a matrix
    // through a call to the out-of-line operator= (matching-tricks.md, "lines").
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);                                  // original, out of line

    // Out-of-line members of the original (matrix.cpp, 0x8008ebf0..0x80090bb0), in order.
    void EndianSwap();
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;
    void TibToMOHFL();
    void ToEulerXZY(float&, float&, float&) const;
    void ToEulerXYZ(float&, float&, float&) const;
    void Ident();
    void Orthonormalize();
    void Transpose();
    void Inverse(const CMatrix&);
    void FastInverse(const CMatrix&);
    CVector3 TransformToLocal(CVector3) const;
    void TransformVector(CVector3&, CVector3) const;
    void TransformPoint(CVector3&, CVector3) const;
    void Transform(CVector4&, CVector3) const;
    void Orthographic(float, float, float, float);
    void Perspective(float, float, float, float);
    void Multiply(const CMatrix&, const CMatrix&);
    void Scale(float);
    void Scale(CVector3);
    void RotateZ(long);
    void RotateY(long);
    void RotateX(long);
    void RotateZ(float);
    void RotateY(float);
    void RotateX(float);
    void Rotate(CVector3, float);
    void Translate(CVector3);
    void PreTranslate(CVector3);
    void SetPos(CVector3);
    void SetUp(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);
    void BuildTrans(CVector3);
    void BuildRotZ(float);
    void BuildRotX(float);
    void BuildRot(CVector3, float);
    void BuildScale(CVector3);
    void BuildScale(float);
    static void InitClass();

    // original: weak copy at 0x8009b91c, Multiply(*this, m).
    void PostMultiply(const CMatrix& m) { Multiply(*this, m); }

    // Inferred: the original inlines these everywhere (no weak copies). By-value returns,
    // the most common form among the matched views.
    CVector3 GetRight() const { return right; }
    CVector3 GetFront() const { return front; }
    CVector3 GetUp() const { return up; }
    CVector3 GetPos() const { return pos; }
    CVector3 GetRow(int i) const { return (&right)[i]; }
    // Inferred: inverse in place through the class's scratch matrix (box_pointdist.cpp).
    void Inverse() {
        s_TempMat = *this;
        FastInverse(s_TempMat);
    }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;

    static bool s_ClassInit;                                             // original (.sbss)
    static CMatrix s_TempMat;                                            // original (.bss)
} __attribute__((aligned(16)));

#endif
