// CMatrixStack: a stack of matrices allocated as one aligned block, with the
// current stack and the global stack created at class initialisation.
// CMatrixStack, CMatrix and CVector3 are named by the mangled symbols; the
// members are inferred from offsets and are not original.
extern "C" int MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

#include "game/CVector3.h"

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix& operator=(const CMatrix&);
    void Ident();
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
};

class CMatrixStack {
public:
    CMatrixStack();
    ~CMatrixStack();
    static void InitClass();
    static void EndClass();
    void Allocate(unsigned long);
    void SetCurrent();
    void Reset();
    void Push();
    void PushUnit();
    void Pop();
    void Ident();
    void Load(const CMatrix*);
    void Store(CMatrix*);
    void ScaleRow(CVector3);

    unsigned long m_size;
    int m_index;
    CMatrix* m_stack;

    static CMatrixStack* s_currentStack;
};

CMatrixStack* g_matStack;
CMatrixStack* CMatrixStack::s_currentStack;

void CMatrixStack::ScaleRow(CVector3 scale) {
    CMatrix* matrix = &m_stack[m_index];

    matrix->m[0][0] *= scale.x;
    matrix->m[0][1] *= scale.x;
    matrix->m[0][2] *= scale.x;
    matrix->m[1][0] *= scale.y;
    matrix->m[1][1] *= scale.y;
    matrix->m[1][2] *= scale.y;
    matrix->m[2][0] *= scale.z;
    matrix->m[2][1] *= scale.z;
    matrix->m[2][2] *= scale.z;
}

void CMatrixStack::Store(CMatrix* matrix) {
    *matrix = m_stack[m_index];
}

void CMatrixStack::Load(const CMatrix* matrix) {
    m_stack[m_index] = *matrix;
}

void CMatrixStack::Ident() {
    m_stack[m_index].Ident();
}

void CMatrixStack::Pop() {
    m_index--;
}

void CMatrixStack::PushUnit() {
    m_stack[m_index + 1] = m_stack[m_index];
    m_index++;
    m_stack[m_index].Ident();
}

void CMatrixStack::Push() {
    m_stack[m_index + 1] = m_stack[m_index];
    m_index++;
}

void CMatrixStack::Reset() {
    m_index = 0;
    m_stack[m_index].Ident();
}

void CMatrixStack::SetCurrent() {
    if (s_currentStack != this)
        s_currentStack = this;
}

void CMatrixStack::Allocate(unsigned long count) {
    m_size = count;
    m_index = 0;
    m_stack = (CMatrix*)DWI_allocalign(0, count * sizeof(CMatrix), 16, 1024);
}

CMatrixStack::~CMatrixStack() {
    if (m_stack) {
        MEM_free(m_stack);
        m_stack = 0;
        m_size = 0;
        m_index = 0;
    }
}

CMatrixStack::CMatrixStack() : m_size(0), m_index(0), m_stack(0) {
}

void CMatrixStack::EndClass() {
    delete g_matStack;
}

void CMatrixStack::InitClass() {
    g_matStack = new CMatrixStack;
    g_matStack->Allocate(8);
    g_matStack->SetCurrent();
}

// The static initialisation runs the inline CMatrix constructor (CMatrix::
// InitClass on first use) for a file-level CMatrix that nothing else
// references; the object is not identified (no symbol survives), so its name
// is a placeholder.
static CMatrix s_unidentifiedMatrix;
