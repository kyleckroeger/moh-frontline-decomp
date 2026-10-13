// A fragment of attachobject.cpp (0x80097844): CAttachableObject's three
// AttachObject overloads and its two constructors. Each AttachObject refuses a
// null object, takes the slot of the mesh attach point with the id (else slot
// 0, copying that point's matrix), stores the object and its script object's
// id and a kind flag (2 animated, 3 player, 1 static) and stops the scene
// colliding the two; the animated object is told it is attached, the player
// gets two flag bits and its weapon hidden or shown, and is placed at once
// through the per-slot update UpdateAttachedObjects also inlines (an inferred
// inline: the slot is dropped when its object's script id changed, else the
// object follows this object's transform times the slot matrix, by kind).
// The constructors run the matching CStaticObject constructor, construct the
// ten slot matrices and clear the ten slots. The classes and methods are
// named by the mangled symbols; ISceneNode's virtuals are declared in table
// order (placeholders where not used) and the members are inferred from
// offsets (names by role or offset). CMatrix's constructor takes a default
// argument whose value is not known (hence the generated __defctor the array
// construction calls, a duplicate of another file's copy).
// CAttachableObject declares MarkForDestruction (defined elsewhere) first so
// its global table stays elsewhere; the base destructor gives the
// constructors' cleanup entries. The player flag bits are bool bit-fields.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    void Inverse(const CMatrix&);
    CVector3 GetPos() const { return *(const CVector3*)m[3]; }
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

struct ScriptObjectView {
    unsigned char unknown00[16];
    int id;
};

class CStaticObject;
class CAnimObject;
class CPlayerObject;

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
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
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual CStaticObject* AsStaticObject();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual CAnimObject* AsAnimObject();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual CPlayerObject* AsPlayerObject();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class CScene {
public:
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);

    char data[328];
};

extern CScene g_scene;

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

class CStaticMesh;

class CStaticObject : public ISceneNode {
public:
    struct MotionFrame;

    CStaticObject();
    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    virtual ~CStaticObject();

    unsigned char data004[44];
    MeshView* m_mesh;
    unsigned char data034[12];
    CMatrix m_tm;
    unsigned char data080[512];
};

class CAnimObject : public ISceneNode {
public:
    void SetAttached(bool);

    unsigned char data004[3900];
    CMatrix m_unknownF40;
    unsigned char data0f80[5428];
    bool m_unknown24b4;
};

class CPlayerObject : public ISceneNode {
public:
    void SetHideWeapon(bool);

    unsigned char data004[914];
    bool m_flag80 : 1;
    bool m_flag40 : 1;
    bool m_flag20 : 1;
    bool m_flag10 : 1;
    bool m_flag08 : 1;
    bool m_flag04 : 1;
    bool m_flag02 : 1;
    bool m_flag01 : 1;
    unsigned char data397[937];
    CVector3 m_position;
};

class CAttachableObject : public CStaticObject {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    virtual ~CAttachableObject();
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CAttachableObject();
    int AttachObject(CAnimObject*, int); /* result type inferred */
    int AttachObject(CPlayerObject*, int, bool); /* result type inferred */
    int AttachObject(CStaticObject*, int); /* result type inferred */
    void UpdateAttachedObject(int);

    ISceneNode* m_attached[10];
    unsigned char unknown2a8[8];
    CMatrix m_attachMatrix[10];
    int m_attachFlag[10];
    int m_attachScriptId[10];
};

inline void CAttachableObject::UpdateAttachedObject(int i) {
    CMatrix tm;
    if (m_attached[i]->GetScriptObject()->id != m_attachScriptId[i]) {
        m_attached[i] = 0;
    } else {
        tm = m_tm;
        tm.Multiply(m_attachMatrix[i], tm);
        switch (m_attachFlag[i]) {
        case 1:
            m_attached[i]->AsStaticObject()->SetTMLocalToWorld(tm);
            break;
        case 2: {
            CAnimObject* anim = m_attached[i]->AsAnimObject();
            if (anim->m_unknown24b4) {
                CMatrix inverse;
                inverse.Inverse(anim->m_unknownF40);
                tm.Multiply(inverse, tm);
            }
            anim->SetTMLocalToWorld(tm);
            break;
        }
        case 3:
            if (m_attached[i]->AsPlayerObject()->m_flag01)
                m_attached[i]->AsPlayerObject()->m_position = tm.GetPos();
            else
                m_attached[i]->AsPlayerObject()->SetTMLocalToWorld(tm);
            break;
        }
    }
}

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
    m_attached[index] = object;
    m_attachScriptId[index] = object->GetScriptObject()->id;
    m_attachFlag[index] = 2;
    object->SetAttached(this);
    g_scene.ExcludeCollision(*this, *object, -1);
    return 1;
}

int CAttachableObject::AttachObject(CPlayerObject* player, int id, bool hideWeapon) {
    int index = 0;
    if (!player)
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
    m_attached[index] = player;
    m_attachScriptId[index] = player->GetScriptObject()->id;
    m_attachFlag[index] = 3;
    player->m_flag02 = 1;
    player->m_flag01 = !hideWeapon;
    player->SetHideWeapon(hideWeapon);
    g_scene.ExcludeCollision(*this, *player, -1);
    UpdateAttachedObject(index);
    return 1;
}

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
