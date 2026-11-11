// matrix.cpp: its zero-initialised globals (0x1 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

class CMatrix {
public:
    static bool s_ClassInit;
};

bool CMatrix::s_ClassInit;
