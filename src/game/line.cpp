// The start of line.cpp (0x800922f4..0x80092c54). CLine3::IsDegenerate:
// whether the squared length of the direction (computed once and cached with
// its flag) is below 1e-12 (an entry of the file's .sdata2 pool). CLine3 is
// named by the mangled symbols; the layout (start, end and direction vectors,
// then the cached values and flags) is the view used in cdbobject_line.cpp,
// and the caching inline is inferred.
// CLine3::GetSquaredDistanceBetweenSegments follows the well-known
// segment-to-segment squared distance (Eberly's SqrDistance); local names
// follow that formulation and are not original. The bool result is inferred:
// the original leaves the parallel test in r3 until the return, and callers
// ignore it. CLine3::GetProjection projects a point onto the line (t along
// the direction) and falls back to the start point with t = 0.5 for a
// degenerate line; the by-value point follows the mangled signature.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    bool IsDegenerate() const;
    bool GetSquaredDistanceBetweenSegments(const CLine3&, float&, float&, float*) const;
    void GetProjection(CVector3, CVector3&, float&) const;
    float GetLengthSquared() const {
        if (!m_flag39) {
            m_value34 = m_dir.d.v[0] * m_dir.d.v[0] + m_dir.d.v[1] * m_dir.d.v[1] + m_dir.d.v[2] * m_dir.d.v[2];
            m_flag39 = 1;
        }
        return m_value34;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    mutable float m_value30;
    mutable float m_value34;
    mutable unsigned char m_flag38;
    mutable unsigned char m_flag39;
} __attribute__((aligned(16)));

inline double fabs(double x) {
    return __fabs(x);
}

inline float Dot(const CVector3& a, const CVector3& b) {
    return a.d.v[0] * b.d.v[0] + a.d.v[1] * b.d.v[1] + a.d.v[2] * b.d.v[2];
}

bool CLine3::IsDegenerate() const {
    return GetLengthSquared() < 1e-12f;
}

