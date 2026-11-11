// world_object.cpp: its zero-initialised globals (0x4 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

class CWorldObject {
public:
    static int m_sCompartmentSize;
};

int CWorldObject::m_sCompartmentSize;
