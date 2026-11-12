// CDrawContext's constructor: keep the four source matrices, clear the
// state, and build the combined matrices (local-to-view, view-to-screen and
// their product), then load the projection and view matrices into GX.
// CMatrix and CDrawContext are named by the mangled symbols; their members
// are inferred from offsets and are not original.
extern "C" {
void GXSetProjection(const float mtx[4][4], int type);
void GXLoadPosMtxImm(const float mtx[3][4], unsigned long id);
}

#include "game/CMatrix.h"


class CDrawContext {
public:
    CDrawContext(const CMatrix&, const CMatrix&, const CMatrix&, const CMatrix&, int);

    CMatrix m_matrix0;
    CMatrix m_matrix40;
    CMatrix m_matrix80;
    int m_fieldC0;
    const CMatrix& m_source0;
    const CMatrix& m_source1;
    const CMatrix& m_source2;
    const CMatrix& m_source3;
    int m_fieldD4;
    int m_fieldD8;
    int m_fieldDC;
    float m_fieldE0;
    float m_fieldE4;
    float m_fieldE8;
    int m_fieldEC;
    int m_fieldF0;
    int m_fieldF4;
    int m_fieldF8;
    int m_fieldFC;
    char m_field100[0x14];
    int m_field114;
};

CDrawContext::CDrawContext(const CMatrix& source0, const CMatrix& source1, const CMatrix& source2,
                           const CMatrix& source3, int field114)
    : m_fieldC0(0), m_source0(source0), m_source1(source1), m_source2(source2), m_source3(source3),
      m_fieldD4(0), m_fieldD8(0), m_fieldE0(1.0f), m_fieldE4(1.0f), m_fieldE8(1.0f), m_fieldF0(0),
      m_fieldF8(0), m_fieldFC(0), m_field114(field114) {
    float mtx[3][4];

    m_matrix0.Multiply(m_source0, m_source1);
    m_matrix40.Multiply(m_source2, m_source3);
    m_matrix80.Multiply(m_matrix0, m_matrix40);
    GXSetProjection(((float(*)[4])&m_source1.right), 0);
    m_source0.GetGAMECUBEMatrix34(mtx);
    GXLoadPosMtxImm(mtx, 0);
}