bool CLine3::GetSquaredDistanceBetweenSegments(const CLine3& other, float& fS, float& fT, float* pfSqrDist) const {
    CVector3 kDiff;
    kDiff.d.v[0] = m_start.d.v[0] - other.m_start.d.v[0];
    kDiff.d.v[1] = m_start.d.v[1] - other.m_start.d.v[1];
    kDiff.d.v[2] = m_start.d.v[2] - other.m_start.d.v[2];
    float fA00 = GetLengthSquared();
    float fA01 = -Dot(m_dir, other.m_dir);
    float fA11 = other.GetLengthSquared();
    float fB0 = Dot(m_dir, kDiff);
    float fB1 = -Dot(other.m_dir, kDiff);
    float fC = Dot(kDiff, kDiff);
    float fDet = fabs(fA00 * fA11 - fA01 * fA01);
    float fSqrDist;
    bool parallel;
    float fTmp;

    parallel = fDet < 1e-5f;
    if (parallel == false) {
        fS = fA01 * fB1 - fA11 * fB0;
        fT = fA01 * fB0 - fA00 * fB1;

        if (fS >= 0.0f) {
            if (fS <= fDet) {
                if (fT >= 0.0f) {
                    if (fT <= fDet) {
                        float fInvDet = 1.0f / fDet;
                        fS *= fInvDet;
                        fT *= fInvDet;
                        fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT * (fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
                    } else {
                        fT = 1.0f;
                        fTmp = fA01 + fB0;
                        if (fTmp >= 0.0f) {
                            fS = 0.0f;
                            fSqrDist = fA11 + 2.0f * fB1 + fC;
                        } else if (-fTmp >= fA00) {
                            fS = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB1 + fTmp);
                        } else {
                            fS = -fTmp / fA00;
                            fSqrDist = fTmp * fS + fA11 + 2.0f * fB1 + fC;
                        }
                    }
                } else {
                    fT = 0.0f;
                    if (fB0 >= 0.0f) {
                        fS = 0.0f;
                        fSqrDist = fC;
                    } else if (-fB0 >= fA00) {
                        fS = 1.0f;
                        fSqrDist = fA00 + 2.0f * fB0 + fC;
                    } else {
                        fS = -fB0 / fA00;
                        fSqrDist = fB0 * fS + fC;
                    }
                }
            } else {
                if (fT >= 0.0) {
                    if (fT <= fDet) {
                        fS = 1.0f;
                        fTmp = fA01 + fB1;
                        if (fTmp >= 0.0f) {
                            fT = 0.0f;
                            fSqrDist = fA00 + 2.0f * fB0 + fC;
                        } else if (-fTmp >= fA11) {
                            fT = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB0 + fTmp);
                        } else {
                            fT = -fTmp / fA11;
                            fSqrDist = fTmp * fT + fA00 + 2.0f * fB0 + fC;
                        }
                    } else {
                        fTmp = fA01 + fB0;
                        if (-fTmp <= fA00) {
                            fT = 1.0f;
                            if (fTmp >= 0.0f) {
                                fS = 0.0f;
                                fSqrDist = fA11 + 2.0f * fB1 + fC;
                            } else {
                                fS = -fTmp / fA00;
                                fSqrDist = fTmp * fS + fA11 + 2.0f * fB1 + fC;
                            }
                        } else {
                            fS = 1.0f;
                            fTmp = fA01 + fB1;
                            if (fTmp >= 0.0f) {
                                fT = 0.0f;
                                fSqrDist = fA00 + 2.0f * fB0 + fC;
                            } else if (-fTmp >= fA11) {
                                fT = 1.0f;
                                fSqrDist = fA00 + fA11 + fC + 2.0f * (fB0 + fTmp);
                            } else {
                                fT = -fTmp / fA11;
                                fSqrDist = fTmp * fT + fA00 + 2.0f * fB0 + fC;
                            }
                        }
                    }
                } else {
                    if (-fB0 < fA00) {
                        fT = 0.0f;
                        if (fB0 >= 0.0f) {
                            fS = 0.0f;
                            fSqrDist = fC;
                        } else {
                            fS = -fB0 / fA00;
                            fSqrDist = fB0 * fS + fC;
                        }
                    } else {
                        fS = 1.0f;
                        fTmp = fA01 + fB1;
                        if (fTmp >= 0.0f) {
                            fT = 0.0f;
                            fSqrDist = fA00 + 2.0f * fB0 + fC;
                        } else if (-fTmp >= fA11) {
                            fT = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB0 + fTmp);
                        } else {
                            fT = -fTmp / fA11;
                            fSqrDist = fTmp * fT + fA00 + 2.0f * fB0 + fC;
                        }
                    }
                }
            }
        } else {
            if (fT >= 0.0f) {
                if (fT <= fDet) {
                    fS = 0.0f;
                    if (fB1 >= 0.0f) {
                        fT = 0.0f;
                        fSqrDist = fC;
                    } else if (-fB1 >= fA11) {
                        fT = 1.0f;
                        fSqrDist = fA11 + 2.0f * fB1 + fC;
                    } else {
                        fT = -fB1 / fA11;
                        fSqrDist = fB1 * fT + fC;
                    }
                } else {
                    fTmp = fA01 + fB0;
                    if (fTmp < 0.0f) {
                        fT = 1.0f;
                        if (-fTmp >= fA00) {
                            fS = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB1 + fTmp);
                        } else {
                            fS = -fTmp / fA00;
                            fSqrDist = fTmp * fS + fA11 + 2.0f * fB1 + fC;
                        }
                    } else {
                        fS = 0.0f;
                        if (fB1 >= 0.0f) {
                            fT = 0.0f;
                            fSqrDist = fC;
                        } else if (-fB1 >= fA11) {
                            fT = 1.0f;
                            fSqrDist = fA11 + 2.0f * fB1 + fC;
                        } else {
                            fT = -fB1 / fA11;
                            fSqrDist = fB1 * fT + fC;
                        }
                    }
                }
            } else {
                if (fB0 < 0.0f) {
                    fT = 0.0f;
                    if (-fB0 >= fA00) {
                        fS = 1.0f;
                        fSqrDist = fA00 + 2.0f * fB0 + fC;
                    } else {
                        fS = -fB0 / fA00;
                        fSqrDist = fB0 * fS + fC;
                    }
                } else {
                    fS = 0.0f;
                    if (fB1 >= 0.0f) {
                        fT = 0.0f;
                        fSqrDist = fC;
                    } else if (-fB1 >= fA11) {
                        fT = 1.0f;
                        fSqrDist = fA11 + 2.0f * fB1 + fC;
                    } else {
                        fT = -fB1 / fA11;
                        fSqrDist = fB1 * fT + fC;
                    }
                }
            }
        }
    } else {
        if (fA01 > 0.0f) {
            if (fB0 >= 0.0f) {
                fS = 0.0f;
                fT = 0.0f;
                fSqrDist = fC;
            } else if (-fB0 <= fA00) {
                fS = -fB0 / fA00;
                fT = 0.0f;
                fSqrDist = fB0 * fS + fC;
            } else {
                fS = 1.0f;
                fTmp = fA00 + fB0;
                if (-fTmp >= fA01) {
                    fT = 1.0f;
                    fSqrDist = fA00 + fA11 + fC + 2.0f * (fA01 + fB0 + fB1);
                } else {
                    fT = -fTmp / fA01;
                    fSqrDist = fA00 + 2.0f * fB0 + fC + fT * (fA11 * fT + 2.0f * (fA01 + fB1));
                }
            }
        } else {
            if (-fB0 >= fA00) {
                fS = 1.0f;
                fT = 0.0f;
                fSqrDist = fA00 + 2.0f * fB0 + fC;
            } else if (fB0 <= 0.0f) {
                fS = -fB0 / fA00;
                fT = 0.0f;
                fSqrDist = fB0 * fS + fC;
            } else {
                fS = 0.0f;
                if (fB0 >= -fA01) {
                    fT = 1.0f;
                    fSqrDist = fA11 + 2.0f * fB1 + fC;
                } else {
                    fT = -fB0 / fA01;
                    fSqrDist = fC + fT * (2.0f * fB1 + fA11 * fT);
                }
            }
        }
    }
    if (pfSqrDist)
        *pfSqrDist = fabs(fSqrDist);
    return parallel;
}


void CLine3::GetProjection(CVector3 point, CVector3& projection, float& t) const {
    if (!IsDegenerate()) {
        CVector3 diff;
        diff.d.v[0] = point.d.v[0] - m_start.d.v[0];
        diff.d.v[1] = point.d.v[1] - m_start.d.v[1];
        diff.d.v[2] = point.d.v[2] - m_start.d.v[2];
        t = Dot(m_dir, diff) / GetLengthSquared();
        float s = t;
        projection.d.v[0] = s * m_dir.d.v[0];
        projection.d.v[1] = s * m_dir.d.v[1];
        projection.d.v[2] = s * m_dir.d.v[2];
        projection.d.v[0] += m_start.d.v[0];
        projection.d.v[1] += m_start.d.v[1];
        projection.d.v[2] += m_start.d.v[2];
    } else {
        projection = m_start;
        t = 0.5f;
    }
}
