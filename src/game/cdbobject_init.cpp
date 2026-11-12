// A fragment of cdbobject.cpp (0x8007133c): CCDBObject::Init. It keeps the
// CDB image, rebases the image's pointers (tris, tri groups, leaves, branch
// nodes, materials, then each group's tris and their vertices, each leaf's
// groups and each branch node's children) by the image's address, and sets
// the object's bounds from the image's box: the half extents along the world
// axes are the box size weighted by the absolute rotation rows (rows 0 and 1
// halved in float, row 2 in double), and the bounds are the position minus
// and plus them; an image without leaves gets empty bounds. The file name is
// this project's; the original record is cdbobject.cpp. CCDBObject, CDB and
// offsetPtr are named by the mangled symbols; the CDB image layout, the
// member names and the base class view are inferred from offsets. The
// position is read into locals before the bounds are stored and read again
// for the upper bound (the stores may alias it), as the original does.
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

#include "game/CVector3.h"

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
        float x;
        float y;
        float z;
        float px;
        float py;
        float pz;
        float qx;
        float qy;
        float qz;
        px = d->m_position.x;
        py = d->m_position.y;
        pz = d->m_position.z;
        x = d->m_size.x * (0.5f * (float)fabs(d->m_rows[0].x)) + d->m_size.y * (0.5f * (float)fabs(d->m_rows[1].x)) + d->m_size.z * (0.5 * fabs(d->m_rows[2].x));
        y = d->m_size.x * (0.5f * (float)fabs(d->m_rows[0].y)) + d->m_size.y * (0.5f * (float)fabs(d->m_rows[1].y)) + d->m_size.z * (0.5 * fabs(d->m_rows[2].y));
        z = d->m_size.x * (0.5f * (float)fabs(d->m_rows[0].z)) + d->m_size.y * (0.5f * (float)fabs(d->m_rows[1].z)) + d->m_size.z * (0.5 * fabs(d->m_rows[2].z));
        m_min.x = px - x;
        m_min.y = py - y;
        m_min.z = pz - z;
        qx = d->m_position.x;
        qy = d->m_position.y;
        qz = d->m_position.z;
        qx += x;
        qy += y;
        qz += z;
        m_max.x = qx;
        m_max.y = qy;
        m_max.z = qz;
    } else {
        m_min.z = 0.0f;
        m_min.y = 0.0f;
        m_min.x = 0.0f;
        m_max.z = 0.0f;
        m_max.y = 0.0f;
        m_max.x = 0.0f;
    }
}
