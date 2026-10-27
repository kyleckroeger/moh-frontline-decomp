// A fragment of staticmesh.cpp (0x800f151c): CStaticAnimMesh::Init (the
// animated mesh data kept; when relocating, an inline helper makes the mesh
// table and each frame's mesh pointer absolute through the file's weak
// offsetPtr templates (defined later in the file, so called) and initialises
// each frame's mesh as the current static mesh; the first frame's mesh is made
// current) and CStaticAnimMesh::Draw (the frame's mesh is made current, the
// static mesh drawn and the first frame's mesh restored). The file name is
// this project's; the original record is staticmesh.cpp. The classes are named
// by the mangled symbols; the animated mesh data, the members and the
// relocation helper are inferred. CStaticMesh declares Draw and
// CStaticAnimMesh its destructor (both defined elsewhere) first so no table
// is emitted here.
class CDrawContext;
struct StaticMesh;

/* Inferred: the animated mesh data's mesh table at +8 and frame count at +12. */
struct SAnimMeshData {
    unsigned char unknown00[8];
    StaticMesh** m_meshes;
    int m_count;
};

template <class T> void offsetPtr(T*&, int);

class CStaticMesh {
public:
    virtual void Draw(CDrawContext&, int);
    virtual ~CStaticMesh();
    void Init();

    StaticMesh* m_mesh;
};

class CStaticAnimMesh : public CStaticMesh {
public:
    virtual ~CStaticAnimMesh();
    virtual void Draw(CDrawContext&, int);

    unsigned char unknown08[4];
    SAnimMeshData* m_data;
    void Init(void*, bool);
    void Relocate(int base) {
        offsetPtr(m_data->m_meshes, base);
        for (int i = 0; i < m_data->m_count; i++) {
            offsetPtr(m_data->m_meshes[i], base);
            m_mesh = m_data->m_meshes[i];
            CStaticMesh::Init();
        }
    }
};

void CStaticAnimMesh::Init(void* data, bool relocate) {
    m_data = (SAnimMeshData*)data;
    if (relocate) {
        Relocate((int)m_data);
        m_mesh = m_data->m_meshes[0];
    } else {
        m_mesh = m_data->m_meshes[0];
    }
}

void CStaticAnimMesh::Draw(CDrawContext& context, int frame) {
    m_mesh = m_data->m_meshes[frame];
    CStaticMesh::Draw(context, frame);
    m_mesh = m_data->m_meshes[0];
}
