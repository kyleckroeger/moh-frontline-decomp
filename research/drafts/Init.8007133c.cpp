inline double fabs(double x) { return __fabs(x); }
class CDBNode;
class CDBMaterial;

struct CDBVector {
    float x, y, z;
};

struct CDBTri {
    CDBVector* m_verts[3];
};

struct CDBTriGroup {
    unsigned char unknown00[16];
    int m_count;
    CDBTri* m_tris[1];
};

struct CDBLeafNode {
    unsigned char unknown00[64];
    int m_groupCount;
    CDBTriGroup* m_groups[1];
};

struct CDBBranchNode {
    int m_type;
    CDBNode* m_front;
    CDBNode* m_back;
    unsigned char unknown0c[16];
};

class CDB {
public:
    unsigned char unknown00[20];
    int m_groupCount;
    int m_leafCount;
    int m_branchCount;
    int unknown20;
    CDBTri* m_tris;
    CDBTriGroup* m_groups;
    CDBLeafNode* m_leaves;
    CDBBranchNode* m_branches;
    CDBMaterial* m_materials;
    CDBVector m_size;
    CDBVector m_rows[3];
    CDBVector m_position;
};

template <class T> void offsetPtr(T*& pointer, int base);

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

class IVolume {
public:
    virtual ~IVolume();
};

class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
    void Init(CDB*);

    CDB* m_db;
    CVector3 m_min;
    CVector3 m_max;
};

void CCDBObject::Init(CDB* db) {
    m_db = db;
    int base = (int)m_db;
    offsetPtr(m_db->m_tris, base);
    offsetPtr(m_db->m_groups, base);
    offsetPtr(m_db->m_leaves, base);
    offsetPtr(m_db->m_branches, base);
    offsetPtr(m_db->m_materials, base);
    CDBTriGroup* group = m_db->m_groups;
    for (int i = 0; i < m_db->m_groupCount; i++) {
        for (int j = 0; j < group->m_count; j++) {
            offsetPtr(group->m_tris[j], base);
            CDBTri* tri = group->m_tris[j];
            offsetPtr(tri->m_verts[0], base);
            offsetPtr(tri->m_verts[1], base);
            offsetPtr(tri->m_verts[2], base);
        }
        group = (CDBTriGroup*)((char*)group + (group->m_count * 4 + 20));
    }
    CDBLeafNode* leaf = m_db->m_leaves;
    for (int i = 0; i < m_db->m_leafCount; i++) {
        for (int j = 0; j < leaf->m_groupCount; j++)
            offsetPtr(leaf->m_groups[j], base);
        leaf = (CDBLeafNode*)((char*)leaf + (leaf->m_groupCount * 4 + 68));
    }
    CDBBranchNode* branch = m_db->m_branches;
    for (int i = 0; i < m_db->m_branchCount; i++) {
        offsetPtr(branch->m_front, base);
        offsetPtr(branch->m_back, base);
        branch++;
    }
    if (m_db->m_leafCount > 0) {
        CDB* d = m_db;
        float sx;
        float py;
        float qy;
        float qx;
        float pz;
        float sz;
        float z;
        float qz;
        float y;
        float px;
        float x;
        float sy;
        sz = d->m_size.z;
        sx = d->m_size.x;
        sy = d->m_size.y;
        px = d->m_position.x;
        py = d->m_position.y;
        pz = d->m_position.z;
        x = sx * (0.5f * (float)fabs(d->m_rows[0].x)) + sy * (0.5f * (float)fabs(d->m_rows[1].x)) + sz * (0.5 * fabs(d->m_rows[2].x));
        y = sx * (0.5f * (float)fabs(d->m_rows[0].y)) + sy * (0.5f * (float)fabs(d->m_rows[1].y)) + sz * (0.5 * fabs(d->m_rows[2].y));
        z = sx * (0.5f * (float)fabs(d->m_rows[0].z)) + sy * (0.5f * (float)fabs(d->m_rows[1].z)) + sz * (0.5 * fabs(d->m_rows[2].z));
        m_min.d.v[0] = px - x;
        m_min.d.v[1] = py - y;
        m_min.d.v[2] = pz - z;
        qx = d->m_position.x;
        qy = d->m_position.y;
        qz = d->m_position.z;
        m_max.d.v[0] = qx + x;
        m_max.d.v[1] = qy + y;
        m_max.d.v[2] = qz + z;
    } else {
        m_min.d.v[2] = 0.0f;
        m_min.d.v[1] = 0.0f;
        m_min.d.v[0] = 0.0f;
        m_max.d.v[2] = 0.0f;
        m_max.d.v[1] = 0.0f;
        m_max.d.v[0] = 0.0f;
    }
}
