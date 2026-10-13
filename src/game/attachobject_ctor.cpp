// A fragment of attachobject.cpp (0x80097c0c): CAttachableObject::AttachObject
// (null objects refused; the mesh attach point with the id gives the slot and
// its matrix, else slot 0; the object, its script object's id and a flag are
// stored and the scene stops colliding the two), then its two
// constructors (each runs the matching CStaticObject constructor, constructs
// the ten attachment matrices at +688 and clears the ten attached-object
// pointers at +640). The classes are named by the mangled symbols; the
// members are inferred from offsets (AttachObject stores the attached object
// at +640 and copies its matrix to +688 + 64 * index). CMatrix's constructor
// takes a default argument (hence the generated __defctor the array
// construction calls); its value is not known. CAttachableObject declares
// MarkForDestruction (defined elsewhere) first so its global table stays
// elsewhere; the base destructor gives the constructors' cleanup entries.
// CStaticObject's virtuals are placeholders up to GetScriptObject (+96 in its
// table); the mesh, attach-point and script views are inferred.
class CStaticMesh;

class CMatrix {
public:
    CMatrix(int unknown = 0);
    CMatrix& operator=(const CMatrix&);

    float m[4][4];
} __attribute__((aligned(16)));

class ISceneNode {
public:
    virtual ~ISceneNode();
};

class CScene {
public:
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);

    char data[328];
};

extern CScene g_scene;

// Inferred: the script object's id word at +16.
struct ScriptObjectView {
    unsigned char unknown00[16];
    int id;
};

// Inferred: the mesh's attach points, 80 bytes each (a matrix, then the id at
// +64), reached through the object's mesh (+48) and its data (+4).
struct AttachPointView {
    CMatrix matrix;
    int id;
    unsigned char unknown44[12];
};

struct MeshDataView {
    unsigned char unknown00[40];
    AttachPointView* points;
    int count;
};

struct MeshView {
    unsigned char unknown00[4];
    MeshDataView* data;
};

class CStaticObject : public ISceneNode {
public:
    struct MotionFrame;

    CStaticObject();
    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    virtual ~CStaticObject();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual ScriptObjectView* GetScriptObject() const;

    unsigned char data004[44];
    MeshView* m_mesh;
    unsigned char data034[588];
};

class CAttachableObject : public CStaticObject {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    virtual ~CAttachableObject();
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CAttachableObject();
    int AttachObject(CStaticObject*, int); /* result type inferred */

    CStaticObject* m_attached[10];
    unsigned char unknown2a8[8];
    CMatrix m_attachMatrix[10];
    int m_attachFlag[10];
    int m_attachScriptId[10];
};

int CAttachableObject::AttachObject(CStaticObject* object, int id) {
    int index = 0;
    if (!object)
        return 0;
    MeshDataView* data = m_mesh->data;
    AttachPointView* point = data->points;
    for (int i = 0; i < data->count; i++, point++) {
        if (point->id == id) {
            index = i;
            m_attachMatrix[i] = point->matrix;
            break;
        }
    }
    m_attached[index] = object;
    m_attachScriptId[index] = object->GetScriptObject()->id;
    m_attachFlag[index] = 1;
    g_scene.ExcludeCollision(*this, *object, -1);
    return 1;
}

CAttachableObject::CAttachableObject(CStaticMesh* mesh, int a, int b, int c, int d, int e,
                                     CStaticObject::MotionFrame* frame)
    : CStaticObject(mesh, a, b, c, d, e, frame) {
    for (int i = 0; i < 10; i++)
        m_attached[i] = 0;
}

CAttachableObject::CAttachableObject() {
    for (int i = 0; i < 10; i++)
        m_attached[i] = 0;
}
