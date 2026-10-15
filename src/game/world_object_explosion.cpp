// A fragment of world_object.cpp (0x800b72ec): CWorldObject::HandleExplosion
// passes the explosion to each of the world object's compartments, and
// GetCompartment finds the compartment with the given index (its word at
// +0x50, the texture-swap set of compartment_texswap.cpp), or none. The
// classes and functions are named by the mangled symbols; the compartment
// list view follows world_object_draw.cpp (the compartment is a 104-byte
// view here). The rest of the file is not part of this unit.
class CExplosion;

class CCompartment {
public:
    void HandleExplosion(const CExplosion&);

    unsigned char unknown00[80];
    int m_index;
    unsigned char unknown54[20];
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
    CCompartment* GetCompartment(int);

    unsigned char unknown000[80];
    WorldListLinkView m_compartments;
};

void CWorldObject::HandleExplosion(const CExplosion& explosion) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next)
        node->compartment.HandleExplosion(explosion);
}

CCompartment* CWorldObject::GetCompartment(int index) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next) {
        if (index == node->compartment.m_index)
            return &node->compartment;
    }
    return 0;
}
