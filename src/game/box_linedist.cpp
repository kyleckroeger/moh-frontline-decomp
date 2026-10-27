// The start of box.cpp (0x800c0320..0x800c11d8): BoxLineSqrDistance, the
// squared distance from a line to a box, following the well-known
// line-to-box distance (Eberly's SqrDistance(Line3, Box3)): the line is taken
// into the box's frame (a matrix built from the basis rows and the centre,
// then inverted through the static scratch matrix), reflected so that the
// direction has no negative component, and handed to the case for its zero
// components; the box point is reflected back when asked. Components at or
// below 2^-122 (a pooled constant) count as zero. CaseNoZeros and Case000 are
// inlined here, as in the original. Case00 (the line meets the box face
// along i0 with the other two direction components zero: the point moves
// onto the face and the other two coordinates are clamped) and Case0 (one
// zero component, i2: the i0/i1 projection is solved in the plane and the
// i2 coordinate is clamped) follow as local functions, with the three
// extents read up front through a by-reference form of the extent switch
// (the value form gives other float registers), and Face (no zero
// component: the line meets the face of axis i0, then an edge or the
// region past it is chosen by the i1/i2 products), which CaseNoZeros calls
// with the point minus the extents precomputed. Parameter,
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
    void Extent(int i, float& extent) const {
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
static void Case0(int, int, int, CVector3&, const CVector3&, const CVolBox&, float*, float&);
static void Face(int, int, int, CVector3&, const CVector3&, const CVolBox&, const CVector3&, float*, float&);

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
    float fE0, fE1, fE2;
    rkBox.Extent(i0, fE0);
    rkBox.Extent(i1, fE1);
    rkBox.Extent(i2, fE2);
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

static void Case0(int i0, int i1, int i2, CVector3& rkPnt, const CVector3& rkDir, const CVolBox& rkBox, float* pfLParam, float& rfSqrDistance) {
    float fE0, fE1, fE2;
    rkBox.Extent(i0, fE0);
    rkBox.Extent(i1, fE1);
    rkBox.Extent(i2, fE2);
    float fPmE0 = rkPnt[i0] - fE0;
    float fPmE1 = rkPnt[i1] - fE1;
    float fProd0 = rkDir[i1] * fPmE0;
    float fProd1 = rkDir[i0] * fPmE1;
    float fDelta, fInvLSqr, fInv;

    if (fProd0 >= fProd1) {
        rkPnt[i0] = fE0;

        float fPpE1 = rkPnt[i1] + fE1;
        fDelta = fProd0 - rkDir[i0] * fPpE1;
        if (fDelta >= 0.0f) {
            fInvLSqr = 1.0f / (rkDir[i0] * rkDir[i0] + rkDir[i1] * rkDir[i1]);
            rfSqrDistance += fDelta * fDelta * fInvLSqr;
            if (pfLParam) {
                rkPnt[i1] = -fE1;
                *pfLParam = -(rkDir[i0] * fPmE0 + rkDir[i1] * fPpE1) * fInvLSqr;
            }
        } else {
            if (pfLParam) {
                fInv = 1.0f / rkDir[i0];
                rkPnt[i1] -= fProd0 * fInv;
                *pfLParam = -fPmE0 * fInv;
            }
        }
    } else {
        rkPnt[i1] = fE1;

        float fPpE0 = rkPnt[i0] + fE0;
        fDelta = fProd1 - rkDir[i1] * fPpE0;
        if (fDelta >= 0.0f) {
            fInvLSqr = 1.0f / (rkDir[i0] * rkDir[i0] + rkDir[i1] * rkDir[i1]);
            rfSqrDistance += fDelta * fDelta * fInvLSqr;
            if (pfLParam) {
                rkPnt[i0] = -fE0;
                *pfLParam = -(rkDir[i0] * fPpE0 + rkDir[i1] * fPmE1) * fInvLSqr;
            }
        } else {
            if (pfLParam) {
                fInv = 1.0f / rkDir[i1];
                rkPnt[i0] -= fProd1 * fInv;
                *pfLParam = -fPmE1 * fInv;
            }
        }
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

static void Face(int i0, int i1, int i2, CVector3& rkPnt, const CVector3& rkDir, const CVolBox& rkBox, const CVector3& rkPmE, float* pfLParam, float& rfSqrDistance) {
    float fE0, fE1, fE2;
    rkBox.Extent(i0, fE0);
    rkBox.Extent(i1, fE1);
    rkBox.Extent(i2, fE2);
    CVector3 kPpE;
    float fLSqr, fInv, fTmp, fParam, fT, fDelta;

    kPpE[i1] = rkPnt[i1] + fE1;
    kPpE[i2] = rkPnt[i2] + fE2;
    if (rkDir[i0] * kPpE[i1] >= rkDir[i1] * rkPmE[i0]) {
        if (rkDir[i0] * kPpE[i2] >= rkDir[i2] * rkPmE[i0]) {
            if (pfLParam) {
                rkPnt[i0] = fE0;
                fInv = 1.0f / rkDir[i0];
                rkPnt[i1] -= rkDir[i1] * rkPmE[i0] * fInv;
                rkPnt[i2] -= rkDir[i2] * rkPmE[i0] * fInv;
                *pfLParam = -rkPmE[i0] * fInv;
            }
        } else {
            fLSqr = rkDir[i0] * rkDir[i0] + rkDir[i2] * rkDir[i2];
            fTmp = fLSqr * kPpE[i1] - rkDir[i1] * (rkDir[i0] * rkPmE[i0] + rkDir[i2] * kPpE[i2]);
            if (fTmp <= 2.0f * fLSqr * fE1) {
                fT = fTmp / fLSqr;
                fLSqr += rkDir[i1] * rkDir[i1];
                fTmp = kPpE[i1] - fT;
                fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * fTmp + rkDir[i2] * kPpE[i2];
                fParam = -fDelta / fLSqr;
                rfSqrDistance += rkPmE[i0] * rkPmE[i0] + fTmp * fTmp + kPpE[i2] * kPpE[i2] + fDelta * fParam;
                if (pfLParam) {
                    *pfLParam = fParam;
                    rkPnt[i0] = fE0;
                    rkPnt[i1] = fT - fE1;
                    rkPnt[i2] = -fE2;
                }
            } else {
                fLSqr += rkDir[i1] * rkDir[i1];
                fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * rkPmE[i1] + rkDir[i2] * kPpE[i2];
                fParam = -fDelta / fLSqr;
                rfSqrDistance += rkPmE[i0] * rkPmE[i0] + rkPmE[i1] * rkPmE[i1] + kPpE[i2] * kPpE[i2] + fDelta * fParam;
                if (pfLParam) {
                    *pfLParam = fParam;
                    rkPnt[i0] = fE0;
                    rkPnt[i1] = fE1;
                    rkPnt[i2] = -fE2;
                }
            }
        }
    } else {
        if (rkDir[i0] * kPpE[i2] >= rkDir[i2] * rkPmE[i0]) {
            fLSqr = rkDir[i0] * rkDir[i0] + rkDir[i1] * rkDir[i1];
            fTmp = fLSqr * kPpE[i2] - rkDir[i2] * (rkDir[i0] * rkPmE[i0] + rkDir[i1] * kPpE[i1]);
            if (fTmp <= 2.0f * fLSqr * fE2) {
                fT = fTmp / fLSqr;
                fLSqr += rkDir[i2] * rkDir[i2];
                fTmp = kPpE[i2] - fT;
                fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * kPpE[i1] + rkDir[i2] * fTmp;
                fParam = -fDelta / fLSqr;
                rfSqrDistance += rkPmE[i0] * rkPmE[i0] + kPpE[i1] * kPpE[i1] + fTmp * fTmp + fDelta * fParam;
                if (pfLParam) {
                    *pfLParam = fParam;
                    rkPnt[i0] = fE0;
                    rkPnt[i1] = -fE1;
                    rkPnt[i2] = fT - fE2;
                }
            } else {
                fLSqr += rkDir[i2] * rkDir[i2];
                fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * kPpE[i1] + rkDir[i2] * rkPmE[i2];
                fParam = -fDelta / fLSqr;
                rfSqrDistance += rkPmE[i0] * rkPmE[i0] + kPpE[i1] * kPpE[i1] + rkPmE[i2] * rkPmE[i2] + fDelta * fParam;
                if (pfLParam) {
                    *pfLParam = fParam;
                    rkPnt[i0] = fE0;
                    rkPnt[i1] = -fE1;
                    rkPnt[i2] = fE2;
                }
            }
        } else {
            fLSqr = rkDir[i0] * rkDir[i0] + rkDir[i2] * rkDir[i2];
            fTmp = fLSqr * kPpE[i1] - rkDir[i1] * (rkDir[i0] * rkPmE[i0] + rkDir[i2] * kPpE[i2]);
            if (fTmp >= 0.0f) {
                if (fTmp <= 2.0f * fLSqr * fE1) {
                    fT = fTmp / fLSqr;
                    fLSqr += rkDir[i1] * rkDir[i1];
                    fTmp = kPpE[i1] - fT;
                    fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * fTmp + rkDir[i2] * kPpE[i2];
                    fParam = -fDelta / fLSqr;
                    rfSqrDistance += rkPmE[i0] * rkPmE[i0] + fTmp * fTmp + kPpE[i2] * kPpE[i2] + fDelta * fParam;
                    if (pfLParam) {
                        *pfLParam = fParam;
                        rkPnt[i0] = fE0;
                        rkPnt[i1] = fT - fE1;
                        rkPnt[i2] = -fE2;
                    }
                } else {
                    fLSqr += rkDir[i1] * rkDir[i1];
                    fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * rkPmE[i1] + rkDir[i2] * kPpE[i2];
                    fParam = -fDelta / fLSqr;
                    rfSqrDistance += rkPmE[i0] * rkPmE[i0] + rkPmE[i1] * rkPmE[i1] + kPpE[i2] * kPpE[i2] + fDelta * fParam;
                    if (pfLParam) {
                        *pfLParam = fParam;
                        rkPnt[i0] = fE0;
                        rkPnt[i1] = fE1;
                        rkPnt[i2] = -fE2;
                    }
                }
                return;
            }

            fLSqr = rkDir[i0] * rkDir[i0] + rkDir[i1] * rkDir[i1];
            fTmp = fLSqr * kPpE[i2] - rkDir[i2] * (rkDir[i0] * rkPmE[i0] + rkDir[i1] * kPpE[i1]);
            if (fTmp >= 0.0f) {
                if (fTmp <= 2.0f * fLSqr * fE2) {
                    fT = fTmp / fLSqr;
                    fLSqr += rkDir[i2] * rkDir[i2];
                    fTmp = kPpE[i2] - fT;
                    fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * kPpE[i1] + rkDir[i2] * fTmp;
                    fParam = -fDelta / fLSqr;
                    rfSqrDistance += rkPmE[i0] * rkPmE[i0] + kPpE[i1] * kPpE[i1] + fTmp * fTmp + fDelta * fParam;
                    if (pfLParam) {
                        *pfLParam = fParam;
                        rkPnt[i0] = fE0;
                        rkPnt[i1] = -fE1;
                        rkPnt[i2] = fT - fE2;
                    }
                } else {
                    fLSqr += rkDir[i2] * rkDir[i2];
                    fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * kPpE[i1] + rkDir[i2] * rkPmE[i2];
                    fParam = -fDelta / fLSqr;
                    rfSqrDistance += rkPmE[i0] * rkPmE[i0] + kPpE[i1] * kPpE[i1] + rkPmE[i2] * rkPmE[i2] + fDelta * fParam;
                    if (pfLParam) {
                        *pfLParam = fParam;
                        rkPnt[i0] = fE0;
                        rkPnt[i1] = -fE1;
                        rkPnt[i2] = fE2;
                    }
                }
                return;
            }

            fLSqr += rkDir[i2] * rkDir[i2];
            fDelta = rkDir[i0] * rkPmE[i0] + rkDir[i1] * kPpE[i1] + rkDir[i2] * kPpE[i2];
            fParam = -fDelta / fLSqr;
            rfSqrDistance += rkPmE[i0] * rkPmE[i0] + kPpE[i1] * kPpE[i1] + kPpE[i2] * kPpE[i2] + fDelta * fParam;
            if (pfLParam) {
                *pfLParam = fParam;
                rkPnt[i0] = fE0;
                rkPnt[i1] = -fE1;
                rkPnt[i2] = -fE2;
            }
        }
    }
}
