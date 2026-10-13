// A fragment of attachobject.cpp (0x80097cf8): CAttachableObject's two
// constructors (each runs the matching CStaticObject constructor, constructs
// the ten attachment matrices at +688 and clears the ten attached-object
// pointers at +640). The classes are named by the mangled symbols; the
// members are inferred from offsets (AttachObject stores the attached object
// at +640 and copies its matrix to +688 + 64 * index). CMatrix's constructor
// takes a default argument (hence the generated __defctor the array
// construction calls); its value is not known. CAttachableObject declares
// MarkForDestruction (defined elsewhere) first so its global table stays
// elsewhere; the base destructor gives the constructors' cleanup entries.
class CStaticMesh;

class CMatrix {
public:
    CMatrix(int unknown = 0);

    float m[4][4];
} __attribute__((aligned(16)));

class CStaticObject {
public:
    struct MotionFrame;

    CStaticObject();
    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    virtual ~CStaticObject();

    unsigned char unknown004[636];
};

class CAttachableObject : public CStaticObject {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    virtual ~CAttachableObject();
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CAttachableObject();

    CStaticObject* m_attached[10];
    unsigned char unknown2a8[8];
    CMatrix m_attachMatrix[10];
};

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
