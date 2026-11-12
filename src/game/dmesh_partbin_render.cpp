// A fragment of dmesh.cpp (0x800f58cc): CPartBin::Render (the part's
// 608-byte render record is copied into the packet, followed by the current
// perspective matrix; returns 0). The file name is this project's; the
// original record is dmesh.cpp. CPartBin and g_PerspectiveMatrix are named by
// the mangled symbols; the record size and the packet cursor are inferred.
// The matrix is file-local in the original and declared extern to link to
// it. CPartBin declares its destructor (defined elsewhere) first so its table
// is not emitted here.
extern "C" void* memcpy(void*, const void*, unsigned long);

class CDmaTag;

/* Only the assignment and the 16-aligned layout of the matrix. */
#include "game/CMatrix.h"


/* Inferred: the packet's write cursor at +8. */
class CDmaPacket {
public:
    unsigned char unknown00[8];
    unsigned char* m_cur;
};

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
};

class CPartBin : public CRenderBin {
public:
    virtual ~CPartBin();
    virtual int Render(CDmaPacket&, void*);
};

extern CMatrix g_PerspectiveMatrix;

int CPartBin::Render(CDmaPacket& packet, void* data) {
    memcpy(packet.m_cur, data, 608);
    packet.m_cur += 608;
    *(CMatrix*)packet.m_cur = g_PerspectiveMatrix;
    packet.m_cur += sizeof(CMatrix);
    return 0;
}
