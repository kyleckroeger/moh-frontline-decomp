// A fragment of Soldier_object.cpp (0x800a9f1c): CSoldierObject's
// SetHelmetOffset (the "head" node's hand matrix moved by the offset turned
// into the matrix's frame) and AttachMesh (attaches a mesh at the named
// skeleton node when the node has nothing attached: the mesh is lit and the
// node's matrix becomes the node's own matrix, or, given an attachment CRC,
// the inverse of the mesh's matching attachment point times it; returns
// whether it was attached), the weak CStaticMesh::EnableLighting (inline
// virtual; emitted after its first use), DetachObject (clears the named
// node's attachment, and the bullet emitter's validity when it was that
// node; returns the old attachment) and AttachObject (as AttachMesh for a
// static object's mesh: the attachment marked 1, the mesh lit when there is
// one, and the matching attachment point's inverse applied after the node's
// matrix) and FireMMG (fires the mounted gun's weapon from the gun's muzzle,
// its matrix times the muzzle attachment point 0xE6E804D5, toward the
// target, the aim turned from the gun's front direction by a slerp limited
// to a cosine of 0.9848 (about 10 degrees); records the gun and the firing flag). The classes and functions are named by the mangled symbols; the
// members and the skeleton, attachment and mesh views are inferred (the node
// search is the inline helper of Soldier_object_cleanup.cpp; the skeleton is
// read through an inferred inline accessor, as GetLookMatrix does; FireMMG's
// vector helpers and the weapon view are inferred, and its locals are
// declared in the order that gives the original's slots), and the CVector3
// view copies through a double pair as in player_weapon.cpp. The rest of the
// file is not part of this unit.
extern "C" int stricmp(const char*, const char*);
extern "C" double acos(double);
extern "C" double sin(double);

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    CVector3& operator-=(const CVector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    CVector3& operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    void Inverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;
    void SetPos(CVector3);
    CVector3 GetPos() const { return pos; }
    CVector3 GetFront() const { return front; }
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

/* inferred: an attachment point of a mesh (80 bytes) */
struct AttachPointView {
    CMatrix m_matrix;
    int m_crc;
    unsigned char unknown44[12];
};

struct AttachListView {
    unsigned char unknown00[40];
    AttachPointView* m_points;
    int m_count;
};

class CStaticMesh {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void EnableLighting(bool on) { m_lit = on; }

    AttachListView* m_attachList;
    unsigned char m_lit : 1;
};

/* inferred: the static object's mesh at +48 */
class CStaticObject {
public:
    unsigned char unknown00[48];
    CStaticMesh* m_mesh;
    unsigned char unknown34[12];
    CMatrix m_matrix;
};

enum EWeaponShootType {};

/* inferred: the weapon's shot direction (+672) and origin (+688) */
class CWeapon {
public:
    void Shoot(EWeaponShootType, float, float);

    unsigned char unknown000[672];
    CVector3 m_direction;
    CVector3 m_origin;
};

/* inferred: a skeleton node (24 bytes): name, bone, own matrix */
struct SkeletonNodeView {
    char m_name[16];
    int m_bone;
    CMatrix* m_matrix;
};

struct SkeletonView {
    unsigned char unknown00[16];
    unsigned int m_count;
    SkeletonNodeView* m_nodes;

    /* inferred: the index of the node with the given name, or -1 */
    unsigned int Find(const char* name) const {
        for (unsigned int i = 0; i < m_count; i++) {
            if (stricmp(m_nodes[i].m_name, name) == 0)
                return i;
        }
        return (unsigned int)-1;
    }
};

class CSoldierObject {
public:
    void SetHelmetOffset(CVector3&);
    bool AttachMesh(CStaticMesh*, char*, int);
    CStaticMesh* DetachObject(char*);
    bool AttachObject(CStaticObject*, char*, int);
    bool FireMMG(CVector3&, CStaticObject*);

    unsigned char unknown0000[3300];
    SkeletonView* m_skeleton;
    SkeletonView* GetSkeleton() const { return m_skeleton; }
    unsigned char unknown0ce8[13132];
    CStaticMesh* m_attachments[8];
    unsigned int m_attachTypes[8];
    unsigned char unknown4074[12];
    CMatrix m_handMatrices[8];
    bool m_emitterValid;
    unsigned int m_emitterIndex;
    unsigned char unknown4288[508];
    bool m_firingMMG;
    CStaticObject* m_mmg;
    CWeapon* m_mmgWeapon;
};

void CSoldierObject::SetHelmetOffset(CVector3& offset) {
    unsigned int head = m_skeleton->Find("head");
    CMatrix& matrix = m_handMatrices[head];
    CVector3 pos = matrix.GetPos();
    CVector3 move;
    matrix.TransformVector(move, offset);
    pos += move;
    matrix.SetPos(pos);
}

bool CSoldierObject::AttachMesh(CStaticMesh* mesh, char* name, int crc) {
    SkeletonView* skeleton = GetSkeleton();
    if (!mesh)
        return false;
    unsigned int node = skeleton->Find(name);
    if (node == (unsigned int)-1)
        return false;
    if (m_attachTypes[node])
        return false;
    m_attachments[node] = mesh;
    m_attachTypes[node] = 2;
    m_attachments[node]->EnableLighting(true);
    CMatrix inverse;
    if (crc) {
        int i;
        AttachListView* list = mesh->m_attachList;
        int count = list->m_count;
        AttachPointView* point = list->m_points;
        for (i = 0; i < count; i++, point++) {
            if (point->m_crc == crc) {
                i = count;
                inverse.Inverse(point->m_matrix);
            }
        }
        m_handMatrices[node].Multiply(inverse, *skeleton->m_nodes[node].m_matrix);
    } else {
        m_handMatrices[node] = *skeleton->m_nodes[node].m_matrix;
    }
    return true;
}

CStaticMesh* CSoldierObject::DetachObject(char* name) {
    SkeletonView* skeleton = GetSkeleton();
    unsigned int i;
    for (i = 0; i < skeleton->m_count; i++) {
        if (stricmp(skeleton->m_nodes[i].m_name, name) == 0)
            break;
    }
    if (i == skeleton->m_count)
        return 0;
    CStaticMesh* mesh = m_attachments[i];
    m_attachments[i] = 0;
    m_attachTypes[i] = 0;
    if (m_emitterIndex == i)
        m_emitterValid = false;
    return mesh;
}

bool CSoldierObject::AttachObject(CStaticObject* object, char* name, int crc) {
    SkeletonView* skeleton = GetSkeleton();
    if (!object)
        return false;
    unsigned int node = skeleton->Find(name);
    if (node == (unsigned int)-1)
        return false;
    if (m_attachTypes[node])
        return false;
    m_attachments[node] = (CStaticMesh*)object;
    m_attachTypes[node] = 1;
    CStaticMesh* mesh = object->m_mesh;
    if (mesh)
        mesh->EnableLighting(true);
    CMatrix inverse;
    if (crc) {
        int i;
        AttachListView* list = mesh->m_attachList;
        int count = list->m_count;
        AttachPointView* point = list->m_points;
        for (i = 0; i < count; i++, point++) {
            if (point->m_crc == crc) {
                inverse.Inverse(point->m_matrix);
                break;
            }
        }
        m_handMatrices[node] = *skeleton->m_nodes[node].m_matrix;
        if (i < count)
            m_handMatrices[node].Multiply(inverse, m_handMatrices[node]);
    } else {
        m_handMatrices[node] = *skeleton->m_nodes[node].m_matrix;
    }
    return true;
}

bool CSoldierObject::FireMMG(CVector3& target, CStaticObject* gun) {
    AttachListView* list = gun->m_mesh->m_attachList;
    int count = list->m_count;
    AttachPointView* point = list->m_points;
    CMatrix matrix;
    matrix = gun->m_matrix;
    for (int i = 0; i < count; i++, point++) {
        if (point->m_crc == 0xE6E804D5) {
            matrix.Multiply(point->m_matrix, matrix);
            break;
        }
    }
    CVector3 origin = matrix.pos;
    CVector3 forward = matrix.front;
    CVector3 direction = target;
    direction -= origin;
    CVector3 aim = direction;
    aim.Normalize();
    float cosine = forward.Dot(aim);
    const float limit = 0.9848f;
    if (cosine < limit) {
        float maxAngle = acos(limit);
        float angle = acos(cosine);
        float sinAngle = sin(angle);
        float a = (float)sin(angle - maxAngle) / sinAngle;
        float b = (float)sin(maxAngle) / sinAngle;
        aim *= b;
        forward *= a;
        aim += forward;
    }
    m_mmgWeapon->m_direction = aim;
    m_mmgWeapon->m_origin = origin;
    m_mmgWeapon->Shoot((EWeaponShootType)2, 0.0f, -1.0f);
    m_firingMMG = true;
    m_mmg = gun;
    return true;
}
