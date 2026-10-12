// A fragment of box.cpp (0x800c11d8): BoxPointSqrDistance, the squared
// distance from a point to a box, following the well-known point-to-box
// distance (Eberly's SqrDistance(Vector3, Box3)): the point is taken into the
// box's frame (a matrix built from the basis rows and the centre, then
// inverted through the static scratch matrix), clamped to the half extents,
// and the clamped coordinates are returned when asked. Parameter and local
// names follow that formulation and are not original. The file name is this
// project's; the original record is box.cpp and the functions around this one
// are not part of this unit. CVector3, CMatrix, IVolume and CVolBox are named
// by the mangled symbols; the vector view (a float/double-pair union), the
// matrix's 16-byte alignment, its inline constructor (class set-up and
// identity) and inline inverse are inferred, as are the CVolBox members (see
// box_setters.cpp). The '- -extent' form keeps the original's operand order.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
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

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    CVector3 m_basis[3];
    CVector3 m_center;
};

float BoxPointSqrDistance(const CVector3& rkPoint, const CVolBox& rkBox, float* pfBParam0, float* pfBParam1, float* pfBParam2) {
    CMatrix kMat;
    kMat.SetRight(rkBox.m_basis[0]);
    kMat.SetFront(rkBox.m_basis[1]);
    kMat.SetUp(rkBox.m_basis[2]);
    kMat.SetPos(rkBox.m_center);
    kMat.Inverse();
    CVector3 kClosest;
    kMat.TransformPoint(kClosest, rkPoint);

    float fSqrDistance = 0.0f;
    float fDelta;

    if (kClosest[0] < -rkBox.m_halfWidth) {
        fDelta = kClosest[0] - -rkBox.m_halfWidth;
        fSqrDistance += fDelta * fDelta;
        kClosest[0] = -rkBox.m_halfWidth;
    } else if (kClosest[0] > rkBox.m_halfWidth) {
        fDelta = kClosest[0] - rkBox.m_halfWidth;
        fSqrDistance += fDelta * fDelta;
        kClosest[0] = rkBox.m_halfWidth;
    }

    if (kClosest[1] < -rkBox.m_halfDepth) {
        fDelta = kClosest[1] - -rkBox.m_halfDepth;
        fSqrDistance += fDelta * fDelta;
        kClosest[1] = -rkBox.m_halfDepth;
    } else if (kClosest[1] > rkBox.m_halfDepth) {
        fDelta = kClosest[1] - rkBox.m_halfDepth;
        fSqrDistance += fDelta * fDelta;
        kClosest[1] = rkBox.m_halfDepth;
    }

    if (kClosest[2] < -rkBox.m_halfHeight) {
        fDelta = kClosest[2] - -rkBox.m_halfHeight;
        fSqrDistance += fDelta * fDelta;
        kClosest[2] = -rkBox.m_halfHeight;
    } else if (kClosest[2] > rkBox.m_halfHeight) {
        fDelta = kClosest[2] - rkBox.m_halfHeight;
        fSqrDistance += fDelta * fDelta;
        kClosest[2] = rkBox.m_halfHeight;
    }

    if (pfBParam0) {
        *pfBParam0 = kClosest[0];
        *pfBParam1 = kClosest[1];
        *pfBParam2 = kClosest[2];
    }

    return fSqrDistance;
}
