// The start of box.cpp (0x800c0320..0x800c09f8): BoxLineSqrDistance, the
// squared distance from a line to a box, following the well-known
// line-to-box distance (Eberly's SqrDistance(Line3, Box3)): the line is taken
// into the box's frame (a matrix built from the basis rows and the centre,
// then inverted through the static scratch matrix), reflected so that the
// direction has no negative component, and handed to the case for its zero
// components; the box point is reflected back when asked. Components at or
// below 2^-122 (a pooled constant) count as zero. CaseNoZeros and Case000 are
// inlined here, as in the original. Case00 (the line meets the box face
// along i0 with the other two direction components zero: the point moves
// onto the face and the other two coordinates are clamped) follows as a
// local function, with the three extents read up front as in the original;
// Case0 and Face, the file's next local functions, are not part of this
// unit. Parameter,
// local and helper names follow that formulation and are not original. The
// file name is this project's; the original record is box.cpp.
// CVector3, CMatrix, CLine3, IVolume and CVolBox are named by the mangled
// symbols; the vector view (a float/double-pair union) and its inline
// constructors, the matrix's 16-byte alignment, inline constructor and
// inverse, the line layout (see line.cpp), the CVolBox members (see
// box_setters.cpp) and the inline extent switch (no default, as in the
// original's case functions) are inferred.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    float& operator[](int i) { return d.v[i]; }
    const float& operator[](int i) const { return d.v[i]; }

    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
        Ident();
    }
    void Ident();
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    CMatrix& operator=(const CMatrix&);
    void FastInverse(const CMatrix&);
    void Inverse() {
        s_TempMat = *this;
        FastInverse(s_TempMat);
    }
    void TransformPoint(CVector3&, CVector3) const;
    void TransformVector(CVector3&, CVector3) const;
    static void InitClass();
    static bool s_ClassInit;
    static CMatrix s_TempMat;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    float Extent(int i) const {
        float extent;
        switch (i) {
        case 0:
            extent = m_halfWidth;
            break;
        case 1:
            extent = m_halfDepth;
            break;
        case 2:
            extent = m_halfHeight;
            break;
        }
        return extent;
    }

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    CVector3 m_basis[3];
    CVector3 m_center;
};

class CLine3 {
public:
    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
};

static void Case00(int, int, int, CVector3&, const CVector3&, const CVolBox&, float*, float&);
void Case0(int, int, int, CVector3&, const CVector3&, const CVolBox&, float*, float&);
void Face(int, int, int, CVector3&, const CVector3&, const CVolBox&, const CVector3&, float*, float&);

static inline void CaseNoZeros(CVector3& rkPnt, const CVector3& rkDir, const CVolBox& rkBox, float* pfLParam, float& rfSqrDistance) {
    CVector3 kPmE(rkPnt[0] - rkBox.Extent(0), rkPnt[1] - rkBox.Extent(1), rkPnt[2] - rkBox.Extent(2));

    float fProdDxPy = rkDir[0] * kPmE[1];
    float fProdDyPx = rkDir[1] * kPmE[0];
    float fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

    if (fProdDyPx >= fProdDxPy) {
        fProdDzPx = rkDir[2] * kPmE[0];
        fProdDxPz = rkDir[0] * kPmE[2];
        if (fProdDzPx >= fProdDxPz)
            Face(0, 1, 2, rkPnt, rkDir, rkBox, kPmE, pfLParam, rfSqrDistance);
        else
            Face(2, 0, 1, rkPnt, rkDir, rkBox, kPmE, pfLParam, rfSqrDistance);
    } else {
        fProdDzPy = rkDir[2] * kPmE[1];
        fProdDyPz = rkDir[1] * kPmE[2];
        if (fProdDzPy >= fProdDyPz)
            Face(1, 2, 0, rkPnt, rkDir, rkBox, kPmE, pfLParam, rfSqrDistance);
        else
            Face(2, 0, 1, rkPnt, rkDir, rkBox, kPmE, pfLParam, rfSqrDistance);
    }
}

