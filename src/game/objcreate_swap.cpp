// A fragment of objcreate.cpp (0x8003d130): DestroySubHierObject calls the
// virtual function at slot 0x164 (a placeholder named by offset) of a
// hierarchical object's sub-object; SwapHierObject swaps a
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
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
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
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual void unknown140();
    virtual void unknown144();
    virtual void unknown148();
    virtual void unknown14c();
    virtual void unknown150();
    virtual void unknown154();
    virtual void unknown158();
    virtual void unknown15c();
    virtual void unknown160();
    virtual void unknown164(); /* result type not known */

    CHierObject* GetSubObject(int);
    void SwapOut(CStaticMesh*, MotionData*);
};

void DestroySubHierObject(CHierObject* object, int sub) {
    object->GetSubObject(sub)->unknown164();
}

void SwapHierObject(CHierObject* object, int sub, int index) {
    int type = object->unknown060()->m_link->m_owner->m_type;
    CStaticMesh* mesh = CStaticObjectFactory::GetStaticMesh(type, index);
    MotionData* motion = CStaticObjectFactory::GetMotionData(type, index);
    object->GetSubObject(sub)->SwapOut(mesh, motion);
}
