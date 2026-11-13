// AI spline paths, the functions at the start of the file: the path manager
// generates a test path and its reverse (the reverse in the second half of
// the path array, tagged with bit 24), frees and allocates its generation buffers. The class and
// structure names come from the mangled symbols; the members are inferred from
// offsets. The buffer names follow the header's prime list in the file's .rodata.
extern "C" void MEM_free(void*);
void* DWI_alloc(const char*, int, int);

// Header-defined hash-table primes; every unit that includes the header carries a copy.
#include "game/DwiHashMap.h"

struct _aifilter_simple_v3;
class CAISplinePathManager;

class CAISplinePath {
public:
    void GenerateTestSplinePath(const CAISplinePathManager&, unsigned long, const _aifilter_simple_v3*, bool);

    unsigned char unknown00[8];
    unsigned long m_id;
};

class CAISplinePathManager {
public:
    void GenerateSplinePath(unsigned long, unsigned long, unsigned long, const _aifilter_simple_v3*);
    void FreeGenerateBuffers();
    void AllocateGenerateBuffers();

    unsigned long m_count;
    CAISplinePath* m_paths;
    void* m_points;
    void* m_diagonals;
    void* m_deltas;
    void* m_derivatives;
};

void CAISplinePathManager::GenerateSplinePath(unsigned long index, unsigned long id, unsigned long count,
                                              const _aifilter_simple_v3* points) {
    m_paths[index].GenerateTestSplinePath(*this, count, points, false);
    m_paths[index].m_id = id;
    unsigned long reverse = index + m_count / 2;
    m_paths[reverse].GenerateTestSplinePath(*this, count, points, true);
    m_paths[reverse].m_id = id + 0x1000000;
}

void CAISplinePathManager::FreeGenerateBuffers() {
    MEM_free(m_points);
    MEM_free(m_diagonals);
    MEM_free(m_deltas);
    MEM_free(m_derivatives);
    m_points = 0;
    m_diagonals = 0;
    m_deltas = 0;
    m_derivatives = 0;
}

void CAISplinePathManager::AllocateGenerateBuffers() {
    m_points = DWI_alloc("AI Spline Path Points", 16384, 256);
    m_diagonals = DWI_alloc("AI Spline Path Diagonals", 4096, 256);
    m_deltas = DWI_alloc("AI Spline Path Deltas", 16384, 256);
    m_derivatives = DWI_alloc("AI Spline Path Derivatives", 16384, 256);
}
