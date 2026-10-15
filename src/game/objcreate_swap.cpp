// A fragment of objcreate.cpp (0x8003d160): SwapHierObject swaps a
// sub-object of a hierarchical object to the static mesh and motion data
// of another index of the object's type. The functions and classes are
// named by the symbols; the virtual function at slot 0x60 (a placeholder
// named by offset) and the chain from its result to the type (+8, +4,
// +72) are inferred, and the parameter meanings are inferred from the calls.
// The rest of the file is not part of this unit.
class CStaticMesh;
class MotionData;

class CStaticObjectFactory {
public:
    static CStaticMesh* GetStaticMesh(int, int);
    static MotionData* GetMotionData(int, int);
};

/* inferred: the views from the slot-0x60 result to the type */
struct FactoryOwnerView {
    unsigned char unknown00[72];
    int m_type;
};

struct FactoryLinkView {
    unsigned char unknown0[4];
    FactoryOwnerView* m_owner;
};

struct FactoryRefView {
    unsigned char unknown0[8];
    FactoryLinkView* m_link;
};

class CHierObject {
public:
    virtual void unknown008();
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
    virtual FactoryRefView* unknown060();

    CHierObject* GetSubObject(int);
    void SwapOut(CStaticMesh*, MotionData*);
};

void SwapHierObject(CHierObject* object, int sub, int index) {
    int type = object->unknown060()->m_link->m_owner->m_type;
    CStaticMesh* mesh = CStaticObjectFactory::GetStaticMesh(type, index);
    MotionData* motion = CStaticObjectFactory::GetMotionData(type, index);
    object->GetSubObject(sub)->SwapOut(mesh, motion);
}
