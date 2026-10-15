// A fragment of world_object.cpp (0x800b72ec): CWorldObject::HandleExplosion
// passes the explosion to each of the world object's compartments. The
// classes and functions are named by the mangled symbols; the compartment
// list view follows world_object_draw.cpp (the compartment is a 104-byte
// view here). The rest of the file is not part of this unit.
class CExplosion;

class CCompartment {
public:
    void HandleExplosion(const CExplosion&);

    unsigned char unknown00[104];
};

struct WorldCompartmentNodeView {
    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
    CCompartment compartment;
};

struct WorldListLinkView {
    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
};

class CWorldObject {
public:
    void HandleExplosion(const CExplosion&);

    unsigned char unknown000[80];
    WorldListLinkView m_compartments;
};

void CWorldObject::HandleExplosion(const CExplosion& explosion) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next)
        node->compartment.HandleExplosion(explosion);
}
