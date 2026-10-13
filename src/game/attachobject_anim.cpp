// A fragment of attachobject.cpp (0x80097844): the CAnimObject overload of
// CAttachableObject::AttachObject (as the CStaticObject one in
// attachobject_ctor.cpp, with flag 2 and the object told it is attached).
// The CPlayerObject overload after it is not reconstructed. The classes are
// named by the mangled symbols; the views are those of attachobject_ctor.cpp
// (inferred), with CAnimObject's virtuals as placeholders up to
// GetScriptObject (+96 in its table).
class CStaticMesh;

class CMatrix {
public:
    CMatrix(int unknown = 0);
    CMatrix& operator=(const CMatrix&);

    float m[4][4];
} __attribute__((aligned(16)));

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
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

class CAnimObject : public ISceneNode {
public:
    virtual ~CAnimObject();
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
    void SetAttached(bool);
};

class CAttachableObject : public CStaticObject {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    virtual ~CAttachableObject();
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CAttachableObject();
    int AttachObject(CAnimObject*, int); /* result type inferred */
    int AttachObject(CStaticObject*, int); /* result type inferred */

    CStaticObject* m_attached[10];
    unsigned char unknown2a8[8];
    CMatrix m_attachMatrix[10];
    int m_attachFlag[10];
    int m_attachScriptId[10];
};

int CAttachableObject::AttachObject(CAnimObject* object, int id) {
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
    m_attached[index] = (CStaticObject*)object;
    m_attachScriptId[index] = object->GetScriptObject()->id;
    m_attachFlag[index] = 2;
    object->SetAttached(this);
    g_scene.ExcludeCollision(*this, *object, -1);
    return 1;
}