static inline void Case000(CVector3& rkPnt, const CVolBox& rkBox, float& rfSqrDistance) {
    float fDelta;

    if (rkPnt[0] < -rkBox.m_halfWidth) {
        fDelta = rkPnt[0] - -rkBox.m_halfWidth;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[0] = -rkBox.m_halfWidth;
    } else if (rkPnt[0] > rkBox.m_halfWidth) {
        fDelta = rkPnt[0] - rkBox.m_halfWidth;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[0] = rkBox.m_halfWidth;
    }

    if (rkPnt[1] < -rkBox.m_halfDepth) {
        fDelta = rkPnt[1] - -rkBox.m_halfDepth;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[1] = -rkBox.m_halfDepth;
    } else if (rkPnt[1] > rkBox.m_halfDepth) {
        fDelta = rkPnt[1] - rkBox.m_halfDepth;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[1] = rkBox.m_halfDepth;
    }

    if (rkPnt[2] < -rkBox.m_halfHeight) {
        fDelta = rkPnt[2] - -rkBox.m_halfHeight;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[2] = -rkBox.m_halfHeight;
    } else if (rkPnt[2] > rkBox.m_halfHeight) {
        fDelta = rkPnt[2] - rkBox.m_halfHeight;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[2] = rkBox.m_halfHeight;
    }
}

float BoxLineSqrDistance(const CLine3& rkLine, const CVolBox& rkBox, float* pfLParam, float* pfBParam0, float* pfBParam1, float* pfBParam2) {
    CMatrix kMat;
    kMat.SetRight(rkBox.m_basis[0]);
    kMat.SetFront(rkBox.m_basis[1]);
    kMat.SetUp(rkBox.m_basis[2]);
    kMat.SetPos(rkBox.m_center);
    kMat.Inverse();
    CVector3 kPnt;
    kMat.TransformPoint(kPnt, rkLine.m_start);
    CVector3 kDir;
    kMat.TransformVector(kDir, rkLine.m_dir);

    bool bReflect[3];
    int i;
    for (i = 0; i < 3; i++) {
        bReflect[i] = kDir[i] < 0.0f;
        if (bReflect[i]) {
            kPnt[i] = -kPnt[i];
            kDir[i] = -kDir[i];
        }
    }

    float fSqrDistance = 0.0f;

    if (kDir[0] > 1.880790961e-37f) {
        if (kDir[1] > 1.880790961e-37f) {
            if (kDir[2] > 1.880790961e-37f)
                CaseNoZeros(kPnt, kDir, rkBox, pfLParam, fSqrDistance);
            else
                Case0(0, 1, 2, kPnt, kDir, rkBox, pfLParam, fSqrDistance);
        } else {
            if (kDir[2] > 1.880790961e-37f)
                Case0(0, 2, 1, kPnt, kDir, rkBox, pfLParam, fSqrDistance);
            else
                Case00(0, 1, 2, kPnt, kDir, rkBox, pfLParam, fSqrDistance);
        }
    } else {
        if (kDir[1] > 1.880790961e-37f) {
            if (kDir[2] > 1.880790961e-37f)
                Case0(1, 2, 0, kPnt, kDir, rkBox, pfLParam, fSqrDistance);
            else
                Case00(1, 0, 2, kPnt, kDir, rkBox, pfLParam, fSqrDistance);
        } else {
            if (kDir[2] > 1.880790961e-37f) {
                Case00(2, 0, 1, kPnt, kDir, rkBox, pfLParam, fSqrDistance);
            } else {
                Case000(kPnt, rkBox, fSqrDistance);
                if (pfLParam)
                    *pfLParam = 0.0f;
            }
        }
    }

    if (pfLParam) {
        for (i = 0; i < 3; i++) {
            if (bReflect[i])
                kPnt[i] = -kPnt[i];
        }
        *pfBParam0 = kPnt[0];
        *pfBParam1 = kPnt[1];
        *pfBParam2 = kPnt[2];
    }

    return fSqrDistance;
}

static void Case00(int i0, int i1, int i2, CVector3& rkPnt, const CVector3& rkDir, const CVolBox& rkBox, float* pfLParam, float& rfSqrDistance) {
    float fE0 = rkBox.Extent(i0);
    float fE1 = rkBox.Extent(i1);
    float fE2 = rkBox.Extent(i2);
    float fDelta;

    if (pfLParam)
        *pfLParam = (fE0 - rkPnt[i0]) / rkDir[i0];

    rkPnt[i0] = fE0;

    if (rkPnt[i1] < -fE1) {
        fDelta = rkPnt[i1] + fE1;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[i1] = -fE1;
    } else if (rkPnt[i1] > fE1) {
        fDelta = rkPnt[i1] - fE1;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[i1] = fE1;
    }

    if (rkPnt[i2] < -fE2) {
        fDelta = rkPnt[i2] + fE2;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[i2] = -fE2;
    } else if (rkPnt[i2] > fE2) {
        fDelta = rkPnt[i2] - fE2;
        rfSqrDistance += fDelta * fDelta;
        rkPnt[i2] = fE2;
    }
}
