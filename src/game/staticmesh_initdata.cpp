// A fragment of staticmesh.cpp (0x800f2cb8): CStaticMesh::Init(void*, bool),
// which stores the mesh data pointer and runs the full Init() when asked. The
// file name is this project's; the original record is staticmesh.cpp. The class
// and both Init overloads are named by the mangled symbols; the data pointer
// member at +4 is an inferred view.
class CStaticMesh {
public:
    void Init();
    void Init(void* data, bool init);

    unsigned char unknown00[4];
    void* m_data;
};

void CStaticMesh::Init(void* data, bool init) {
    m_data = data;
    if (init)
        Init();
}
